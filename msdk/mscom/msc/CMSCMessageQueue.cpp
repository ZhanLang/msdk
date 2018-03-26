#include "StdAfx.h"
#include "CMSCMessageQueue.h"
#include "XSynchronization.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
CCMSCMessageQueue::CCMSCMessageQueue()
{
	m_hEvent = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CCMSCMessageQueue::~CCMSCMessageQueue()
{
	Uninitialize();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCMSCMessageQueue::Initialize()
{
	_ASSERT(NULL == m_hEvent);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hEvent)
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CCMSCMessageQueue::Uninitialize()
{
	while (!m_queueMessage.empty())
	{
		delete[] (LPBYTE)m_queueMessage.front();
		m_queueMessage.pop();
	}

	if (NULL != m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CCMSCMessageQueue::Push(XMessage* lpXMessage)
{
	_ASSERT(NULL != m_hEvent);
	SYN_OBJ();
	m_queueMessage.push(lpXMessage);
	if (1 == m_queueMessage.size())
	{
		SetEvent(m_hEvent);
	}
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CCMSCMessageQueue::Pop()
{
	SYN_OBJ();
	if (m_queueMessage.empty())
	{
		return NULL;
	}

	XMessage* lpXMessage = m_queueMessage.front();
	m_queueMessage.pop();
	return lpXMessage;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CCMSCMessageQueue::Event()
{
	_ASSERT(NULL != m_hEvent);
	return m_hEvent;
}
