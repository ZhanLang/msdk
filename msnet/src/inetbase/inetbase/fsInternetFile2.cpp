/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/


#include "StdAfx.h"
#include "fsInternetFile2.h"
#include <curl/curl.h>
#ifndef NOCURL
fsInternetFile2::fsInternetFile2()
{
	m_curl = NULL;
	m_bDownloading = false;
	m_bNeedStop = false;
	m_irLastError = IR_SUCCESS;

	m_hevReadDataReq = m_hevReadDataDone = NULL;

	m_fileTime = -1;
	m_uFileSize = _UI64_MAX;

	m_bDoPause = FALSE;

	m_uStartPos = 0;

	m_enRST = RST_UNKNOWN;
}

fsInternetFile2::~fsInternetFile2()
{
	StopDownloading ();
	for (int i = 0; m_bDownloading && i < 33; i++)
		Sleep (33 * 3);

	if (m_hevReadDataReq)
		CloseHandle (m_hevReadDataReq);
	if (m_hevReadDataDone)
		CloseHandle (m_hevReadDataDone);
}

fsInternetResult fsInternetFile2::Initialize()
{
	if (m_curl == NULL)
	{
		
		m_curl = curl_easy_init ();
		
		if (m_curl)
		{
			
			if (m_hevReadDataReq == NULL)
			{
				m_hevReadDataReq = CreateEvent (NULL, TRUE, FALSE, NULL);
				m_hevReadDataDone = CreateEvent (NULL, TRUE, FALSE, NULL);
			}

			
			curl_easy_setopt (m_curl, CURLOPT_WRITEFUNCTION, _WriteData);
			curl_easy_setopt (m_curl, CURLOPT_WRITEDATA, this);

			curl_easy_setopt (m_curl, CURLOPT_HEADERFUNCTION, _WriteHeader);
			curl_easy_setopt (m_curl, CURLOPT_HEADERDATA, this);

			curl_easy_setopt (m_curl, CURLOPT_FOLLOWLOCATION, TRUE);

			curl_easy_setopt (m_curl, CURLOPT_SSL_VERIFYPEER, FALSE);
			curl_easy_setopt (m_curl, CURLOPT_SSL_VERIFYHOST, FALSE);
		}
	}

	return m_curl == NULL ? IR_ERROR : IR_SUCCESS;
}

size_t fsInternetFile2::_WriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	fsInternetFile2* pthis = (fsInternetFile2*) stream;
	return pthis->OnWriteData ((LPBYTE)ptr, size * nmemb);
}

void fsInternetFile2::set_URL(LPCSTR pszURL)
{
	curl_easy_setopt (m_curl, CURLOPT_URL, pszURL);

	if (strstr (pszURL, ".megashares.com/") != NULL)
		m_bDoPause = TRUE;
}

fsInternetResult fsInternetFile2::StartDownloading()
{
	if (m_bDownloading == false)
	{
		m_bDownloading = true;
		m_bNeedStop = false;
		m_bAnswrFromServRcvd = false;
		m_irLastError = IR_SUCCESS;
		m_strHttpHeader = "";
		m_enRST = RST_UNKNOWN;

		
		ResetEvent (m_hevReadDataReq);
		
		ResetEvent (m_hevReadDataDone);

		DWORD dw;
		CloseHandle (
			CreateThread (NULL, 0, _threadDownload, this, 0, &dw));

		
		while (m_bAnswrFromServRcvd == false)
			Sleep (10);

		if (m_irLastError == IR_SUCCESS)
			ExtractFileInfoFromResponse ();

		if (m_strHttpHeader.IsEmpty () == FALSE)
			Dialog (IFDD_FROMSERVER, m_strHttpHeader);

		
		return m_irLastError;
	}

	return IR_S_FALSE;	
}

