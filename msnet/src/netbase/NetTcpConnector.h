#pragma once
#include "ObjPoolFactory.hpp"
#include "netbase.h"

#include <boost\smart_ptr\enable_shared_from_this.hpp>
#include <boost\noncopyable.hpp>
#include "NetIoObj.h"


using namespace msnet;

//上下文
class CTcpContext
	: private boost::noncopyable  //这个对象不能被copy
{
public:
	CTcpContext()
	{
		m_Act = NULL;
		m_dwByteWriteTotal = 0;
		m_dwByteWriteHasWirte = 0;
	}

	~CTcpContext()
	{
		
	}

	UTIL::com_ptr<IMsBuffer> m_MsBuffer;
	DWORD m_dwByteWriteTotal;
	DWORD m_dwByteWriteHasWirte;
	const void*  m_Act;
};


typedef CObjPoolFactory<CTcpContext> TcpContextFactory;

//内部使用的接口
typedef boost::asio::ip::tcp::socket CTcpSocket;
struct IGetSocketHandle : public IMSBase
{
	STDMETHOD_(CTcpSocket& ,GetSocket)() = 0;
};
MS_DEFINE_IID(IGetSocketHandle , "{C998AAED-9C77-44D8-9CD3-BA923D5A2299}");


class CNetTcpConnecter : public INetTcpConnecter , 
						 public IGetSocketHandle,
						 public  boost::noncopyable,
						 public CUnknownImp
{

public:
	UNKNOWN_IMP2_(INetTcpConnecter , IGetSocketHandle);
	CNetTcpConnecter(INetIoSrv* pNetIoSrv);

	~CNetTcpConnecter();
	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter)
	{
		return S_OK;
	}

public:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uinit)();

	//
	STDMETHOD(ReadStream)(IMsBuffer* lpBuf , const void * act = 0);
	STDMETHOD(WriteStream)(IMsBuffer* lpBuf, const void * act = 0);
	STDMETHOD(Connect)(LPCSTR lpAddr,WORD wPort);
	STDMETHOD(Close)();
	STDMETHOD(Cancel)();
	STDMETHOD(SetCallBack)(IOnNetTcpConnecter* pCallBack);

	STDMETHOD_(CTcpSocket&, GetSocket)();
private:
	void handle_resolve(const boost::system::error_code& err,
						boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void handle_connect(const boost::system::error_code& err,
						boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void handle_connect2(const boost::system::error_code& err);

	void handle_read(const boost::system::error_code& error,
					 size_t bytes_transferred,CTcpContext* ptr);
	
	void handle_write(const boost::system::error_code& error,
					   size_t bytes_transferred,CTcpContext* ptr);

private:
	
	typedef boost::asio::io_service::strand CStrand;
	typedef boost::asio::ip::tcp::resolver CTcpResolver;

	UTIL::com_ptr<INetIoSrv> m_pNetIoSrv;
	UTIL::com_ptr<IOnNetTcpConnecter> m_pCallBack;

	CTcpSocket   m_pTcpSocket;
	CTcpResolver m_pTcpResolver;
	CStrand      m_pStrand;


	boost::array<BYTE, 8096> m_Data;
	boost::asio::ip::tcp::endpoint m_RemoteEndPoint;
};

