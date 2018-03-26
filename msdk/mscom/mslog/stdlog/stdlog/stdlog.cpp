// stdlog.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <time.h>
#include <atlbase.h>
#include <atlconv.h>
#include <mslog/syslay/dbg.h>
#include <mslog/stdlog/stdlog.h>

#include <algorithm>
#include <list>
#include <string>
using namespace std;

#include "ExtendedTrace.h"

//控制台
class CDbgCons
{
public:
	HANDLE m_hOutput;
	BOOL m_bAllocConsole;
	CDbgCons()
	{
		m_hOutput = INVALID_HANDLE_VALUE;
		m_bAllocConsole = FALSE;
	}
	void InitCons(int iReserved)
	{
		m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE); 
		if(INVALID_HANDLE_VALUE==m_hOutput || NULL==m_hOutput)
		{
			m_bAllocConsole = ::AllocConsole();
			m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
		}
		else
		{
			m_bAllocConsole = TRUE;
		}
	// 	FILE* pFile = NULL;
	// 	freopen_s(&pFile, "CON", "w", stdout);
	}

	VOID SetDbgType(DbgType_t type)
	{
		DWORD wAttributes = 0;
		switch(type)
		{
		case DBG_ERROR:
			wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
		case DBG_WARNING:
			wAttributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case DBG_LOG:
			wAttributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
// 		case DBG_MESSAGE:
// 			wAttributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
// 			break;
		case DBG_ASSERT:
			wAttributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		default:
			wAttributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
		}
		BOOL bResult = SetConsoleTextAttribute(m_hOutput, wAttributes);
	}

	void ShutCons()
	{
		if(INVALID_HANDLE_VALUE!=m_hOutput)
		{
			//FreeConsole();	//让他自然销毁吧
			m_hOutput = NULL;
		}
	}
	void OutputCons(wchar_t const *pMsg)
	{
		if(m_bAllocConsole)
		{
			DWORD charsWritten = 0;
			if(INVALID_HANDLE_VALUE!=m_hOutput)
				::WriteConsole( m_hOutput, pMsg, _tcslen(pMsg), &charsWritten, NULL);
		}
		else
			wprintf_s(_T("%s"), pMsg);		
	}
};
//管道
#define BUFSIZE		DEBUG_MESSAGE_BUFLEN_MAX

class CDbgPipe
{
public:
	std::basic_string<TCHAR> m_strPipeName;
	HANDLE	m_hPipe;
	CDbgPipe()
	{
		m_strPipeName = _T("DebugWindow");
		m_hPipe = INVALID_HANDLE_VALUE;
	}
	static VOID AcceptThread(LPVOID lpvParam)
	{
		CDbgPipe *pthis = (CDbgPipe*)lpvParam;
		pthis->Run();
	}
	VOID Run()
	{
		BOOL fConnected; 
		HANDLE hPipe; 
		std::basic_string<TCHAR> lpszPipename = _T("\\\\.\\pipe\\");
		lpszPipename += m_strPipeName;

		SECURITY_DESCRIPTOR sd;
		BOOL b = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);	

		// This is our SID for group "Everyone"
		BYTE sidEveryone[12] = {1,1,0,0,0,0,0,1,0,0,0,0};

		// Determine size for DACL, allocate and initialize it.
		DWORD dwSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + sizeof(sidEveryone) - sizeof(DWORD);
		PACL pDacl = (PACL) malloc(dwSize);
		b = InitializeAcl(pDacl, dwSize, ACL_REVISION);

		// Allow access for everyone
		b = AddAccessAllowedAce(pDacl, ACL_REVISION, GENERIC_ALL, sidEveryone);

