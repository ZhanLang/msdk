#pragma once

namespace msdk{;
namespace msapi{;

//获取当前时间
static LPCTSTR GetCurrentDataTimeStr(LPTSTR lpszTime, DWORD dwCch)
{
	SYSTEMTIME sysTime = {0};
	GetLocalTime(&sysTime);
	_stprintf_s(lpszTime, dwCch, _T("%04d-%02d-%02d %02d:%02d:%02d"),
		sysTime.wYear, 
		sysTime.wMonth, 
		sysTime.wDay,
		sysTime.wHour, 
		sysTime.wMinute,
		sysTime.wSecond);

	return lpszTime;
}


static LPCTSTR GetCurrentDateStr(LPTSTR lpszTime, DWORD dwCch, BOOL bSplit = TRUE)
{
	SYSTEMTIME sysTime = {0};
	GetLocalTime(&sysTime);
	_stprintf_s(lpszTime, dwCch, bSplit ? _T("%04d-%02d-%02d") : _T("%04d%02d%02d"),
		sysTime.wYear, 
		sysTime.wMonth, 
		sysTime.wDay);

	return lpszTime;
}

static std::basic_string<TCHAR> GetCurrentDateStr(BOOL bSplit = TRUE)
{
	TCHAR lpszTime[ MAX_PATH ] = { 0 };
	return GetCurrentDateStr( lpszTime, _countof(lpszTime), bSplit);
}

static LPCTSTR GetCurrentTimeStr(LPTSTR lpszTime, DWORD dwCch, BOOL bSplit = TRUE)
{
	SYSTEMTIME sysTime = {0};
	GetLocalTime(&sysTime);
	_stprintf_s(lpszTime, dwCch, bSplit ? _T("%02d:%02d:%02d") : _T("%02d%02d%02d"),
		sysTime.wHour, 
		sysTime.wMinute,
		sysTime.wSecond);

	return lpszTime;
}

static std::basic_string<TCHAR> GetCurrentTimeStr(BOOL bSplit = TRUE)
{
	TCHAR lpszTime[ MAX_PATH ] = { 0 };
	return GetCurrentTimeStr( lpszTime, _countof(lpszTime), bSplit);
}


};};