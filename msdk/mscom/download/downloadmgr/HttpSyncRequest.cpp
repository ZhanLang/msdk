#include "StdAfx.h"
#include "HttpSyncRequest.h"
#include <vector>

CHttpSyncRequest::CHttpSyncRequest(void)
{
}


CHttpSyncRequest::~CHttpSyncRequest(void)
{
}

STDMETHODIMP CHttpSyncRequest::HttpRequest(LPCWSTR lpszUrl, /*IMsBuffer*/IMSBase** ppBuffer)
{
	RASSERT(lpszUrl && _tcslen(lpszUrl), E_INVALIDARG);

	UTIL::com_ptr<IMsBuffer> pBuffer;
	DllQuickCreateInstance(CLSID_MsBuffer, re_uuidof(IMsBuffer), pBuffer,NULL);
	RASSERT(pBuffer, E_FAIL);

	fsURL url;
	fsInternetResult fsRet = url.Crack(lpszUrl);
	if (fsRet != IR_SUCCESS)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsURL::Crack(%s)"), lpszUrl);
		return E_FAIL;
	}

	
	fsInternetSession Session;
	fsHttpConnection HttpConnection;
	fsHttpFile HttpFile;


	fsRet = Session.Create(_T("HttpSyncRequest"), IAT_NOPROXY, _T(""));
	fsRet = HttpConnection.Initialize(&Session);
	fsRet = HttpFile.Initialize(&HttpConnection);

	BOOL bRedirInner;
	LPTSTR pszNewUrl = NULL;

	fsRet = HttpOpenUrl(lpszUrl, url.GetUserName(), url.GetPassword(), &HttpConnection, &HttpFile, &pszNewUrl, &bRedirInner, 0);
	BYTE pBuf[1024] = {0};
	DWORD dwRead = 0;


	do 
	{
		dwRead = 0;
		fsRet = HttpFile.Read(pBuf, sizeof(pBuf), &dwRead);
		if (fsRet == IR_SUCCESS)
		{
			if (dwRead)
			{
				pBuffer->AddTail(pBuf, dwRead);
			}
			else //½áÊø
			{
				break;
			}
		}
		else
		{
			GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsHttpFile::Read(%s)"), lpszUrl);
			break;
		}
	} while (TRUE);
	

	if (fsRet == IR_SUCCESS)
	{
		pBuffer->QueryInterface(re_uuidof(IMsBuffer), (void**)ppBuffer);
		return S_OK;
	}

	GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest(%s)"), lpszUrl);
	return E_FAIL;
}


fsInternetResult CHttpSyncRequest::HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer,
	fsHttpFile *pFile, LPTSTR *ppRedirectedUrl,
	BOOL *pbRedirInner, UINT64 uPos /*=0*/)
{

	fsInternetResult ir;

	pFile->Initialize (pServer);

	*ppRedirectedUrl = NULL;


	ir = pFile->Open (pszPath, uPos);
	//m_strCookes = pFile->GetCookies();
	if (ir != IR_SUCCESS)
	{
		if (ir == IR_NEEDREDIRECT)
		{
			CString strUrl = pFile->GetLastError ();
			fsURL u;
			BOOL bRelUrl = FALSE;
			//strUrl+=pszPath;
			if (u.Crack (strUrl) != IR_SUCCESS)
				bRelUrl = TRUE;

			*pbRedirInner = TRUE;

			if (bRelUrl)
			{
				if (*pFile->GetLastError () != '/' && *pFile->GetLastError () != '\\')
				{

					strUrl = pszPath;
					int len = strUrl.GetLength();
					while (strUrl [len - 1] != '/' && strUrl [len - 1] != '\\')
						len--;
					strUrl.SetAt(len, '\0') ;
					strUrl += pFile->GetLastError ();

				}
				else
				{
					strUrl = pFile->GetLastError ();
				}




				ir = HttpOpenPath (strUrl, pServer, pFile, ppRedirectedUrl, pbRedirInner, uPos);
			}
			else
			{
				strUrl = pFile->GetLastError ();
				ir =  HttpOpenUrl (strUrl, NULL, NULL, pServer, pFile, ppRedirectedUrl, pbRedirInner, uPos);
			}


			if (*ppRedirectedUrl == NULL)
			{
				*ppRedirectedUrl = new TCHAR[strUrl.GetLength() + 1];

				lstrcpy (*ppRedirectedUrl, strUrl);
			}


			*pbRedirInner = *pbRedirInner && bRelUrl;
		}

		return ir;
	}

	return IR_SUCCESS;
}

fsInternetResult CHttpSyncRequest::HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser,
	LPCTSTR pszPassword, fsHttpConnection *pServer,
	fsHttpFile *pFile, LPTSTR *ppRedirectedUrl,
	BOOL *pbRedirInner , UINT64 uPos /*=0*/)
{
	fsURL url;
	fsInternetResult ir;

	ir = url.Crack (pszUrl);
	if (ir != IR_SUCCESS)
		return ir;

	ir = pServer->Connect (url.GetHostName (), pszUser ? pszUser : url.GetUserName (),
		pszPassword ? pszPassword : url.GetPassword (), url.GetPort ());

	if (ir != IR_SUCCESS)
		return ir;

	pFile->UseSecure (url.GetInternetScheme () == INTERNET_SCHEME_HTTPS);
	pFile->UseCookie(TRUE);
	//pFile->SetCookies(m_strCookes);
	return HttpOpenPath (url.GetPath (), pServer, pFile, ppRedirectedUrl, pbRedirInner, uPos);
}

