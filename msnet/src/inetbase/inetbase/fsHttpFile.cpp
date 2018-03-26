/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsHttpFile.h"
#include <stdio.h>
#include "common.h"
#include <limits.h>
#include "fsInternetSession.h"
#include "strparsing.h"
#include "fsURL.h"

fsHttpFile::fsHttpFile()
{
	m_pszReferer = NULL;
	m_pszCookies = NULL;
	m_pszPostData = NULL;
	m_pszHttpVersion = new char [100];
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

fsInternetResult fsHttpFile::Open(LPCSTR pszFilePath, UINT64 uStartPos)
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

void fsHttpFile::UseHttp11(BOOL bUse)
{
	if (bUse)
		strcpy (m_pszHttpVersion, "HTTP/1.1");
	else
		strcpy (m_pszHttpVersion, "HTTP/1.0");
}

void fsHttpFile::SetReferer(LPCSTR pszReferer)
{
	SAFE_DELETE_ARRAY (m_pszReferer);

	if (pszReferer)
	{
		fsnew (m_pszReferer, CHAR, strlen (pszReferer) + 1);
		strcpy (m_pszReferer, pszReferer);
	}
}

void fsHttpFile::UseSecure(BOOL bUse)
{
	if (bUse)
		m_dwFlags |= INTERNET_FLAG_SECURE;	
	else
		m_dwFlags &= ~INTERNET_FLAG_SECURE;
}

void fsHttpFile::UseCookie(BOOL bUse)
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

	return ((fsInternetSession*)m_pServer->GetSession ())->ApplyProxyAuth (m_hFile);
}

void fsHttpFile::RetreiveHeadersOnly(BOOL b)
{
	m_bHeadersOnly = b;
}

fsInternetResult fsHttpFile::ProcessRangesResponse()
{
	CHAR sz [10000];
	DWORD dw = sizeof (sz);

	
	
	BOOL bAcceptRanges = FALSE;
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_ACCEPT_RANGES, sz, &dw, NULL))
	{
		if (stricmp (sz, "bytes") == 0)
			bAcceptRanges = TRUE;
	}

	m_enRST = RST_NONE;

	dw = sizeof (sz);

	
	
	if (!HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_RANGE, sz, &dw, NULL))
		return bAcceptRanges ? IR_DOUBTFUL_RANGESRESPONSE : IR_RANGESNOTAVAIL;

	
	if (strncmp (sz, "bytes", 5))
		return bAcceptRanges ? IR_DOUBTFUL_RANGESRESPONSE : IR_RANGESNOTAVAIL;

	int pos = 0;
	while (sz [pos++] != ' '); 

	if (isdigit (sz [pos]) == false)
		return IR_RANGESNOTAVAIL;
	
	UINT64 first = (UINT64) _atoi64 (sz + pos);	

	while (sz [pos] >= '0' && sz [pos] <= '9')	
		pos++;
	pos++;

	UINT64 last = (UINT64) _atoi64 (sz + pos);	

	if (last < first)	
		return IR_RANGESNOTAVAIL;	

	while (sz [pos] >= '0' && sz [pos] <= '9') 
		pos++;
	pos++;	

	m_uFileSize = (UINT64) _atoi64 (sz + pos);	

	if (m_uFileSize < last)	
		return IR_RANGESNOTAVAIL;

	m_enRST = RST_PRESENT;

	return IR_SUCCESS;
}

