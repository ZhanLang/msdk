#include "stdafx.h"
#include "HttpDownloadEx.h"
#include "P2PDownloader.h"


// --------------------------------------------------------
// class CHttpDownloadEx
LPHttpFileLengthListener CHttpDownloadEx::m_pfnFl = NULL;
LPHttpDownloadInfoListener CHttpDownloadEx::m_pfnDi = NULL;
LPHttpFileDownloadCompleteListener CHttpDownloadEx::m_pfnFdc = NULL;

BOOL CHttpDownloadEx::Notify(int msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_HTTPDOWN_NOTIFY_MSG:
		if(lp != NULL)
		{
			HTTPDOWNLOADSTATUS* ds = (HTTPDOWNLOADSTATUS*)lp;
			switch(ds->nStatusType)
			{
			case DOWNLOAD_STATUS_FILENAME:
				assert(m_context != NULL);
				if(m_context->FileName() == NULL)
				{
					// notice: file name is 'wchar_t'
					m_context->SetOpt(Opt_DownFileName, ds->strFileName.GetBuffer(), ds->strFileName.GetLength());
				}

				break;

			case DOWNLOAD_STATUS_FILESIZE:
				assert(m_context != NULL);
				if(m_pfnFl != NULL)
				{
					m_pfnFl(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), ds->dwFileSize);
				}

				break;

			case DOWNLOAD_STATUS_DOWNLOADEDSIZE:
				if(m_pfnDi != NULL && 
					(GetTickCount() - m_timer) >= (unsigned int)(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->Setting()->Interval()))
				{
					m_timer = GetTickCount();

					// calculate rate
					m_pfnDi(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), ds->dwFileDownloadedSize, 
						1/*m_context->DownRate()*/, m_context->DownTimes());
				}

				break;

			case DOWNLOAD_STATUS_DOWNLOADCOMPLETE:
				//if(m_pfnFdc != NULL && ds->dwFileDownloadedSize >= ds->dwFileSize)
				//{
				//	m_pfnFdc(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), ds->dwFileDownloadedSize);
				//}

				break;
			}
		}
		break;
	}

	return TRUE;
}

HRESULT CHttpDownloadEx::Download(LPCTSTR lpszURL, LPCTSTR lpszSaveFileName /* = NULL */, 
								BOOL bAutoRedirect /*= FALSE*/, LPCTSTR lpszResetHost/* = NULL*/)
{
	// assert(m_bStop == false);
	m_bStop = FALSE;

	// exist no download
	//MSP2PLOG(MSP2PLOG_NOTICE, "%S", lpszSaveFileName);
	if(CP2PClientTool::FindFileW(lpszSaveFileName))
	{
		if(NULL == m_context->FileName())
		{
			const wchar_t* p = wcsrchr(lpszURL, L'/');
			if(p != NULL && ++p != NULL)
			{
				m_context->SetOpt(Opt_DownFileName, (void*)p, wcslen(p));
			}
		}

		m_pfnFl(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), CP2PClientTool::GetFileSize(lpszSaveFileName));
		m_pfnFdc(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), 0);
		return S_OK;
	}

	HRESULT hr = ERR_HTTP_SUCCESS;
	int times = 0;
	do
	{
		hr = CHttpDownload::Download(lpszURL, lpszSaveFileName, TRUE, lpszResetHost);
		if(ERR_HTTP_SUCCESS != hr)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CHttpDownloadEx::Download() Failed, ErrNo = 0x%x, recount = %d", hr, times);
			Sleep(1000);
		}
	}while(ERR_HTTP_SUCCESS   != hr && 
		   ERR_HTTP_USER_STOP != hr && 
		   times++ < 2);

	m_pfnFdc(static_cast<CMsP2PHttpDownloaderContext*>(m_context)->UrlHash(), 
		ERR_HTTP_SUCCESS == hr && m_dwFileSize > 0 ? m_dwFileSize : -1);

	return hr;
}

void CHttpDownloadEx::StopDownload()
{
	CHttpDownload::StopDownload();
}
