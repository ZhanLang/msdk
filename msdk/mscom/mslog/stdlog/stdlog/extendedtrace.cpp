//////////////////////////////////////////////////////////////////////////////////////
//
// Written by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
// The source is free, but if you want to use it, mention my name and e-mail address
//
// History:
//    1.0      Initial version                  Zoltan Csizmadia
//
//////////////////////////////////////////////////////////////////////////////////////
//
// ExtendedTrace.cpp
//

// Include StdAfx.h, if you're using precompiled 
// header through StdAfx.h
#include "stdafx.h"

#if defined(_DEBUG) && defined(WIN32)

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <atlconv.h>

#include "mslog/syslay/dbg.h"


#include "cdbghelp.h"

#include "ExtendedTrace.h"


#define BUFFERSIZE   0x200

CMoudleDbghelp s_dbghelp;

// Unicode safe char* -> TCHAR* conversion
void PCSTR2LPTSTR( PCSTR lpszIn, LPTSTR lpszOut )
{
	USES_CONVERSION;
	_tcscpy_s(lpszOut, _MAX_PATH, A2T(lpszIn));
}

// Let's figure out the path for the symbol files
// Search path= ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;" + lpszIniPath
// Note: There is no size check for lpszSymbolPath!
void InitSymbolPath( PTSTR lpszSymbolPath, PCTSTR lpszIniPath )
{
	TCHAR lpszPath[BUFFERSIZE]={0};

   // Creating the default path
   // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	int len = wcscpy_s( lpszSymbolPath, BUFFERSIZE, _T(".") );
	len++;
	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable( _T("_NT_SYMBOL_PATH"), lpszPath, BUFFERSIZE ) )
	{
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T(";") );
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, lpszPath );
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable( _T("_NT_ALTERNATE_SYMBOL_PATH"), lpszPath, BUFFERSIZE ) )
	{
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T(";") );
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, lpszPath );
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariable( _T("SYSTEMROOT"), lpszPath, BUFFERSIZE ) )
	{
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T(";") );
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, lpszPath );
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T(";") );

		// SYSTEMROOT\System32
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, lpszPath );
		len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T("\\System32") );
	}

   // Add user defined path
	if ( lpszIniPath != NULL )
		if ( lpszIniPath[0] != '\0' )
		{
			len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, _T(";") );
			len += swprintf_s( lpszSymbolPath+len, BUFFERSIZE-len, lpszIniPath );
		}
}

// Uninitialize the loaded symbol files
BOOL UninitSymInfo()
{
	return s_dbghelp.SymCleanup( GetCurrentProcess() );
}

// Initializes the symbol files
BOOL InitSymInfo( PCTSTR lpszInitialSymbolPath )
{
	if(0!=s_dbghelp.Load()) return FALSE;
	
	TCHAR     lpszSymbolPath[BUFFERSIZE];
	DWORD    symOptions = s_dbghelp.SymGetOptions();

	symOptions |= SYMOPT_LOAD_LINES; 
	symOptions &= ~SYMOPT_UNDNAME;
	s_dbghelp.SymSetOptions( symOptions );

   // Get the search path for the symbol files
	InitSymbolPath( lpszSymbolPath, lpszInitialSymbolPath );

	USES_CONVERSION;
	return s_dbghelp.SymInitialize( GetCurrentProcess(), T2A(lpszSymbolPath), TRUE);
}

// Get the module name from a given address
BOOL GetModuleNameFromAddress( UINT address, LPWSTR lpszModule )
{
	BOOL              ret = FALSE;
	IMAGEHLP_MODULE   moduleInfo;

	::ZeroMemory( &moduleInfo, sizeof(moduleInfo) );
	moduleInfo.SizeOfStruct = sizeof(moduleInfo);

	if ( s_dbghelp.SymGetModuleInfo( GetCurrentProcess(), (DWORD)address, &moduleInfo ) )
	{	   
		//return wcscpy_s(lpszModule, 32, moduleInfo.ModuleName)==0;
		PCSTR2LPTSTR( moduleInfo.ModuleName, lpszModule );
		ret = TRUE;
	}
	else
	   // Not found :(
		_tcscpy_s( lpszModule, 32, _T("?") );
	
	return ret;
}

