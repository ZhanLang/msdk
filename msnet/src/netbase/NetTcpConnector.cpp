#pragma once
#include "stdafx.h"
#include "NetTcpConnector.h"
using namespace boost::asio::ip;

////////////////////////////////////////////////////////////////////////////////
//

template <typename Endpoint_type,typename Handler>
void async_connect_ex(boost::asio::ip::tcp::socket& socket, const Endpoint_type& peer_endpoint ,Handler handler)
{
	using boost::asio::windows::overlapped_ptr;
	overlapped_ptr overlapped(socket.get_io_service(), handler);
	boost::system::error_code ec;
	ec.clear();
	if (socket.is_open())
	{
		socket.close();
	}

	if(socket.open(peer_endpoint.protocol(),ec))
	{
		overlapped.complete(ec, 0);
		return;
	}

	boost::asio::socket_base::non_blocking_io command(true);
	ec.clear();
	if(socket.io_control(command,ec))
	{
		overlapped.complete(ec, 0);
		return;
	}
	ec.clear();

	boost::asio::socket_base::linger option(true, 0);
	if (socket.set_option(option,ec))
	{
		overlapped.complete(ec, 0);
		return;
	}

	ec.clear();
	tcp::endpoint endpoint(peer_endpoint.protocol(), 0);
	//if (socket.bind(Endpoint_type(),ec))
	if (socket.bind(endpoint,ec))
	{
		overlapped.complete(ec, 0);
		return;
	}

	//获得ConnectEx地址(此函数没暴露出来)
	DWORD dwBytes = 0;
	LPFN_CONNECTEX   lpfnConnectEx   =   NULL;  
	GUID   GuidConnectEx   =   WSAID_CONNECTEX;   //   The Guid 
	BOOL dwErr   =   ::WSAIoctl(socket.native(), 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidConnectEx, 
		sizeof(GuidConnectEx), 
		&lpfnConnectEx, 
		sizeof(lpfnConnectEx), 
		&dwBytes, 
		NULL, 
		NULL); 
	if(dwErr  ==  SOCKET_ERROR) 
	{
		boost::system::error_code ec(GetLastError(),
			boost::asio::error::get_system_category());
		overlapped.complete(ec, 0);
		return;
	}

	u_long bytes_write =0;
	DWORD btw = 0;
	BOOL ok =  (lpfnConnectEx)(socket.native(),
		(sockaddr*)peer_endpoint.data(),
		peer_endpoint.size(),
		0,
		btw,
		&bytes_write,
		overlapped.get());  

	DWORD last_error = ::GetLastError();

	if (!ok && last_error != ERROR_IO_PENDING)
	{
		boost::system::error_code ec(last_error,
			boost::asio::error::get_system_category());
		overlapped.complete(ec, 0);
	}
	else
	{
		overlapped.release();
	}
}


////////////////////////////////////////////////////////////////////////
//使用了 未导出的ConnectEx 函数，在XP以上支持
static BOOL IsXPAbove()
{
	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if( ! ::GetVersionEx( &osver ))
		return FALSE;

	if(	osver.dwPlatformId == VER_PLATFORM_WIN32_NT 
		&& ( osver.dwMajorVersion >5 || (osver.dwMajorVersion == 5 && osver.dwMinorVersion >0 )))
		return TRUE;
	return FALSE;
}


CNetTcpConnecter::CNetTcpConnecter(INetIoSrv* pNetIoSrv)
:m_pTcpSocket(pNetIoSrv->GetIoService()),
m_pTcpResolver(pNetIoSrv->GetIoService()),
m_pStrand(pNetIoSrv->GetIoService())
{
	m_pNetIoSrv = pNetIoSrv;
	Init(NULL);
}

CNetTcpConnecter::~CNetTcpConnecter()
{
	Uinit();
}


