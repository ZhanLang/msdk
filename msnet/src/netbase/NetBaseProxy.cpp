#include "StdAfx.h"
#include "NetBaseProxy.h"
#include "NetUdpAdapter.h"
#include "NetTimer.h"
CNetBaseProxy::CNetBaseProxy(void)
{
}

CNetBaseProxy::~CNetBaseProxy(void)
{
	
}

STDMETHODIMP CNetBaseProxy::CreateNetIoObj(INetIoObj** pNetIoObj)
{
	RASSERTP(pNetIoObj , E_INVALIDARG);
	CNetIoObj* pObj  = new CNetIoObj();
	RASSERTP(pObj , E_FAIL);
	return  pObj->QueryInterface(re_uuidof(INetIoObj) , (void**)pNetIoObj);
}

STDMETHODIMP CNetBaseProxy::CreateTcpConnecter(INetIoObj* pNetIoObj , INetTcpConnecter**pConnecter)
{
	RASSERTP(pNetIoObj , E_INVALIDARG);
	RASSERTP(pConnecter , E_INVALIDARG);

	CNetTcpConnecter* pC =new CNetTcpConnecter(UTIL::com_ptr<INetIoSrv>(pNetIoObj));
	RASSERTP(pC , E_FAIL);
	return pC->QueryInterface(re_uuidof(INetTcpConnecter) , (void**)pConnecter);
}

STDMETHODIMP CNetBaseProxy::CreateTcpAccepter(INetIoObj* pNetIoObj , INetTcpAccepter**pAccepter)
{
	RASSERTP(pNetIoObj , E_INVALIDARG);
	RASSERTP(pAccepter , E_INVALIDARG);

	CNetTcpAccepter* pAcp = new CNetTcpAccepter(pNetIoObj);
	RASSERTP(pAcp , E_FAIL);
	return pAcp->QueryInterface(re_uuidof(INetTcpAccepter) , (void**)pAccepter);
}

STDMETHODIMP CNetBaseProxy::CreateUdpAdapter(INetIoObj* pNetIoObj , INetUdpAdapter**pUdpAdapter)
{
	RASSERTP(pNetIoObj , E_INVALIDARG);
	RASSERTP(pUdpAdapter , E_INVALIDARG);

	CNetUdpAdapter* pA = new CNetUdpAdapter(UTIL::com_ptr<INetIoSrv>(pNetIoObj));
	RASSERTP(pA , E_FAIL);
	return pA->QueryInterface(re_uuidof(INetUdpAdapter) , (void**)pUdpAdapter);
}

STDMETHODIMP CNetBaseProxy::CreateNetTimer(INetIoObj* pNetIoObj ,INetTimer** pNetTimer)
{
	RASSERTP(pNetIoObj , E_INVALIDARG);
	RASSERTP(pNetTimer , E_INVALIDARG);

	CNetTimer* pT = new CNetTimer(UTIL::com_ptr<INetIoSrv>(pNetIoObj));
	RASSERTP(pT , E_FAIL);
	return pT->QueryInterface(re_uuidof(INetTimer) , (void**)pNetTimer);
}