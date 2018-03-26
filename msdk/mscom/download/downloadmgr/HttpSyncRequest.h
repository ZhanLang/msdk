#pragma once

#include "download/IDownload.h"
#include "mscom/srvbase.h"
class CHttpSyncRequest : 
	public IHttpSyncRequest,
	public CUnknownImp
{
public:
	UNKNOWN_IMP1_(IHttpSyncRequest);
	CHttpSyncRequest(void);
	~CHttpSyncRequest(void);

	STDMETHOD(HttpPost)(LPCWSTR lpszUrl, /*IMsBuffer*/ LPCWSTR lpszPath, LPCWSTR lpszParam, IMSBase** pBuffer );
	STDMETHOD(HttpRequest)(LPCWSTR lpszUrl, /*IMsBuffer*/IMSBase** pBuffer);
	STDMETHOD(HttpDownload)(LPCWSTR lpszUrl, LPCWSTR lpszSafeFile);
	fsInternetResult HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer, 
		fsHttpFile *pFile, LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);
	fsInternetResult HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser, 
		LPCTSTR pszPassword, fsHttpConnection *pServer, fsHttpFile *pFile, 
		LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);
};

