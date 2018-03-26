#include "StdAfx.h"
#include "CXMSCDispatcher.h"
#include "CXMessage.h"
#include "XMMSC.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
CCXMSCDispatcher::CCXMSCDispatcher(BOOL bAsync, XMSCDispatcher* lpXMSCDispatcher,IUnknown* pBufferMgr)
	:m_bAsync(bAsync),
	 m_lpXMSCDispatcher(lpXMSCDispatcher),
	 m_pRsBufferManager(pBufferMgr)
{
	_ASSERT(NULL != lpXMSCDispatcher);
	m_hEvent             = NULL;
	m_lpCXMSCProxyThread = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CCXMSCDispatcher::~CCXMSCDispatcher()
{
	_ASSERT(NULL == m_lpCXMSCProxyThread);
	_ASSERT(NULL == m_hEvent);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCXMSCDispatcher::Initialize()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hEvent)
	{
		return FALSE;
	}

	if (m_bAsync)
	{
		return TRUE;
	}
	
	BOOL bResult = FALSE;
	m_lpCXMSCProxyThread = new CCXMSCProxyThread(m_lpXMSCDispatcher, bResult,m_pRsBufferManager);
	if (NULL == m_lpCXMSCProxyThread)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
		return FALSE;
	}

	if (!bResult)
	{
		SAFE_DELETE(m_lpCXMSCProxyThread);

		CloseHandle(m_hEvent);
		m_hEvent = NULL;
		return FALSE;
	}

	if (!m_lpCXMSCProxyThread->Start())
	{
		SAFE_DELETE(m_lpCXMSCProxyThread);

		CloseHandle(m_hEvent);
		m_hEvent = NULL;
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CCXMSCDispatcher::Uninitialize()
{
	if (!m_bAsync)
	{
		if (NULL != m_lpCXMSCProxyThread)
		{
			m_lpCXMSCProxyThread->Stop();
			delete m_lpCXMSCProxyThread;
			m_lpCXMSCProxyThread = NULL;
		}
	}

	if (NULL != m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CCXMSCDispatcher::Handle(XMessage* lpXMessage)
{
	_ASSERT(NULL != lpXMessage);
	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	if (XMMSC_LOGIN_RESPONSE == lpCXMessage->MessageID())
	{
		SetEvent(m_hEvent);
		delete[] (LPBYTE)lpXMessage;
		lpXMessage = NULL;
		return;
	}

	if (m_bAsync)
	{

#ifdef   USE_COMMONDLL
		m_lpXMSCDispatcher->Handle(lpXMessage);
#endif

#ifdef  USE_MARGINCOM
		//ZM1_GrpDbg(GroupName,_T("[MSC]HandleÏûÏ¢ID=0x%016I64X\n"),lpXMessage->dwMessageID);

		UTIL::com_ptr<IMsBuffer> pRsBuffer =(IUnknown*)0;
		if(lpXMessage->dwDataLen && m_pRsBufferManager)
		{
			m_pRsBufferManager->CreateMsBuf((IMsBuffer**)&pRsBuffer,lpXMessage->dwDataLen);
			if(pRsBuffer)
				pRsBuffer->SetBuffer(lpCXMessage->Data(),lpXMessage->dwDataLen,0);
		}
		m_lpXMSCDispatcher->Handle(lpXMessage->dwMessageID,pRsBuffer);
		delete[] (LPBYTE)lpXMessage;
#endif

	}
	else
	{
		m_lpCXMSCProxyThread->MessageQueue()->Push(lpXMessage);
	}
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CCXMSCDispatcher::Event()
{
	return m_hEvent;
}

/////////////////////////////////////////////////////////////////////////////
VOID CCXMSCDispatcher::FireConnectBroken()
{
	m_lpXMSCDispatcher->FireConnectBroken();
}