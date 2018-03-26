/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsURL.h"
#include <fsString.h>
#include "common.h"
#include "fsinet.h"

fsURL::fsURL()
{

}

fsURL::~fsURL()
{

}

fsInternetResult fsURL::Crack(LPCSTR pszUrl, BOOL bCheckScheme)
{
	
	if (lstrcmpi (pszUrl, "http://") == 0)
		pszUrl = "http://url";
	else if (lstrcmpi (pszUrl, "ftp://") == 0)
		pszUrl = "ftp://url";
	else if (lstrcmpi (pszUrl, "https://") == 0)
		pszUrl = "https://url";

	DWORD urlLen = strlen (pszUrl) * 2;
	CHAR *pszCanUrl = NULL;
	fsString strUrl;

	
	if (*pszUrl == '"' || *pszUrl == '\'')
	{
		
		
		strUrl = pszUrl + 1;
		if (strUrl [0] == 0)
			return IR_BADURL;
		strUrl [strUrl.Length () - 1] = 0;
		pszUrl = strUrl;
	}
	
	fsnew (pszCanUrl, CHAR, urlLen);
	if (pszUrl [0] == '\\' && pszUrl [1] == '\\') 
	{
		m_url.nScheme = INTERNET_SCHEME_FILE; 
		strcpy (m_szPath, pszUrl);
		strcpy (m_szScheme, "file");
		*m_szUser = *m_szPassword = 0;
		m_url.nPort = 0;
		goto _lFileUrl;
	}

	if (strnicmp (pszUrl, "file://", 7)) 
	{
		
		if (!InternetCanonicalizeUrl (pszUrl, pszCanUrl, &urlLen, ICU_BROWSER_MODE))
		{
			delete pszCanUrl;

			if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
			{
				fsnew (pszCanUrl, CHAR, urlLen+1);
				if (!InternetCanonicalizeUrl (pszUrl, pszCanUrl, &urlLen, ICU_BROWSER_MODE))
				{
					delete pszCanUrl;
					return fsWinInetErrorToIR ();
				}
			}
			else
				return fsWinInetErrorToIR ();
		}
	}
	else
	{
		
		strcpy (pszCanUrl, pszUrl);
	}

	ZeroMemory (&m_url, sizeof (m_url));
	m_url.dwStructSize = sizeof (m_url);
	
	m_url.lpszHostName = m_szHost;
	m_url.lpszPassword = m_szPassword;
	m_url.lpszScheme = m_szScheme;
	m_url.lpszUrlPath = m_szPath;
	m_url.lpszUserName = m_szUser;

	m_url.dwHostNameLength = URL_HOSTNAME_SIZE;
	m_url.dwPasswordLength = URL_PASSWORD_SIZE;
	m_url.dwSchemeLength = URL_SCHEME_SIZE;
	m_url.dwUrlPathLength = URL_PATH_SIZE;
	m_url.dwUserNameLength = URL_USERNAME_SIZE;

	
	if (!InternetCrackUrl (pszCanUrl, urlLen, 
			_strnicmp (pszCanUrl, "ftp://", 6) == 0 ? ICU_DECODE : 0, &m_url))
	{
		delete pszCanUrl;
		return fsWinInetErrorToIR ();
	}

	

	delete pszCanUrl;
	
	if (bCheckScheme)	
	{
		if (m_url.nScheme != INTERNET_SCHEME_HTTP && m_url.nScheme != INTERNET_SCHEME_FTP &&
			 m_url.nScheme != INTERNET_SCHEME_HTTPS && m_url.nScheme != INTERNET_SCHEME_FILE)
		{
			return IR_BADURL;
		}
	}

	FixWinInetBug ();

_lFileUrl:
	if (m_url.nScheme == INTERNET_SCHEME_FILE)
	{
		
		if (m_szPath [0] == '\\' && m_szPath [1] == '\\') 
														
		{
			
			size_t iPathStart = strcspn (m_szPath + 2, "\\/") + 2; 
			if (iPathStart == strlen (m_szPath))
				return IR_BADURL;
			strncpy (m_szHost, m_szPath + 2, iPathStart - 2); 
			m_szHost [iPathStart - 2] = 0;
			strcpy (m_szPath, m_szPath + iPathStart);	
		}
	}

	return IR_SUCCESS;
}

