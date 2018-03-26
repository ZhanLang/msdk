#pragma once

//include之前先定义下面的宏
//#define MODULE_NAME	_T("")

namespace DuiKit{;

inline void MSDK_OUTPUT(const TCHAR * format, ... )
{
	//计算时间
	static DWORD dwTick = 0;
	if(0 == dwTick)
		dwTick = GetTickCount();

	DWORD dwTickCount = GetTickCount();
	TCHAR szHead[50];
	_stprintf_s(szHead, 50, _T("%10s_OUTPUT:%10dms | "), MODULE_NAME, dwTickCount - dwTick);
	dwTick = dwTickCount;
	int nHeadLen = _tcslen(szHead);

	va_list args;
	// retrieve the variable arguments
	va_start( args, format );

	int inlen = _vsctprintf( format, args ) // _vscprintf doesn't count
		+ 2 // terminating '\0 + last '\n'
		+ nHeadLen; //头的大小

	TCHAR *buffer = (TCHAR*)malloc( inlen * sizeof(TCHAR) );
	ZeroMemory(buffer, sizeof(inlen * sizeof(TCHAR)));
	if(buffer)
	{
		_tcsncpy_s(buffer, inlen, szHead, nHeadLen);
		_vstprintf_s( buffer + nHeadLen, inlen - nHeadLen, format, args );
		_tcscat_s(buffer, inlen, _T("\n"));
		OutputDebugString(buffer);
		free( buffer );
		buffer = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
class CFuncTime
{
public:
	CFuncTime(const TCHAR* lpFuncName,const TCHAR* lpFile)
	{
		m_dwPorcID = GetCurrentProcessId();
		m_dwThreadID = GetCurrentThreadId();

		_tcscpy_s(m_strFunName , MAX_PATH, lpFuncName);
		_tcscpy_s(m_strFileName , MAX_PATH, lpFile ? lpFile:_T(""));
		m_dwBegin = GetTickCount();
		
		TCHAR strCurrentTime[MAX_PATH] = {0};
		GetCurrentFileTime(strCurrentTime , MAX_PATH);

		TCHAR strMsg[512] = {0};
		int len = _stprintf_s( strMsg, 512,_T("[%s][P:%d][T:%d][%s][%s] +%s\n"),strCurrentTime ,m_dwPorcID,m_dwThreadID, MODULE_NAME,m_strFunName,m_strFileName);
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

		TCHAR strCurrentTime[MAX_PATH] = {0};
		GetCurrentFileTime(strCurrentTime , MAX_PATH);


		TCHAR strMsg[512] = {0};
		int len = _stprintf_s( strMsg, 512,_T("[%s][P:%d][T:%d][%s][%s] -%s | %s %d\n"),strCurrentTime,m_dwPorcID,m_dwThreadID,MODULE_NAME,m_strFunName,m_strFileName,wLev,m_dwEnd-m_dwBegin);
		OutputDebugString(strMsg);
	}

	static DWORD GetCurrentFileTime(LPTSTR lpBuf , DWORD dwLen)
	{
		RASSERT(lpBuf && dwLen, 0);

		SYSTEMTIME systemTime = { 0 };
		GetLocalTime(&systemTime);

		return  _stprintf_s(lpBuf,dwLen, _T("%02d:%02d:%02d:%03d"),
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds         
			);
	}

private:
	TCHAR m_strFunName[MAX_PATH];
	TCHAR m_strFileName[MAX_PATH];
	DWORD    m_dwBegin;
	DWORD    m_dwEnd;
	DWORD    m_dwPorcID;
	DWORD    m_dwThreadID;
};

#ifdef OUT_DbgFunc_Time
#	define FUNC_TIME(x,y)  CFuncTime _objFuncTimes(x,y);
#else
#	define FUNC_TIME(x,y);
#endif


} //namespace DuiKit