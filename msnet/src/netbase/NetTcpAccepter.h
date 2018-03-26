#pragma once
#include "netbase.h"
#include <boost\smart_ptr\enable_shared_from_this.hpp>
#include <boost\noncopyable.hpp>
#include <boost\asio.hpp>
#include "NetIoObj.h"
#include "NetTcpConnector.h"
using namespace msnet;
using namespace boost::asio::ip;
class CNetTcpAccepter : public  INetTcpAccepter ,  
						private CUnknownImp, 
						private boost::noncopyable
{
public:
	UNKNOWN_IMP1(INetTcpAccepter);
	STDMETHOD(init_class)(IMSBase* prot , IMSBase* pout)
	{
		return S_OK;
	}
	CNetTcpAccepter(INetIoObj* pINetObj);
	~CNetTcpAccepter();
public:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uinit)();
	STDMETHOD(Close)();
	STDMETHOD(Cancel)();

	STDMETHOD(Accept)(LPCSTR lpAddr,WORD wListenPort);
	STDMETHOD(SetCallBack)(IOnNetTcpAccepter* pCallBack);
private:
	void handle_accept(UTIL::com_ptr<INetTcpConnecter> new_session, const boost::system::error_code& error);
	
	void start_accept(); 
private:
	UTIL::com_ptr<INetIoSrv> m_pNetIoSrv;
	typedef boost::asio::ip::tcp::acceptor CTcpAcceptor;
	CTcpAcceptor m_AcceptorObj;
	UTIL::com_ptr<IOnNetTcpAccepter> m_pCallBack;
};