		// Insert DACL into security descriptor
		b = SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);

		// Initialize SECURITY_ATTRIBUTES structure
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;

		for (;;) 
		{ 
			hPipe = CreateNamedPipe( 
				lpszPipename.c_str(),             // pipe name 
				PIPE_ACCESS_DUPLEX,       // read/write access 
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				BUFSIZE,                  // output buffer size 
				BUFSIZE,                  // input buffer size 
				NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
				&sa);                    // default security attribute 
			if (hPipe == INVALID_HANDLE_VALUE) 
			{
				_tprintf_s(_T("CreatePipe failed\n")); 
				return ;
			}

			_tprintf_s(_T("Try to connect pipe: %s...\n"), lpszPipename.c_str());

			fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
			if (fConnected) 
			{ 
				m_hPipe = hPipe;
				OutputPipe(m_strPipeName.c_str());
				OutputPipe(_T(" is opened!\n"));
			} 
			else 
				// The client could not connect, so close the pipe. 
				CloseHandle(hPipe); 
		}
		return ;
	}
	void InitPipe(int iReserved)
	{
		DWORD dwThreadId;
		HANDLE hThread = CreateThread( 
			NULL,              // no security attribute 
			0,                 // default stack size 
			(LPTHREAD_START_ROUTINE) AcceptThread, 
			(LPVOID) this,    // thread parameter 
			0,                 // not suspended 
			&dwThreadId);      // returns thread ID 

		if (hThread == NULL) 
		{
			_tprintf_s(_T("CreateThread failed: AcceptThread\n")); 
			return ;
		}
		else 
			CloseHandle(hThread); 
		Sleep(1000);
	}
	void ShutPipe()
	{
		if(INVALID_HANDLE_VALUE != m_hPipe) 
		{
			OutputPipe(m_strPipeName.c_str());
			OutputPipe(_T(" is closed!\n"));
			DisconnectNamedPipe(m_hPipe); 
			CloseHandle(m_hPipe); 
			m_hPipe = INVALID_HANDLE_VALUE;
		}
	}
	void OutputPipe(wchar_t const *pMsg)
	{
		DWORD dwInputLen = (wcslen(pMsg) + 1)*2;
		DWORD dwBytesWritten = 0;
		while((INVALID_HANDLE_VALUE != m_hPipe) && dwInputLen>0)
		{		
			if(!WriteFile(m_hPipe,	// Handle to the pipe
				((unsigned char *)pMsg)+dwBytesWritten,	// Pointer to data to write to file
				dwInputLen,			// Number of bytes to write
				&dwBytesWritten,	// Number of bytes written
				NULL))				// Pointer to OVERLAPPED structure
			{
				DisconnectNamedPipe(m_hPipe);
				m_hPipe = INVALID_HANDLE_VALUE;
				break;
			}
			
			dwInputLen -= dwBytesWritten;
		}
	}
};
//WM_COPYDATA
void InitWmcpy(int iReserved)
{

}
void ShutWmcpy()
{

}
void OutputWmcpy(wchar_t const *pMsg)
{

}
//网络TCP
void InitTcp(int iReserved)
{

}
void ShutTcp()
{

}
void OutputTcp(wchar_t const *pMsg)
{

}