STDMETHODIMP CHttpSyncRequest::HttpDownload(LPCWSTR lpszUrl, LPCWSTR lpszSafeFile)
{
	RASSERT(lpszUrl && _tcslen(lpszUrl) && lpszSafeFile && _tcslen(lpszSafeFile), E_INVALIDARG);
	
	mspath::CPath::CreateDirectoryEx(lpszSafeFile);

	fsURL url;
	fsInternetResult fsRet = url.Crack(lpszUrl);
	if (fsRet != IR_SUCCESS)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsURL::Crack(%s)"), lpszUrl);
		return E_FAIL;
	}


	fsInternetSession Session;
	fsHttpConnection HttpConnection;
	fsHttpFile HttpFile;


	fsRet = Session.Create(_T("HttpDownload"), IAT_NOPROXY, _T(""));
	fsRet = HttpConnection.Initialize(&Session);
	fsRet = HttpFile.Initialize(&HttpConnection);

	BOOL bRedirInner;
	LPTSTR pszNewUrl = NULL;

	CString strSaveTempFile = lpszSafeFile;
	strSaveTempFile += ".download";
	::DeleteFile(strSaveTempFile);

	UTIL::sentry<HANDLE, UTIL::handle_sentry> hSaveFile = CreateFile(strSaveTempFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hSaveFile == INVALID_HANDLE_VALUE)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpDownload::CreateFile(%s)"), strSaveTempFile.GetBuffer());
		return E_FAIL;
	}

	fsRet = HttpOpenUrl(lpszUrl, url.GetUserName(), url.GetPassword(), &HttpConnection, &HttpFile, &pszNewUrl, &bRedirInner, 0);
	BYTE pBuf[1024] = {0};
	DWORD dwRead = 0;


	do 
	{
		dwRead = 0;
		fsRet = HttpFile.Read(pBuf, sizeof(pBuf), &dwRead);
		if (fsRet == IR_SUCCESS)
		{
			if (dwRead)
			{
				DWORD dwWrite = 0;
				WriteFile(hSaveFile, pBuf, dwRead, &dwWrite, NULL);
				FlushFileBuffers(hSaveFile);
			}
			else //½áÊø
			{
				break;
			}
		}
		else
		{
			GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsHttpFile::Read(%s)"), lpszUrl);
			break;
		}
	} while (TRUE);


	hSaveFile = NULL;
	if (fsRet == IR_SUCCESS)
	{
		DeleteFile(lpszSafeFile);
		if (MoveFile(strSaveTempFile, lpszSafeFile))
		{
			return S_OK;
		}
	}

	GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest(%s)"), lpszUrl);
	return E_FAIL;
}

STDMETHODIMP CHttpSyncRequest::HttpPost(LPCWSTR lpszUrl,  LPCWSTR lpszPath, LPCWSTR lpszParam,/*IMsBuffer*/IMSBase** ppBuffer)
{
	RASSERT(lpszUrl && _tcslen(lpszUrl), E_INVALIDARG);

	UTIL::com_ptr<IMsBuffer> pBuffer;
	DllQuickCreateInstance(CLSID_MsBuffer, re_uuidof(IMsBuffer), pBuffer,NULL);
	RASSERT(pBuffer, E_FAIL);

	fsURL url;
	fsInternetResult fsRet = url.Crack(lpszUrl);
	if (fsRet != IR_SUCCESS)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsURL::Crack(%s)"), lpszUrl);
		return E_FAIL;
	}


	UTIL::sentry<fsInternetSession* ,UTIL::default_sentry> Session = new fsInternetSession();
	UTIL::sentry<fsHttpConnection* ,UTIL::default_sentry> HttpConnection = new fsHttpConnection();
	UTIL::sentry<fsHttpFile* ,UTIL::default_sentry> HttpFile = new fsHttpFile();
	//fsInternetSession Session;
	//fsHttpConnection HttpConnection;
	//fsHttpFile HttpFile;


	fsRet = Session->Create(_T("HttpSyncRequest"), IAT_NOPROXY, _T(""));
	fsRet = HttpConnection->Initialize(Session);

	HttpFile->Initialize(HttpConnection);
	HttpFile->SetPostData(lpszParam);

	fsRet = HttpConnection->Connect(url.GetHostName (),  url.GetUserName (),url.GetPassword (), url.GetPort ());
	if (fsRet != IR_SUCCESS)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpConnection::Connect(%s)"), lpszUrl);
		return E_FAIL;
	}


	//fsRet = HttpFile.Initialize(&HttpConnection);
	
	fsRet = HttpFile->Open(lpszPath, 0);
	if (fsRet != IR_SUCCESS)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpFile::Open(%s)"), lpszUrl);
		return E_FAIL;
	}

	BYTE pBuf[1024] = {0};
	DWORD dwRead = 0;


	do 
	{
		dwRead = 0;
		fsRet = HttpFile->Read(pBuf, sizeof(pBuf), &dwRead);
		if (fsRet == IR_SUCCESS)
		{
			if (dwRead)
			{
				pBuffer->AddTail(pBuf, dwRead);
			}
			else //½áÊø
			{
				break;
			}
		}
		else
		{
			GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest::fsHttpFile::Read(%s)"), lpszUrl);
			break;
		}
	} while (TRUE);


	if (fsRet == IR_SUCCESS)
	{
		pBuffer->QueryInterface(re_uuidof(IMsBuffer), (void**)ppBuffer);
		return S_OK;
	}

	
	GrpError(GroupName, MsgLevel_Error, _T("CHttpSyncRequest::HttpRequest(%s)"), lpszUrl);
	return E_FAIL;
}
