#ifndef _X_MSC_DISPATCHER_H_
#define _X_MSC_DISPATCHER_H_

#include "XMessage.h"
#include "combase\IMsBuffer.h"

/////////////////////////////////////////////////////////////////////////////
#ifdef USE_COMMONDLL
class XMSCDispatcher
{
public:
	virtual VOID WINAPI Handle(XMessage* lpXMessage) = 0;
	virtual VOID WINAPI FireConnectBroken() = 0;
};
#endif 

#ifdef USE_MARGINCOM
class XMSCDispatcher
{
public:
	virtual VOID WINAPI Handle(RSMSGID dwMessageID,IMsBuffer* lpXMessage) = 0;
	virtual VOID WINAPI FireConnectBroken() = 0;
};
#endif

/////////////////////////////////////////////////////////////////////////////
#endif