#define MAX_STDLOGFILE_SIZE		(512*1024)
//文件日志输出
class CDbgFile
{
public:
	DWORD	m_logFileMaxSize;
	std::basic_string<TCHAR> m_strFullFileName;
	CDbgFile()
	{
		m_logFileMaxSize = MAX_STDLOGFILE_SIZE;
		m_strFullFileName = _T("output.log");
	}
	void InitFile(int iReserved)
	{
	// 	errno_t err;
	// 	err = _wfreopen_s(&s_flog, m_strFullFileName.c_str(), _T("a+"),	stderr);
	// 	if( err != 0 )
	// 		fprintf( stderr, "error on freopen\n" );
	}
	void ShutFile()
	{
	// 	if(s_flog)
	// 	{
	// 		fclose(s_flog);
	// 		s_flog = NULL;
	// 	}
	}
	void OutputFile(wchar_t const *pMsg)
	{
		// 打开文件
		HANDLE hLogFile = CreateFile( 
			m_strFullFileName.c_str() , 
			GENERIC_WRITE | GENERIC_READ, 
			FILE_SHARE_READ | FILE_SHARE_WRITE , 
			NULL , 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL,
			NULL );
		if( hLogFile == INVALID_HANDLE_VALUE )
			return ;

		//增加对日志文件大小限制的处理 add by ysb 2011-11-21//////////////////////
		DWORD dwSize = GetFileSize(hLogFile, NULL);
		if (dwSize > m_logFileMaxSize && dwSize != INVALID_FILE_SIZE)
		{
			std::basic_string<TCHAR> strBakFile = m_strFullFileName;
			int sFind = strBakFile.rfind(_T('.'));
			if(sFind>0)
			{
				CloseHandle(hLogFile);
				strBakFile.insert(sFind, _T("2")); //在原文件名上面加2做为备份文件
				SetFileAttributes(strBakFile.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(strBakFile.c_str());
				MoveFile(m_strFullFileName.c_str(), strBakFile.c_str());
				hLogFile = CreateFile(m_strFullFileName.c_str(), GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL, NULL);
				if( hLogFile == INVALID_HANDLE_VALUE )
					return ;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// 设置文件位置
		SetFilePointer( hLogFile , 0L , NULL , FILE_END );

		// 输出日志到文件
		DWORD dwBytesWritten = 0;
		
		//貌似以unicode写进去log很难看，所以转成窄的
		USES_CONVERSION_EX;
		string pm = W2A_EX(pMsg, 256);
		pm+="\r\n";
		WriteFile( hLogFile , pm.c_str() , pm.length() , &dwBytesWritten , NULL );
		//FlushFileBuffers(  hLogFile );//处于速度考虑这里注释掉

		CloseHandle( hLogFile );
	// 	if(s_flog)
	// 	{
	// 		fwprintf_s(s_flog, pMsg);	
	// 	}
	}
};

// PDB调试堆栈打印
void InitSym(int iReserved)
{
//	EXTENDEDTRACEINITIALIZE(_T("D:\\svn\\appproduct2010\\utm\\bin;D:\\svn\\appproduct2010\\utm\\src\\pubcomm\\stdlog\\testLog\\Debug"));
}
void ShutSym()
{
//	EXTENDEDTRACEUNINITIALIZE();
}
void OutputSym(wchar_t const *pMsg)
{
//	STACKTRACE();
}
//日志管理类

class CLog;
static CLog  * s_log = NULL;

class CLog
{
public:
	CRITICAL_SECTION	m_csLock;

	CDbgCons m_cCons;
	CDbgPipe m_cPipe;
	CDbgFile m_cFile;

	void LogLock()
	{
		EnterCriticalSection(&m_csLock);
	}

	void LogUnlock()
	{
		LeaveCriticalSection(&m_csLock);
	}

	CLog()
	{
		bTime = bTid = bPid = bDbgstr = bCons = bPipe = bWmcpy = bTcp = bFile = bFilei = bSym = false;
		m_bLock = false;
	}

	void Init(int iReserved)
	{
		if(m_bLock)
		{
			InitializeCriticalSection(&this->m_csLock);
		}

		if (bCons)
		{
			m_cCons.InitCons(iReserved);
			
		}
		if (bPipe)
		{
			m_cPipe.InitPipe(iReserved);
		}
		if (bFile)
		{
			m_cFile.InitFile(iReserved);
		}

		if (bWmcpy)
		{
			InitWmcpy(iReserved);
		}
		if (bTcp)
		{
			InitTcp(iReserved);
		}
		if (bSym)
		{
			InitSym(iReserved);
		}
	}

	bool AllIsNotSet()
	{
		return (!bDbgstr && !bCons && !bPipe && !bWmcpy && !bTcp && !bSym);
	}

	void Shut()
	{
		m_cPipe.ShutPipe();
		m_cCons.ShutCons();
		m_cFile.ShutFile();
		ShutSym();
		ShutTcp();
		ShutWmcpy();

		if(m_bLock)
		{
			DeleteCriticalSection(&this->m_csLock);
		}
	}
public:
	static DbgRetval_t DbgOutputA( DbgType_t spewType, char const *pMsg )
	{
		USES_CONVERSION_EX;
		return DbgOutputW(spewType, A2W_EX(pMsg, 256));
	}
	static DbgRetval_t DbgOutputW( DbgType_t spewType, wchar_t const *pMsg )
	{
		TCHAR szExt[128]={0};
		if(s_log->bTime)
		{
			struct tm tmTemp;
			time_t t = time(NULL);
			localtime_s(&tmTemp, &t);
			if (!_tcsftime(szExt, ARRAYSIZE(szExt), _T("%Y-%m-%d %H:%M:%S"), &tmTemp))
				OutputDebugStringW(L"\nstrftime error???\n");
		}
		int len = _tcslen(szExt);
		if(s_log->bPid)
		{
			len += swprintf_s( szExt+len, 128-len, _T("[P:%d]"), GetCurrentProcessId());
		}
		if(s_log->bTid)
		{
			len += swprintf_s( szExt+len, 128-len, _T("[T:%d]"), GetCurrentThreadId());
		}

		std::basic_string<TCHAR> strBuff = szExt;
		strBuff+=pMsg;
		std::basic_string<TCHAR> strMsg = strBuff + _T("\n");		
		if(s_log->m_bLock) s_log->LogLock();
		LPCTSTR pszMsg = strMsg.c_str();

		//modify by magj 无论何时都向dbgview打出日志，方便查看
		//if (s_log->bDbgstr /*|| s_log->AllIsNotSet()*/)
		//{			
		OutputDebugStringW(pszMsg);
		//}

		//简单的调试器输出
		//modify by magj 如果没有任何设置，不做任何处理
		if (s_log->AllIsNotSet())
		{
			return DBG_RETURNED_CONTINUES;
		}

		

		if (s_log->bCons)
		{		
			//设置控制台字体颜色
			s_log->m_cCons.SetDbgType(spewType);
			s_log->m_cCons.OutputCons(pszMsg);
		}
		if (s_log->bPipe)
		{		
			s_log->m_cPipe.OutputPipe(pszMsg);
		}
		if (s_log->bWmcpy)
		{
			OutputWmcpy(pszMsg);
		}
		if (s_log->bTcp)
		{
			OutputTcp(pszMsg);
		}
		if (s_log->bFile)
		{		
			s_log->m_cFile.OutputFile(strBuff.c_str());
		}
		if(s_log->bFilei)
		{
			strMsg+=_T("\r\n");
			s_log->m_cFile.InitFile(0);			
			s_log->m_cFile.OutputFile(strBuff.c_str());
			s_log->m_cFile.ShutFile();
		}


		if(s_log->m_bLock) s_log->LogUnlock();

		// 对spewType默认的处理方法
		if( spewType == DBG_ASSERT )
		{
			if(s_log->bSym)
				OutputSym(pMsg);
			return DBG_RETURNED_DEBUGGER;
		}
		else if( spewType == DBG_ERROR )
		{
			if(s_log->bSym)
				OutputSym(pMsg);
			return DBG_RETURNED_ABORT;
		}
		else
			return DBG_RETURNED_CONTINUES;
	}

	bool bDbgstr;
	bool bCons;
	bool bPipe;
	bool bWmcpy;
	bool bTcp;
	bool bFile;
	bool bFileSize;
	bool bFilei;	//立即记录文件
	bool bSym;

	bool bTid;
	bool bPid;
	bool bTime;

	//锁定单一输出
	bool				m_bLock;
};


#include <shellapi.h>
extern "C" STDLOG_API  void InitDumpLib(int iReserved/* = 0*/, HMODULE hModule /*= 0*/)
{
	static bool bInit = false;
	if (bInit)
	{
		return;
	}

	if (!bInit)
	{
		s_log = new CLog();
		bInit = true;
	}

	//add by bob 2011-8-10
	//考虑服务里使用，不能传递命令行，故同时考虑用配置文件

	//获取好路径信息
	TCHAR szBuf[MAX_PATH*2];
	::GetModuleFileName(hModule, szBuf, MAX_PATH*2);
	if(lstrlen(szBuf) <= 0)
		return;

	std::basic_string<TCHAR> strRunPath;
	std::basic_string<TCHAR> strProcessName;

	std::basic_string<TCHAR> strBuf = szBuf;
	size_t npos = strBuf.rfind(_T('\\'));
	strRunPath = strBuf.substr(0, npos);

	std::basic_string<TCHAR> strFullName = strBuf.substr(npos + 1);
	npos = strFullName.rfind(_T('.'));
	if(npos >= 0)
		strProcessName = strFullName.substr(0, npos);
	else
		strProcessName = strFullName;

	LPWSTR lpCommandLine = GetCommandLineW();
	
	//尝试读配置文件里的
	std::basic_string<TCHAR> strCfgFile = strRunPath + _T("\\stdlog.ini");
	
	WCHAR szCommandLine[MAX_PATH*2];
	GetPrivateProfileStringW(_T("CmdLine"), strProcessName.c_str(), NULL, szCommandLine, MAX_PATH*2, strCfgFile.c_str());

	if(_tcslen(szCommandLine) > 0 )
	{
		std::basic_string<TCHAR> strCommandLine = lpCommandLine;
		strCommandLine += _T(" ");
		strCommandLine += szCommandLine;
		_tcscpy_s(szCommandLine, MAX_PATH*2, strCommandLine.c_str());

		lpCommandLine = szCommandLine;
	}

	//////////////////////////////////////////////////////////////////////////
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(lpCommandLine, &nArgs);
	if( NULL == szArglist )
	{
		wprintf(L"CommandLineToArgvW failed\n");
	}

	list< std::basic_string<TCHAR> > lstCmdLine;
	list< std::basic_string<TCHAR> >::iterator g;

	for( int i=0; i<nArgs; i++ )
		lstCmdLine.push_back( std::basic_string<TCHAR>( szArglist[i] ) );

	s_log->bDbgstr = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-dbgstr") ) != lstCmdLine.end() );
	s_log->bCons = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-cons") ) != lstCmdLine.end() );
	s_log->bPipe = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-pipe") ) != lstCmdLine.end() );
	s_log->bWmcpy = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-wmcpy") ) != lstCmdLine.end() );
	s_log->bTcp = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-tcp") ) != lstCmdLine.end() );
	s_log->bFile = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-file") ) != lstCmdLine.end() );
	s_log->bFileSize = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-filesize") ) != lstCmdLine.end() );
	s_log->bFilei = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-filei") ) != lstCmdLine.end() );
	s_log->bSym = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-pdb") ) != lstCmdLine.end() );
	s_log->bTid = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-tid") ) != lstCmdLine.end() );
	s_log->bPid = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-pid") ) != lstCmdLine.end() );
	s_log->bTime = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-time") ) != lstCmdLine.end() );
	s_log->m_bLock = ( find( lstCmdLine.begin(), lstCmdLine.end(), _T("-mt") ) != lstCmdLine.end() );

	//hook一下
	DbgOutputFuncA(CLog::DbgOutputA);
	DbgOutputFuncW(CLog::DbgOutputW);

	//先解析一些附加参数
	s_log->m_cFile.m_strFullFileName = std::basic_string<TCHAR>(szArglist[0]) + _T(".log");	// 默认的日志名字
	if(s_log->bFile)
	{
		g = find( lstCmdLine.begin(), lstCmdLine.end(), _T("-file") );
		if(++g != lstCmdLine.end() )
		{
			if((*g)[0] != _T('-'))
			{
				s_log->m_cFile.m_strFullFileName = *g;	//自定义日志名称
			}
		}
	}

	if(s_log->bFileSize)
	{
		g = find( lstCmdLine.begin(), lstCmdLine.end(), _T("-filesize") );
		if(++g != lstCmdLine.end() )
		{
			if((*g)[0] != _T('-'))
			{
				//设置日志文件的容量最大值
				s_log->m_cFile.m_logFileMaxSize = _ttol((*g).c_str()) * 1024; //配置里带的单位是K
			}
		}
	}

	if(s_log->bFilei)
	{
		g = find( lstCmdLine.begin(), lstCmdLine.end(), _T("-filei") );
		if(++g != lstCmdLine.end() )
		{
			if((*g)[0] != _T('-'))
			{
				s_log->m_cFile.m_strFullFileName = *g;	//自定义日志名称
			}
		}
	}

	//
	if(s_log->bPipe)
	{
		g = find( lstCmdLine.begin(), lstCmdLine.end(), _T("-pipe") );
		if(++g != lstCmdLine.end() )
		{
			if((*g)[0] != _T('-'))
			{
				s_log->m_cPipe.m_strPipeName = *g;	//自定义管道名称
			}
		}
	}

	//然后分别初始化
	s_log->Init(iReserved);

	//最后调整一下日志输出级别
	g = find( lstCmdLine.begin(), lstCmdLine.end(), _T("-group") );
	while(g != lstCmdLine.end() )
	{
		if(++g == lstCmdLine.end() )
			break;

		std::basic_string<TCHAR> group = *g++;
		if(group[0] == _T('-'))
			break;

		if(g != lstCmdLine.end() )
		{
			std::basic_string<TCHAR> level = *g;
			if(level[0]<_T('0') && level[0]>_T('9'))
				break;

			DbgActivate(group.c_str(), _ttoi(level.c_str()));
		}		
	}	
}

extern "C" STDLOG_API  void	 TermDumpLib()
{
	if(!s_log) return ;

	CLog* p = s_log;
	s_log = NULL;

	DbgOutputFuncA(NULL);
	DbgOutputFuncW(NULL);	

	p->Shut();
	delete p;

	
}