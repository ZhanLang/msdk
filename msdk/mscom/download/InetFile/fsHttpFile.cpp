
#include "stdafx.h"
#include "fsHttpFile.h"
#include <stdio.h>
//#include "common.h"
#include <limits.h>
#include "fsInternetSession.h"
#include "strparsing.h"
#include "fsURL.h"
fsHttpFile::fsHttpFile()
{
	m_pszReferer = NULL;
	m_pszCookies = NULL;
	m_pszPostData = NULL;
	m_pszHttpVersion = new TCHAR [100];
	m_dwFlags = INTERNET_FLAG_KEEP_CONNECTION;
	UseHttp11 (FALSE);
	m_bHeadersOnly = FALSE;
	m_pszAdditionalHeaders = NULL;
	m_uLeftToUpload = 0;
	m_bUseMultipart = TRUE;
	m_bEnableAutoRedirect = FALSE;
}

fsHttpFile::~fsHttpFile()
{
	if (m_pszReferer)
		delete [] m_pszReferer;

	if (m_pszCookies)
		delete [] m_pszCookies;

	if (m_pszPostData)
		delete [] m_pszPostData;

	delete [] m_pszHttpVersion;
}

fsInternetResult fsHttpFile::Open(LPCTSTR pszFilePath, UINT64 uStartPos)
{
	return OpenEx (pszFilePath, uStartPos, _UI64_MAX);
}

fsInternetResult fsHttpFile::Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead)
{
	if (m_hFile == NULL)
	{
		*pdwRead = 0;
		return IR_NOTINITIALIZED;
	}

	if (!InternetReadFile (m_hFile, pBuffer, dwToRead, pdwRead))
		return fsWinInetErrorToIR ();

	return IR_SUCCESS;
}

VOID fsHttpFile::UseHttp11(BOOL bUse)
{
	if (bUse)
		lstrcpy (m_pszHttpVersion, _T("HTTP/1.1"));
	else
		lstrcpy (m_pszHttpVersion, _T("HTTP/1.0"));
}

VOID fsHttpFile::SetReferer(LPCTSTR pszReferer)
{
	SAFE_DELETE_ARRAY (m_pszReferer);

	if (pszReferer)
	{
		fsnew (m_pszReferer, TCHAR, lstrlen (pszReferer) + 1);
		lstrcpy (m_pszReferer, pszReferer);
	}
}

VOID fsHttpFile::UseSecure(BOOL bUse)
{
	if (bUse)
		m_dwFlags |= INTERNET_FLAG_SECURE;	
	else
		m_dwFlags &= ~INTERNET_FLAG_SECURE;
}

VOID fsHttpFile::UseCookie(BOOL bUse)
{
	if (bUse)
		m_dwFlags &= ~ INTERNET_FLAG_NO_COOKIES;
	else
		m_dwFlags |= INTERNET_FLAG_NO_COOKIES;
}

fsInternetResult fsHttpFile::SetupProxy()
{
	if (m_hFile == NULL)
		return IR_NOTINITIALIZED;

	return m_pServer->GetSession ()->ApplyProxyAuth (m_hFile);
}

VOID fsHttpFile::RetreiveHeadersOnly(BOOL b)
{
	m_bHeadersOnly = b;
}