LPCSTR fsURL::GetPath()
{
	return m_szPath;
}

LPCSTR fsURL::GetHostName()
{
	return m_szHost;
}

LPCSTR fsURL::GetScheme()
{
	return m_szScheme;
}

LPCSTR fsURL::GetUserName()
{
	return m_szUser;
}

LPCSTR fsURL::GetPassword()
{
	return m_szPassword;
}

INTERNET_SCHEME fsURL::GetInternetScheme()
{
	return m_url.nScheme;
}

INTERNET_PORT fsURL::GetPort()
{
	return m_url.nPort;
}

fsInternetResult fsURL::Create(INTERNET_SCHEME nScheme, LPCTSTR lpszHostName, INTERNET_PORT nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, LPCTSTR lpszUrlPath, LPSTR lpszUrl, DWORD *pdwUrlLength)
{
	if (!lpszUrl)
		return IR_ERROR;

	*lpszUrl = 0;

	ZeroMemory (&m_url, sizeof (m_url));
	m_url.dwStructSize = sizeof (m_url);

	if (lpszUserName && *lpszUserName == 0)
		lpszUserName = NULL;

	if (lpszPassword && *lpszPassword == 0)
		lpszPassword = NULL;

	fsString strHost, strPath;

	strHost = lpszHostName;
	strPath = lpszUrlPath;

	if (nScheme == INTERNET_SCHEME_FILE)	
	{
		

		lpszUserName = NULL;	
		lpszPassword = NULL;

		if (strHost.Length ())
		{
			fsString str;
			str = "\\\\"; 
			str += strHost; 
			if (strPath [0] != '\\' && strPath [0] != '/')
				str += '\\';
			str += strPath;	
			strHost = "";
			strPath = str;
		}

		strcpy (lpszUrl, "file://"); 
		strcat (lpszUrl, strPath);	
		return IR_SUCCESS;
	}

	char szUser [1000] = "", szPwd [1000] = "";
	if (lpszUserName)
	{
		Encode (lpszUserName, szUser);
		lpszUserName = szUser;
	}

	if (lpszPassword)
	{
		Encode (lpszPassword, szPwd);
		lpszPassword = szPwd;	
	}
	
	m_url.lpszHostName = (LPTSTR) strHost;
	m_url.lpszPassword = (LPTSTR) lpszPassword;
	m_url.lpszUrlPath = (LPTSTR) strPath;
	m_url.lpszUserName = (LPTSTR) lpszUserName;
	m_url.nScheme = nScheme;
	m_url.nPort = nPort;

	
	if (!InternetCreateUrl (&m_url, 0, lpszUrl, pdwUrlLength))
		return fsWinInetErrorToIR ();

	return IR_SUCCESS;
}

void fsURL::FixWinInetBug()
{
	LPSTR psz = strchr (m_szHost, '@');
	if (psz == NULL)
		return;

	strcat (m_szPassword, "@");
	strncat (m_szPassword, m_szHost, psz - m_szHost);
	strcpy (m_szHost, psz + 1);
}

void fsURL::Encode(LPCSTR psz, LPSTR sz)
{
	while (*psz)
	{
		char c = *psz++;
		char sz2 [10];

		if (c == ':' || c == '@' || c == '%')
		{
			sprintf (sz2, "%%%x", (int)(BYTE)c);		
		}
		else
		{
			sz2 [0] = c; sz2 [1] = 0;
		}

		lstrcat (sz, sz2);
	}
}
