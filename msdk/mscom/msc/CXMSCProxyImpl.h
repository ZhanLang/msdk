#ifndef _CX_MSC_PROXY_IMPL_H_
#define _CX_MSC_PROXY_IMPL_H_

#include "XThread.h"
#include "CXMSCDispatcher.h"
#include "CPipe.h"
/////////////////////////////////////////////////////////////////////////////
class CXMSCProxyImpl :public XThread
{
public:
	CXMSCProxyImpl();
	virtual ~CXMSCProxyImpl();

	BOOL   Initialize(LPCTSTR lpszName, XMSCDispatcher* lpXMSCDispatcher,IUnknown* pMgr = NULL,BOOL bAsync = TRUE);
	VOID   Uninitialize();
	BOOL   Issue(XMessage* lpXMessage);
	BOOL   Send(XMessage* lpXMessage);
	HANDLE Event();
	
protected:
	virtual VOID SVC();

private:
	BOOL Connect();
	BOOL PendingRead();
	BOOL PendingWrite();
	BOOL TryPendingWrite();
	BOOL Read(DWORD dwErrCode, DWORD dwTransfered);
	BOOL Write(DWORD dwErrCode, DWORD dwTransfered);
	VOID Close(BOOL bFire = TRUE);
	BOOL IsConnectBroken();
	VOID SetConnectBroken(BOOL bBroken = TRUE);

	BOOL TrySend(XMessage* lpXMessage);
	
	static VOID WINAPI CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);
	static VOID WINAPI CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);

	BOOL    m_bConnectBroken;
	BOOL    m_bExitting;
	XMutex  m_xMutex;
	LPTSTR  m_lpszName;
	CCPipe* m_lpCPipe;
	CCXMSCDispatcher* m_lpCXMSCDispatcher;
	CCMSCMessageQueue m_objCMSCMessageQueue;
	UTIL::com_ptr<IMsBufferMgr> m_pRsBufferManager;

};

/////////////////////////////////////////////////////////////////////////////
#endif