fsInternetResult fsHttpFile::ProcessRangesResponse()
{
	TCHAR sz [10000]={0};
	DWORD dw = sizeof (sz);

	BOOL bAcceptRanges = FALSE;
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_ACCEPT_RANGES, sz, &dw, NULL))
	{
		if (_tcsicmp (sz, _T("bytes")) == 0)
			bAcceptRanges = TRUE;
	}

	m_enRST = RST_NONE;

	dw = sizeof (sz);

	
	
	if (!HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_RANGE, sz, &dw, NULL))
		return bAcceptRanges ? IR_DOUBTFUL_RANGESRESPONSE : IR_RANGESNOTAVAIL;

	
	if (_tcsncmp(sz, _T("bytes"), 5))
		return bAcceptRanges ? IR_DOUBTFUL_RANGESRESPONSE : IR_RANGESNOTAVAIL;

	int pos = 0;
	while (sz [pos++] != ' '); 

	if (isdigit (sz [pos]) == false)
		return IR_RANGESNOTAVAIL;
	
	UINT64 first = (UINT64) _ttoi64 (sz + pos);	

	while (sz [pos] >= '0' && sz [pos] <= '9')	
		pos++;
	pos++;

	UINT64 last = (UINT64) _ttoi64 (sz + pos);	

	if (last < first)	
		return IR_RANGESNOTAVAIL;	

	while (sz [pos] >= '0' && sz [pos] <= '9') 
		pos++;
	pos++;	

	m_uFileSize = (UINT64) _ttoi64 (sz + pos);	

	if (m_uFileSize < last)	
		return IR_RANGESNOTAVAIL;

	m_enRST = RST_PRESENT;

	return IR_SUCCESS;
}

VOID fsHttpFile::RetreiveSuggFileName()
{
	m_strSuggFileName = _T("");

	TCHAR sz [MAX_PATH];
	TCHAR szFile [MAX_PATH];
	DWORD dwFL = MAX_PATH;
	
	if (FALSE == HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_DISPOSITION, sz, &dwFL, NULL))
		return;

	
	LPCTSTR psz = fsStrStrNoCase (sz, _T("filename"));
	if (psz == NULL)
		return;

	psz += 8;	
	while (*psz == ' ') 
		psz++;
	bool bCharset = false;
	if (*psz == '*') 
	{
		bCharset = true;
		psz++;
	}

	if (*psz++ != '=')
		return;
	while (*psz == ' ') psz++;

	BOOL bInvComms = FALSE; 
	if  (*psz == '"' || *psz == '\'')
	{
		bInvComms = TRUE;
		psz++;
	}

	LPTSTR pszFile = szFile;

	while (*psz != ';' && *psz != 0)
		*pszFile++ = *psz++;	

	if (bInvComms)	
		*(pszFile-1) = 0;	
	else
		*pszFile = 0;

	if (bCharset)
	{
		//////////////////////////////////////////////////////////////////////////
		//зЂвт
		LPCTSTR psz = _tcsstr (szFile, _T("''"));
		if (psz != NULL)
		{
			if (_tcsncicmp(szFile, _T("utf-8"), 5) == 0)
			{
				/*
				wchar_t wsz [MAX_PATH];
				MultiByteToWideChar (CP_UTF8, 0, (LPCSTR)psz+2, -1, wsz, MAX_PATH);
				WideCharToMultiByte (CP_ACP, 0, wsz, -1, szFile, MAX_PATH, "_", NULL);
				*/
			}
			else
			{
				lstrcpy (szFile, psz+2);
			}			
		}
	}

	m_strSuggFileName = szFile;
}

fsInternetResult fsHttpFile::QuerySize(LPCTSTR pszFilePath)
{
	fsInternetResult ir;					 
	ir = Open (pszFilePath, 0);	
	CloseHandle ();	
	return ir;
}

