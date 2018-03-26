#include "stdafx.h"
#include "NetTcpAccepter.h"
#include "NetTcpConnector.h"

CNetTcpAccepter::CNetTcpAccepter(INetIoObj* pINetObj):
m_AcceptorObj(UTIL::com_ptr<INetIoSrv>(pINetObj)->GetIoService())
{
	m_pNetIoSrv = pINetObj;
	Init(NULL);
}
CNetTcpAccepter::~CNetTcpAccepter()
{
	Uinit();
}

STDMETHODIMP CNetTcpAccepter::Init(void*)
{
	return S_OK;
}

STDMETHODIMP CNetTcpAccepter::Uinit()
{
	Close();
	m_pCallBack = INULL;
	m_pNetIoSrv = INULL;
	return S_OK;
}

STDMETHODIMP CNetTcpAccepter::Close()
{
	boost::system::error_code ec;
	m_AcceptorObj.close(ec);
	return S_OK;
}

STDMETHODIMP CNetTcpAccepter::Cancel()
{
	boost::system::error_code ec;
	return S_OK;
}

STDMETHODIMP CNetTcpAccepter::Accept(LPCSTR lpAddr,WORD wListenPort)
{
	DWORD dwError = 0;
	
	boost::system::error_code ec;
	if(IsIPV6Address(lpAddr))
	{
		m_AcceptorObj.open(boost::asio::ip::tcp::v6(),ec);
		if(ec)
		{
			return ec.value();
		}

		m_AcceptorObj.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(),wListenPort),ec);
		if(ec)
		{
			dwError = ec.value();
			m_AcceptorObj.close(ec);
			return   dwError;
		}
	}
	else
	{
		m_AcceptorObj.open(boost::asio::ip::tcp::v4(),ec);
		if(ec)
		{
			return ec.value();
		}

		m_AcceptorObj.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),wListenPort),ec);
		if(ec)
		{
			dwError = ec.value();
			m_AcceptorObj.close(ec);
			return   dwError;
		}
	}

	m_AcceptorObj.listen(wListenPort,ec);
	if(ec)
	{
		dwError = ec.value();
		m_AcceptorObj.close(ec);
		return dwError;
	}

	start_accept();

	return S_OK;
}

void CNetTcpAccepter::start_accept()
{
	UTIL::com_ptr<INetTcpConnecter> pNewTcpSession (new CNetTcpConnecter(m_pNetIoSrv));
	if (pNewTcpSession)
	{
		m_AcceptorObj.async_accept(UTIL::com_ptr<IGetSocketHandle>(pNewTcpSession)->GetSocket(),
			boost::bind(&CNetTcpAccepter::handle_accept, this, UTIL::com_ptr<INetTcpConnecter>(pNewTcpSession),
			boost::asio::placeholders::error));
	}
	else
	{
		ZM1_GrpError(GrpName , _T("CNetTcpConnecter start_accept alloc CNetTcpConnecter Failed."));
		if (m_AcceptorObj.is_open())
			m_AcceptorObj.close();
		
		if (m_pCallBack)
			m_pCallBack->OnClose(ERROR_NOT_ENOUGH_MEMORY);
	}
}

void CNetTcpAccepter::handle_accept(UTIL::com_ptr<INetTcpConnecter> pSession, const boost::system::error_code& error)
{
	if (!error)
	{
		if (m_pCallBack)
		{
			UTIL::com_ptr<IGetSocketHandle> pSocketHandle = pSession;
			boost::system::error_code ec;
			//设定一系列的option
			boost::asio::socket_base::linger option(true, 0);
			pSocketHandle->GetSocket().set_option(option,ec);

			std::string straddr = "";
			WORD wPort = 0;

			tcp::endpoint endpoint = pSocketHandle->GetSocket().remote_endpoint(ec);
			if (!ec)
			{
				straddr =  GetIPString(endpoint);;
				wPort = endpoint.port();
			}

			tcp::endpoint localendpoint = pSocketHandle->GetSocket().local_endpoint();
			std::string strLocalIP = GetIPString(localendpoint);
			m_pCallBack->OnReport(strLocalIP.c_str());
			
			//回调上面
			m_pCallBack->OnAccept(UTIL::com_ptr<INetTcpConnecter>(pSession), straddr.c_str(),wPort, error.value());
			
			//继续开始监听连接
			start_accept();

		}
	}
	else
	{
		if ( ! m_AcceptorObj.is_open() && m_pCallBack)
		{
			m_pCallBack->OnClose(error.value());
		}
		
	}
}


STDMETHODIMP CNetTcpAccepter::SetCallBack(IOnNetTcpAccepter* pCallBack)
{
	RASSERTP(pCallBack , E_INVALIDARG);
	m_pCallBack = pCallBack;
	return S_OK;
}