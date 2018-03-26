
#pragma once

#include <windows.h>
#include <wininet.h>
#include "fsinet.h"

enum fsInternetAccessType	
{
	IAT_NOPROXY,			
	IAT_PRECONFIGPROXY,		
	IAT_MANUALPROXY			
};

class fsInternetSession  
{
public:
	VOID get_Proxy (LPCTSTR& pszProxyName, LPCTSTR& pszProxyUser, LPCTSTR& pszProxyPwd);
	LPCTSTR get_UserAgent();
	static VOID InitializeWinInet();
	VOID CloseHandle();
	fsInternetResult ApplyProxyAuth (HINTERNET hInetHandle);
	VOID SetProxyAuth (LPCTSTR pszUser, LPCTSTR pszPassword);
	VOID SetTimeout (UINT uTimeout);
	HINTERNET GetHandle();
	fsInternetResult Create (LPCTSTR pszAgent, fsInternetAccessType accType, LPCTSTR pszProxy);
	static VOID AdjustWinInetConnectionLimit ();

	fsInternetSession();
	virtual ~fsInternetSession();
protected:
	fsString m_strUserAgent;		
	fsString m_strProxy;			
	fsString m_strProxyUser;		
	fsString m_strProxyPassword;	
	HINTERNET m_hSession;			
private:
};