STDMETHODIMP CNetTcpConnecter::Init(void*)
{
	RASSERTP(m_pNetIoSrv , E_FAIL);
	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::Uinit()
{
	if(m_pTcpSocket.is_open())
	{
		m_pTcpSocket.close();
	}
	
	m_pTcpSocket.close();
	m_pTcpResolver.cancel();
	
	m_pNetIoSrv = INULL;
	m_pCallBack = INULL;
	return S_OK;
}

void CNetTcpConnecter::handle_resolve(const boost::system::error_code& err,
									  boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;	
		m_RemoteEndPoint = endpoint;
		//先不用它，断开重连会有错

		if(IsXPAbove())
		{   
			async_connect_ex(m_pTcpSocket,endpoint,
				boost::bind(&CNetTcpConnecter::handle_connect, this,
				boost::asio::placeholders::error,
				++endpoint_iterator)
				);
		}	
		else //2000下要用这个    
		{    
			m_pTcpSocket.async_connect(endpoint,
				boost::bind(&CNetTcpConnecter::handle_connect, this,
				boost::asio::placeholders::error,
				++endpoint_iterator)
				);
		}
	}
	else
	{	
		if (m_pCallBack)
		{
			m_pCallBack->OnConnet(0, 0, err.value());
		}
	}
}

void CNetTcpConnecter::handle_connect(const boost::system::error_code& err,
					boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if(err)
	{
		if (endpoint_iterator != tcp::resolver::iterator())
		{
			m_pTcpSocket.close();
			tcp::endpoint endpoint = *endpoint_iterator;
			m_RemoteEndPoint = endpoint;
			m_pTcpSocket.async_connect(endpoint,
				boost::bind(&CNetTcpConnecter::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator));
		}
		else
		{
			if (m_pCallBack)
			{
				m_pCallBack->OnConnet(0, 0, err.value());//???
			}
		}
	}
	else
	{
		if (m_pCallBack)
		{
			tcp::endpoint localendpoint = m_pTcpSocket.local_endpoint();
			std::string strLocalIP = GetIPString(localendpoint);
			
			m_pCallBack->OnReport(strLocalIP.c_str());

			std::string straddr = GetIPString(m_RemoteEndPoint);
			WORD wPort = m_RemoteEndPoint.port();
			m_pCallBack->OnConnet(straddr.c_str(), wPort, err.value());
		}
	}
}

void CNetTcpConnecter::handle_connect2(const boost::system::error_code& err)
{
	if(err)
	{
		if (m_pCallBack)
		{
			m_pCallBack->OnConnet(0, 0, err.value());
		}
	}
	else
	{
		if (m_pCallBack)
		{
			tcp::endpoint localendpoint = m_pTcpSocket.local_endpoint();
			std::string strLocalIP = GetIPString(localendpoint);
			m_pCallBack->OnReport(strLocalIP.c_str());

			std::string straddr = GetIPString(m_RemoteEndPoint);//endpoint.address().to_string();
			WORD wPort = m_RemoteEndPoint.port();
			m_pCallBack->OnConnet(straddr.c_str(), wPort, err.value());
		}
	}
}

void CNetTcpConnecter::handle_read(const boost::system::error_code& error,
				 size_t bytes_transferred,CTcpContext* ptr)
{
	if (!error)
	{			
		//读取成功，然后回调给相应函数
		if (ptr->m_MsBuffer)
		{
			ptr->m_MsBuffer->AddTail(m_Data.data(),(DWORD)bytes_transferred);
		}

		if (m_pCallBack)
		{
			m_pCallBack->OnReadStream(ptr->m_MsBuffer,error.value(),bytes_transferred,ptr->m_Act);
		}
	}
	else
	{
		//错误处理
		if (m_pCallBack)
		{
			m_pCallBack->OnClose(error.value());
		}
	}

	TcpContextFactory::Free(ptr);
}

