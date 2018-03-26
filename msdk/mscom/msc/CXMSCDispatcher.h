#ifndef _CX_MSC_DISPATCHER_H_
#define _CX_MSC_DISPATCHER_H_

#include "XMSCDispatcher.h"
#include "CXMSCProxyThread.h"
/////////////////////////////////////////////////////////////////////////////
class CCXMSCDispatcher
{
public:
	CCXMSCDispatcher(BOOL bAsync, XMSCDispatcher* lpXMSCDispatcher,IUnknown* pBufferMgr=NULL);
	~CCXMSCDispatcher();

	BOOL   Initialize();
	VOID   Uninitialize();
	VOID   Handle(XMessage* lpXMessage);
	VOID   FireConnectBroken();
	HANDLE Event();


private:
	BOOL   m_bAsync;
	HANDLE m_hEvent;
	XMSCDispatcher*    m_lpXMSCDispatcher;
	CCXMSCProxyThread* m_lpCXMSCProxyThread;
	UTIL::com_ptr<IMsBufferMgr> m_pRsBufferManager;

};

/////////////////////////////////////////////////////////////////////////////
#endif