void fsHttpFile::RetreiveSuggFileName()
{
	m_strSuggFileName = "";

	char sz [MAX_PATH];
	char szFile [MAX_PATH];
	DWORD dwFL = MAX_PATH;
	
	if (FALSE == HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_DISPOSITION, sz, &dwFL, NULL))
		return;

	
	LPCSTR psz = fsStrStrNoCase (sz, "filename");
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

	LPSTR pszFile = szFile;

	while (*psz != ';' && *psz != 0)
		*pszFile++ = *psz++;	

	if (bInvComms)	
		*(pszFile-1) = 0;	
	else
		*pszFile = 0;

	if (bCharset)
	{
		LPCSTR psz = strstr (szFile, "''");
		if (psz != NULL)
		{
			if (strnicmp (szFile, "utf-8", 5) == 0)
			{
				wchar_t wsz [MAX_PATH];
				MultiByteToWideChar (CP_UTF8, 0, psz+2, -1, wsz, MAX_PATH);
				WideCharToMultiByte (CP_ACP, 0, wsz, -1, szFile, MAX_PATH, "_", NULL);
			}
			else
			{
				lstrcpy (szFile, psz+2);
			}			
		}
	}

	m_strSuggFileName = szFile;
}

fsInternetResult fsHttpFile::QuerySize(LPCSTR pszFilePath)
{
	
	fsInternetResult ir;
	
							 
							 
							 
	ir = Open (pszFilePath, 0);	
	
	CloseHandle ();	
	return ir;
}

