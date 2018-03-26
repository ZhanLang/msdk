#pragma once
#include "fsinet.h"

#define URL_SCHEME_SIZE		100		
#define URL_HOSTNAME_SIZE	1000	
#define URL_USERNAME_SIZE	100		
#define URL_PASSWORD_SIZE	100		
#define URL_PATH_SIZE		10000	

class fsURL  
{
public:
	fsInternetResult Crack (LPCTSTR pszUrl, BOOL bCheckScheme = TRUE);
	fsInternetResult Create (INTERNET_SCHEME nScheme, LPCTSTR lpszHostName, INTERNET_PORT nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, LPCTSTR lpszUrlPath, LPTSTR lpszUrl, DWORD* pdwUrlLength);
	INTERNET_PORT GetPort();
	INTERNET_SCHEME GetInternetScheme();
	LPCTSTR GetPassword();
	LPCTSTR GetUserName();
	LPCTSTR GetScheme();
	LPCTSTR GetHostName();
	LPCTSTR GetPath();
	VOID FixWinInetBug();
		
	fsURL();
	virtual ~fsURL();

protected:
	
	static VOID Encode (LPCTSTR psz, LPTSTR sz);
	URL_COMPONENTS m_url;
	TCHAR m_szScheme [URL_SCHEME_SIZE];	
	TCHAR m_szHost [URL_HOSTNAME_SIZE];	
	TCHAR m_szUser [URL_USERNAME_SIZE];	
	TCHAR m_szPassword [URL_PASSWORD_SIZE];	
	TCHAR m_szPath [URL_PATH_SIZE];		
};