fsInternetResult fsHttpFile::Open_imp(LPCTSTR pszFilePath, UINT64 uStartPos, int cTryings)
{
	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = m_pServer->GetHandle ();  

	if (!hServer)	
		return IR_NOTINITIALIZED;

	CloseHandle ();

	if (lstrlen (pszFilePath) > 9000)
		return IR_BADURL;

	if (cTryings > 1)
		return IR_WININETUNKERROR;

	DWORD dwFlags = m_dwFlags;
	if (m_pszCookies)
		dwFlags |= INTERNET_FLAG_NO_COOKIES;
	if (m_bEnableAutoRedirect == FALSE)
		dwFlags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	LPTSTR ppszAcceptedTypes [2] = { _T("*/*"), NULL }; 


	LPCTSTR pszVerb = _T("GET");
	if (m_pszPostData)
		pszVerb = _T("POST");
	else if (m_bHeadersOnly)
		pszVerb = _T("HEAD");

	
	m_hFile = HttpOpenRequest (hServer, pszVerb, pszFilePath, m_pszHttpVersion,
		m_pszReferer, (LPCTSTR*) ppszAcceptedTypes, 
		dwFlags | INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | 
		INTERNET_FLAG_NO_CACHE_WRITE, NULL);

	if (m_hFile == NULL)
		return fsWinInetErrorToIR ();

	fsInternetResult ir = SetupProxy ();
	if (ir != IR_SUCCESS)
	{
		CloseHandle ();
		return ir;
	}

	

	
	TCHAR szHdr [20000] = _T("");

	if (uStartPos)
		_stprintf(szHdr, _T("Range: bytes=%I64u-\r\n"), uStartPos); 

	if (m_pszCookies)
		_stprintf(szHdr+ lstrlen (szHdr), _T("Cookie: %s\r\n"), m_pszCookies); 

	if (m_pszPostData)
		_stprintf(szHdr, _T("Content-Type: application/x-www-form-urlencoded\r\n"));

	if (m_pszAdditionalHeaders)
		lstrcat (szHdr, m_pszAdditionalHeaders);

	if (cTryings == 0)
	{
		
		
		TCHAR szReq [90000] = {0};
		_stprintf(szReq,_T("%s %s %s\r\nReferer: %s"), pszVerb, 
			pszFilePath, m_pszHttpVersion, 
			m_pszReferer ? m_pszReferer : _T("-"));

		

		if (*szHdr)
		{
			lstrcat (szReq, _T("\r\n"));
			lstrcat (szReq, szHdr);
			szReq [lstrlen (szReq) - 2] = 0;	
		}

		if ((dwFlags & INTERNET_FLAG_NO_COOKIES) == 0)
		{
			TCHAR szUrl [10000]={0}; DWORD dw = sizeof (szUrl);
			fsURL url;
			url.Create (m_dwFlags & INTERNET_FLAG_SECURE ? INTERNET_SCHEME_HTTPS : INTERNET_SCHEME_HTTP,
				m_pServer->GetServerName (), m_pServer->GetServerPort (), 
				NULL, NULL, pszFilePath, szUrl, &dw);

			TCHAR szCookie [10000]={0}; dw = sizeof (szCookie);
			*szCookie = 0;
			
			InternetGetCookie (szUrl, NULL, szCookie, &dw);

			if (*szCookie)
			{
				lstrcat (szReq, _T("\r\n"));
				lstrcat (szReq, _T("Cookie: "));
				lstrcat (szReq, szCookie);
			}
		}

		lstrcat (szReq, _T("\r\nHost: "));
		lstrcat (szReq, m_pServer->GetServerName ());

		if (m_pszPostData)
		{
			lstrcat (szReq, _T("\r\n"));
			lstrcat (szReq, m_pszPostData);
		}


	}

	
	IgnoreSecurityProblems ();

	
	if (!HttpSendRequest (m_hFile, *szHdr ? szHdr : NULL, (UINT)-1, 
			m_pszPostData, m_pszPostData ? lstrlen (m_pszPostData) : 0))
	{
		ir = fsWinInetErrorToIR ();

									
		CloseHandle ();
		return  ir; 
	}

	TCHAR szResp [10000]={0};
	DWORD dwRespLen = sizeof (szResp), dwIndex = 0;
	
	
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_RAW_HEADERS_CRLF, szResp, &dwRespLen, &dwIndex))
	{
		int cLines = 0; 

		

		LPCTSTR pszLine = szResp;
		while (pszLine)
		{
			
			pszLine = _tcschr (pszLine, '\n');
			if (pszLine)
			{
				while (*pszLine == '\r' || *pszLine == '\n')
					pszLine++;
				cLines++;
			}
		}

		if (cLines == 0 || cLines == 1)
		{
			
			
			return Open_imp (pszFilePath, uStartPos, ++cTryings);
		}
	}


	DWORD dwStatusCode;	
	DWORD dwSize = sizeof (DWORD);
	if (!HttpQueryInfo(m_hFile, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, 
			&dwStatusCode, &dwSize, NULL))	
		return fsWinInetErrorToIR ();

	if (dwStatusCode < 200 || dwStatusCode >= 300)	
	{
		ir = fsHttpStatusCodeToIR (dwStatusCode);

		if (ir == IR_NEEDREDIRECT)	
		{
			DWORD dwNeedLen = 0;

			HttpQueryInfo (m_hFile, HTTP_QUERY_LOCATION, NULL, &dwNeedLen,
				NULL);

			if (::GetLastError () == ERROR_INSUFFICIENT_BUFFER)
			{
				SAFE_DELETE_ARRAY (m_pszLastError);
				try {
					m_pszLastError = new TCHAR [++dwNeedLen];
				}catch (...) {return IR_OUTOFMEMORY;}
				if (m_pszLastError == NULL)
					return IR_OUTOFMEMORY;
				if (!HttpQueryInfo (m_hFile, HTTP_QUERY_LOCATION, m_pszLastError, &dwNeedLen,
						NULL)) 
					return IR_SERVERUNKERROR;
			}
			else
				return IR_SERVERUNKERROR;

		}

		return ir;
	}

	
	TCHAR szContLen [1000]={0};
	DWORD dwLen = sizeof (szContLen);
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_LENGTH,	szContLen, &dwLen, NULL)) {
		__int64 iSize = _ttoi64 (szContLen);
		
		if (iSize < 0)
			return IR_SERVERUNKERROR;
		m_uFileSize = (UINT64) iSize;
	}
	else
		m_uFileSize = _UI64_MAX; 

	ir = IR_SUCCESS;
	if (uStartPos)
	{
		
		ir = ProcessRangesResponse ();
		if (ir == IR_RANGESNOTAVAIL) 
			return ir;
	}
	else
	{
		TCHAR sz [10000]={0};
		DWORD dw = sizeof (sz);
		
		
		if (HttpQueryInfo (m_hFile, HTTP_QUERY_ACCEPT_RANGES, sz, &dw, NULL))
		{
			if (_tcsicmp (sz, _T("bytes")) == 0)
				m_enRST = RST_PRESENT;
			else
				m_enRST = RST_NONE;
		}
		else
			m_enRST = RST_UNKNOWN;
	}

	m_bContentTypeValid = FALSE;
	m_bDateValid = FALSE;

	TCHAR szContentType [10000]={0};	
	DWORD dwCL = sizeof (szContentType);
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_TYPE, szContentType, &dwCL, NULL))
	{
		m_strContentType = szContentType;
		m_bContentTypeValid = TRUE;
	}

	SYSTEMTIME time; 
	DWORD dwTL = sizeof (time);
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_LAST_MODIFIED | HTTP_QUERY_FLAG_SYSTEMTIME,
		&time, &dwTL, NULL))
	{
		SystemTimeToFileTime (&time, &m_date);
		m_bDateValid = TRUE;
	}

	RetreiveSuggFileName ();	

	return ir;
}

