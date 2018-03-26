
#include "stdafx.h"
#include "fsURL.h"

fsURL::fsURL()
{

}

fsURL::~fsURL()
{

}

fsInternetResult fsURL::Crack(LPCTSTR pszUrl, BOOL bCheckScheme)
{
	
	if (lstrcmpi (pszUrl, _T("http://")) == 0)
		pszUrl = _T("http://url");
	else if (lstrcmpi (pszUrl, _T("ftp://")) == 0)
		pszUrl = _T("ftp://url");
	else if (lstrcmpi (pszUrl, _T("https://")) == 0)
		pszUrl = _T("https://url");

	DWORD urlLen = lstrlen (pszUrl) * 2;
	TCHAR *pszCanUrl = NULL;
	fsString strUrl;

	
	if (*pszUrl == '"' || *pszUrl == '\'')
	{
		strUrl = pszUrl + 1;
		if (strUrl [0] == 0)
			return IR_BADURL;
		strUrl [strUrl.length() - 1] = 0;
		pszUrl = strUrl.c_str();
	}
	
	fsnew (pszCanUrl, TCHAR, urlLen);
	if (pszUrl [0] == '\\' && pszUrl [1] == '\\') 
	{
		m_url.nScheme = INTERNET_SCHEME_FILE; 
		lstrcpy (m_szPath, pszUrl);
		lstrcpy (m_szScheme, _T("file"));
		*m_szUser = *m_szPassword = 0;
		m_url.nPort = 0;
		goto _lFileUrl;
	}

	if (_tcsncicmp (pszUrl, _T("file://"), 7) ==0) 
	{
		if (!InternetCanonicalizeUrl (pszUrl, pszCanUrl, &urlLen, ICU_BROWSER_MODE))
		{
			delete pszCanUrl;

			if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
			{
				fsnew (pszCanUrl, TCHAR, urlLen+1);
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
		lstrcpy (pszCanUrl, pszUrl);
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
			_tcsncicmp (pszCanUrl, _T("ftp://"), 6) == 0 ? ICU_DECODE : 0, &m_url))
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
			
			size_t iPathStart = _tcscspn (m_szPath + 2, _T("\\/")) + 2; 
			if (iPathStart == lstrlen (m_szPath))
				return IR_BADURL;

			_tcsncpy(m_szHost, m_szPath + 2, iPathStart - 2); 
			m_szHost [iPathStart - 2] = 0;
			lstrcpy (m_szPath, m_szPath + iPathStart);	
		}
	}

	return IR_SUCCESS;
}

LPCTSTR fsURL::GetPath()
{
	return m_szPath;
}

LPCTSTR fsURL::GetHostName()
{
	return m_szHost;
}

LPCTSTR fsURL::GetScheme()
{
	return m_szScheme;
}

LPCTSTR fsURL::GetUserName()
{
	return m_szUser;
}

LPCTSTR fsURL::GetPassword()
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

fsInternetResult fsURL::Create(INTERNET_SCHEME nScheme, LPCTSTR lpszHostName, INTERNET_PORT nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, LPCTSTR lpszUrlPath, LPTSTR lpszUrl, DWORD *pdwUrlLength)
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

		if (strHost.length())
		{
			fsString str;
			str = _T("\\\\"); 
			str += strHost; 
			if (strPath [0] != '\\' && strPath [0] != '/')
				str += '\\';
			str += strPath;	
			strHost = _T("");
			strPath = str;
		}

		lstrcpy (lpszUrl, _T("file://")); 
		lstrcat (lpszUrl, strPath.c_str());	
		return IR_SUCCESS;
	}

	TCHAR szUser [1000] = _T(""), szPwd [1000] = _T("");
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
	
	m_url.lpszHostName = (LPTSTR) strHost.c_str();
	m_url.lpszPassword = (LPTSTR) lpszPassword;
	m_url.lpszUrlPath = (LPTSTR) strPath.c_str();
	m_url.lpszUserName = (LPTSTR) lpszUserName;
	m_url.nScheme = nScheme;
	m_url.nPort = nPort;

	
	if (!InternetCreateUrl (&m_url, 0, lpszUrl, pdwUrlLength))
		return fsWinInetErrorToIR ();

	return IR_SUCCESS;
}

VOID fsURL::FixWinInetBug()
{
	LPTSTR psz = _tcsrchr (m_szHost, '@');
	if (psz == NULL)
		return;

	lstrcat (m_szPassword, _T("@"));
	_tcsncat(m_szPassword, m_szHost, psz - m_szHost);
	lstrcpy (m_szHost, psz + 1);
}

VOID fsURL::Encode(LPCTSTR psz, LPTSTR sz)
{
	while (*psz)
	{
		TCHAR c = *psz++;
		TCHAR sz2 [10]={0};

		if (c == ':' || c == '@' || c == '%')
		{
			_stprintf(sz2, _T("%%%x"), (int)(BYTE)c);		
		}
		else
		{
			sz2 [0] = c; sz2 [1] = 0;
		}

		lstrcat (sz, sz2);
	}
}
