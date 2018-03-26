// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define  _WTL_NO_CSTRING		//不使用WTL的CString
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

#include "mscom/mscominc.h"
using namespace mscom;

#include <mslog/stdlog_s.h>
#define GroupName _T("softeng")
#define MODULE_NAME GroupName
#include "mslog/msdkoutput.h"
#include "logtool.h"



extern HINSTANCE g_hInst;
extern TCHAR	 g_PhysicalDrive;	//实际的操作系统盘符
extern CString	 g_BkDrive;
#include "PeApi.h"
#include "util/registry.h"



static LPTSTR GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize)
{
	LPTSTR lpszTemp = 0;
	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		0,
		GetLastError(),
		LANG_NEUTRAL, //默认的语言种类
		(LPTSTR)&lpszTemp,
		0,
		0
		);

	if (!dwRet || (dwSize < dwRet + 14))
		lpszBuf[0] = TEXT('\0');
	else {
		lpszTemp[_tcsclen(lpszTemp) - 2] = TEXT('\0');  //remove cr/nl characters
		_tcscpy_s(lpszBuf, dwSize, lpszTemp);
	}

	if (lpszTemp)
		LocalFree(HLOCAL(lpszTemp));

	return lpszBuf;
}

static TCHAR GetRemovableDosDrive()
{
	DWORD unitmask = GetLogicalDrives();
	for( int j = 2; j  < 32; j ++ )
	{
		DWORD dwBit = 0x01 << j ;
		if( !(dwBit & unitmask) ){
			continue;
		}

		TCHAR szRootPath[] = _T("?:"); 
		szRootPath[0] = _T('A') + j;

		if ( GetDriveType(szRootPath) == DRIVE_REMOVABLE )
		{
			return  _T('A') + j;
		}
	}

	return 0;
}