// Get function prototype and parameter info from ip address and stack address
BOOL GetFunctionInfoFromAddresses( ULONG fnAddress, ULONG stackAddress, LPTSTR lpszSymbol )
{
	BOOL              ret = FALSE;
	DWORD             dwDisp = 0;
	DWORD             dwSymSize = 10000;
	TCHAR             lpszUnDSymbol[BUFFERSIZE]=_T("?");
	CHAR              lpszNonUnicodeUnDSymbol[BUFFERSIZE]="?";
	LPTSTR            lpszParamSep = NULL;
	LPCTSTR           lpszParsed = lpszUnDSymbol;
	PIMAGEHLP_SYMBOL  pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc( GMEM_FIXED, dwSymSize );

	::ZeroMemory( pSym, dwSymSize );
	pSym->SizeOfStruct = dwSymSize;
	pSym->MaxNameLength = dwSymSize - sizeof(IMAGEHLP_SYMBOL);

   // Set the default to unknown
	_tcscpy_s( lpszSymbol, BUFFERSIZE, _T("?") );

	// Get symbol info for IP
	if ( s_dbghelp.SymGetSymFromAddr( GetCurrentProcess(), (ULONG)fnAddress, &dwDisp, pSym ) )
	{
	   // Make the symbol readable for humans
		s_dbghelp.UnDecorateSymbolName( pSym->Name, lpszNonUnicodeUnDSymbol, BUFFERSIZE, 
			UNDNAME_COMPLETE | 
			UNDNAME_NO_THISTYPE |
			UNDNAME_NO_SPECIAL_SYMS |
			UNDNAME_NO_MEMBER_TYPE |
			UNDNAME_NO_MS_KEYWORDS |
			UNDNAME_NO_ACCESS_SPECIFIERS );

      // Symbol information is ANSI string
		PCSTR2LPTSTR( lpszNonUnicodeUnDSymbol, lpszUnDSymbol );

      // I am just smarter than the symbol file :)
		if ( _tcscmp(lpszUnDSymbol, _T("_WinMain@16")) == 0 )
			_tcscpy_s(lpszUnDSymbol, BUFFERSIZE, _T("WinMain(HINSTANCE,HINSTANCE,LPCTSTR,int)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_main")) == 0 )
			_tcscpy_s(lpszUnDSymbol, BUFFERSIZE, _T("main(int,TCHAR * *)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_mainCRTStartup")) == 0 )
			_tcscpy_s(lpszUnDSymbol, BUFFERSIZE, _T("mainCRTStartup()"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_wmain")) == 0 )
			_tcscpy_s(lpszUnDSymbol, BUFFERSIZE, _T("wmain(int,TCHAR * *,TCHAR * *)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_wmainCRTStartup")) == 0 )
			_tcscpy_s(lpszUnDSymbol, BUFFERSIZE, _T("wmainCRTStartup()"));

		lpszSymbol[0] = _T('\0');

      // Let's go through the stack, and modify the function prototype, and insert the actual
      // parameter values from the stack
		if ( _tcsstr( lpszUnDSymbol, _T("(void)") ) == NULL && _tcsstr( lpszUnDSymbol, _T("()") ) == NULL)
		{
			ULONG index = 0;
			for( ; ; index++ )
			{
				lpszParamSep = (LPTSTR)_tcschr( lpszParsed, _T(',') );
				if ( lpszParamSep == NULL )
					break;

				*lpszParamSep = _T('\0');

				_tcscat_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), lpszParsed );
				_stprintf_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), _T("=0x%08X,"), *((ULONG*)(stackAddress) + 2 + index) );

				lpszParsed = lpszParamSep + 1;
			}

			lpszParamSep = (LPTSTR)_tcschr( lpszParsed, _T(')') );
			if ( lpszParamSep != NULL )
			{
				*lpszParamSep = _T('\0');

				_tcscat_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), lpszParsed );
				_stprintf_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), _T("=0x%08X)"), *((ULONG*)(stackAddress) + 2 + index) );

				lpszParsed = lpszParamSep + 1;
			}
		}

		_tcscat_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), lpszParsed );
   
		ret = TRUE;
	}
	else
	{
		_stprintf_s( lpszSymbol + _tcslen(lpszSymbol), BUFFERSIZE -_tcslen(lpszSymbol), _T("ret 0x%08X arg1=0x%08X)"), fnAddress, *((ULONG*)(stackAddress) + 2) );		
	}

	GlobalFree( pSym );

	return ret;
}

// Get source file name and line number from IP address
// The output format is: "sourcefile(linenumber)" or
//                       "modulename!address" or
//                       "address"
BOOL GetSourceInfoFromAddress( UINT address, LPTSTR lpszSourceInfo )
{
	BOOL           ret = FALSE;
	IMAGEHLP_LINE  lineInfo;
	DWORD          dwDisp;
	TCHAR          lpszFileName[BUFFERSIZE] = _T("");
	TCHAR          lpModuleInfo[BUFFERSIZE] = _T("");

	_tcscpy_s( lpszSourceInfo, BUFFERSIZE,  _T("?(?)") );

	::ZeroMemory( &lineInfo, sizeof( lineInfo ) );
	lineInfo.SizeOfStruct = sizeof( lineInfo );

	if ( s_dbghelp.SymGetLineFromAddr( GetCurrentProcess(), address, &dwDisp, &lineInfo ) )
	{
	   // Got it. Let's use "sourcefile(linenumber)" format
		PCSTR2LPTSTR( lineInfo.FileName, lpszFileName );		
		ret = TRUE;
	}
	else
	{
      // There is no source file information. :(
      // Let's use the "modulename!address" format
	  	GetModuleNameFromAddress( address, lpModuleInfo );

		if ( lpModuleInfo[0] == _T('?') || lpModuleInfo[0] == _T('\0'))
		   // There is no modulename information. :((
         // Let's use the "address" format
			_stprintf_s( lpszSourceInfo, BUFFERSIZE, _T("?") );
		else
			_stprintf_s( lpszSourceInfo, BUFFERSIZE, _T("%s"), lpModuleInfo );

		ret = FALSE;
	}
	
	return ret;
}

