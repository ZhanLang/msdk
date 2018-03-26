#pragma once
#include "mscomhelper\UseConnectionPoint.h"
#include <mscom\mainplugin.h>
class CMainRun :
	public IMsPlugin,
	public IMsPluginRun,
	public IExit,
	//public IServiceCtrlConnectPoint,
	protected CUnknownImp
{
public:
	UNKNOWN_IMP3_(IMsPlugin , IMsPluginRun , IExit/*, IServiceCtrlConnectPoint*/);
	HRESULT init_class(IMSBase* pRot , IMSBase* pOuter)
	{
		RASSERT(pRot, E_INVALIDARG);
		//m_pRot = pRot;
		return S_OK;
	}
public:
	CMainRun(void);
	~CMainRun(void);

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();
	STDMETHOD(NotifyExit)(bool* bExit = NULL);


	//IServiceCtrlConnectPoint
	//STDMETHOD_(DWORD, OnServiceCtrl)(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);


private:
	//UseConnectPoint<IServiceCtrlConnectPoint> m_ServiceCtrlConnect;
	//UTIL::com_ptr<IMscomRunningObjectTable>	  m_pRot;

	DWORD m_dwThreadID;
};


