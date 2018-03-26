#include "StdAfx.h"
#include "FtpDownload.h"

CFtpDownload::CFtpDownload(void)
{
	m_pInternetSession = NULL;
	m_pFtpConnection   = NULL;
	m_pFtpFile		   = NULL;
	m_pSetting		   = NULL;
	m_bAbort           = FALSE;
	m_uFileSize        = 0;
	m_uDlSize		   = 0;
	m_uPos			   = 0;
	m_hSaveFile= NULL;
}

CFtpDownload::~CFtpDownload(void)
{

}

HRESULT CFtpDownload::Open(IDownloadTask*pTask,IDownLoadEventNotify* pEvent , LPCTSTR lpstrUrl,LPCTSTR lpstrSavePath)
{
	RASSERT(pTask&&pEvent&&lpstrUrl&&lpstrSavePath,E_FAIL);
	m_strUrl = lpstrUrl;
	m_strSavePath = lpstrSavePath;
	m_pNotify = pEvent;
	m_pInternetSession = new fsInternetSession();
	RASSERT(m_pInternetSession,E_FAIL);
	m_pSetting = pTask->GetSetting();

	fsInternetAccessType iat = IAT_NOPROXY;

	/*
	const IAT_ProxyInfo* proxyInfo = m_pSetting->GetProxyInfo();
	switch(proxyInfo->accessType)
	{
	case IAT_NO_PROXY:
		{
			iat= IAT_NOPROXY;
			break;
		}
	case IAT_PRECONFIG_PROXY:
		{
			iat = IAT_PRECONFIGPROXY;
			break;
		}
	case IAT_MANUAL_PROXY:
		{
			iat= IAT_MANUALPROXY;
			break;
		}
	}
	m_pInternetSession->SetProxyAuth(proxyInfo->szProxyUser,proxyInfo->szProxyPwd);
	*/
	m_pInternetSession->Create(_T("downmgr"),iat,NULL);

	m_pFtpConnection= new fsFtpConnection();
	m_pFtpFile = new fsFtpFile();

	RASSERT(m_pFtpConnection && m_pFtpFile,E_FAIL);
	m_pFtpConnection->Initialize(m_pInternetSession);

	return S_OK;
}

HRESULT CFtpDownload::Close()
{
	SAFE_DELETE(m_pInternetSession);
	SAFE_DELETE(m_pFtpConnection);
	SAFE_DELETE(m_pFtpFile);
	return S_OK;
}

HRESULT CFtpDownload::DeleteConfigFile()
{
	return S_OK;
}

HRESULT CFtpDownload::Abort()
{
	m_bAbort = TRUE;
	WriteFile(TRUE);
	if (m_pFtpFile)
	{
		m_pFtpFile->CloseHandle();
	}

	return S_OK;
}

UINT64  CFtpDownload::GetFileSize()
{
	return m_uFileSize;
}

VOID    CFtpDownload::DeleteThis()
{
	delete this;
}

DLError CFtpDownload::GetLastError()
{
	return 0;
}

HRESULT CFtpDownload::Download()
{
	m_bAbort = FALSE;
	m_pNotify->OnDownLoadEventNotify(DLEvent_Connecting,NULL,NULL);

	HRESULT hRet = Init();
	if (hRet != S_OK)
	{
		m_pNotify->OnDownLoadEventNotify(m_bAbort ? DLEvent_Abort:DLEvent_Error,NULL,NULL);
		return E_FAIL;
	}

	m_pNotify->OnDownLoadEventNotify(DLEvent_DownloadStart,(WPARAM)&m_uPos,(LPARAM)&m_uFileSize);

	BOOL bSuccess = FALSE;
	while(!m_bAbort)
	{
		BYTE pBuf[1000] = {0};
		DWORD dwRead = 0;
		fsInternetResult fsRet = m_pFtpFile->Read(pBuf,sizeof(pBuf),&dwRead);
		if (fsRet == IR_SUCCESS)
		{
			if (dwRead){
				{
					m_TempBuf.AddTail(pBuf,dwRead);
				}

				WriteFile(FALSE);
				m_pNotify->OnDownLoadEventNotify(DLEvent_Downloading,dwRead,(LPARAM)pBuf);
			}else{
				WriteFile(TRUE);
				bSuccess = TRUE;
				break;
			}
		}
		else
		{
			break;
		}
	}

	SAFE_CLOSEHANDLE(m_hSaveFile);
	if (bSuccess)
	{
		m_pNotify->OnDownLoadEventNotify(DLEvent_Done,NULL,NULL);
	}
	else
	{
		m_pNotify->OnDownLoadEventNotify(m_bAbort?DLEvent_Abort:DLEvent_Error,NULL,NULL);
	}
	return S_OK;
}

BOOL    CFtpDownload::IsAbort()
{
	return m_bAbort;
}

HRESULT CFtpDownload::Init()
{
	fsURL url;
	RASSERT(url.Crack(m_strUrl) == IR_SUCCESS,E_FAIL);
	fsInternetResult fsRet = m_pFtpConnection->Connect(url.GetHostName(),url.GetUserName(),url.GetPassword(),url.GetPort());
	RASSERT(fsRet == IR_SUCCESS, E_FAIL);
	m_pFtpFile->Initialize(m_pFtpConnection);
	fsRet = m_pFtpFile->Open(url.GetPath(),0);
	RASSERT(fsRet == IR_SUCCESS, E_FAIL);

	m_uFileSize = m_pFtpFile->GetFileSize();
	BOOL bResume = m_pFtpFile->IsResumeSupported() == RST_PRESENT ? TRUE : FALSE;
	if (!bResume || m_uPos > m_uFileSize)
	{
		DeleteFile(m_strSavePath);
		m_uPos = 0;
	}
	else
	{
		m_uFileSize = m_uPos = mspath::CPath::GetFileSize(m_strSavePath);
	}

	fsRet = m_pFtpFile->Open(url.GetPath(),m_uPos);
	//RASSERT(fsRet == IR_SUCCESS,E_FAIL);


	m_hSaveFile = CreateFile(m_strSavePath,GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	RASSERT(m_hSaveFile!=NULL && m_hSaveFile!=INVALID_HANDLE_VALUE , E_FAIL);

	LARGE_INTEGER point;
	ZeroMemory(&point,sizeof(point));
	point.QuadPart = m_uPos;
	SetFilePointer(m_hSaveFile,point.LowPart,&point.HighPart,FILE_BEGIN);

	return S_OK;

}

HRESULT CFtpDownload::WriteFile(BOOL bPush /*=FALSE*/)
{
	AUTOLOCK_CS(TempBuf);
	if (m_TempBuf.GetDataSize() >= Max_TempBufSize || bPush)
	{
		LPBYTE pTemBuf = 0;
		DWORD dwTempBufSize = 0;
		DWORD dwWrite = 0;
		m_TempBuf.GetRawBuffer(&pTemBuf,&dwTempBufSize);
		::WriteFile(m_hSaveFile,pTemBuf,dwTempBufSize,&dwWrite,NULL);
		m_TempBuf.Clear();
		FlushFileBuffers(m_hSaveFile);
	}

	return S_OK;
}