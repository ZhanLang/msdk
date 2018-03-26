// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <Winuser.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的


#define  _WTL_NO_CSTRING		
#include <atlbase.h>
#include <atlstr.h>	
#include <atlapp.h>

#include "mscom/mscominc.h"

extern CAppModule Module;

static CString FormatUINT64(UINT64 uvalue)
{
	CString strRet;
	_i64tot_s(uvalue,strRet.GetBufferSetLength(MAX_PATH),MAX_PATH,10);
	strRet.ReleaseBuffer();
	return strRet;
}

static UINT64 FileTimeToUINT64(const FILETIME& time)
{
	LARGE_INTEGER l;
	l.LowPart = time.dwLowDateTime;
	l.HighPart = time.dwHighDateTime;
	return l.QuadPart;
}

static FILETIME UINT64ToFileTime(const UINT64& val64)
{
	FILETIME time = {0};
	LARGE_INTEGER l;
	l.QuadPart = val64;

	time.dwHighDateTime = l.HighPart;
	time.dwLowDateTime =  l.LowPart;
	return time;
}

static UINT64 GetCurrentFileTime()
{

	FILETIME   createFileTime = {0};
	SYSTEMTIME createSysTime =  {0};
	GetLocalTime(&createSysTime);
	SystemTimeToFileTime(&createSysTime,&createFileTime);
	return  FileTimeToUINT64(createFileTime);
}


static CString FormatFileTimeToStr(UINT64 uFileTime ,BOOL bYears = TRUE)
{
	FILETIME fileTime = UINT64ToFileTime(uFileTime);

	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&fileTime,&sysTime);

	TCHAR str[100] = {0};
	if (bYears)
	{
		_stprintf(str,_T("%4d-%02d-%02d %2d:%2d:%2d"), 
			sysTime.wYear,  
			sysTime.wMonth, 
			sysTime.wDay, 
			sysTime.wHour, 
			sysTime.wMinute,
			sysTime.wSecond);//将时间格式化 
	}
	
	else
	{
		_stprintf(str,_T("%2d:%2d:%2d"), 
			sysTime.wHour, 
			sysTime.wMinute,
			sysTime.wSecond);//将时间格式化 
	}

	CString strTime = str;
	return strTime;
}