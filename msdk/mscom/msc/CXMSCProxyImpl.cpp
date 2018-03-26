#include "StdAfx.h"
#include "CXMSCProxyImpl.h"
#include "XSynchronization.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
CXMSCProxyImpl::CXMSCProxyImpl()
{
	m_bConnectBroken = TRUE;
	m_bExitting = TRUE;
	m_lpszName = NULL;
	m_lpCPipe  = NULL;
	m_lpCXMSCDispatcher = NULL;
	m_pRsBufferManager = (IUnknown*)0;
}

/////////////////////////////////////////////////////////////////////////////
CXMSCProxyImpl::~CXMSCProxyImpl()
{
	Uninitialize();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::Initialize(LPCTSTR lpszName, XMSCDispatcher* lpXMSCDispatcher,IUnknown* pMgr,BOOL bAsync)
{
	m_pRsBufferManager = pMgr;
	DWORD dwLen = (_tcslen(lpszName) + 10);
	m_lpszName = new TCHAR[dwLen * sizeof(TCHAR)];
	if (NULL == m_lpszName)
	{
		return FALSE;
	}
	m_lpszName[dwLen - 1] = _T('\0');
	_tcscpy(m_lpszName, _T("\\\\.\\PIPE\\"));
	_tcscat(m_lpszName, lpszName);
	
	m_lpCXMSCDispatcher = new CCXMSCDispatcher(bAsync, lpXMSCDispatcher,m_pRsBufferManager);
	if (NULL == m_lpCXMSCDispatcher)
	{
		Uninitialize();
		return FALSE;
	}

	if (!m_lpCXMSCDispatcher->Initialize())
	{
		Uninitialize();
		return FALSE;
	}

	if (!m_objCMSCMessageQueue.Initialize())
	{
		Uninitialize();
		return FALSE;
	}

	if (!Connect())
	{
		Uninitialize();
		return FALSE;
	}

	if (!Start())
	{
		Uninitialize();
		return FALSE;
	}

	SetConnectBroken(FALSE);
	m_bExitting = FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::Uninitialize()
{
	Stop();
	_ASSERT(m_bConnectBroken);
	SAFE_DELETE(m_lpCPipe);

	m_objCMSCMessageQueue.Uninitialize();
	if (NULL != m_lpCXMSCDispatcher)
	{
		m_lpCXMSCDispatcher->Uninitialize();
		SAFE_DELETE(m_lpCXMSCDispatcher);
	}

	SAFE_DELETE_BUFFER(m_lpszName);

}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::Issue(XMessage* lpXMessage)
{
	if (IsConnectBroken())
	{
		return FALSE;
	}

	m_objCMSCMessageQueue.Push(lpXMessage);
	return TRUE;
}

BOOL CXMSCProxyImpl::Send(XMessage* lpXMessage)
{
	if (IsConnectBroken())
	{
		return FALSE;
	}

	return TrySend(lpXMessage);
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CXMSCProxyImpl::Event()
{
	_ASSERT(NULL != m_lpCXMSCDispatcher);
	return m_lpCXMSCDispatcher->Event();
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::SVC()
{
	if (!PendingRead())
	{
		return;
	}

	HANDLE hWaitHandles[2];
	hWaitHandles[0] = m_hExitEvent;
	hWaitHandles[1] = m_objCMSCMessageQueue.Event();

	while (TRUE)
	{
		DWORD dwResult = WaitForMultipleObjectsEx(2, hWaitHandles, FALSE, INFINITE, TRUE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			m_bExitting = TRUE;
			break;
			
		case WAIT_OBJECT_0 + 1:
			TryPendingWrite();
			break;
			
		case WAIT_IO_COMPLETION:
			break;
			
		default:
			_ASSERT(FALSE);
		}

		if (m_bExitting)
		{
			break;
		}
	}

	Close(FALSE);
	SleepEx(0, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::Connect()
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	while (TRUE)
	{
		hPipe = CreateFile(
			m_lpszName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);
		
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			break;
		}
		DWORD dwErr = GetLastError();
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			break;
		}
		
		if (!WaitNamedPipe(m_lpszName, NMPWAIT_USE_DEFAULT_WAIT))
		{
			break;
		}
	}

	if (INVALID_HANDLE_VALUE == hPipe)
	{
		return FALSE;
	}
	
	m_lpCPipe = new CCPipe(hPipe);
	if (NULL == m_lpCPipe)
	{
		CloseHandle(hPipe);
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::PendingRead()
{
	if (m_lpCPipe->IsBroken())
	{
		Close();
		return FALSE;
	}
	
	LPOVERLAPPED lpOverlapped = m_lpCPipe->ReadOverlapped();
	lpOverlapped->hEvent = this;

	BOOL bResult = ReadFileEx(
		m_lpCPipe->Handle(),
		m_lpCPipe->ReadBuffer(),
		m_lpCPipe->ReadLen(),
		lpOverlapped,
		CompletedReadRoutine);
	
	if (!bResult)
	{
		Close();
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::PendingWrite()
{
	_ASSERT(NULL != m_lpCPipe);
	if (m_lpCPipe->IsBroken())
	{
		Close();
		return FALSE;
	}
	
	LPOVERLAPPED lpOverlapped = m_lpCPipe->WriteOverlapped();
	lpOverlapped->hEvent = this;

	BOOL bResult = WriteFileEx(
		m_lpCPipe->Handle(),
		m_lpCPipe->WriteBuffer(),
		m_lpCPipe->WriteLen(),
		lpOverlapped,
		CompletedWriteRoutine);
	
	if (!bResult)
	{
		Close();
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::TryPendingWrite()
{
	if (!m_lpCPipe->IsCanAttachWriteBuffer())
	{
		return FALSE;
	}

	XMessage* lpXMessage = m_objCMSCMessageQueue.Pop();
	if (NULL == lpXMessage)
	{
		return FALSE;
	}

	m_lpCPipe->AttachWriteBuffer(lpXMessage);
	return PendingWrite();
}

BOOL CXMSCProxyImpl::TrySend(XMessage* lpXMessage)
{
    _ASSERT(NULL != m_lpCPipe);
    if (m_lpCPipe->IsBroken())
    {
        Close();
        return FALSE;
    }

    LPBYTE lpSendBuf =   (BYTE*)lpXMessage; 
    DWORD dwBytesToWrite =  lpXMessage->dwDataLen + sizeof(XMessage); 
    DWORD dwBytesWritten = 0;

    while (dwBytesWritten < dwBytesToWrite)
    {
        DWORD dwBytesWrittenOne = 0;

        if( FALSE == WriteFile(m_lpCPipe->Handle(),           // open file handle
            lpSendBuf + dwBytesWritten,     // start of data to write
            dwBytesToWrite - dwBytesWritten, // number of bytes to write
            &dwBytesWrittenOne, // number of bytes that were written
            NULL)            // no overlapped structure
            )
        {
            Close();
            return FALSE;
        }
        dwBytesWritten += dwBytesWrittenOne;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::Read(DWORD dwErrCode, DWORD dwTransfered)
{
	_ASSERT(NULL != m_lpCPipe);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		Close();
		return FALSE;
	}
	
	if (m_bExitting)
	{
		Close();
		return FALSE;
	}
	
	if (!m_lpCPipe->Read(dwTransfered))
	{
		Close();
		return FALSE;
	}
	
	if (m_lpCPipe->IsReadFull())
	{
		m_lpCXMSCDispatcher->Handle(m_lpCPipe->DettachReadBuffer());
	}
	
	return PendingRead();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::Write(DWORD dwErrCode, DWORD dwTransfered)
{
	_ASSERT(NULL != m_lpCPipe);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		Close();
		return FALSE;
	}
	
	if (m_bExitting)
	{
		Close();
		return FALSE;
	}
	
	m_lpCPipe->Write(dwTransfered);
	if (m_lpCPipe->IsWriteFull())
	{
		m_lpCPipe->DettachWriteBuffer();
		return TryPendingWrite();
	}
	else
	{
		return PendingWrite();
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::Close(BOOL bFire)
{
	_ASSERT(NULL != m_lpCPipe);
	_ASSERT(NULL != m_lpCXMSCDispatcher);
	if (!m_lpCPipe->IsBroken())
	{
		m_lpCPipe->SetBroken();
		SetConnectBroken();
		CancelIo(m_lpCPipe->Handle());

		if (bFire)
		{
			m_lpCXMSCDispatcher->FireConnectBroken();
			//SetEvent(m_hExitEvent);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxyImpl::IsConnectBroken()
{
	SYN_OBJ_EX(&m_xMutex);
	return m_bConnectBroken;
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::SetConnectBroken(BOOL bBroken)
{
	SYN_OBJ_EX(&m_xMutex);
	m_bConnectBroken = bBroken;
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	CXMSCProxyImpl* lpThis = (CXMSCProxyImpl*)lpOverlapped->hEvent;
	_ASSERT(NULL != lpThis);

	lpThis->Read(dwErrCode, dwTransfered);
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxyImpl::CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	CXMSCProxyImpl* lpThis = (CXMSCProxyImpl*)lpOverlapped->hEvent;
	_ASSERT(NULL != lpThis);

	lpThis->Write(dwErrCode, dwTransfered);
}