fsInternetResult fsHttpFile::Open_imp(LPCSTR pszFilePath, UINT64 uStartPos, int cTryings)
{
	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = ((fsInternetServerConnection*)m_pServer.m_p)->GetHandle ();  

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

	LPTSTR ppszAcceptedTypes [2] = { "*/*", NULL }; 

	
	
	
	

	LPCSTR pszVerb = "GET";
	if (m_pszPostData)
		pszVerb = "POST";
	else if (m_bHeadersOnly)
		pszVerb = "HEAD";

	
	m_hFile = HttpOpenRequest (hServer, pszVerb, pszFilePath, m_pszHttpVersion,
		m_pszReferer, (LPCSTR*) ppszAcceptedTypes, 
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

	
	CHAR szHdr [20000] = "";

	if (uStartPos)
		sprintf (szHdr, "Range: bytes=%I64u-\r\n", uStartPos); 

	if (m_pszCookies)
		sprintf (szHdr + lstrlen (szHdr), "Cookie: %s\r\n", m_pszCookies); 

	if (m_pszPostData)
		strcat (szHdr, "Content-Type: application/x-www-form-urlencoded\r\n");

	if (m_pszAdditionalHeaders)
		strcat (szHdr, m_pszAdditionalHeaders);

	if (cTryings == 0)
	{
		
		char szReq [90000];
		sprintf (szReq, "%s %s %s\r\nReferer: %s", pszVerb, 
			pszFilePath, m_pszHttpVersion, 
			m_pszReferer ? m_pszReferer : "-");

		if (*szHdr)
		{
			strcat (szReq, "\r\n");
			strcat (szReq, szHdr);
			szReq [strlen (szReq) - 2] = 0;	
		}

		if ((dwFlags & INTERNET_FLAG_NO_COOKIES) == 0)
		{
			char szUrl [10000]; DWORD dw = sizeof (szUrl);
			fsURL url;
			url.Create (m_dwFlags & INTERNET_FLAG_SECURE ? INTERNET_SCHEME_HTTPS : INTERNET_SCHEME_HTTP,
				m_pServer->GetServerName (), m_pServer->GetServerPort (), 
				NULL, NULL, pszFilePath, szUrl, &dw);

			char szCookie [10000]; dw = sizeof (szCookie);
			*szCookie = 0;
			
			InternetGetCookie (szUrl, NULL, szCookie, &dw);

			if (*szCookie)
			{
				strcat (szReq, "\r\n");
				strcat (szReq, "Cookie: ");
				strcat (szReq, szCookie);
			}
		}

		strcat (szReq, "\r\nHost: ");
		strcat (szReq, m_pServer->GetServerName ());

		if (m_pszPostData)
		{
			strcat (szReq, "\r\n");
			strcat (szReq, m_pszPostData);
		}

		Dialog (IFDD_TOSERVER, szReq);	
	}

	
	IgnoreSecurityProblems ();

	
	if (!HttpSendRequest (m_hFile, *szHdr ? szHdr : NULL, (UINT)-1, 
			m_pszPostData, m_pszPostData ? lstrlen (m_pszPostData) : 0))
	{
		ir = fsWinInetErrorToIR ();

		DialogHttpResponse (m_hFile);	
									
		CloseHandle ();
		return  ir; 
	}

	char szResp [10000];
	DWORD dwRespLen = sizeof (szResp), dwIndex = 0;
	
	
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_RAW_HEADERS_CRLF, szResp, &dwRespLen, &dwIndex))
	{
		int cLines = 0; 

		

		LPCSTR pszLine = szResp;
		while (pszLine)
		{
			pszLine = strchr (pszLine, '\n');
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

	DialogHttpResponse (m_hFile);	

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
					m_pszLastError = new char [++dwNeedLen];
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

	
	char szContLen [1000];
	DWORD dwLen = sizeof (szContLen);
	if (HttpQueryInfo (m_hFile, HTTP_QUERY_CONTENT_LENGTH,	szContLen, &dwLen, NULL)) {
		__int64 iSize = _atoi64 (szContLen);
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
		CHAR sz [10000];
		DWORD dw = sizeof (sz);
		
		
		if (HttpQueryInfo (m_hFile, HTTP_QUERY_ACCEPT_RANGES, sz, &dw, NULL))
		{
			if (stricmp (sz, "bytes") == 0)
				m_enRST = RST_PRESENT;
			else
				m_enRST = RST_NONE;
		}
		else
			m_enRST = RST_UNKNOWN;
	}

	m_bContentTypeValid = FALSE;
	m_bDateValid = FALSE;

	CHAR szContentType [10000];	
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

void fsHttpFile::IgnoreSecurityProblems()
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
	return lstrcmpi (m_pszHttpVersion, "HTTP/1.1") == 0;
}

LPCSTR fsHttpFile::get_Referer()
{
	return m_pszReferer;
}

BOOL fsHttpFile::get_UseCookie()
{
	return (m_dwFlags & INTERNET_FLAG_NO_COOKIES) == 0;
}

void fsHttpFile::SetCookies(LPCSTR pszCookies)
{
	SAFE_DELETE_ARRAY (m_pszCookies);

	if (pszCookies && *pszCookies)
	{
		fsnew (m_pszCookies, CHAR, lstrlen (pszCookies) + 1);
		lstrcpy (m_pszCookies, pszCookies);
	}
}

void fsHttpFile::SetPostData(LPCSTR pszPostData)
{
	SAFE_DELETE_ARRAY (m_pszPostData);

	if (pszPostData && *pszPostData)
	{
		fsnew (m_pszPostData, CHAR, lstrlen (pszPostData) + 1);
		lstrcpy (m_pszPostData, pszPostData);
	}
}

LPCSTR fsHttpFile::GetCookies()
{
	return m_pszCookies;
}

LPCSTR fsHttpFile::GetPostData()
{
	return m_pszPostData;
}

void fsHttpFile::SetAdditionalHeaders(LPCSTR pszAdditionalHeaders)
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

		fsnew (m_pszAdditionalHeaders, CHAR, lstrlen (pszAdditionalHeaders) + 1);
		lstrcpy (m_pszAdditionalHeaders, pszAdditionalHeaders);
	}
}

