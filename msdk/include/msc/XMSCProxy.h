#ifndef _X_MSC_PROXY_H_
#define _X_MSC_PROXY_H_

#include "XMessage.h"
#include "XMSCDispatcher.h"
#include "combase/IMsBuffer.h"



//
/////////////////////////////////////////////////////////////////////////////
class XMSCProxy
{
public:
	virtual DWORD WINAPI AddRef() = 0;
	virtual DWORD WINAPI Release() = 0;
	virtual BOOL  WINAPI Login(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync = TRUE, DWORD dwTimeOut = INFINITE, LPCTSTR lpszName = NULL) = 0;
	virtual VOID  WINAPI Logoff() = 0;
	virtual BOOL  WINAPI Issue(XMessage* lpXMessage) = 0;
	virtual BOOL  WINAPI Subscribe(DWORD dwCount, RSMSGID* lpMessageID) = 0;
	virtual BOOL  WINAPI UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID) = 0;
	virtual VOID  WINAPI FreeMessage(XMessage* lpXMessage) = 0;
	virtual XMessage* WINAPI AllocMessage(DWORD dwDataLen) = 0;
};
///////////////////////////////////////////////////////////////
//新的COM接口
interface IXMsgBusProxy:public IUnknown
{
	STDMETHOD_(BOOL , Login)(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync = TRUE, DWORD dwTimeOut = INFINITE, LPCTSTR lpszName = NULL) = 0;
	STDMETHOD_(VOID , Logoff)() = 0;
	STDMETHOD_(BOOL , Issue)(RSMSGID dwMessageID,IMsBuffer* lpXMessage) = 0;
	STDMETHOD_(BOOL , Subscribe)(DWORD dwCount, RSMSGID* lpMessageID) = 0;
	STDMETHOD_(BOOL , UnSubscribe)(DWORD dwCount, RSMSGID* lpMessageID) = 0;
	//保留不要用，
	STDMETHOD_(VOID , FreeMessage)(XMessage* lpXMessage) = 0;
	STDMETHOD_(XMessage*, AllocMessage)(DWORD dwDataLen) = 0;
};
MS_DEFINE_IID(IXMsgBusProxy,"{64B2CAA9-D11D-436a-894F-BE6CD245620F}");
MS_DEFINE_GUID(CLSID_MsgBusProxy,
			   // {1F289203-C14D-44c6-8ABB-0CE576B7B752}
			   0x1f289203, 0xc14d, 0x44c6, 0x8a, 0xbb, 0xc, 0xe5, 0x76, 0xb7, 0xb7, 0x52);

interface IXMsgBusProxy2:public IXMsgBusProxy
{
	STDMETHOD_(BOOL , Send)(RSMSGID dwMessageID,IMsBuffer* lpXMessage) = 0;
};
MS_DEFINE_IID(IXMsgBusProxy2,"{4FBBE44B-F40B-4c67-B73A-08E3B21245D4}");

/////////////////////////////////////////////////////////////////////////////
#endif
