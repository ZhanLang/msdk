// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <mscom/mscominc.h>
#include <filemonitor/IFileMonitor.h>
#include <mslog/stdlog_dll.h>

#define GroupName _T("filemonitor")
#include "logtool.h"

static LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
	LPTSTR lpszTemp = 0;
	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		0,
		GetLastError(),
		LANG_NEUTRAL, //默认的语言种类
		(LPTSTR)&lpszTemp,
		0,
		0
		);

	if( !dwRet || (dwSize < dwRet+14) )
		lpszBuf[0] = TEXT('\0');
	else {
		lpszTemp[_tcsclen(lpszTemp)-2] = TEXT('\0');  //remove cr/nl characters
		_tcscpy_s(lpszBuf, dwSize, lpszTemp);
	}

	if( lpszTemp )
		LocalFree(HLOCAL(lpszTemp));

	return lpszBuf;
}
// TODO: 在此处引用程序需要的其他头文件

#define MODULE_NAME _T("filemonitor")
class CFuncTime
{
public:

	static LPCTSTR GetCurrentFileTime(LPTSTR lpszTime, DWORD dwLen)
	{
		SYSTEMTIME systemTime = { 0 };
		GetLocalTime(&systemTime);
		_stprintf_s(lpszTime, dwLen, _T("%02d:%02d:%02d:%03d"),
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds);

		return lpszTime;
	}

	CFuncTime(const TCHAR* lpFuncName,const TCHAR* lpFile)
	{
		m_dwThreadID = GetCurrentThreadId();

		_tcscpy_s(m_strFunName , MAX_PATH, lpFuncName);
		_tcscpy_s(m_strFileName , MAX_PATH, lpFile ? lpFile:L"");
		m_dwBegin = GetTickCount();

		TCHAR strMsg[1024] = {0};
		TCHAR szTime[MAX_PATH] = {0};
		int len = _stprintf_s( strMsg, 1024,L"%s [%d] [%s] +[%s] <-- %s\n",GetCurrentFileTime(szTime, MAX_PATH),m_dwThreadID, MODULE_NAME,m_strFunName,m_strFileName);
		OutputDebugString(strMsg);
	}
	~CFuncTime()
	{
		m_dwEnd = GetTickCount();
		UINT sec = (m_dwEnd-m_dwBegin)/1000;
		LPCWSTR wLev = L"SPD:Normal  ";
		if( sec == 0 ) wLev = L"SPD:Fast    ";
		if( sec > 5 ) wLev = L"SPD:Slow    ";
		if( sec > 10 ) wLev = L"SPD:VerySlow";

		TCHAR strMsg[1024] = {0};
		TCHAR szTime[MAX_PATH] = {0};
		int len = _stprintf_s( strMsg, 1024,L"%s [%d] [%s] -[%s] -->%s | %s %d\n",GetCurrentFileTime(szTime, MAX_PATH),m_dwThreadID,MODULE_NAME,m_strFunName,m_strFileName,wLev,m_dwEnd-m_dwBegin);
		OutputDebugString(strMsg);
	}
private:
	TCHAR m_strFunName[MAX_PATH];
	TCHAR m_strFileName[MAX_PATH];
	DWORD    m_dwBegin;
	DWORD    m_dwEnd;
	DWORD    m_dwThreadID;
};

#define FUNC_TIME(x,y)  CFuncTime _objFuncTimes(x,y)