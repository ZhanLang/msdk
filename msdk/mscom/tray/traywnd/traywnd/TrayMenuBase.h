#pragma once
#include <mscom\msplugin.h>
#include <tray\trayplugin.h>
#include <tray/traymenudef.h>
#include <mscomhelper/connectionpointhelper.h>
#include <mscomhelper/UseConnectionPoint.h>
#include <mscomhelper/TraySrvPluginHelper.h>
#include <list>

struct ST_MENUPLUGIN_INFO
{
	UINT	idBegin;	//命令区间开始 或者是默认命令的ID
	UINT	idEnd;		//命令区间结束 或者是默认命令的位置
	DWORD	dwPluginCookie; //插件在连接点表里的Cookie
	BOOL	bDefaultMenu;

	ST_MENUPLUGIN_INFO()
		:idBegin(0),idEnd(0),dwPluginCookie(-1),bDefaultMenu(FALSE)
	{}

	ST_MENUPLUGIN_INFO(UINT u1, UINT u2, DWORD dw, BOOL bDefault = FALSE)
		:idBegin(u1),idEnd(u2),dwPluginCookie(dw),bDefaultMenu(bDefault)
	{}
};

class CUseMenuPlugin
{
public:
	CUseMenuPlugin();
	~CUseMenuPlugin();

	HRESULT CreateMenu();
	HRESULT ReleaseMenu();
	UINT QueryPluginMenu(ITrayMenuBaseConnectPoint* pPoint, DWORD dwPointCookie, UINT idCmdFirst, UINT uFlags, UINT uMenuType);
	BOOL FindMenuIdPlugin(UINT id, ST_MENUPLUGIN_INFO& info);
	BOOL FindDefaultMenuIdPlugin(ST_MENUPLUGIN_INFO& info);
	UINT GetMenuPluginCount();

	operator HMENU () const {return m_hMenu;}
private:
	HMENU	m_hMenu;

	typedef std::list<ST_MENUPLUGIN_INFO> MENUPLUGIN_INFOLIST;
	MENUPLUGIN_INFOLIST m_listMenuPluginInfo;
	
	BOOL FindPlugin(UINT id, ST_MENUPLUGIN_INFO& info, BOOL bFindDefault);
};

//////////////////////////////////////////////////////////////////////////
class CTrayMenuBase : public ITrayMenuConnectPoint
	, public IMsPlugin
	, public CTraySrvPluginHelper<CTrayMenuBase>
	, public CConnectionPointContainerHelper<CTrayMenuBase> //实现连接点
	, public CUnknownImp
{
public:
	CTrayMenuBase(void);
	~CTrayMenuBase(void);

	UNKNOWN_IMP4_(ITrayMenuConnectPoint, IMsPlugin, ITraySrvPlugin, IMsConnectionPointContainer);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter);

	//IMsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	//ITraySrvPlugin 重载CTraySrvPluginHelper对应方法
	STDMETHOD(SetTraySrv)(ITraySrv* pTraySrv);  //这个方法仅仅给TraySrv组件用

	//ITrayMenuConnectPoint
	STDMETHOD_(LRESULT, OnShowLeftButtonMenu)(INT x, INT y, BOOL& bHandle);			//左键菜单
	STDMETHOD_(LRESULT, OnShowRightButtonMenu)(INT x, INT y, BOOL& bHandle);		//右键菜单
	STDMETHOD_(LRESULT, OnMenuCommand)(WORD nMenuId, BOOL &bHandle);
	STDMETHOD_(LRESULT, OnMenuCommand)(UINT nMenuIndex, HMENU hMenu, BOOL &bHandle);
	STDMETHOD_(LRESULT, OnDefaultMenu)(BOOL &bHandle);	//左键双击响应默认菜单

	//bShow=FALSE 刚开始的时候测试一下，这样才能取到默认菜单，双击的时候好响应
	LRESULT ShowTrayMenu(int nMenuType, INT x, INT y, BOOL& bHandle, BOOL bShow = TRUE);

private:
	//实现连接点ITrayMenuBaseConnectPoint
	CConnectionPointHelper m_TrayMenuBaseConnectPoint;

	//连接TraySrv的连接点实现
	UseConnectPoint<ITrayMenuConnectPoint> m_UseTrayMenuConnectPoint;

private:
	UTIL::com_ptr<IMscomRunningObjectTable>	m_pRot;	//ROT
	UTIL::com_ptr<ITrayWnd>		m_pTrayWnd;			//主窗口
	CUseMenuPlugin	m_useMenuplugin;
};