fsInternetResult fsHttpFile::OpenEx(LPCSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize, UINT64 uUploadTotalSize)
{
	if (uUploadTotalSize == _UI64_MAX)
		return Open_imp (pszFilePath, uStartPos, 0);

	if (uStartPos + uUploadPartSize > uUploadTotalSize)
		return IR_INVALIDPARAM;

	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = ((fsInternetServerConnection*)m_pServer.m_p)->GetHandle ();  

	if (!hServer)
		return IR_NOTINITIALIZED;

	CloseHandle ();

	if (lstrlen (pszFilePath) > 9000)
		return IR_BADURL;

	fsString strFilePath = pszFilePath;
	fsString strFileName;
	if (m_bUseMultipart)
	{
		LPSTR psz = strrchr (strFilePath, '/');
		if (psz)
		{
			strFileName = psz + 1;
			psz [1] = 0;
		}
		else
			strFileName = pszFilePath;
	}

	LPTSTR ppszAcceptedTypes [2] = { "*/*", NULL }; 

	m_hFile = HttpOpenRequest (hServer, "POST", strFilePath, m_pszHttpVersion,
		NULL, (LPCSTR*)ppszAcceptedTypes, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (m_hFile == NULL)
		return fsWinInetErrorToIR ();

	fsInternetResult ir = SetupProxy ();
	if (ir != IR_SUCCESS)
	{
		CloseHandle ();
		return ir;
	}

	
	CHAR szHdr [10000] = "";
	
	if (m_bUseMultipart)
		lstrcpy (szHdr, "Content-Type: multipart/form-data; boundary=---------------------------284583012225247");
	else
	{
		lstrcpy (szHdr, "Content-Type: application/x-www-form-urlencoded");
		if (m_strCharset.IsEmpty () == FALSE)
		{
			lstrcat (szHdr, "; charset=");
			lstrcat (szHdr, m_strCharset);
		}
	}

	if (uStartPos || uUploadPartSize != uUploadTotalSize)
	{
		if (*szHdr)
			lstrcat (szHdr, "\r\n");
		sprintf (szHdr + lstrlen (szHdr), "Range: bytes=%I64u-%I64u/%I64u", uStartPos, 
			uStartPos + uUploadPartSize - 1, uUploadTotalSize); 
	}

	if (m_pszCookies)
	{
		if (*szHdr)
			lstrcat (szHdr, "\r\n");
		sprintf (szHdr + lstrlen (szHdr), "Cookie: %s", m_pszCookies); 
	}

	if (m_pszAdditionalHeaders)
	{
		if (*szHdr)
			lstrcat (szHdr, "\r\n");
		lstrcat (szHdr, m_pszAdditionalHeaders);
	}

	
	IgnoreSecurityProblems ();

	int nSizeAdd = 0;
	fsString strMultipartHdr;

	if (m_bUseMultipart)
	{
		m_strLabel = "-----------------------------284583012225247";

		strMultipartHdr = m_strLabel; strMultipartHdr += "\r\n";
		strMultipartHdr += "Content-Disposition: form-data; name=\"uploadFormFile\"; filename=\"";
		strMultipartHdr += strFileName; strMultipartHdr += "\"\r\n";
		strMultipartHdr += "Content-Type: application/octet-stream\r\n\r\n";

		nSizeAdd = strMultipartHdr.GetLength () + m_strLabel.GetLength () + 6;
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
		if (FALSE == InternetWriteFile (m_hFile, strMultipartHdr, strMultipartHdr.GetLength (), &dw))
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
			fsString str = "\r\n"; str += m_strLabel; str += "--\r\n";
			DWORD dw;
			if (FALSE == InternetWriteFile (m_hFile, str, str.GetLength (), &dw))
				return fsWinInetErrorToIR ();
		}

		bRet = HttpEndRequest (m_hFile, NULL, 0, NULL);
		if (!bRet)
			return fsWinInetErrorToIR ();
	}

	return IR_SUCCESS;
}

void fsHttpFile::set_UseMultipart(BOOL b)
{
	m_bUseMultipart = b;
}

std::string fsHttpFile::GetCookiesFromResponse()
{
	char sz [10000];
	DWORD dw = sizeof (sz) - 1;

	if (FALSE == HttpQueryInfo (m_hFile, HTTP_QUERY_SET_COOKIE, sz, &dw, NULL))
		return "";

	return sz;
}

void fsHttpFile::set_EnableAutoRedirect(BOOL b)
{
	m_bEnableAutoRedirect = b;
}

void fsHttpFile::set_Charset(LPCSTR psz)
{
	m_strCharset = psz;
}
