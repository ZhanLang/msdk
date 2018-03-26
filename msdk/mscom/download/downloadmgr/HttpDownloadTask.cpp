#include "StdAfx.h"
#include "HttpDownloadTask.h"
#include "Crc32.h"

CHttpDownloadTask::CHttpDownloadTask(void)
{
	m_dwTaskID = 0;
	m_hDownloadThread = NULL;
	m_dwDownloadThreadID = 0;
	m_hDownloadExitEvent = 0;
	m_dlTaskState = DL_TaskState_UnKnown;
	m_dlError = DLE_SUCCESS;
	m_bAddToStore = FALSE;

	m_pSession = NULL;
	m_pHttpConnection = NULL;
	m_pHttpFile = NULL;
	m_llTotalSize	= 0;
	m_llCurSize = 0;

	m_TempBuf.Reserve(Max_TempBufSize);
}


CHttpDownloadTask::~CHttpDownloadTask(void)
{
	DestroyTask();
}

STDMETHODIMP_(LPCWSTR) CHttpDownloadTask::GetUrl()
{
	return m_strUrl;
}

STDMETHODIMP_(LPCWSTR		) CHttpDownloadTask::GetSavePath()
{
	return m_strSavePath;
}

STDMETHODIMP_(DL_TaskType	) CHttpDownloadTask::GetTaskType()
{
	CStringW strUrl = m_strUrl;
	strUrl.MakeLower();
	if (strUrl.Find(_T("http://")) == 0)
	{
		return DL_TaskType_Http;
	}

	if (strUrl.Find(_T("https://")) == 0)
	{
		return DL_TaskType_Https;
	}

	return DL_TaskType_Http;
}

STDMETHODIMP_(LONGLONG		) CHttpDownloadTask::GetTotalFileSize()
{
	return m_llTotalSize;
}

STDMETHODIMP_(LONGLONG		) CHttpDownloadTask::GetCurrentFileSize()
{
	return m_llCurSize;
}

STDMETHODIMP_(LPCWSTR		) CHttpDownloadTask::GetProxyService()
{
	return m_strProxyService;
}

STDMETHODIMP_(LPCWSTR		) CHttpDownloadTask::GetProxyUser()
{
	return m_strProxyUser;
}

STDMETHODIMP_(LPCWSTR		) CHttpDownloadTask::GetProxyPwd()
{
	return m_strProxyPwd;
}

STDMETHODIMP_(DWORD		) CHttpDownloadTask::GetTaskId()
{
	return m_dwTaskID;
}

STDMETHODIMP_(DL_Error		) CHttpDownloadTask::GetTaskError()
{
	return m_dlError;
}

