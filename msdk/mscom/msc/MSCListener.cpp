#include "StdAfx.h"
#include <queue>
#include "MSCListener.h"
#include "Profile.h"
#include "SMSCSubscriberQueue.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


CMSCListener* CMSCListener::s_this = NULL;
/////////////////////////////////////////////////////////////////////////////
CMSCListener::CMSCListener()
{
	m_bExitting    = FALSE;
	m_hAcceptEvent = NULL;

    m_hEvent = NULL;

	m_hPipeAccept  = INVALID_HANDLE_VALUE;
	m_lpszPipeName = NULL;
	ZeroMemory(&m_overlappedAccept, sizeof(OVERLAPPED));
}

/////////////////////////////////////////////////////////////////////////////
CMSCListener::~CMSCListener()
{
	Uninitialize();
	s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMSCListener* CMSCListener::GetInstance()
{
	if (NULL == s_this)
	{
		s_this = new CMSCListener();
	}

	return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::DestroyInstance()
{
	SAFE_DELETE(s_this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::Initialize()
{
    m_bStartResult = FALSE;

	_ASSERT(NULL == m_lpszPipeName);
	_ASSERT(NULL == m_hAcceptEvent);

	LPCTSTR lpszPipeName = CProfile::GetInstance()->GetPipeName();
	_ASSERT(NULL != lpszPipeName);
	DWORD dwLen = _tcslen(lpszPipeName) + 1;
	m_lpszPipeName = new TCHAR[dwLen];
	if (NULL == m_lpszPipeName)
	{
		return FALSE;
	}
	m_lpszPipeName[dwLen - 1] = _T('\0');
	_tcscpy_s(m_lpszPipeName, dwLen,lpszPipeName);

	m_hAcceptEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hAcceptEvent)
	{
		SAFE_DELETE_BUFFER(m_lpszPipeName);
		return FALSE;
	}
	m_overlappedAccept.hEvent = m_hAcceptEvent;
	m_bExitting = FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::Uninitialize()
{
	Stop();

	_ASSERT(INVALID_HANDLE_VALUE == m_hPipeAccept);
	_ASSERT(0 == m_listSPipe.size());

	if (NULL != m_hAcceptEvent)
	{
		CloseHandle(m_hAcceptEvent);
		m_hAcceptEvent = NULL;
	}

    if(NULL != m_hEvent)
    {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }

	SAFE_DELETE_BUFFER(m_lpszPipeName);

}

BOOL CMSCListener::Start()
{
    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_hEvent)
    {
       //ZM1_GrpDbg(GroupName,_T("[MSC]CMSCListener::Start() CreateEvent失败 0\n"));
       return FALSE;
    }

    if(! XThread::Start())
    {
       //ZM1_GrpDbg(GroupName,_T("[MSC]CMSCListener::Start() 失败 0\n"));
       return FALSE;
    }

    //要等待线程起来，知道管道打开的结果
    DWORD dwResult = WaitForSingleObject(m_hEvent,INFINITE);
    if (dwResult == WAIT_OBJECT_0)
    {
        if( ! m_bStartResult)
        {
            //ZM1_GrpDbg(GroupName,_T("[MSC]CMSCListener::Start() 失败\n"));
            return FALSE;
        }
    }
    else
    {
        //ZM1_GrpDbg(GroupName,_T("[MSC]CMSCListener::Start() 失败\n"));
        return FALSE;
    }
    return TRUE;
}

VOID CMSCListener::Stop()
{
    m_bStartResult = FALSE;

    XThread::Stop();

    if(NULL != m_hEvent)
    {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
}
/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::SVC()
{
	if (!PendingAcceptEx())
	{
        m_bStartResult = FALSE;
        SetEvent(m_hEvent);
		return;
	}

	_ASSERT(NULL != m_hAcceptEvent);
	HANDLE hWaits[2];
	hWaits[0] = m_hExitEvent;
	hWaits[1] = m_hAcceptEvent;
	
    m_bStartResult = TRUE;
    SetEvent(m_hEvent);

	while (TRUE)
	{
		DWORD dwResult = WaitForMultipleObjectsEx(2, hWaits, FALSE, MSC_LOGONCHECKPERIOD, TRUE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			m_bExitting = TRUE;
			break;
		
		case WAIT_OBJECT_0 + 1:
			// Not need this: if (!Accept()) return;
			Accept();
			if (!PendingAcceptEx())
			{
				m_bExitting = TRUE;
			}
			break;
		
		case WAIT_IO_COMPLETION:
			break;
		
		case WAIT_TIMEOUT:
			LoginCheck();
			break;
		
		default:
			_ASSERT(FALSE);
		}

		if (m_bExitting)
		{
			break;
		}
	}

	std::list<CSPipe*>::iterator it;
	std::queue<CSPipe*> queueSPipe;
	for (it = m_listSPipe.begin(); it != m_listSPipe.end(); it++)
	{
		queueSPipe.push((*it));
	}
	
	CSPipe* lpSPipe = NULL;
	while (!queueSPipe.empty())
	{
		lpSPipe = queueSPipe.front();
		queueSPipe.pop();
		
		CancelIo(lpSPipe->Handle());
	}

	if (INVALID_HANDLE_VALUE != m_hPipeAccept)
	{
		CancelIo(m_hPipeAccept);
		CloseHandle(m_hPipeAccept);
		m_hPipeAccept = INVALID_HANDLE_VALUE;
	}

	SleepEx(0, TRUE);
	_ASSERT(0 == m_listSPipe.size());	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::PendingAccept()
{
	SECURITY_DESCRIPTOR     sDesc = {0};
	SECURITY_ATTRIBUTES     sa = {sizeof(sa)};	

	if (!InitializeSecurityDescriptor(&sDesc, SECURITY_DESCRIPTOR_REVISION))
	{
		return FALSE;
	}

	if (!SetSecurityDescriptorDacl(&sDesc, TRUE, (PACL) NULL, FALSE))	  
	{
		return FALSE;
	}

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &sDesc;
	sa.bInheritHandle = TRUE;

	if (INVALID_HANDLE_VALUE == m_hPipeAccept)
	{
		m_hPipeAccept = CreateNamedPipe(
			m_lpszPipeName,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			MSC_BUFSIZE,
			MSC_BUFSIZE,
			MSC_CLIENTWAITTIMEOUT,
			&sa);
		if (INVALID_HANDLE_VALUE == m_hPipeAccept)
		{
			return FALSE;
		}
	}
	
	// Overlapped ConnectNamedPipe should always return zero. 
	ConnectNamedPipe(m_hPipeAccept, &m_overlappedAccept);
	
	switch (GetLastError())
	{
		// The overlapped connection in progress. 
	case ERROR_IO_PENDING: 
		break;
		
		// Client is already connected, so signal an event. 
	case ERROR_PIPE_CONNECTED: 
		SetEvent(m_hAcceptEvent);
		break;
		
		// If an error occurs during the connect operation... 
	default:
		CloseHandle(m_hPipeAccept);
		m_hPipeAccept = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::PendingAcceptEx()
{
	DWORD dwRetry = 0;
	while (dwRetry++ < MSC_RETRYMAXCOUNT)
	{
		if (PendingAccept())
		{
			return TRUE;
		}
		SleepEx(500, TRUE);
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::PendingRead(CSPipe* lpSPipe)
{
	_ASSERT(NULL != lpSPipe);

	BOOL bResult = ReadFileEx(
		lpSPipe->Handle(),
		lpSPipe->Buffer(),
		lpSPipe->Len(),
		lpSPipe->Overlapped(),
		CompletedReadRoutine);

	if (!bResult)
	{
		Close(lpSPipe);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::PendingWrite(CSPipe* lpSPipe)
{
	_ASSERT(NULL != lpSPipe);

	BOOL bResult = WriteFileEx(
		lpSPipe->Handle(),
		lpSPipe->Buffer(),
		lpSPipe->Len(),
		lpSPipe->Overlapped(),
		CompletedWriteRoutine);

	if (!bResult)
	{
		Close(lpSPipe);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::Accept()
{
	_ASSERT(INVALID_HANDLE_VALUE != m_hPipeAccept);

	CSPipe* lpSPipe = new CSPipe(m_hPipeAccept);
	if (NULL == lpSPipe)
	{
		DisconnectNamedPipe(m_hPipeAccept);
		return FALSE;
	}

	m_listSPipe.push_back(lpSPipe);
	m_hPipeAccept = INVALID_HANDLE_VALUE;

	return PendingRead(lpSPipe);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::Read(DWORD dwErrCode, DWORD dwTransfered, CSPipe* lpSPipe)
{
	_ASSERT(NULL != lpSPipe);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		// error, should remove the lpSPipe from the m_listSPipe and close the pipe
		Close(lpSPipe);
		return FALSE;
	}

	if (m_bExitting)
	{
		Close(lpSPipe);
		return FALSE;
	}

	if (!lpSPipe->Read(dwTransfered))
	{
		Close(lpSPipe);
		return FALSE;
	}
	
	if (lpSPipe->IsFull())
	{
		if (!lpSPipe->Verify())
		{
			Close(lpSPipe);
			return FALSE;
		}

		if (!lpSPipe->MakeResponse())
		{
			Close(lpSPipe);
			return FALSE;
		}

		return PendingWrite(lpSPipe);
	}
	else
	{
		return PendingRead(lpSPipe);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCListener::Write(DWORD dwErrCode, DWORD dwTransfered, CSPipe* lpSPipe)
{
	_ASSERT(NULL != lpSPipe);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		// error, should remove the lpSPipe from the m_listSPipe and close the pipe
		Close(lpSPipe);
		return FALSE;
	}

	if (m_bExitting)
	{
		Close(lpSPipe);
		return FALSE;
	}

	lpSPipe->Write(dwTransfered);

	if (lpSPipe->IsFull())
	{
		CSubscriber* lpSubsriber = lpSPipe->Logon();
		Close(lpSPipe);

		if (NULL != lpSubsriber)
		{
			CSMSCSubscriberQueue* lpSMSCSubsriberQueue = CSMSCSubscriberQueue::GetInstance();
			_ASSERT(NULL != lpSMSCSubsriberQueue);

			lpSMSCSubsriberQueue->Push(lpSubsriber);
		}
		return TRUE;
	}
	else
	{
		return PendingWrite(lpSPipe);
	}
}
/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::Close(CSPipe* lpSPipe)
{
	std::list<CSPipe*>::iterator it;
	for (it = m_listSPipe.begin(); it != m_listSPipe.end(); it++)
	{
		if ((*it) == lpSPipe)
		{
			break;
		}
	}

	_ASSERT(it != m_listSPipe.end());
	m_listSPipe.erase(it);

	delete lpSPipe;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::LoginCheck()
{
	DWORD dwCheckTime = GetTickCount();
	std::list<CSPipe*>::iterator it;
	std::queue<CSPipe*> queueSPipe;
	for (it = m_listSPipe.begin(); it != m_listSPipe.end(); it++)
	{
		if ((*it)->IsTimeOut(dwCheckTime))
		{
			queueSPipe.push((*it));
		}
	}

	CSPipe* lpSPipe = NULL;
	while (!queueSPipe.empty())
	{
		lpSPipe = queueSPipe.front();
		queueSPipe.pop();

		CancelIo(lpSPipe->Handle());
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	_ASSERT(NULL != s_this);
	s_this->Read(dwErrCode, dwTransfered, (CSPipe*)lpOverlapped);
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCListener::CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	_ASSERT(NULL != s_this);
	s_this->Write(dwErrCode, dwTransfered, (CSPipe*)lpOverlapped);
}

