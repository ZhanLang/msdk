#ifndef _S_MSC_SUBSCRIBER_QUEUE_H_
#define _S_MSC_SUBSCRIBER_QUEUE_H_

#include <queue>
#include "XMutex.h"
#include "Subscriber.h"
/////////////////////////////////////////////////////////////////////////////
class CSMSCSubscriberQueue :public XMutex
{
private:
	CSMSCSubscriberQueue();
	virtual ~CSMSCSubscriberQueue();

public:
	static CSMSCSubscriberQueue* GetInstance();
	static VOID DestroyInstance();

	BOOL Initialize();
	VOID Uninitialize();

	HANDLE Event();
	VOID   Push(CSubscriber* lpSubscriber);
	CSubscriber* Pop();
	
private:
	HANDLE m_hEvent;
	std::queue<CSubscriber*> m_queueSubscriber;
	static CSMSCSubscriberQueue* s_this;
};

/////////////////////////////////////////////////////////////////////////////
#endif