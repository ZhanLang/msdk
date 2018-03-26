#include "StdAfx.h"
#include "SPipe.h"
#include "CXMessage.h"
#include "CXCredit.h"
#include "XMMSC.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
CSPipe::CSPipe(HANDLE hPipe)
{
	_ASSERT(INVALID_HANDLE_VALUE != hPipe);
	m_hPipe = hPipe;
	m_dwAcceptTime = GetTickCount();

	m_xMessageBuffer.AttachBuffer((LPBYTE)&m_xMessage, sizeof(XMessage));
	m_lpMessageInBuffer  = NULL;
	m_lpMessageOutBuffer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSPipe::~CSPipe()
{
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
LPOVERLAPPED CSPipe::Overlapped()
{
	ZeroMemory(this, sizeof(OVERLAPPED));
	return this;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CSPipe::Handle()
{
	return m_hPipe;
}

/////////////////////////////////////////////////////////////////////////////
LPBYTE CSPipe::Buffer()
{
	if (NULL == m_lpMessageOutBuffer)
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
	else
	{
		return m_lpMessageOutBuffer->RestBuffer();
	}
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSPipe::Len()
{
	if (NULL == m_lpMessageOutBuffer)
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
	else
	{
		return m_lpMessageOutBuffer->RestLen();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSPipe::IsFull()
{
	if (NULL == m_lpMessageOutBuffer)
	{
		if (NULL == m_lpMessageInBuffer)
		{
			return FALSE;
		}
		
		return m_lpMessageInBuffer->IsFull();
	}
	else
	{
		return m_lpMessageOutBuffer->IsFull();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSPipe::Read(DWORD dwDelta)
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
VOID CSPipe::Write(DWORD dwDelta)
{
	_ASSERT(NULL != m_lpMessageOutBuffer);
	m_lpMessageOutBuffer->Advance(dwDelta);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSPipe::Verify()
{
	_ASSERT(NULL != m_lpMessageInBuffer);
	CXMessage* lpXMessage = (CXMessage*)m_lpMessageInBuffer->Header();
	if (XMMSC_LOGIN_REQUEST != lpXMessage->MessageID())
	{
		return FALSE;
	}

	if (lpXMessage->DataLen() < sizeof(XCredit))
	{
		return FALSE;
	}

	CXCredit* lpXCredit = (CXCredit*)m_lpMessageInBuffer->Data();
	if (lpXCredit->IsExpire(m_dwAcceptTime))
	{
		return FALSE;
	}

	return lpXCredit->VerifySignature();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSPipe::MakeResponse()
{
	_ASSERT(NULL == m_lpMessageOutBuffer);
	
	BOOL bResult = FALSE;
	m_lpMessageOutBuffer = new XMessageBuffer(0, bResult);
	if (NULL == m_lpMessageOutBuffer)
	{
		return FALSE;
	}

	if (!bResult)
	{
		SAFE_DELETE(m_lpMessageOutBuffer);
		return FALSE;
	}

	_ASSERT(NULL != m_lpMessageInBuffer);
	CXMessage* lpInXMessage = (CXMessage*)m_lpMessageInBuffer;

	CXMessage* lpOutXMessage = (CXMessage*)m_lpMessageOutBuffer->Header();
	lpOutXMessage->Initialize(XMMSC_LOGIN_RESPONSE, 0, lpInXMessage->UniqueID());

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSPipe::IsTimeOut(DWORD dwCheckTime)
{
	if (NULL != m_lpMessageOutBuffer)
	{
		return FALSE;
	}

	_ASSERT(dwCheckTime >= m_dwAcceptTime);
	return dwCheckTime - m_dwAcceptTime > XCredit::XCREDIT_EXPIRE;
}

/////////////////////////////////////////////////////////////////////////////
CSubscriber* CSPipe::Logon()
{
	CXCredit* lpXCredit = (CXCredit*)m_lpMessageInBuffer->Data();
	CSubscriber* lpSubscriber = new CSubscriber(lpXCredit->ProcessID(), lpXCredit->ThreadID(), lpXCredit->SubSystemID(), m_hPipe);
	if (NULL == lpSubscriber)
	{
		return NULL;
	}
	else
	{
		m_hPipe = INVALID_HANDLE_VALUE;
		return lpSubscriber;
	}
}