DWORD WINAPI fsInternetFile2::_threadDownload(LPVOID lp)
{
	fsInternetFile2* pthis = (fsInternetFile2*)lp;

	try{

	if (pthis->m_bDoPause)
		Sleep (1500);

	
	
	
	
	
	
	
	
	
	
	CURLcode res = curl_easy_perform (pthis->m_curl);

	
	pthis->m_irLastError = CURLcodeToIR (res);

	if (pthis->m_irLastError == IR_SUCCESS)
	{
		long nRespCode = 200;
		curl_easy_getinfo (pthis->m_curl, CURLINFO_RESPONSE_CODE, &nRespCode);
		pthis->m_irLastError = fsHttpStatusCodeToIR (nRespCode);
	}

	
	curl_easy_cleanup (pthis->m_curl);
	pthis->m_curl = NULL;

	
	pthis->m_bAnswrFromServRcvd = true;

	pthis->m_bDownloading = false;

	
	SetEvent (pthis->m_hevReadDataReq);
	SetEvent (pthis->m_hevReadDataDone);

	}catch (...) {}

	return 0;
}

fsInternetResult fsInternetFile2::CURLcodeToIR(int code)
{
	switch (code)
	{
	case CURLE_OK:
		return IR_SUCCESS;

	case CURLE_COULDNT_RESOLVE_HOST:
		return IR_NAMENOTRESOLVED;

	case CURLE_COULDNT_CONNECT:
		return IR_CANTCONNECT;

	case CURLE_FTP_USER_PASSWORD_INCORRECT:
		return IR_LOGINFAILURE;

	case CURLE_OPERATION_TIMEOUTED:
		return IR_TIMEOUT;

	case CURLE_HTTP_RANGE_ERROR:
		return IR_RANGESNOTAVAIL;

	
	default:
		return IR_ERROR;
	}
}

fsInternetResult fsInternetFile2::Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead)
{
	if (pdwRead)
		*pdwRead = 0; 

	if (dwToRead == 0)
		return IR_SUCCESS;

	if (m_bDownloading == false)
		return m_irLastError;	

	
	m_ReadReq.abData.clear ();
	m_ReadReq.nDataLeft = dwToRead;

	
	SetEvent (m_hevReadDataReq);
	
	while (WAIT_TIMEOUT == WaitForSingleObject (m_hevReadDataDone, 1000))
	{
		if (m_bNeedStop)
			return IR_S_FALSE;
	}
	
	ResetEvent (m_hevReadDataDone);

	if (m_bNeedStop)
		return IR_S_FALSE;

	if (m_ReadReq.nDataLeft == dwToRead)
		return m_irLastError != IR_SUCCESS ? m_irLastError : IR_ERROR;

	
	
	DWORD dwRead = dwToRead - m_ReadReq.nDataLeft;

	CopyMemory (pBuffer, m_ReadReq.abData, dwRead);
	if (pdwRead)
		*pdwRead = dwRead;

	return m_ReadReq.nDataLeft ? m_irLastError : IR_SUCCESS;
}

size_t fsInternetFile2::OnWriteData(LPBYTE ptr, size_t size)
{
	
	m_bAnswrFromServRcvd = true;
	size_t sizeRcvd = size;

_lReadData:
	
	
	while (WAIT_TIMEOUT == WaitForSingleObject (m_hevReadDataReq, 1000))
	{
		if (m_bNeedStop)
			return 0;
	}

	if (m_bNeedStop || m_bDownloading == false)
		return 0;	

	
	int n; n  = min (m_ReadReq.nDataLeft, size);
	
	m_ReadReq.abData.add ((LPBYTE)ptr, n);
	
	m_ReadReq.nDataLeft -= n;
	size -= n;
	ptr += n;

	if (m_ReadReq.nDataLeft == 0) {
		
		
		ResetEvent (m_hevReadDataReq);
		
		SetEvent (m_hevReadDataDone);
	}

	if (size == 0)
		
		
		return sizeRcvd; 

	
	
	
	goto _lReadData;
}

void fsInternetFile2::StopDownloading()
{
	m_bNeedStop = true;

	
	SetEvent (m_hevReadDataReq);
	SetEvent (m_hevReadDataDone);
}

void fsInternetFile2::set_ResumeFrom (UINT64 uStart)
{
	curl_easy_setopt (m_curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)uStart);
	m_uStartPos = uStart;
}

void fsInternetFile2::set_UserAgent(LPCSTR psz)
{
	curl_easy_setopt (m_curl, CURLOPT_USERAGENT, psz);
}

