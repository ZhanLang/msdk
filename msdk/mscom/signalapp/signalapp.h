#pragma once
#include "signalapp\ISignalApp.h"
#include "util\globalevent.h"
#include "commx\commx3.h"
#include "mscomhelper\connectionpointhelper.h"
//#include "commx\commx_call.h"

class CSignalAppliction:
	public IMsPlugin,
	public IMessageCallBack,
	public CMsComBase<CSignalAppliction>,
	public CConnectionPointContainerHelper<CSignalAppliction> //实现连接点
{
public:
	
	UNKNOWN_IMP3_( IMsPlugin , IMessageCallBack,IMsConnectionPointContainer);

	CSignalAppliction()
	{
		m_pCenter = NULL;
		m_pClientObject = NULL;
		AddConnectionPoint(re_uuidof(ISingleAppMsgConnectPoint), m_SignalAppConnectPoint);
	}

	~CSignalAppliction()
	{
	}

	STDMETHOD(Init)(void*) ;
	STDMETHOD(Uninit)();
	HRESULT Dispatch(IMessage* pMsg);
private:
	HRESULT GetGlobalMutexName();
	CString ReadProcessInfo();
	HRESULT SendMessage(CString str);

	HRESULT CreateComX3();
	HRESULT ReleaseComX3();

	HRESULT CallConnectPoint(LPCWSTR pXml);

	HRESULT NotifyExit();
private:
	CString	m_strGlobalMutexName;
	CString m_ComX3NameSpace;

	UTIL::com_ptr<IComX3>   m_pComX3;
	CGlobalMutex m_globalMutex;

	ICCenter *m_pCenter;
	IClientObject* m_pClientObject;
	CConnectionPointHelper m_SignalAppConnectPoint;

//	CComX3CallImpl m_comX3impl;
};