void CNetTcpConnecter::handle_write(const boost::system::error_code& error,
				  size_t bytes_transferred,CTcpContext* ptr)
{
	if (!error)
	{
		//写成功，看看有没有完毕，如果没有完毕就继续写

		DWORD dwByteSend = ptr->m_dwByteWriteHasWirte+bytes_transferred;
		DWORD dwByteDonotSend = ptr->m_dwByteWriteTotal-dwByteSend;

		if (dwByteSend < ptr->m_dwByteWriteTotal)
		{
			LPBYTE lpBuf = NULL;
			DWORD dwBufSize = 0;

			ptr->m_MsBuffer->GetRawBuffer(&lpBuf,&dwBufSize);

			m_pTcpSocket.async_send(boost::asio::buffer(lpBuf+dwByteSend ,dwByteDonotSend),
				boost::bind(&CNetTcpConnecter::handle_write,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				ptr));
		}
		else
		{
			if (m_pCallBack)
			{
				m_pCallBack->OnWriteStream(ptr->m_MsBuffer,error.value(),bytes_transferred,ptr->m_Act);
			}
		}
	}
	else
	{
		//错误处理
		if (m_pCallBack)
		{
			m_pCallBack->OnClose(error.value());
		}
	}
	TcpContextFactory::Free(ptr);
}

STDMETHODIMP CNetTcpConnecter::ReadStream(IMsBuffer* lpBuf , const void * act/* = 0*/)
{
	CTcpContext* ptr = TcpContextFactory::Alloc();
	if(ptr == 0)
	{
		return S_FALSE;
	}

	ptr->m_MsBuffer = lpBuf;
	ptr->m_Act = act;

	RASSERT((ptr->m_MsBuffer),E_INVALIDARG);

	m_pTcpSocket.async_read_some(boost::asio::buffer(m_Data),
		m_pStrand.wrap
		(
		boost::bind(&CNetTcpConnecter::handle_read,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred,ptr)
		)
		);

	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::WriteStream(IMsBuffer* lpBuf, const void * act/* = 0*/)
{
	CTcpContext* ptr = TcpContextFactory::Alloc();
	if(ptr == 0)
	{
		return S_FALSE;
	}

	ptr->m_MsBuffer = lpBuf;
	ptr->m_Act = act;

	RASSERT(ptr->m_MsBuffer,E_INVALIDARG);

	LPBYTE lpByteBuf = NULL;
	DWORD dwBufSize = 0;

	ptr->m_MsBuffer->GetRawBuffer(&lpByteBuf,&dwBufSize);
	ptr->m_dwByteWriteTotal = dwBufSize;

	m_pTcpSocket.async_send(boost::asio::buffer(lpByteBuf,dwBufSize),
		m_pStrand.wrap
		(
		boost::bind(&CNetTcpConnecter::handle_write,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred,
		ptr)
		)
		);

	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::Connect(LPCSTR lpAdd,WORD wPort)
{
	RTEST( 0== wPort , E_INVALIDARG);
	RASSERT(lpAdd ,E_INVALIDARG);

	if(IsIPV6Address(lpAdd))
	{
		tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), wPort);
		endpoint.address(boost::asio::ip::address_v6::from_string(lpAdd));
		m_RemoteEndPoint = endpoint;
		//
		if(IsXPAbove())
			async_connect_ex(m_pTcpSocket,endpoint,
			boost::bind(&CNetTcpConnecter::handle_connect2, this,boost::asio::placeholders::error));
		else
			m_pTcpSocket.async_connect(endpoint,
			boost::bind(&CNetTcpConnecter::handle_connect2, this, boost::asio::placeholders::error));
		return S_OK;       
	}

	//加入一个解析器
	char szServerName[20] = {0};
	sprintf_s(szServerName,20,"%d",wPort);
	tcp::resolver::query query(lpAdd, szServerName);
	m_pTcpResolver.async_resolve(query,
		boost::bind(&CNetTcpConnecter::handle_resolve, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));

	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::Close()
{
	boost::system::error_code ec;
	m_pTcpSocket.close(ec);
	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::Cancel()
{
	boost::system::error_code ec;
	return S_OK;
}

STDMETHODIMP CNetTcpConnecter::SetCallBack(IOnNetTcpConnecter* pCallBack)
{
	m_pCallBack = pCallBack;
	return S_OK;
}


STDMETHODIMP_(CTcpSocket&) CNetTcpConnecter::GetSocket()
{
	return m_pTcpSocket;
}