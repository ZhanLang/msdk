#pragma once

#include "XMSCProxy.h"
#include "XMutex.h"
#include "CXMSCProxyImpl.h"
#include "RSA.h"
/////////////////////////////////////////////////////////////////////////////
class CMsXMSCProxy :public IXMsgBusProxy2,
					public CUnknownImp
{
public:
	CMsXMSCProxy(IUnknown * pBufferManager);
	CMsXMSCProxy();
	~CMsXMSCProxy();
	UNKNOWN_IMP2(IXMsgBusProxy, IXMsgBusProxy2);

	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter);
	STDMETHOD_(BOOL,Login)(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync = TRUE, DWORD dwTimeOut = INFINITE, LPCTSTR lpszName = NULL);
	STDMETHOD_(VOID,Logoff)();
	STDMETHOD_(BOOL,Issue)(RSMSGID dwMessageID,IMsBuffer* lpXMessage);
	STDMETHOD_(BOOL,Send)(RSMSGID dwMessageID,IMsBuffer* lpXMessage);
	STDMETHOD_(BOOL,Subscribe)(DWORD dwCount, RSMSGID* lpMessageID);
	STDMETHOD_(BOOL,UnSubscribe)(DWORD dwCount, RSMSGID* lpMessageID);
	STDMETHOD_(VOID,FreeMessage)(XMessage* lpXMessage);
	STDMETHOD_(XMessage*, AllocMessage)(DWORD dwDataLen);

private:
	XMessage* CreateIssueMessage(RSMSGID dwMessageID,DWORD dwDataLen);
	XMessage* CreateLoginMessage(DWORD dwSubSystemID);
	XMessage* CreateSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID);
	XMessage* CreateUnSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID);
	VOID GetPrivateKey(R_RSA_PRIVATE_KEY* privateKey);
	VOID ReverseKey(R_RSA_PRIVATE_KEY* privateKey);

	DWORD  m_dwRefCount;
	BOOL   m_bLogin;
	XMutex m_xMutex;

	UTIL::com_ptr<IMsBufferMgr> m_pRsBufferManager;
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRuningTable;

	CXMSCProxyImpl m_objXMSCProxyImpl;

};

/////////////////////////////////////////////////////////////////////////////
