#include "StdAfx.h"
#include "MSCHandler.h"
#include "SMSCSubscriberQueue.h"
#include "CXMessage.h"
#include "XMMSC.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


CMSCHandler* CMSCHandler::s_this = NULL;
/////////////////////////////////////////////////////////////////////////////
CMSCHandler::CMSCHandler()
{
	m_bExitting  = FALSE;
    m_hEvent = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMSCHandler::~CMSCHandler()
{
	Stop();
	s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMSCHandler* CMSCHandler::GetInstance()
{
	if (NULL == s_this)
	{
		s_this = new CMSCHandler();
	}

	return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::DestroyInstance()
{
	SAFE_DELETE(s_this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::Start()
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

    //要等待线程起来
    DWORD dwResult = WaitForSingleObject(m_hEvent,INFINITE);
    if (dwResult != WAIT_OBJECT_0)
    {
       //ZM1_GrpDbg(GroupName,_T("[MSC]CMSCListener::Start() 失败\n"));
        return FALSE;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::Stop()
{
    XThread::Stop();

    if(NULL != m_hEvent)
    {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
}
/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::SVC()
{
	HANDLE hWaits[2];
	hWaits[0] = m_hExitEvent;
	hWaits[1] = CSMSCSubscriberQueue::GetInstance()->Event();
    SetEvent(m_hEvent);
	while (TRUE)
	{
		DWORD dwResult = WaitForMultipleObjectsEx(2, hWaits, FALSE, INFINITE, TRUE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			m_bExitting = TRUE;
			break;

		case WAIT_OBJECT_0 + 1:
			AddSubscriber();
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

	std::queue<CSubscriber*> queueSubscriber;
	m_objSubscriberList.GetAll(queueSubscriber);
	
	CSubscriber* lpSubscriber = NULL;
	while (!queueSubscriber.empty())
	{
		lpSubscriber = queueSubscriber.front();
		queueSubscriber.pop();
		
		CancelIo(lpSubscriber->Handle());
		lpSubscriber->Release();
	}
	
	SleepEx(0, TRUE);

	_ASSERT(0 == m_objSubscriberList.Size());
	_ASSERT(0 == m_objMessageMap.Size());
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::AddSubscriber()
{
	CSMSCSubscriberQueue* lpSMSCSubscriberQueue = CSMSCSubscriberQueue::GetInstance();
	_ASSERT(NULL != lpSMSCSubscriberQueue);

	CSubscriber* lpSubscriber = NULL;
	while (lpSubscriber = lpSMSCSubscriberQueue->Pop())
	{
		m_objSubscriberList.Add(lpSubscriber);
		PendingRead(lpSubscriber);
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::Handle(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpMessageBuffer);
	_ASSERT(NULL != lpSubscriber);
	CXMessage* lpXMessage = (CXMessage*)lpMessageBuffer->Header();

	switch (lpXMessage->MessageID())
	{
	case XMMSC_SUBSCRIBE:
		m_objMessageMap.Subscribe(
			*(DWORD*)(lpXMessage->Data()),
			(RSMSGID*)(lpXMessage->Data() + sizeof(DWORD)),
			lpSubscriber);
		delete lpMessageBuffer;
		break;
	case XMMSC_UNSUBSCRIBE:
		m_objMessageMap.UnSubscribe(
			*(DWORD*)(lpXMessage->Data()),
			(RSMSGID*)(lpXMessage->Data() + sizeof(DWORD)),
			lpSubscriber);
		delete lpMessageBuffer;
		break;
	case XMMSC_RELATION_REQUEST:
		m_objMessageMap.Relation(lpMessageBuffer, lpSubscriber);
		break;
	default:
		m_objMessageMap.Handle(lpMessageBuffer, lpSubscriber);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::PendingRead(CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	if (lpSubscriber->IsBroken())
	{
		Close(lpSubscriber);
		return FALSE;
	}

	BOOL bResult = ReadFileEx(
		lpSubscriber->Handle(),
		lpSubscriber->ReadBuffer(),
		lpSubscriber->ReadLen(),
		lpSubscriber->ReadOverlapped(),
		CompletedReadRoutine);
	
	if (!bResult)
	{
		Close(lpSubscriber);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::PendingWrite(CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	if (lpSubscriber->IsBroken())
	{
		Close(lpSubscriber);
		return FALSE;
	}

	BOOL bResult = WriteFileEx(
		lpSubscriber->Handle(),
		lpSubscriber->WriteBuffer(),
		lpSubscriber->WriteLen(),
		lpSubscriber->WriteOverlapped(),
		CompletedWriteRoutine);
	
	if (!bResult)
	{
		Close(lpSubscriber);
	}
	
	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::TryPendingWrite(CSubscriber* lpSubscriber)
{
	if (lpSubscriber->AttachWriteBuffer())
	{
		return PendingWrite(lpSubscriber);
	}
	
	lpSubscriber->Release();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::Read(DWORD dwErrCode, DWORD dwTransfered, CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		Close(lpSubscriber);
		return FALSE;
	}
	
	if (m_bExitting)
	{
		Close(lpSubscriber);
		return FALSE;
	}
	
	if (!lpSubscriber->Read(dwTransfered))
	{
		Close(lpSubscriber);
		return FALSE;
	}
	
	if (lpSubscriber->IsReadFull())
	{
		Handle(lpSubscriber->DettachReadBuffer(), lpSubscriber);
	}

	return PendingRead(lpSubscriber);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCHandler::Write(DWORD dwErrCode, DWORD dwTransfered, CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	if (0 != dwErrCode || 0 == dwTransfered)
	{
		Close(lpSubscriber);
		return FALSE;
	}
	
	if (m_bExitting)
	{
		Close(lpSubscriber);
		return FALSE;
	}
	
	lpSubscriber->Write(dwTransfered);
	if (lpSubscriber->IsWriteFull())
	{
		lpSubscriber->DettachWriteBuffer();
		return TryPendingWrite(lpSubscriber);
	}
	else
	{
		return PendingWrite(lpSubscriber);
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::Close(CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	
	if (!lpSubscriber->IsBroken())
	{
		lpSubscriber->SetBroken();
		CancelIo(lpSubscriber->Handle());
		m_objSubscriberList.Remove(lpSubscriber);
		m_objMessageMap.Remove(lpSubscriber);
	}
	
	lpSubscriber->Release();
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	_ASSERT(NULL != s_this);
	s_this->Read(dwErrCode, dwTransfered, (CSubscriber*)lpOverlapped->hEvent);
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCHandler::CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped)
{
	_ASSERT(NULL != s_this);
	s_this->Write(dwErrCode, dwTransfered, (CSubscriber*)lpOverlapped->hEvent);
}
