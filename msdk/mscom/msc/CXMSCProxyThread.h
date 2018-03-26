#ifndef _CX_MSC_PROXY_THREAD_H_
#define _CX_MSC_PROXY_THREAD_H_

#include "XThread.h"
#include "CMSCMessageQueue.h"
#include "XMSCDispatcher.h"
/////////////////////////////////////////////////////////////////////////////
class CCXMSCProxyThread :public XThread
{
public:
	CCXMSCProxyThread(XMSCDispatcher* lpXMSCDispatcher, BOOL& bResult,IUnknown* pBufferMgr=NULL);
	virtual ~CCXMSCProxyThread();
	CCMSCMessageQueue* MessageQueue();

protected:
	virtual VOID SVC();

private:
	VOID Handle();

	CCMSCMessageQueue m_objCMSCMessageQueue;
	XMSCDispatcher*   m_lpXMSCDispatcher;
	UTIL::com_ptr<IMsBufferMgr> m_pRsBufferManager;

};
/////////////////////////////////////////////////////////////////////////////
#endif