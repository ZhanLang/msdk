/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSINTERNETSESSION_H__3E047CCF_86D6_4197_9BB2_8C94D552A262__INCLUDED_)
#define AFX_FSINTERNETSESSION_H__3E047CCF_86D6_4197_9BB2_8C94D552A262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <windows.h>
#include <wininet.h>
#include "fsinet.h"
#include <fsString.h>
#include "fsInternetSession_i.h"

class fsInternetSession  : public fsInternetSession_i , CUnknownImp
{
public:
	
	UNKNOWN_IMP1(fsInternetSession_i);

	void get_Proxy (LPCSTR& pszProxyName, LPCSTR& pszProxyUser, LPCSTR& pszProxyPwd);
	
	LPCSTR get_UserAgent();
	
	
	static void InitializeWinInet();
	
	void CloseHandle();
	
	fsInternetResult ApplyProxyAuth (HINTERNET hInetHandle);
	
	void SetProxyAuth (LPCSTR pszUser, LPCSTR pszPassword);
	
	void SetTimeout (UINT uTimeout);
	
	HINTERNET GetHandle();
	
	
	
	
	fsInternetResult Create (LPCSTR pszAgent, fsInternetAccessType accType, LPCSTR pszProxy);
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

#endif 
