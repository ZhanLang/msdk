#include "StdAfx.h"
#include "NetUdpAdapter.h"

CNetUdpAdapter::CNetUdpAdapter(INetIoSrv* pNetIoSrv)
{
	assert(!pNetIoSrv);
	m_pNetIoSrv = pNetIoSrv;
	m_pSocket = boost::shared_ptr<CUdpSocket>(new CUdpSocket(m_pNetIoSrv->GetIoService()));
	
	m_bClose = FALSE;
	m_bIPV6 = FALSE;
}

CNetUdpAdapter::~CNetUdpAdapter(void)
{
	if(m_pSocket->is_open())
	{
		boost::system::error_code ec;
		m_pSocket->close(ec);
	}

	m_pNetIoSrv = INULL;
	m_lpCallBack = INULL;
}

STDMETHODIMP CNetUdpAdapter::SetCallBack(IOnNetUdpAdapter* callBack)
{
	RASSERTP(callBack , E_INVALIDARG);
	m_lpCallBack = callBack;	
	return S_OK;
}

STDMETHODIMP CNetUdpAdapter::ReadDgram(IMsBuffer* lpBuf,const void * act)
{
	UdpContext_ptr ptr(new CUdpContext);
	ptr->m_MsBuffer = lpBuf;
	ptr->m_Act = act;

	RASSERT((ptr->m_MsBuffer),E_INVALIDARG);

	m_pSocket->async_receive_from(
		boost::asio::buffer(m_Data), ptr->m_Sender_endpoint,
		boost::bind(&CNetUdpAdapter::handle_receive_from, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred,
		ptr));

	return S_OK;
}

static BOOL IsBroadcast(LPCSTR lpAddr,INT nLen,OUT LPSTR strIP)
{
	if(NULL == lpAddr)
		return FALSE;
	strcpy_s(strIP, nLen, lpAddr);
	int nSize = (int)strlen(lpAddr);
	BOOL bBroadcast = FALSE;
	for(int i=0;i<nSize;i++)
	{
		if( *(strIP+i) == ('*'))
		{
			*(strIP+i) = '1';
			bBroadcast = TRUE;
		}
	}
	return bBroadcast;
}


STDMETHODIMP CNetUdpAdapter::WriteDgram(IMsBuffer* lpBuf,LPCSTR lpAddr,WORD wPort,const void * act)
{
	UdpContext_ptr ptr(new CUdpContext);
	ptr->m_MsBuffer = lpBuf;
	ptr->m_Act = act;
	RASSERT(ptr->m_MsBuffer,E_INVALIDARG);

	udp::endpoint  sendpoint; 
	if( m_bIPV6 )
	{  
		//不支持广播
		if(! IsIPV6Address(lpAddr))
			return E_INVALIDARG;
		udp::endpoint endpoint(boost::asio::ip::udp::v6(), wPort);
		endpoint.address(boost::asio::ip::address::from_string(lpAddr));
		sendpoint = endpoint;
	}
	else
	{
		if( IsIPV6Address(lpAddr))
			return E_INVALIDARG;

		udp::endpoint endpoint(boost::asio::ip::udp::v4(), wPort);
		char szIP[500]= {0};
		if(NULL == lpAddr )
		{
			//是广播
			boost::asio::socket_base::broadcast option(true);
			m_pSocket->set_option(option);
			endpoint.address(boost::asio::ip::address_v4::broadcast());
			sendpoint = endpoint;
		}
		else if(IsBroadcast(lpAddr,500,szIP))
		{
			boost::asio::socket_base::broadcast option(true);
			m_pSocket->set_option(option);
			endpoint.address(boost::asio::ip::address_v4::broadcast(
				boost::asio::ip::address_v4::from_string(szIP),
				boost::asio::ip::address_v4::netmask(boost::asio::ip::address_v4::from_string(szIP)))
				);
		}
		else
		{
			boost::asio::socket_base::broadcast option(false);
			m_pSocket->set_option(option);
			endpoint.address(boost::asio::ip::address::from_string(lpAddr));
		}
		sendpoint = endpoint;
	}


	LPBYTE lpByteBuf = NULL;
	DWORD dwBufSize = 0;
	ptr->m_MsBuffer->GetRawBuffer(&lpByteBuf,&dwBufSize);

	m_pSocket->async_send_to(boost::asio::buffer(lpByteBuf,dwBufSize),sendpoint,
		boost::bind(&CNetUdpAdapter::handle_send_to,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred,
		ptr));

	return S_OK;
}

STDMETHODIMP CNetUdpAdapter::Listen(LPCSTR lpAddr,WORD wPort)
{
	m_bClose = FALSE;
	boost::system::error_code ec;
	if( m_bIPV6 = IsIPV6Address(lpAddr))
	{
		udp::endpoint endpoint(boost::asio::ip::udp::v6(), wPort);
		m_pSocket->open(boost::asio::ip::udp::v6());
		m_pSocket->bind(endpoint,ec);
	}
	else
	{
		udp::endpoint endpoint(boost::asio::ip::udp::v4(), wPort);
		m_pSocket->open(boost::asio::ip::udp::v4());
		m_pSocket->bind(endpoint,ec);
	}

	if (ec)
	{
		return ec.value();
	}

	return S_OK;
}

STDMETHODIMP CNetUdpAdapter::Close()
{
	m_bClose = TRUE;
	boost::system::error_code ec;
	m_pSocket->close(ec);
	return S_OK;
}

STDMETHODIMP CNetUdpAdapter::Cancel()
{
	//boost::system::error_code ec;
	//m_Socket.cancel(ec);
	return S_OK;
}

void CNetUdpAdapter::handle_send_to(const boost::system::error_code& error,size_t bytes_sent,UdpContext_ptr ptr)
{
	if (!error)
	{
		if (m_lpCallBack)
		{
			m_lpCallBack->OnWriteDgram(ptr->m_MsBuffer,error.value(),ptr->m_Act);
		}
	}
	else
	{
		//错误处理
		OutputDebugString(_T("[RSENGINE]发数据包有错误!\n"));
		if (m_lpCallBack && ! m_pSocket->is_open())
		{
			m_lpCallBack->OnClose(error.value());
		}
	}
}
//发送数据
//
void CNetUdpAdapter::handle_receive_from(const boost::system::error_code& error,size_t bytes_recvd,
									  UdpContext_ptr ptr)
{
	if (!error && bytes_recvd>0)
	{			
		//读取成功，然后回调给相应函数
		if (ptr->m_MsBuffer)
		{
			ptr->m_MsBuffer->AddTail(m_Data.data(),(DWORD)bytes_recvd);
		}
		if (m_lpCallBack)
		{
			udp::endpoint& endpoint1 = ptr->m_Sender_endpoint;

			std::string straddr = endpoint1.address().to_string();
			WORD wPort = endpoint1.port();
			m_lpCallBack->OnReadDgram(ptr->m_MsBuffer,straddr.c_str(),wPort,error.value(),ptr->m_Act);
		}
	}
	else
	{
		OutputDebugString(_T("[RSENGINE]收数据包有错误!\n"));
		//对方机器启动但是没开此端口，会回来一个拒绝包，返上层，再收下个包
		if (m_lpCallBack) //可能关闭时要产生
		{
			if( ! m_pSocket->is_open() || m_bClose) 
				m_lpCallBack->OnClose(error.value());
			else if(m_pSocket->is_open())//错误处理
				m_lpCallBack->OnReadDgram(NULL,"",0,error.value(),ptr->m_Act);

		}
	}
}