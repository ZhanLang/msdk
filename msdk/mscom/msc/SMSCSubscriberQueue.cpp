#include "StdAfx.h"
#include "SMSCSubscriberQueue.h"
#include "XSynchronization.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


CSMSCSubscriberQueue* CSMSCSubscriberQueue::s_this = NULL;
/////////////////////////////////////////////////////////////////////////////
CSMSCSubscriberQueue::CSMSCSubscriberQueue()
{
	m_hEvent = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSMSCSubscriberQueue::~CSMSCSubscriberQueue()
{
	Uninitialize();
	s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSMSCSubscriberQueue* CSMSCSubscriberQueue::GetInstance()
{
	if (NULL == s_this)
	{
		s_this = new CSMSCSubscriberQueue();
	}

	return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberQueue::DestroyInstance()
{
	SAFE_DELETE(s_this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSMSCSubscriberQueue::Initialize()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	return (m_hEvent != NULL);
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberQueue::Uninitialize()
{
	CSubscriber* lpSubscriber = NULL;
	while (!m_queueSubscriber.empty())
	{
		lpSubscriber = m_queueSubscriber.front();
		m_queueSubscriber.pop();
		lpSubscriber->Release();
	}
	
	if (NULL != m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CSMSCSubscriberQueue::Event()
{
	return m_hEvent;
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberQueue::Push(CSubscriber* lpSubscriber)
{
	SYN_OBJ();
	_ASSERT(NULL != lpSubscriber);

	lpSubscriber->AddRef();
	m_queueSubscriber.push(lpSubscriber);
	if (1 == m_queueSubscriber.size())
	{
		SetEvent(m_hEvent);
	}
}

/////////////////////////////////////////////////////////////////////////////
CSubscriber* CSMSCSubscriberQueue::Pop()
{
	SYN_OBJ();
	if (m_queueSubscriber.empty())
	{
		return NULL;
	}

	CSubscriber* lpSubscriber = m_queueSubscriber.front();
	m_queueSubscriber.pop();
	return lpSubscriber;
}
