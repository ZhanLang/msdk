#include "StdAfx.h"
#include "Subscriber.h"
#include "CXMessage.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
CSubscriber::CSubscriber(DWORD dwProcessID, DWORD dwThreadID, DWORD dwSubSystemID, HANDLE hPipe)
{
	_ASSERT(INVALID_HANDLE_VALUE != hPipe);

	ZeroMemory(&m_readOverlapped, sizeof(OVERLAPPED));
	ZeroMemory(&m_writeOverlapped, sizeof(OVERLAPPED));
	m_dwProcessID   = dwProcessID;
	m_dwThreadID    = dwThreadID;
	m_dwSubSystemID = dwSubSystemID;
	m_dwRefCount    = 0;
	m_hPipe         = hPipe;
	m_bBroken       = FALSE;
	
	m_xMessageBuffer.AttachBuffer((LPBYTE)&m_xMessage, sizeof(XMessage));
	m_lpMessageInBuffer  = NULL;
	m_lpMessageOutBuffer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSubscriber::~CSubscriber()
{
	while (!m_queueMessageBuffer.empty())
	{
		delete m_queueMessageBuffer.front();
		m_queueMessageBuffer.pop();
	}
	
	SAFE_DELETE(m_lpMessageOutBuffer);
	SAFE_DELETE(m_lpMessageInBuffer);

	LPBYTE lpTempBuffer;
	DWORD  dwTempLen;
	m_xMessageBuffer.DettachBuffer(lpTempBuffer, dwTempLen);
	
	if (INVALID_HANDLE_VALUE != m_hPipe)
	{
		DisconnectNamedPipe(m_hPipe);
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
LPOVERLAPPED CSubscriber::ReadOverlapped()
{
	ZeroMemory(&m_readOverlapped, sizeof(OVERLAPPED));
	m_readOverlapped.hEvent = this;
	return &m_readOverlapped;
}

/////////////////////////////////////////////////////////////////////////////
LPOVERLAPPED CSubscriber::WriteOverlapped()
{
	ZeroMemory(&m_writeOverlapped, sizeof(OVERLAPPED));
	m_writeOverlapped.hEvent = this;
	return &m_writeOverlapped;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CSubscriber::Handle()
{
	return m_hPipe;
}

/////////////////////////////////////////////////////////////////////////////
LPBYTE CSubscriber::ReadBuffer()
{
	if (NULL == m_lpMessageInBuffer)
	{
		return m_xMessageBuffer.RestBuffer();
	}
	else
	{
		return m_lpMessageInBuffer->RestBuffer();
	}
}

/////////////////////////////////////////////////////////////////////////////
DWORD  CSubscriber::ReadLen()
{
	if (NULL == m_lpMessageInBuffer)
	{
		return m_xMessageBuffer.RestLen();
	}
	else
	{
		return m_lpMessageInBuffer->RestLen();
	}
}

/////////////////////////////////////////////////////////////////////////////
LPBYTE CSubscriber::WriteBuffer()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->RestBuffer();
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::WriteLen()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->RestLen();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSubscriber::IsReadFull()
{
	if (NULL == m_lpMessageInBuffer)
	{
		return FALSE;
	}
	
	return m_lpMessageInBuffer->IsFull();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSubscriber::IsWriteFull()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->IsFull();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSubscriber::Read(DWORD dwDelta)
{
	if (NULL == m_lpMessageInBuffer)
	{
		m_xMessageBuffer.Advance(dwDelta);
		if (m_xMessageBuffer.IsFull())
		{
			CXMessage* lpXMessage = (CXMessage*)m_xMessageBuffer.Header();
			if (lpXMessage->DataLen() > XM_MAXDATALEN)
			{
				m_xMessageBuffer.ResetPos();
				return FALSE;
			}
			
			BOOL bResult = FALSE;
			m_lpMessageInBuffer = new XMessageBuffer(lpXMessage->DataLen(), bResult);
			if (NULL == m_lpMessageInBuffer)
			{
				m_xMessageBuffer.ResetPos();
				return FALSE;
			}
			
			if (!bResult)
			{
				m_xMessageBuffer.ResetPos();
				SAFE_DELETE(m_lpMessageInBuffer);
				return FALSE;
			}
			
			CopyMemory(m_lpMessageInBuffer->Buffer(), m_xMessageBuffer.Buffer(), m_xMessageBuffer.Len());
			m_lpMessageInBuffer->Pos(m_xMessageBuffer.Len());
			m_xMessageBuffer.ResetPos();
		}
	}
	else
	{
		m_lpMessageInBuffer->Advance(dwDelta);
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CSubscriber::Write(DWORD dwDelta)
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	m_lpMessageOutBuffer->Advance(dwDelta);
}

/////////////////////////////////////////////////////////////////////////////
VOID CSubscriber::SetBroken()
{
	m_bBroken = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSubscriber::IsBroken()
{
	return m_bBroken;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::AddRef()
{
	_ASSERT(m_dwRefCount != -1);

	// Not need
	// return InterlockedIncrement((LPLONG)&m_dwRefCount);
	return ++m_dwRefCount;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::Release()
{
	_ASSERT(m_dwRefCount > 0);

	// Not need
	// DWORD dwRefCount = InterlockedDecrement((LPLONG)&m_dwRefCount);
	DWORD dwRefCount = --m_dwRefCount;
	if (0 == dwRefCount)
	{
		delete this;
	}
	
	return dwRefCount;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CSubscriber::AttachWriteBuffer()
{
	if (NULL != m_lpMessageOutBuffer)
	{
		return FALSE;
	}

	if (0 == m_queueMessageBuffer.size())
	{
		return FALSE;
	}

	m_lpMessageOutBuffer = m_queueMessageBuffer.front();
	m_queueMessageBuffer.pop();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CSubscriber::DettachWriteBuffer()
{
	_ASSERT(NULL != m_lpMessageOutBuffer && m_lpMessageOutBuffer->IsFull());
	SAFE_DELETE(m_lpMessageOutBuffer);
	
}

/////////////////////////////////////////////////////////////////////////////
VOID CSubscriber::PendingWriteBuffer(XMessageBuffer* lpMessageBuffer)
{
	_ASSERT(NULL != lpMessageBuffer);
	m_queueMessageBuffer.push(lpMessageBuffer);
}

/////////////////////////////////////////////////////////////////////////////
XMessageBuffer* CSubscriber::DettachReadBuffer()
{
	_ASSERT(NULL != m_lpMessageInBuffer);
	m_lpMessageInBuffer->ResetPos();
	XMessageBuffer* lpMessageBuffer = m_lpMessageInBuffer;
	m_lpMessageInBuffer = NULL;
	m_xMessageBuffer.ResetPos();
	return lpMessageBuffer;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::ProcessID()
{
	return m_dwProcessID;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::ThreadID()
{
	return m_dwThreadID;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::SubSystemID()
{
	return m_dwSubSystemID;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSubscriber::SubscriberID()
{
	return (DWORD)m_hPipe;
}