// TRACE message with source link. 
// The format is: sourcefile(linenumber) : message
void SrcLinkTrace( LPCTSTR lpszMessage, LPCTSTR lpszFileName, ULONG nLineNumber )
{
	Msg(	_T("%s(%d) : %s"), 
			lpszFileName, 
			nLineNumber, 
			lpszMessage );
}

void StackTrace( HANDLE hThread, LPCTSTR lpszMessage )
{
	STACKFRAME     callStack;
	BOOL           bResult;
	CONTEXT        context;
	TCHAR          symInfo[BUFFERSIZE] = _T("?");
	TCHAR          srcInfo[BUFFERSIZE] = _T("?");
	HANDLE         hProcess = GetCurrentProcess();

   // If it's not this thread, let's suspend it, and resume it at the end
	if ( hThread != GetCurrentThread() )
		if ( SuspendThread( hThread ) == -1 )
		{
		   // whaaat ?!
		   Msg( _T("Call stack info(thread=0x%X) failed.\n") );
			return;
		}

	::ZeroMemory( &context, sizeof(context) );
	context.ContextFlags = CONTEXT_FULL;

	if ( !GetThreadContext( hThread, &context ) )
	{
      Msg( _T("Call stack info(thread=0x%X) failed.\n") );
	   return;
	}
	
	::ZeroMemory( &callStack, sizeof(callStack) );
	callStack.AddrPC.Offset    = context.Eip;
	callStack.AddrStack.Offset = context.Esp;
	callStack.AddrFrame.Offset = context.Ebp;
	callStack.AddrPC.Mode      = AddrModeFlat;
	callStack.AddrStack.Mode   = AddrModeFlat;
	callStack.AddrFrame.Mode   = AddrModeFlat;

	for( ULONG index = 0; ; index++ ) 
	{
		bResult = s_dbghelp.StackWalk(
			IMAGE_FILE_MACHINE_I386,
			hProcess,
			hThread,
			&callStack,
			NULL, 
			NULL,
			s_dbghelp.SymFunctionTableAccess,
			s_dbghelp.SymGetModuleBase,
			NULL);

		if ( index == 0 )
		   continue;

		if( !bResult || callStack.AddrFrame.Offset == 0 ) 
			break;
	
		GetFunctionInfoFromAddresses( callStack.AddrPC.Offset, callStack.AddrFrame.Offset, symInfo );
		GetSourceInfoFromAddress( callStack.AddrPC.Offset, srcInfo );

		Msg( _T("     %s : %s\n"), srcInfo, symInfo );
	}

	if ( hThread != GetCurrentThread() )
		ResumeThread( hThread );
}

void FunctionParameterInfo()
{
	STACKFRAME     callStack;
	BOOL           bResult = FALSE;
	CONTEXT        context;
	TCHAR          lpszFnInfo[BUFFERSIZE];
	HANDLE         hProcess = GetCurrentProcess();
	HANDLE         hThread = GetCurrentThread();

	::ZeroMemory( &context, sizeof(context) );
	context.ContextFlags = CONTEXT_FULL;

	if ( !GetThreadContext( hThread, &context ) )
	{
	   Msg( _T("Function info(thread=0x%X) failed.\n") );
		return;
	}
	
	::ZeroMemory( &callStack, sizeof(callStack) );
	callStack.AddrPC.Offset    = context.Eip;
	callStack.AddrStack.Offset = context.Esp;
	callStack.AddrFrame.Offset = context.Ebp;
	callStack.AddrPC.Mode      = AddrModeFlat;
	callStack.AddrStack.Mode   = AddrModeFlat;
	callStack.AddrFrame.Mode   = AddrModeFlat;

	for( ULONG index = 0; index < 2; index++ ) 
	{
		bResult = s_dbghelp.StackWalk(
			IMAGE_FILE_MACHINE_I386,
			hProcess,
			hThread,
			&callStack,
			NULL, 
			NULL,
			s_dbghelp.SymFunctionTableAccess,
			s_dbghelp.SymGetModuleBase,
			NULL);
	}

	if ( bResult && callStack.AddrFrame.Offset != 0) 
	{
	   GetFunctionInfoFromAddresses( callStack.AddrPC.Offset, callStack.AddrFrame.Offset, lpszFnInfo );
	   Msg( _T("Function info(thread=0x%X) : %s\n"), GetCurrentThreadId(), lpszFnInfo );
	}
	else
	   Msg( _T("Function info(thread=0x%X) failed.\n") );
}

#endif //_DEBUG && WIN32