BOOL fsHttpFile::Is_Secure()
{
	return m_dwFlags & INTERNET_FLAG_SECURE;
}

VOID fsHttpFile::IgnoreSecurityProblems()
{
	DWORD dwFlags;
    DWORD dwBuffLen = sizeof(dwFlags);

    InternetQueryOption (m_hFile, INTERNET_OPTION_SECURITY_FLAGS,
         (LPVOID)&dwFlags, &dwBuffLen);

    dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | 
		SECURITY_FLAG_IGNORE_WRONG_USAGE |
		SECURITY_FLAG_IGNORE_REVOCATION |
		SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTPS | 
		SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTP |
		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | 
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
	
    InternetSetOption (m_hFile, INTERNET_OPTION_SECURITY_FLAGS,
         &dwFlags, sizeof (dwFlags));
}

BOOL fsHttpFile::get_UseHttp11()
{
	return lstrcmpi (m_pszHttpVersion, _T("HTTP/1.1")) == 0;
}

LPCTSTR fsHttpFile::get_Referer()
{
	return m_pszReferer;
}

BOOL fsHttpFile::get_UseCookie()
{
	return (m_dwFlags & INTERNET_FLAG_NO_COOKIES) == 0;
}

VOID fsHttpFile::SetCookies(LPCTSTR pszCookies)
{
	SAFE_DELETE_ARRAY (m_pszCookies);

	if (pszCookies && *pszCookies)
	{
		fsnew (m_pszCookies, TCHAR, lstrlen (pszCookies) + 1);
		lstrcpy (m_pszCookies, pszCookies);
	}
}

