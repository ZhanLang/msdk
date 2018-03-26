#include "StdAfx.h"
#include "CXMSCProxyThread.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
CCXMSCProxyThread::CCXMSCProxyThread(XMSCDispatcher* lpXMSCDispatcher, BOOL& bResult,IUnknown* pBufferMgr)
	:m_lpXMSCDispatcher(lpXMSCDispatcher)
{
	_ASSERT(NULL != lpXMSCDispatcher);
	bResult = m_objCMSCMessageQueue.Initialize();
	m_pRsBufferManager = pBufferMgr;

}

/////////////////////////////////////////////////////////////////////////////
CCXMSCProxyThread::~CCXMSCProxyThread()
{
	m_objCMSCMessageQueue.Uninitialize();
}


/////////////////////////////////////////////////////////////////////////////
CCMSCMessageQueue* CCXMSCProxyThread::MessageQueue()
{
	return &m_objCMSCMessageQueue;
}

/////////////////////////////////////////////////////////////////////////////
VOID CCXMSCProxyThread::SVC()
{
	HANDLE hWaitHandles[2];
	hWaitHandles[0] = m_hExitEvent;
	hWaitHandles[1] = m_objCMSCMessageQueue.Event();

	DWORD dwResult;
	BOOL bExitting = FALSE;
	while (TRUE)
	{
		dwResult = WaitForMultipleObjects(2, hWaitHandles, FALSE, INFINITE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			bExitting = TRUE;
			break;
		case WAIT_OBJECT_0 + 1:
			Handle();
			break;
		default:
			_ASSERT(FALSE);
		}

		if (bExitting)
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CCXMSCProxyThread::Handle()
{
	_ASSERT(NULL != m_lpXMSCDispatcher);
	XMessage* lpXMessage = NULL;
	while (lpXMessage = m_objCMSCMessageQueue.Pop())
	{
#ifdef USE_COMMONDLL
		m_lpXMSCDispatcher->Handle(lpXMessage);
#endif

#ifdef  USE_MARGINCOM
		UTIL::com_ptr<IMsBuffer> pRsBuffer =(IUnknown*)0;
		if(lpXMessage->dwDataLen && m_pRsBufferManager)
		{
			m_pRsBufferManager->CreateMsBuf((IMsBuffer**)&pRsBuffer,lpXMessage->dwDataLen);
			if(pRsBuffer)
				pRsBuffer->SetBuffer((LPBYTE)((BYTE*)lpXMessage+sizeof(XMessage)),lpXMessage->dwDataLen,0);
		}
		m_lpXMSCDispatcher->Handle(lpXMessage->dwMessageID,pRsBuffer);
		delete[]((LPBYTE)lpXMessage);
#endif 
	}
}
