#ifndef _CX_MSC_PROXY_H_
#define _CX_MSC_PROXY_H_

#include "XMSCProxy.h"
#include "XMutex.h"
#include "CXMSCProxyImpl.h"
#include "RSA.h"
/////////////////////////////////////////////////////////////////////////////
class CXMSCProxy :public XMSCProxy
{
public:
	CXMSCProxy();
	~CXMSCProxy();

	virtual DWORD WINAPI AddRef();
	virtual DWORD WINAPI Release();
	virtual BOOL  WINAPI Login(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync = TRUE, DWORD dwTimeOut = INFINITE, LPCTSTR lpszName = NULL);
	virtual VOID  WINAPI Logoff();
	virtual BOOL  WINAPI Issue(XMessage* lpXMessage);
	virtual BOOL  WINAPI Subscribe(DWORD dwCount, RSMSGID* lpMessageID);
	virtual BOOL  WINAPI UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID);
	virtual VOID  WINAPI FreeMessage(XMessage* lpXMessage);
	virtual XMessage* WINAPI AllocMessage(DWORD dwDataLen);

private:
	XMessage* CreateLoginMessage(DWORD dwSubSystemID);
	XMessage* CreateSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID);
	XMessage* CreateUnSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID);
	VOID GetPrivateKey(R_RSA_PRIVATE_KEY* privateKey);
	VOID ReverseKey(R_RSA_PRIVATE_KEY* privateKey);

	DWORD  m_dwRefCount;
	BOOL   m_bLogin;
	XMutex m_xMutex;
	CXMSCProxyImpl m_objXMSCProxyImpl;
};

/////////////////////////////////////////////////////////////////////////////
#endif