VOID fsHttpFile::SetPostData(LPCTSTR pszPostData)
{
	SAFE_DELETE_ARRAY (m_pszPostData);

	if (pszPostData && *pszPostData)
	{
		fsnew (m_pszPostData, TCHAR, lstrlen (pszPostData) + 1);
		lstrcpy (m_pszPostData, pszPostData);
		OutputDebugString(m_pszPostData);
	}
}

LPCTSTR fsHttpFile::GetCookies()
{
	return m_pszCookies;
}

LPCTSTR fsHttpFile::GetPostData()
{
	return m_pszPostData;
}

VOID fsHttpFile::SetAdditionalHeaders(LPCTSTR pszAdditionalHeaders)
{
	SAFE_DELETE_ARRAY (m_pszAdditionalHeaders);

	if (pszAdditionalHeaders && *pszAdditionalHeaders)
	{
		
		int l = lstrlen (pszAdditionalHeaders);
		if (l < 2)
			return;
		if (pszAdditionalHeaders [l - 2] != '\r')
			return;
		if (pszAdditionalHeaders [l - 1] != '\n')
			return;

		fsnew (m_pszAdditionalHeaders, TCHAR, lstrlen (pszAdditionalHeaders) + 1);
		lstrcpy (m_pszAdditionalHeaders, pszAdditionalHeaders);
	}
}

