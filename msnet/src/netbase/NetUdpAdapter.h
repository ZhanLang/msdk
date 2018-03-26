#pragma once
#include "netbase.h"
#include <boost\noncopyable.hpp>
#include "NetIoObj.h"
using namespace msnet;
using boost::asio::ip::udp;
class CUdpContext
	: private boost::noncopyable  //这个对象不能被copy
{
public:
	CUdpContext()
	{
		m_bIsBroadCast = false;
		m_lBroadCount = 0;
		m_Act = NULL;
	}

	UTIL::com_ptr<IMsBuffer> m_MsBuffer;
	bool m_bIsBroadCast;
	LONG m_lBroadCount;
	udp::endpoint m_Sender_endpoint;
	const void* m_Act;
};
typedef boost::shared_ptr<CUdpContext> UdpContext_ptr;
typedef boost::asio::ip::udp::socket   CUdpSocket;

class CNetUdpAdapter : public  INetUdpAdapter ,
					   private boost::noncopyable, 
					   private CUnknownImp
{
public:
	UNKNOWN_IMP1(INetUdpAdapter);
	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pout)
	{
		return S_OK;
	}

	CNetUdpAdapter(INetIoSrv* pNetIoSrv);
	~CNetUdpAdapter(void);

	STDMETHOD(SetCallBack)(IOnNetUdpAdapter* callBack);
	STDMETHOD(ReadDgram)(IMsBuffer* lpBuf,const void * act);
	STDMETHOD(WriteDgram)(IMsBuffer* lpBuf,LPCSTR lpAddr,WORD wPort,const void * act);
	STDMETHOD(Listen)(LPCSTR lpAddr,WORD wPort);
	STDMETHOD(Close)();
	STDMETHOD(Cancel)();

protected:
	void handle_receive_from(const boost::system::error_code& error,size_t bytes_recvd,UdpContext_ptr ptr);
	void handle_send_to(const boost::system::error_code& error,size_t bytes_sent,UdpContext_ptr ptr);
private:
	UTIL::com_ptr<INetIoSrv> m_pNetIoSrv;

	BOOL  m_bClose;
	boost::shared_ptr<CUdpSocket> m_pSocket;
	boost::array<BYTE, 2048> m_Data;
	UTIL::com_ptr<IOnNetUdpAdapter> m_lpCallBack;
	BOOL m_bIPV6;
};