STDMETHODIMP CHttpDownloadTask::SetCookie( LPCWSTR lpszCookie )
{
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::SetProxyService( LPCWSTR lpszService )
{
	RASSERT(lpszService, E_INVALIDARG);
	m_strProxyService = lpszService;
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::SetProxyUser( LPCWSTR lpszUsr )
{
	RASSERT(lpszUsr, E_INVALIDARG);
	m_strProxyUser = lpszUsr;
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::SetProxyPwd( LPCWSTR lpszPwd )
{
	RASSERT(lpszPwd, E_INVALIDARG);
	m_strProxyPwd = lpszPwd;

	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::CreateTask( LPCWSTR lpszUrl, LPCWSTR lpszSavePath, IDownloadNotify* pNotify , DWORD dwTaskId)
{
	if (!(lpszUrl && lpszSavePath && pNotify))
	{
		m_dlTaskState = DL_TaskState_Error;
		return E_INVALIDARG;
	}

	m_dlNotify = pNotify;
	m_dwTaskID = dwTaskId;

	m_strUrl = lpszUrl;
	m_strSavePath = lpszSavePath;
	
	m_CreateFileTime = UINT64ToFileTime(GetCurrentFileTime());
	
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::DestroyTask()
{

	Stop();

	

	SAFE_RELEASE(m_dlNotify);
	SAFE_RELEASE(m_pRot);
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::Start()
{
	if (m_hDownloadExitEvent)
	{
		return S_OK;
	}

	m_hDownloadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	RASSERT(m_hDownloadExitEvent, E_FAIL);

	m_dlError = DLE_SUCCESS;
	m_hDownloadThread = CreateThread(NULL, 0, &CHttpDownloadTask::_DownloadThead, this, 0, &m_dwDownloadThreadID);
	if (!m_hDownloadThread)
	{
		SAFE_CLOSEHANDLE(m_hDownloadExitEvent);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::Stop()
{
	RASSERT(m_hDownloadExitEvent && m_hDownloadThread, S_OK);
	if (m_hDownloadExitEvent)
	{
		SetEvent(m_hDownloadExitEvent);
	}

	SendNotify(DL_NotifyEvnet_Stop);
	//等两秒时间
	DWORD dwWait = WaitForSingleObject(m_hDownloadThread, 1000*2);
	if (dwWait == WAIT_TIMEOUT)
	{
		TerminateThread(m_hDownloadThread, -1);
		GrpError(GroupName, MsgLevel_Error, _T("Stop Task (%s) timeout."), m_strUrl.GetBuffer());
	}
	CloseDownloadSession();
	
	return S_OK;
}

STDMETHODIMP_(DL_TaskState) CHttpDownloadTask::GetTaskState()
{
	return m_dlTaskState;
}

DWORD WINAPI CHttpDownloadTask::_DownloadThead( LPVOID lpThread )
{
	CHttpDownloadTask* pDl = static_cast<CHttpDownloadTask*>(lpThread);
	return pDl->DownloadThead();
}

DWORD CHttpDownloadTask::DownloadThead()
{

	BOOL bStop = FALSE;
	//连接到服务器
	m_dlError = DLE_SUCCESS;
	SendNotify(DL_NotifyEvnet_Connecting);	//连接通知
	if (!CreateInternetSession())
	{
		SendNotify(DL_NotifyEvent_Error);
		return 0;
	}

	//建立连接
	if (!ConnectToService())
	{
		SendNotify(DL_NotifyEvent_Error);
		return 0;
	}

	if (!CreateCurrentFile())
	{
		SendNotify(DL_NotifyEvent_Error);
		return 0;
	}

	SendNotify(DL_NotifyEvnet_DownloadStart); //开始下载通知
	
	this->SetTaskError(DLE_SUCCESS);
	

	while(TRUE)
	{
		DWORD dwRet = WaitForSingleObject(m_hDownloadExitEvent, 0);
		if (dwRet != WAIT_TIMEOUT)
		{
			bStop = TRUE;
			break;
		}

		BYTE pBuf[1024 * 10] = {0};
		DWORD dwRead = 0;
		fsInternetResult fsRet = m_pHttpFile->Read(pBuf, sizeof(pBuf), &dwRead);
		if (fsRet == IR_SUCCESS)
		{
			m_llCurSize += dwRead;
			m_speedMeter.Done(dwRead);
			m_TempBuf.AddTail(pBuf, dwRead);
			if (dwRead)
			{
				SendNotify(DL_NotifyEvnet_Downloading);
				WriteFile(FALSE);
			}
			else //结束
			{
				WriteFile(TRUE);
				break;
			}
		}
		else
		{
			GrpError(GroupName, MsgLevel_Error, _T("CHttpDownloadTask::DownloadThead::Read(%s)"), m_strUrl.GetBuffer());
			m_dlError = (DL_Error)fsRet;
			break;
		}
	}

	WriteFile(TRUE);

	SAFE_CLOSEHANDLE(m_hSaveFile);
	if (m_dlError == DLE_SUCCESS)
	{
		if (bStop)
		{
			SendNotify(DL_NotifyEvnet_Stop);
		}
		else
		{
			::DeleteFile(m_strSavePath);
			BOOL bRet = ::MoveFile(m_strTempSafePath, m_strSavePath);
			if (bRet)
			{
				SendNotify(DL_NotifyEvnet_Complete);
			}
			else
			{
				GrpError(GroupName, MsgLevel_Error, _T("CHttpDownloadTask::DownloadThead::MoveFile(%s,%s)"), m_strTempSafePath.GetBuffer(), m_strSavePath.GetBuffer());
				m_dlError = DLE_WINERROR;
				SendNotify(DL_NotifyEvent_Error);
			}
		}
	}
	else
	{
		SendNotify(DL_NotifyEvent_Error);
	}

	CloseDownloadSession();
	
	return 0;
}

BOOL CHttpDownloadTask::ConnectToService()
{
	BOOL bRedirInner;
	LPTSTR pszNewUrl = NULL;
	fsInternetResult fsRet = HttpOpenUrl(m_strUrl, m_fsUrl.GetUserName(), m_fsUrl.GetPassword(), m_pHttpConnection, m_pHttpFile, &pszNewUrl, &bRedirInner, 0);
	if (fsRet != IR_SUCCESS)
	{
		m_dlError = (DL_Error)fsRet;
		return FALSE;
	}

	m_llTotalSize = m_pHttpFile->GetFileSize();

	return TRUE;
}

BOOL CHttpDownloadTask::DwonloadToFile()
{
	return 0;
}

STDMETHODIMP CHttpDownloadTask::SetCreateTime( const FILETIME& time )
{
	if (time.dwHighDateTime || time.dwLowDateTime)
	{
		m_CreateFileTime = time;
	}
	else
	{
		m_CreateFileTime = UINT64ToFileTime(GetCurrentFileTime());
	}
	return S_OK;
}

STDMETHODIMP_(FILETIME		) CHttpDownloadTask::GetCreateTime()
{
	return m_CreateFileTime;
}

STDMETHODIMP CHttpDownloadTask::SetTaskState( DL_TaskState state )
{
	m_dlTaskState = state;
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::SetTaskError( DL_Error error )
{
	m_dlError = error;
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::set_AddToStore( BOOL bAdd )
{
	m_bAddToStore = bAdd;
	return S_OK;
}

STDMETHODIMP_(BOOL) CHttpDownloadTask::get_AddToStore()
{
	return m_bAddToStore;
}

BOOL CHttpDownloadTask::CreateInternetSession()
{
	RTEST(m_pSession, TRUE);


	fsInternetResult fsRet = m_fsUrl.Crack(m_strUrl);
	if(fsRet  != IR_SUCCESS)
	{
		m_dlError = (DL_Error)fsRet;
		return FALSE;
	}

	m_pSession = new fsInternetSession();
	fsInternetAccessType iat = IAT_NOPROXY;
	if(m_strProxyService.GetLength())
	{
		iat = IAT_MANUALPROXY;
	}

	m_pSession->SetProxyAuth(m_strProxyUser, m_strProxyPwd);
	fsRet = m_pSession->Create(_T("downmgr"), iat, m_strProxyService);
	if(fsRet != IR_SUCCESS)
	{
		m_dlError = (DL_Error)fsRet;
		return FALSE;
	}

	if (!m_pHttpConnection)
	{
		m_pHttpConnection = new fsHttpConnection();
		fsRet = m_pHttpConnection->Initialize(m_pSession);
		if (fsRet != IR_SUCCESS)
		{
			m_dlError = (DL_Error)fsRet;
			return FALSE;
		}
	}
	if (!m_pHttpFile)
	{
		m_pHttpFile = new fsHttpFile();
		fsRet = m_pHttpFile->Initialize(m_pHttpConnection);
		if (fsRet != IR_SUCCESS)
		{
			m_dlError = (DL_Error)fsRet;
			return FALSE;
		}
	}


	return TRUE;

}




fsInternetResult CHttpDownloadTask::HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer,
	fsHttpFile *pFile, LPTSTR *ppRedirectedUrl,
	BOOL *pbRedirInner, UINT64 uPos /*=0*/)
{

	fsInternetResult ir;

	pFile->Initialize (pServer);

	*ppRedirectedUrl = NULL;


	ir = pFile->Open (pszPath, uPos);
	m_strCookes = pFile->GetCookies();
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

fsInternetResult CHttpDownloadTask::HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser,
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

BOOL CHttpDownloadTask::CreateCurrentFile()
{
	msapi::CreateDirectoryEx(m_strSavePath);
	m_strTempSafePath  = m_strSavePath + _T(".dltmp");
	m_llCurSize = msapi::GetFileSize(m_strTempSafePath);

	//断点续传
	BOOL bResume = m_pHttpFile->IsResumeSupported() == RST_PRESENT ? TRUE : FALSE;
	if (bResume && m_llCurSize < m_llTotalSize)
	{
		LPTSTR pszNewUrl = NULL;
		BOOL bRedirInner;
		
		//重新打开开
		fsInternetResult fsRet = HttpOpenUrl(m_strUrl, m_fsUrl.GetUserName(), m_fsUrl.GetPassword(), m_pHttpConnection, m_pHttpFile, &pszNewUrl, &bRedirInner, m_llCurSize);
		if(fsRet != IR_SUCCESS)
		{
			m_dlError = (DL_Error)fsRet;
			return FALSE;
		}
	}
	else
	{
		m_llCurSize = 0;
		DeleteFile(m_strSavePath);
		DeleteFile(m_strTempSafePath);
	}

	m_hSaveFile = CreateFile(m_strTempSafePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == m_hSaveFile)
	{
		m_dlError = DLE_FILENOTOPENED;
		return FALSE;
	}

	LARGE_INTEGER point;
	ZeroMemory(&point, sizeof(point));
	point.QuadPart = m_llCurSize;
	SetFilePointer(m_hSaveFile, point.LowPart, &point.HighPart, FILE_BEGIN);

	return TRUE;
}

BOOL CHttpDownloadTask::WriteFile(BOOL bPush /*=FALSE*/)
{
	RASSERT(m_hSaveFile , E_INVALIDARG);

	AUTOLOCK_CS(m_TempBuf);
	if (m_TempBuf.GetDataSize() >= Max_TempBufSize || bPush)
	{
		LPBYTE pTemBuf = 0;
		DWORD dwTempBufSize = 0;
		DWORD dwWrite = 0;
		m_TempBuf.GetRawBuffer(&pTemBuf, &dwTempBufSize);
		::WriteFile(m_hSaveFile, pTemBuf, dwTempBufSize, &dwWrite, NULL);
		m_TempBuf.Clear();
		FlushFileBuffers(m_hSaveFile);
	}

	return TRUE;
}

STDMETHODIMP_(DWORD) CHttpDownloadTask::GetSpeed()
{
	 DWORD dwSpeed = m_speedMeter.GetSpeed();
	 m_speedMeter.Reset();
	 return dwSpeed;
}

STDMETHODIMP_(DWORD) CHttpDownloadTask::GetSpeed2()
{
	DWORD dwSpeed = m_speedMeter.GetSpeed();
	return dwSpeed;
}

VOID CHttpDownloadTask::SendNotify( DL_NotifyEvnet event )
{
	switch(event)
	{
	case DL_NotifyEvnet_Connecting:
		{
			m_dlTaskState = DL_TaskState_Connecting;
			break;
		}
	case DL_NotifyEvnet_Downloading:
		{
			m_dlTaskState = DL_TaskState_Downloading;
			break;
		}
	case DL_NotifyEvnet_DownloadStart:
		{
			break;
		}
	case DL_NotifyEvnet_Complete:
		{
			m_dlTaskState = DL_TaskState_Done;
			break;
		}
	case DL_NotifyEvnet_Stop:
		{
			m_dlTaskState = DL_TaskState_Stoped;
			break;
		}
	case DL_NotifyEvent_Error:
		{
			m_dlTaskState = DL_TaskState_Error;
			CloseDownloadSession();
			break;
		}
	}

	m_dlNotify->OnDownloadNotify(event, m_dwTaskID);
}

VOID CHttpDownloadTask::CloseDownloadSession()
{
	SAFE_CLOSEHANDLE(m_hDownloadThread);
	SAFE_CLOSEHANDLE(m_hDownloadExitEvent);

	if (m_pHttpFile)
	{
		//m_pHttpFile->CloseHandle();
		m_pHttpFile->DeleteLatter();
		m_pHttpFile = NULL;
	}

	if (m_pHttpConnection)
	{
		m_pHttpConnection->DeleteLatter();
		m_pHttpConnection = NULL;
	}


	if (m_pSession)
	{
		m_pSession->CloseHandle();
		delete m_pSession;
		m_pSession = NULL;
	}
}

STDMETHODIMP CHttpDownloadTask::SetTotalSize(LONGLONG llTotalSize)
{
	m_llTotalSize = llTotalSize;
	return S_OK;
}

STDMETHODIMP CHttpDownloadTask::SetCurSize(LONGLONG llCurSize)
{
	m_llCurSize = llCurSize;
	return S_OK;
}

STDMETHODIMP_(DWORD) CHttpDownloadTask::GetEngine()
{
	return DL_EngineHttp;
}
