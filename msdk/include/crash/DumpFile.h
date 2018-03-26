/*
 ----------------------------------------------------------------------------------
	DumpFile.h
	模块功能：故障转储模块，Dump文件默认保存在%Temp%目录下
 ----------------------------------------------------------------------------------
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <Dbghelp.h> 
#include <vector>
#include <time.h>
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")

// 自动创建DUMP文件机制
#define DeclareDumpFile() NSDumpFile::RunCrashHandler();

namespace NSDumpFile
{
	// 函数功能：创建DUMP文件并写入相关数据，保存到磁盘
	// 形参列表：__in_opt LPCWSTR lpstrDumpFilePathName - 指定DUMP文件名称，可以指定路径，如果未指定路径，默认为当前工作路径
	//           __in EXCEPTION_POINTERS *pException - 程序异常数据结构指针
	// 返回值：无
	void CreateDumpFile( __in_opt LPCTSTR lpstrDumpFilePathName, __in EXCEPTION_POINTERS *pException )  
	{  
		// 创建Dump文件
		HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		
		// Dump信息
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
		dumpInfo.ExceptionPointers = pException;  
		dumpInfo.ThreadId = GetCurrentThreadId();  
		dumpInfo.ClientPointers = TRUE;

		// 写入Dump文件内容
		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL );
		CloseHandle(hDumpFile);  
	}  


	LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
	{
		return NULL;
	}


	BOOL PreventSetUnhandledExceptionFilter()
	{
		HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
		if ( hKernel32 == NULL )
			return FALSE;


		void *pOrgEntry = GetProcAddress( hKernel32, "SetUnhandledExceptionFilter" );
		if( pOrgEntry == NULL )
			return FALSE;


		unsigned char newJump[100];
		DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
		dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far


		void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
		DWORD dwNewEntryAddr = (DWORD) pNewFunc;
		DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;


		newJump[0] = 0xE9;  // JMP absolute
		memcpy( &newJump[1], &dwRelativeAddr, sizeof(pNewFunc) );
		SIZE_T bytesWritten;
		BOOL bRet = WriteProcessMemory( GetCurrentProcess(),  pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
		return bRet;
	}

	// 函数功能：回调函数，失控的异常错误出现时调用
	// 形参列表：__in struct _EXCEPTION_POINTERS *pException - 异常错误数据结构
	// 返回值：表达式except()的合法的返回值
	LONG WINAPI UnhandledExceptionFilterEx( __in struct _EXCEPTION_POINTERS *pException )
	{
		srand( (unsigned int)time(NULL) );

		TCHAR szMbsFile[MAX_PATH] = { 0 };
		TCHAR szFileName[MAX_PATH] = { 0 };

		GetModuleFileName(NULL, szMbsFile, MAX_PATH );
		LPTSTR pTail = _tcsrchr(szMbsFile, _T('\\'));
		if ( pTail )
			_tcscpy_s(szFileName, MAX_PATH, pTail + 1);

		pTail = _tcsrchr(szFileName, _T('.'));

		if ( pTail )
			pTail[0] = 0;

		pTail = _tcsrchr(szMbsFile, _T('\\'));
		if( pTail )
			pTail[0] = 0;


		// 配置文件路径和文件名
		SYSTEMTIME st;
		GetLocalTime( &st );
		TCHAR szTempStr[MAX_PATH] = {0};
		StringCchPrintf( szTempStr, MAX_PATH, _T("\\%s_%d%02d%02d_%d.dmp"), szFileName, st.wYear, st.wMonth, st.wDay, rand()%10000+1000 );
		StringCchCat( szMbsFile, MAX_PATH, szTempStr );
		CreateDumpFile( szMbsFile, pException );
		return EXCEPTION_CONTINUE_EXECUTION;
	}


	void RunCrashHandler()
	{
		// 确定出现失控的异常发生时调用的函数为UnhandledExceptionFilterEx
		SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
		PreventSetUnhandledExceptionFilter();
	}
};

