#ifndef _C_MSC_MESSAGE_QUEUE_H_
#define _C_MSC_MESSAGE_QUEUE_H_

#include <queue>
#include "XMutex.h"
#include "XMessage.h"
/////////////////////////////////////////////////////////////////////////////
class CCMSCMessageQueue :public XMutex
{
public:
	CCMSCMessageQueue();
	~CCMSCMessageQueue();

	BOOL Initialize();
	VOID Uninitialize();

	VOID Push(XMessage* lpXMessage);
	XMessage* Pop();

	HANDLE Event();

private:
	HANDLE m_hEvent;
	std::queue<XMessage*> m_queueMessage;
};

/////////////////////////////////////////////////////////////////////////////
#endif