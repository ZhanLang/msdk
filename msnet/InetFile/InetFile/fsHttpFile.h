
#pragma once
#include "fsInternetFile.h"
#include "fsinet.h"	

class fsHttpFile : public fsInternetFile  
{
public:
	VOID set_Charset (LPCTSTR psz);
	VOID set_EnableAutoRedirect (BOOL b);
	fsString GetCookiesFromResponse();
	VOID set_UseMultipart (BOOL b);
	fsInternetResult Write (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten);
	fsInternetResult OpenEx(LPCTSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize = _UI64_MAX, UINT64 uUploadTotalSize = _UI64_MAX);
	VOID SetAdditionalHeaders(LPCTSTR pszAdditionalHeaders);
	LPCTSTR GetPostData();
	LPCTSTR GetCookies();
	VOID SetPostData (LPCTSTR pszPostData);
	VOID SetCookies (LPCTSTR pszCookies);
	BOOL get_UseCookie();
	LPCTSTR get_Referer ();
	BOOL get_UseHttp11();
	BOOL Is_Secure();
	fsInternetResult QuerySize (LPCTSTR pszFilePath);
	VOID RetreiveHeadersOnly(BOOL b = TRUE);
	VOID UseCookie (BOOL bUse);
	VOID UseSecure (BOOL bUse);
	VOID SetReferer (LPCTSTR pszReferer);
	VOID UseHttp11 (BOOL bUse = TRUE);
	fsInternetResult Open (LPCTSTR pszFilePath, UINT64 uStartPos);
	fsInternetResult Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead);
	fsHttpFile();
	virtual ~fsHttpFile();

protected:
	fsString m_strCharset;
	BOOL m_bEnableAutoRedirect;
	BOOL m_bUseMultipart;
	fsString m_strLabel;
	UINT64 m_uLeftToUpload;
	VOID IgnoreSecurityProblems ();
	fsInternetResult Open_imp (LPCTSTR pszFilePath, UINT64 uStartPos, int cTryings);
	VOID RetreiveSuggFileName();
	fsInternetResult ProcessRangesResponse();
	fsInternetResult SetupProxy();
	LPTSTR m_pszReferer;			
	BOOL m_bHeadersOnly;		
	DWORD m_dwFlags;			
	LPTSTR m_pszHttpVersion;		
	LPTSTR m_pszCookies;			
	LPTSTR m_pszPostData;		
	LPTSTR m_pszAdditionalHeaders; 
};

