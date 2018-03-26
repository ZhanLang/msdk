#include "StdAfx.h"
#include "CPipe.h"
#include "CXMessage.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
CCPipe::CCPipe(HANDLE hPipe)
	:m_hPipe(hPipe),
	 m_bBroken(FALSE),
	 m_lpMessageInBuffer(NULL),
	 m_lpMessageOutBuffer(NULL)
{
	_ASSERT(INVALID_HANDLE_VALUE != hPipe);
	ZeroMemory(&m_readOverlapped, sizeof(&m_readOverlapped));
	ZeroMemory(&m_writeOverlapped, sizeof(&m_writeOverlapped));

	m_xMessageBuffer.AttachBuffer((LPBYTE)&m_xMessage, sizeof(XMessage));
}

/////////////////////////////////////////////////////////////////////////////
CCPipe::~CCPipe()
{
	SAFE_DELETE(m_lpMessageOutBuffer);
	SAFE_DELETE(m_lpMessageInBuffer);


	LPBYTE lpTempBuffer;
	DWORD  dwTempLen;
	m_xMessageBuffer.DettachBuffer(lpTempBuffer, dwTempLen);
	
	if (INVALID_HANDLE_VALUE != m_hPipe)
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
LPOVERLAPPED CCPipe::ReadOverlapped()
{
	ZeroMemory(&m_readOverlapped, sizeof(OVERLAPPED));
	m_readOverlapped.hEvent = this;
	return &m_readOverlapped;
}

/////////////////////////////////////////////////////////////////////////////
LPOVERLAPPED CCPipe::WriteOverlapped()
{
	ZeroMemory(&m_writeOverlapped, sizeof(OVERLAPPED));
	m_writeOverlapped.hEvent = this;
	return &m_writeOverlapped;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CCPipe::Handle()
{
	return m_hPipe;
}

/////////////////////////////////////////////////////////////////////////////
LPBYTE CCPipe::ReadBuffer()
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
DWORD CCPipe::ReadLen()
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
LPBYTE CCPipe::WriteBuffer()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->RestBuffer();
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCPipe::WriteLen()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->RestLen();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::IsReadFull()
{
	if (NULL == m_lpMessageInBuffer)
	{
		return FALSE;
	}
	
	return m_lpMessageInBuffer->IsFull();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::IsWriteFull()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	return m_lpMessageOutBuffer->IsFull();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::Read(DWORD dwDelta)
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
				SAFE_DELETE(m_lpMessageInBuffer) ;
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
VOID CCPipe::Write(DWORD dwDelta)
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	m_lpMessageOutBuffer->Advance(dwDelta);
}

/////////////////////////////////////////////////////////////////////////////
VOID CCPipe::SetBroken()
{
	m_bBroken = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::IsBroken()
{
	return m_bBroken;
}

/////////////////////////////////////////////////////////////////////////////
VOID CCPipe::DettachWriteBuffer()
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	SAFE_DELETE(m_lpMessageOutBuffer);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::AttachWriteBuffer(XMessage* lpXMessage)
{
	_ASSERT(NULL == m_lpMessageOutBuffer);
	_ASSERT(NULL != lpXMessage);

	m_lpMessageOutBuffer = new XMessageBuffer();
	if (NULL == m_lpMessageOutBuffer)
	{
		return FALSE;
	}

	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	//ZM1_GrpDbg(GroupName,_T("¿Í»§¶ËAttach»º´æ=0x%0x"),lpXMessage);
	m_lpMessageOutBuffer->AttachBuffer((LPBYTE)lpXMessage, lpCXMessage->DataLen() + sizeof(XMessage));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCPipe::IsCanAttachWriteBuffer()
{
	return NULL == m_lpMessageOutBuffer;
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CCPipe::DettachReadBuffer()
{
	_ASSERT(NULL != m_lpMessageInBuffer);
	LPBYTE lpBuffer = NULL;
	DWORD dwLen = 0;
	m_lpMessageInBuffer->DettachBuffer(lpBuffer, dwLen);
	SAFE_DELETE(m_lpMessageInBuffer);
	m_xMessageBuffer.ResetPos();
	return (XMessage*)lpBuffer;
}