#include "StdAfx.h"
#include "HttpDownload.h"
#include "SpeedReporter.h"


CHttpDownload::CHttpDownload()
{
	
}

CHttpDownload::~CHttpDownload(void)
{
}

STDMETHODIMP CHttpDownload::Open(IDownLoadEventNotify * pEvent, 
				LPCWSTR lpstrUrl,
				LPCWSTR lpstrSavePath,
				DWORD dwStartPos/*= 0*/,
				DWORD dwEndPos /*= -1*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpDownload::Close()
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpDownload::DeleteConfigFile()
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpDownload::Abort()
{
	return E_NOTIMPL;
}


STDMETHODIMP CHttpDownload::Download()
{
	return E_NOTIMPL;
}

STDMETHODIMP_(UINT64) CHttpDownload::GetFileSize()
{
	return E_NOTIMPL;
}

STDMETHODIMP_(UINT64) CHttpDownload::GetDownloadSize()
{
	return E_NOTIMPL;
}	

STDMETHODIMP_(DownloadType) CHttpDownload::GetDownloadType()
{
	return E_NOTIMPL;
}

STDMETHODIMP_(BOOL) CHttpDownload::IsAbort()
{
	return E_NOTIMPL;
}



STDMETHODIMP CHttpDownload::SetOption(DWORD option, LPVOID lpData, DWORD dwLen, BOOL bSave /*= FALSE*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CHttpDownload::GetOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen)
{
	return E_NOTIMPL;
}


//ISpeed
STDMETHODIMP_(DWORD) CHttpDownload::GetSpeed()
{
	return E_NOTIMPL;
}

//IDownLoadEventNotify
//这里需要实现一大堆的分片下载调度逻辑，
HRESULT CHttpDownload::OnDownLoadEventNotify(DownloaderEvent dlEvent, WPARAM wp, LPARAM lp)
{
	switch(dlEvent)
	{
	case DLEvent_Connecting:
	case DLEvent_Downloading:
	case DLEvent_DownloadStart:
	case DLEvent_Done:
	case DLEvent_Abort:
	case DLEvent_Error:
	case DLEvent_Speed:
		break;
	}
	return S_OK;
}

fsInternetResult CHttpDownload::HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer,
        fsHttpFile *pFile, LPTSTR *ppRedirectedUrl,
        BOOL *pbRedirInner, UINT64 uPos /*=0*/)
{

    fsInternetResult ir;

    pFile->Initialize (pServer);

    *ppRedirectedUrl = NULL;


    ir = pFile->Open (pszPath, uPos);
    //m_strCookes = pFile->GetCookies(){return E_NOTIMPL;}
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

fsInternetResult CHttpDownload::HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser,
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
   // pFile->SetCookies(m_strCookes){return E_NOTIMPL;}
    return HttpOpenPath (url.GetPath (), pServer, pFile, ppRedirectedUrl, pbRedirInner, uPos);
}



