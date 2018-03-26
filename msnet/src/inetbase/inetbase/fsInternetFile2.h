/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#ifndef NOCURL

#if !defined(AFX_FSINTERNETFILE2_H__2F47FA5E_F9B6_4C4C_B002_66C62E904353__INCLUDED_)
#define AFX_FSINTERNETFILE2_H__2F47FA5E_F9B6_4C4C_B002_66C62E904353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <limits.h>
#include "fsinet.h"	
#include "array.h"
#include "fsInternetFile.h"	

class fsInternetFile2 : public fsSpeaking 
{
public:
	void setInterface (LPCSTR psz);
	void setUseFtpAsciiMode (bool bUse);
	UINT64 GetFileSize();
	LPCSTR get_SuggestedFileName();
	fsResumeSupportType IsResumeSupported();
	BOOL GetLastModifiedDate(FILETIME *pTime);
	LPCSTR get_ContentType();
	
	void set_PostData (LPCSTR psz);
	void set_Proxy (LPCSTR pszProxy, LPCSTR pszUser, LPCSTR pszPwd);
	void set_Auth (LPCSTR pszUser, LPCSTR pszPwd);
	void set_Cookie (LPCSTR psz);
	void set_Referer (LPCSTR psz);
	void set_UseHttp11 (BOOL bUse);
	void set_UserAgent (LPCSTR psz);
	void set_ResumeFrom (UINT64 uStart);
	void StopDownloading();
	fsInternetResult Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead);
	fsInternetResult StartDownloading ();
	
	void set_URL (LPCSTR pszURL);
	fsInternetResult Initialize();
	fsInternetFile2();
	virtual ~fsInternetFile2();

protected:
	fsResumeSupportType m_enRST;
	UINT64 m_uStartPos;
	BOOL m_bDoPause;
	UINT64 m_uFileSize;
	fsString m_strSuggestedFileName;
	fsString m_strHttpHeader;
	static size_t _WriteHeader(void *ptr, size_t size, size_t nmemb, void *stream);
	long m_fileTime;
	void ExtractFileInfoFromResponse();
	
	size_t OnWriteData (LPBYTE ptr, size_t size);
	
	
	struct fsNeedToRead {
		vms::array <BYTE> abData;	
		UINT nDataLeft;				
	} m_ReadReq;	
	
	HANDLE m_hevReadDataReq, m_hevReadDataDone;
	static fsInternetResult CURLcodeToIR (int code);
	fsInternetResult m_irLastError;
	
	static DWORD WINAPI _threadDownload (LPVOID lp);
	
	static size_t _WriteData (void *ptr, size_t size, size_t nmemb, void *stream);
	
	void* m_curl;
	
	bool m_bDownloading;
	
	bool m_bAnswrFromServRcvd;
	
	bool m_bNeedStop;
	fsString m_strContentType;
};

#endif 

#endif 
