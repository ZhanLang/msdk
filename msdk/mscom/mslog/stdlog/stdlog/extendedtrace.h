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
// ExtendedTrace.h
//

#ifndef EXTENDEDTRACE_H_INCLUDED
#define EXTENDEDTRACE_H_INCLUDED

#if defined(_DEBUG) && defined(WIN32)

#include <windows.h>
#include <tchar.h>
#include "mslog/syslay/dbg.h"

//#define DBGHELP_TRANSLATE_TCHAR					//for 64bit unicode call
//#include <ImageHlp.h>
//#pragma comment( lib, "imagehlp.lib" )


#if defined(_AFX) || defined(_AFXDLL)
#define TRACEF									         TRACE
#else
#define TRACEF									         OutputDebugStringFormat
void OutputDebugStringFormat( LPCTSTR, ... );
#endif


#define EXTENDEDTRACEINITIALIZE( IniSymbolPath )		InitSymInfo( IniSymbolPath )
#define EXTENDEDTRACEUNINITIALIZE()						UninitSymInfo()
#define SRCLINKTRACECUSTOM( msg, File, Line)			SrcLinkTrace( msg, File, Line )
#define SRCLINKTRACE( msg )								SrcLinkTrace( msg, __TFILE__, __LINE__ )
#define FNPARAMTRACE()							        FunctionParameterInfo()
#define STACKTRACEMSG( msg )					        StackTrace( msg )
#define STACKTRACE()							        StackTrace( GetCurrentThread(), _T("") )
#define THREADSTACKTRACEMSG( hThread, msg )				StackTrace( hThread, msg )
#define THREADSTACKTRACE( hThread )						StackTrace( hThread, _T("") )

BOOL InitSymInfo( PCTSTR );
BOOL UninitSymInfo();
void SrcLinkTrace( LPCTSTR, LPCTSTR, ULONG );
void StackTrace( HANDLE, LPCTSTR );
void FunctionParameterInfo();

#else

#define EXTENDEDTRACEINITIALIZE( IniSymbolPath )		((void)0)
#define EXTENDEDTRACEUNINITIALIZE()						((void)0)
#define TRACEF									        ((void)0)
#define SRCLINKTRACECUSTOM( msg, File, Line)			((void)0)
#define SRCLINKTRACE( msg )								((void)0)
#define FNPARAMTRACE()							        ((void)0)
#define STACKTRACEMSG( msg )					        ((void)0)
#define STACKTRACE()						         	((void)0)
#define THREADSTACKTRACEMSG( hThread, msg )				((void)0)
#define THREADSTACKTRACE( hThread )						((void)0)

#endif

#endif