fsInternetResult fsHttpFile::OpenEx(LPCTSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize, UINT64 uUploadTotalSize)
{
	if (uUploadTotalSize == _UI64_MAX)
		return Open_imp (pszFilePath, uStartPos, 0);

	if (uStartPos + uUploadPartSize > uUploadTotalSize)
		return IR_INVALIDPARAM;

	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = m_pServer->GetHandle ();  

	if (!hServer)
		return IR_NOTINITIALIZED;

	CloseHandle ();

	if (lstrlen (pszFilePath) > 9000)
		return IR_BADURL;

	fsString strFilePath = pszFilePath;
	fsString strFileName;
	if (m_bUseMultipart)
	{
		LPTSTR psz = (LPTSTR)_tcsrchr (strFilePath.c_str(), '/');
		if (psz)
		{
			strFileName = psz + 1;
			psz [1] = 0;
		}
		else
			strFileName = pszFilePath;
	}

	LPTSTR ppszAcceptedTypes [2] = { _T("*/*"), NULL }; 

	m_hFile = HttpOpenRequest (hServer, _T("POST"), strFilePath.c_str(), m_pszHttpVersion,
		NULL, (LPCTSTR*)ppszAcceptedTypes, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (m_hFile == NULL)
		return fsWinInetErrorToIR ();

	fsInternetResult ir = SetupProxy ();
	if (ir != IR_SUCCESS)
	{
		CloseHandle ();
		return ir;
	}

	
	TCHAR szHdr [10000] = _T("");
	
	if (m_bUseMultipart)
		lstrcpy (szHdr, _T("Content-Type: multipart/form-data; boundary=---------------------------284583012225247"));
	else
	{
		lstrcpy (szHdr, _T("Content-Type: application/x-www-form-urlencoded"));
		if (m_strCharset.empty() == FALSE)
		{
			lstrcat (szHdr, _T("; charset="));
			lstrcat (szHdr, m_strCharset.c_str());
		}
	}

	if (uStartPos || uUploadPartSize != uUploadTotalSize)
	{
		if (*szHdr)
			lstrcat (szHdr, _T("\r\n"));
		
		_stprintf(szHdr + lstrlen (szHdr), _T("Range: bytes=%I64u-%I64u/%I64u"), uStartPos, 
			uStartPos + uUploadPartSize - 1, uUploadTotalSize); 
	}

	if (m_pszCookies)
	{
		if (*szHdr)
			lstrcat (szHdr, _T("\r\n"));
		_stprintf(szHdr + lstrlen (szHdr), _T("Cookie: %s"), m_pszCookies); 
	}

	if (m_pszAdditionalHeaders)
	{
		if (*szHdr)
			lstrcat (szHdr, _T("\r\n"));
		lstrcat (szHdr, m_pszAdditionalHeaders);
	}

	
	IgnoreSecurityProblems ();

	int nSizeAdd = 0;
	fsString strMultipartHdr;

	if (m_bUseMultipart)
	{
		m_strLabel = _T("-----------------------------284583012225247");

		strMultipartHdr = m_strLabel; strMultipartHdr += _T("\r\n");
		strMultipartHdr += _T("Content-Disposition: form-data; name=\"uploadFormFile\"; filename=\"");
		strMultipartHdr += strFileName; strMultipartHdr += _T("\"\r\n");
		strMultipartHdr += _T("Content-Type: application/octet-stream\r\n\r\n");

		nSizeAdd = strMultipartHdr.size() + m_strLabel.length() + 6;
	}
 
	INTERNET_BUFFERS BufferIn = {0};
	BufferIn.dwStructSize = sizeof (INTERNET_BUFFERS);
	BufferIn.lpcszHeader = szHdr;
	BufferIn.dwHeadersLength = BufferIn.dwHeadersTotal = lstrlen (szHdr);
	BufferIn.dwBufferTotal = (DWORD) (uUploadPartSize + nSizeAdd);

	if (!HttpSendRequestEx (m_hFile, &BufferIn, NULL, HSR_INITIATE, 0))
	{
		ir = fsWinInetErrorToIR ();
		CloseHandle ();
		return  ir; 
	}

	if (m_bUseMultipart)
	{
		DWORD dw;
		if (FALSE == InternetWriteFile (m_hFile, strMultipartHdr.c_str(), strMultipartHdr.length(), &dw))
		{
			ir = fsWinInetErrorToIR ();
			CloseHandle ();
			return  ir; 
		}
	}

	m_uLeftToUpload = uUploadPartSize;

	return IR_SUCCESS;
}

fsInternetResult fsHttpFile::Write(LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten)
{
	if (m_hFile == NULL)
	{
		if (pdwWritten)
			*pdwWritten = 0;
		return IR_NOTINITIALIZED;
	}		

	if (m_uLeftToUpload == 0)
		return IR_S_FALSE;

	DWORD dwWritten;

	BOOL bRet = InternetWriteFile (m_hFile, pBuffer, dwSize, &dwWritten);

	if (pdwWritten)
		*pdwWritten = dwWritten;

	if (!bRet)
		return fsWinInetErrorToIR ();

	m_uLeftToUpload -= dwWritten;

	if (m_uLeftToUpload == 0)
	{
		if (m_bUseMultipart)
		{
			fsString str = _T("\r\n"); str += m_strLabel; str += _T("--\r\n");
			DWORD dw;
			if (FALSE == InternetWriteFile (m_hFile, str.c_str(), str.length(), &dw))
				return fsWinInetErrorToIR ();
		}

		bRet = HttpEndRequest (m_hFile, NULL, 0, NULL);
		if (!bRet)
			return fsWinInetErrorToIR ();
	}

	return IR_SUCCESS;
}

VOID fsHttpFile::set_UseMultipart(BOOL b)
{
	m_bUseMultipart = b;
}

fsString fsHttpFile::GetCookiesFromResponse()
{
	TCHAR sz [10000] = {0};
	DWORD dw = sizeof (sz) - 1;

	if (FALSE == HttpQueryInfo (m_hFile, HTTP_QUERY_SET_COOKIE, sz, &dw, NULL))
		return _T("");

	return sz;
}

VOID fsHttpFile::set_EnableAutoRedirect(BOOL b)
{
	m_bEnableAutoRedirect = b;
}

VOID fsHttpFile::set_Charset(LPCTSTR psz)
{
	m_strCharset = psz;
}

VOID fsHttpFile::DeleteLatter()
{
	this->CloseHandle();
	delete this;
}