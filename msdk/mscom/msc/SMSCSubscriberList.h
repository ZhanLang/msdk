#ifndef _S_MSC_SUBSCRIBER_LIST_H_
#define _S_MSC_SUBSCRIBER_LIST_H_

#include <list>
#include <queue>
#include "Subscriber.h"
/////////////////////////////////////////////////////////////////////////////
//表示消息订阅者池
class CSMSCSubscriberList
{
public:
	CSMSCSubscriberList();
	~CSMSCSubscriberList();

	VOID  Add(CSubscriber* lpSubscriber);
	VOID  Remove(CSubscriber* lpSubscriber);
	VOID  GetAll(std::queue<CSubscriber*>& rQueue);
	DWORD Size();

private:
	// I think there is not so much Subsystem that list is effective than map.
	std::list<CSubscriber*> m_listSubscriber;
};

/////////////////////////////////////////////////////////////////////////////
#endif
