/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSHTTPFILE_H__28FE56A5_B9BC_43B2_A475_16CC7616FC32__INCLUDED_)
#define AFX_FSHTTPFILE_H__28FE56A5_B9BC_43B2_A475_16CC7616FC32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 
#include <string>
#include "fsHttpFile_i.h"
#include "fsInternetFile.h"
#include "fsinet.h"	



class fsHttpFile : public fsHttpFile_i ,public fsInternetFile  ,CUnknownImp
{
public:
	UNKNOWN_IMP2_(fsInternetFile_i,fsHttpFile_i);
	//设置字符集
	void set_Charset (LPCSTR psz);

	//是否自动重定向
	void set_EnableAutoRedirect (BOOL b);

	std::string GetCookiesFromResponse();
	void set_UseMultipart (BOOL b);
	fsInternetResult Write (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten);
	fsInternetResult OpenEx(LPCSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize = _UI64_MAX, UINT64 uUploadTotalSize = _UI64_MAX);
	
	
	void SetAdditionalHeaders(LPCSTR pszAdditionalHeaders);
	
	LPCSTR GetPostData();
	
	LPCSTR GetCookies();
	void SetPostData (LPCSTR pszPostData);
	void SetCookies (LPCSTR pszCookies);
	
	BOOL get_UseCookie();
	
	LPCSTR get_Referer ();
	
	BOOL get_UseHttp11();
	
	BOOL Is_Secure();
	
	fsInternetResult QuerySize (LPCSTR pszFilePath);
	
	
	void RetreiveHeadersOnly(BOOL b = TRUE);
	
	void UseCookie (BOOL bUse);
	
	void UseSecure (BOOL bUse);
	
	void SetReferer (LPCSTR pszReferer);
	
	void UseHttp11 (BOOL bUse = TRUE);
	
	fsInternetResult Open (LPCSTR pszFilePath, UINT64 uStartPos);
	
	fsInternetResult Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead);
	fsHttpFile();
	virtual ~fsHttpFile();

protected:
	fsString m_strCharset;
	BOOL m_bEnableAutoRedirect;
	BOOL m_bUseMultipart;
	fsString m_strLabel;
	UINT64 m_uLeftToUpload;
	
	void IgnoreSecurityProblems ();
	fsInternetResult Open_imp (LPCSTR pszFilePath, UINT64 uStartPos, int cTryings);
	
	
	
	void RetreiveSuggFileName();
	
	fsInternetResult ProcessRangesResponse();
	
	fsInternetResult SetupProxy();

	
	LPSTR m_pszReferer;			
	BOOL m_bHeadersOnly;		
	DWORD m_dwFlags;			
	LPSTR m_pszHttpVersion;		
	LPSTR m_pszCookies;			
	LPSTR m_pszPostData;		
	LPSTR m_pszAdditionalHeaders; 
};

#endif 
