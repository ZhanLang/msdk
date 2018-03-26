#pragma once

#include "netbase.h"
#include <boost\noncopyable.hpp>
#include <boost\asio\deadline_timer.hpp>
#include "NetIoObj.h"

class CTimerContext
	: private boost::noncopyable  //这个对象不能被copy
{
public:
	CTimerContext()
	{
		m_lpVoid = NULL;
		m_dwSeconds = NULL;
	}

	UTIL::com_ptr<IOnNetTimer> m_IFuncType;
	LPVOID m_lpVoid;
	DWORD m_dwSeconds;
};
typedef boost::shared_ptr<CTimerContext> TimerContext_ptr;
typedef boost::asio::deadline_timer CDealTime;
class CNetTimer :
	public INetTimer,
	private CUnknownImp,
	private  boost::noncopyable
{
public:
	UNKNOWN_IMP1(INetTimer);
	STDMETHOD(init_class)(IMSBase* , IMSBase*)
	{
		return S_OK;
	}

	CNetTimer(INetIoSrv* pNetIoSrv);
	~CNetTimer(void);
	STDMETHOD(Schedule)(IOnNetTimer* pOnNetTimer,LPVOID lpVoid,DWORD dwSeconds=1);
	STDMETHOD(Cancel)();

private:
	void Handle_Timer(const boost::system::error_code& error ,TimerContext_ptr ptr);
private:
	
	boost::shared_ptr<CDealTime> m_pTimer;
	bool	m_bCancel;
	UTIL::com_ptr<INetIoSrv> m_pNetIoSrv;
};
