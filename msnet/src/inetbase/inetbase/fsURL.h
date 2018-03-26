/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSURL_H__B92E5969_6142_4EC2_8379_56A47FDD6329__INCLUDED_)
#define AFX_FSURL_H__B92E5969_6142_4EC2_8379_56A47FDD6329__INCLUDED_

#include "fsinet.h"	
#if _MSC_VER > 1000
#pragma once
#endif 

#define URL_SCHEME_SIZE		100		
#define URL_HOSTNAME_SIZE	1000	
#define URL_USERNAME_SIZE	100		
#define URL_PASSWORD_SIZE	100		
#define URL_PATH_SIZE		10000	

class fsURL  
{
public:
	
	
	fsInternetResult Crack (LPCSTR pszUrl, BOOL bCheckScheme = TRUE);
	
	
	
	
	
	
	
	
	fsInternetResult Create (INTERNET_SCHEME nScheme, LPCTSTR lpszHostName, INTERNET_PORT nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, LPCTSTR lpszUrlPath, LPSTR lpszUrl, DWORD* pdwUrlLength);
	
	INTERNET_PORT GetPort();
	INTERNET_SCHEME GetInternetScheme();
	LPCSTR GetPassword();
	LPCSTR GetUserName();
	LPCSTR GetScheme();
	LPCSTR GetHostName();
	LPCSTR GetPath();
	
	
	
	
	void FixWinInetBug();
		
	fsURL();
	virtual ~fsURL();

protected:
	
	
	
	static void Encode (LPCSTR psz, LPSTR sz);
	URL_COMPONENTS m_url;
	CHAR m_szScheme [URL_SCHEME_SIZE];	
	CHAR m_szHost [URL_HOSTNAME_SIZE];	
	CHAR m_szUser [URL_USERNAME_SIZE];	
	CHAR m_szPassword [URL_PASSWORD_SIZE];	
	CHAR m_szPath [URL_PATH_SIZE];		
};

#endif 
