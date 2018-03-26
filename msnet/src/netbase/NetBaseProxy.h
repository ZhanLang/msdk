#pragma once
#include "NetTcpAccepter.h"
class CNetBaseProxy : public INetBaseProxy , private CUnknownImp
{

public:
	UNKNOWN_IMP1(INetBaseProxy);
	STDMETHOD(init_class)(IMSBase*prot , IMSBase*pout)
	{
		return S_OK;
	}

	CNetBaseProxy(void);
	~CNetBaseProxy(void);

public:
	STDMETHOD(CreateNetIoObj)(INetIoObj** pNetIoObj);
	STDMETHOD(CreateTcpConnecter)(INetIoObj* pNetIoObj , INetTcpConnecter**pConnecter);
	STDMETHOD(CreateTcpAccepter)(INetIoObj* pNetIoObj , INetTcpAccepter**pConnecter);
	STDMETHOD(CreateUdpAdapter)(INetIoObj* pNetIoObj , INetUdpAdapter**pUdpAdapter);
	STDMETHOD(CreateNetTimer)(INetIoObj* pNetIoObj ,INetTimer** pNetTimer);
};
