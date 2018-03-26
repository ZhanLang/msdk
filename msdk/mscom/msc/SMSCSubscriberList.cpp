#include "StdAfx.h"
#include <algorithm>
#include "SMSCSubscriberList.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
CSMSCSubscriberList::CSMSCSubscriberList()
{
}

/////////////////////////////////////////////////////////////////////////////
CSMSCSubscriberList::~CSMSCSubscriberList()
{
	std::list<CSubscriber*>::iterator it;
	for (it = m_listSubscriber.begin(); it != m_listSubscriber.end(); it++)
	{
		(*it)->Release();
	}
	m_listSubscriber.clear();
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberList::Add(CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	lpSubscriber->AddRef();
	m_listSubscriber.push_back(lpSubscriber);
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberList::Remove(CSubscriber* lpSubscriber)
{
	_ASSERT(NULL != lpSubscriber);
	std::list<CSubscriber*>::iterator it = std::find(m_listSubscriber.begin(), m_listSubscriber.end(), lpSubscriber);

	_ASSERT(it != m_listSubscriber.end());
	if (it != m_listSubscriber.end())
	{
		m_listSubscriber.erase(it);
		lpSubscriber->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCSubscriberList::GetAll(std::queue<CSubscriber*>& rQueue)
{
	std::list<CSubscriber*>::iterator it;
	for (it = m_listSubscriber.begin(); it != m_listSubscriber.end(); it++)
	{
		(*it)->AddRef();
		rQueue.push(*it);
	}
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSMSCSubscriberList::Size()
{
	return m_listSubscriber.size();
}