void fsInternetFile2::set_UseHttp11(BOOL bUse)
{
	curl_easy_setopt (m_curl, CURLOPT_HTTP_VERSION, 
		bUse ? CURL_HTTP_VERSION_1_1 : CURL_HTTP_VERSION_1_0);
}

void fsInternetFile2::set_Referer(LPCSTR psz)
{
	curl_easy_setopt (m_curl, CURLOPT_REFERER, psz);
}

void fsInternetFile2::set_Cookie(LPCSTR psz)
{
	curl_easy_setopt (m_curl, CURLOPT_COOKIE, psz);
}

void fsInternetFile2::set_Auth(LPCSTR pszUser, LPCSTR pszPwd)
{
	char sz [1000];
	sprintf (sz, "%s:%s", pszUser, pszPwd);
	curl_easy_setopt (m_curl, CURLOPT_USERPWD, sz);
}

void fsInternetFile2::set_Proxy(LPCSTR pszProxy, LPCSTR pszUser, LPCSTR pszPwd)
{
	curl_easy_setopt (m_curl, CURLOPT_PROXY, pszProxy);

	if (pszUser && *pszUser)
	{
		curl_easy_setopt (m_curl, CURLOPT_PROXYUSERNAME, pszUser);
		curl_easy_setopt (m_curl, CURLOPT_PROXYPASSWORD, pszPwd);
				
	}
}

void fsInternetFile2::set_PostData(LPCSTR psz)
{
	BOOL bPost = psz != NULL && *psz != 0;
	curl_easy_setopt (m_curl, CURLOPT_POST, bPost);
	if (bPost)
	{
		curl_easy_setopt (m_curl, CURLOPT_POSTFIELDS, psz);
		curl_easy_setopt (m_curl, CURLOPT_POSTFIELDSIZE, lstrlen (psz));
	}
}

void fsInternetFile2::ExtractFileInfoFromResponse()
{
	if (m_curl != NULL)
	{
		LPSTR psz = NULL;
		curl_easy_getinfo (m_curl, CURLINFO_CONTENT_TYPE, &psz);
		m_strContentType = psz ? psz : "";

		curl_easy_getinfo (m_curl, CURLINFO_FILETIME, &m_fileTime);

		double f;
		curl_easy_getinfo (m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &f);
		m_uFileSize = f != 0 ? (UINT64)f + m_uStartPos : _UI64_MAX;
	
		if (m_uStartPos != 0)
			m_enRST = RST_PRESENT;
	}
	else
	{
		m_uFileSize = 0;
	}
}

LPCSTR fsInternetFile2::get_ContentType()
{
	return m_strContentType;
}

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
  
  LONGLONG ll;

  ll = Int32x32To64(t, 10000000) + 116444736000000000;
  pft->dwLowDateTime = (DWORD)ll;
  pft->dwHighDateTime = (DWORD) (ll >> 32);
}

BOOL fsInternetFile2::GetLastModifiedDate(FILETIME *pTime)
{
	if (m_fileTime == -1)
		return FALSE;

	UnixTimeToFileTime (m_fileTime, pTime);

	return TRUE;
}

fsResumeSupportType fsInternetFile2::IsResumeSupported()
{
	return m_enRST;
}

size_t fsInternetFile2::_WriteHeader(void *ptr, size_t size, size_t nmemb, void *stream)
{
	fsInternetFile2* pthis = (fsInternetFile2*) stream;
	char sz [10000];
	strncpy (sz, (LPCSTR)ptr, size * nmemb);
	sz [size * nmemb] = 0;
	pthis->m_strHttpHeader += sz;
	return size * nmemb;
}

LPCSTR fsInternetFile2::get_SuggestedFileName()
{
	return m_strSuggestedFileName;
}

UINT64 fsInternetFile2::GetFileSize()
{
	return m_uFileSize;
}

void fsInternetFile2::setUseFtpAsciiMode(bool bUse)
{
	curl_easy_setopt (m_curl, CURLOPT_TRANSFERTEXT, bUse);
}

void fsInternetFile2::setInterface(LPCSTR psz)
{
	curl_easy_setopt (m_curl, CURLOPT_INTERFACE, psz);
}
#endif