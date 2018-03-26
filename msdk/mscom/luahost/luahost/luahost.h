
#pragma once

#include "luahost\ILuaHost.h"
#include "luahost\luahost.h"
#include "thread\SubThreadImp.h"
#include <mscomhelper/UseConnectionPoint.h>
#include <mscom/mscombase.h>

//暂时将服务通知写到这里面，按理是应该独立出一个组件的
class CLuaHost_S:
	public IMsPlugin,
	public IMsPluginRun,
	public IluaHost,
	public IServiceCtrlConnectPoint,
	public CMsComBase<CLuaHost_S>,
	public CSubThread
{
public:
	UNKNOWN_IMP4_(IMsPlugin, IMsPluginRun, IluaHost, IServiceCtrlConnectPoint);


	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	STDMETHOD(GetLuaVM)(ILuaVM** pVM);


protected:
	STDMETHOD_(DWORD, OnServiceCtrl)(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

protected:
	virtual HRESULT	Run();
private:
	UTIL::com_ptr<ILuaVM>					m_pLuaVM;

	CString	m_strFile;
	CString m_strScript;

	UseConnectPoint<IServiceCtrlConnectPoint> m_UseServiceCtrlConnectPoint;
};