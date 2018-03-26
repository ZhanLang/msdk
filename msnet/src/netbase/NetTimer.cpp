#include "StdAfx.h"
#include "NetTimer.h"

CNetTimer::CNetTimer(INetIoSrv* pNetIoSrv)
{
	m_pNetIoSrv = pNetIoSrv;
	m_pTimer = boost::shared_ptr<CDealTime>(new CDealTime(pNetIoSrv->GetIoService()));
	m_bCancel = false;
}

CNetTimer::~CNetTimer(void)
{
	m_pNetIoSrv = INULL;
	m_pTimer.reset();
}

void CNetTimer::Handle_Timer(const boost::system::error_code& error ,TimerContext_ptr ptr)
{
	if (!error)
	{
		if (!m_bCancel)
		{
			if (ptr->m_IFuncType)
			{
				ptr->m_IFuncType->OnNetTimer(ptr->m_lpVoid);
			}
			
			//这里判断一下，可能会在回调里面的调用取消，所以这个时候就不能定时了
			if (!m_bCancel)
			{
				m_pTimer->expires_from_now(boost::posix_time::seconds(ptr->m_dwSeconds));
				m_pTimer->async_wait(boost::bind(&CNetTimer::Handle_Timer,
				this,
				boost::asio::placeholders::error,
				ptr));
			}
		}
	}
}


STDMETHODIMP CNetTimer::Schedule(IOnNetTimer* pOnNetTimer,LPVOID lpVoid,DWORD dwSeconds/*=1*/)
{
	TimerContext_ptr ptr(new CTimerContext);
	ptr->m_IFuncType = pOnNetTimer;
	RASSERT(ptr->m_IFuncType,E_INVALIDARG);
	ptr->m_lpVoid = lpVoid;
	ptr->m_dwSeconds = dwSeconds;

	m_pTimer->expires_from_now(boost::posix_time::seconds(dwSeconds));
	m_pTimer->async_wait(boost::bind(&CNetTimer::Handle_Timer,
		this,
		boost::asio::placeholders::error,
		ptr));

	return S_OK;
}

STDMETHODIMP CNetTimer::Cancel()
{
	m_bCancel = true;
	boost::system::error_code ec;
	m_pTimer->cancel(ec);
	return S_OK;
}