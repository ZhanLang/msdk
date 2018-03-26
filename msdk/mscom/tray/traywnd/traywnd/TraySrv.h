#pragma once
#include <mscom\msplugin.h>
#include <tray\trayplugin.h>
#include "SysTray.h"
#include <map>
#include <mscomhelper/connectionpointhelper.h>
#include <mscomhelper/UseConnectionPoint.h>
#include <mscomhelper/CreateObjectHelper.h>

class CTraySrv : public ITraySrv
	, public IMsPlugin
	, public IWndMessageConnectPoint
	, public CConnectionPointContainerHelper<CTraySrv> //实现连接点
	, public ITrayEvents	//这个不是从IUnknown来的
	, public CUnknownImp
{
public:
	CTraySrv(void);
	~CTraySrv(void);

	UNKNOWN_IMP4_(ITraySrv, IMsPlugin, IWndMessageConnectPoint, IMsConnectionPointContainer);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter);

	//IMsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	//IWndMessageConnectPoint
	STDMETHOD_(LRESULT, OnWndMessage)(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);

	//ITraySrv
	//InitTray的clsOnTrayMsg组件应该是要实现ITrayMsg方法
	STDMETHOD(CreateTray)(UINT nMsgID, UINT nUID, CLSID clsOnTrayMsg/* = ClSID_COnTrayMsg*/);
	STDMETHOD(CreateTray)(UINT nMsgID, UINT nUID, ITraySrvPlugin* pTrayMsg = NULL);
	STDMETHOD(DestroyTray)();
	STDMETHOD(GetTraySrvPlugin)(ITraySrvPlugin** pTrayMsg);

	STDMETHOD(UpdateTray)(LPCTSTR lpcszIcon, LPCTSTR lpcszTips, BOOL bOfficial = TRUE);
	STDMETHOD(UpdateTray)(HICON hIcon, LPCTSTR lpcszTips, BOOL bOfficial = TRUE);
	STDMETHOD(UpdateTray)(UINT nIcon, LPCTSTR lpcszTips, BOOL bOfficial, IN HINSTANCE hInstance/* = NULL*/);
	STDMETHOD(ShowTray)(bool bShow = TRUE);

	// 显示Tray气球信息
	STDMETHOD(ShowBalloon)(LPCTSTR lpcszTips, LPCTSTR lpcszTitle = NULL, BOOL bSynchronous = FALSE
		, UINT uTimeout = 5000, UINT uStyle=POPTIPSTYLE_INFORMATION, UINT nActionID=TRAY_ACTION_NONE);

	//托盘闪动，nTimerElapse闪动间隔，0：停止闪动
	STDMETHOD(FlickerTray)(UINT nTimerElapse = 0);
	STDMETHOD_(BOOL, IsFlicker)();

	//托盘复位，回到之前最后设定的稳定状态
	STDMETHOD(ResetTray)();

	//ITrayEvents
	virtual LRESULT OnTrayIconMsg(LPARAM lParam, INT x, INT y, BOOL& bHandle);
	virtual LRESULT OnShowLeftButtonMenu(INT x, INT y, BOOL& bHandle);
	virtual LRESULT OnShowRightButtonMenu(INT x, INT y, BOOL& bHandle);
	virtual LRESULT OnBalloonClicked(UINT nActionID, BOOL& bHandle);				//气球被点击
	virtual LRESULT OnOtherMsg(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);

	LRESULT OnCommand(WPARAM wParam, LPARAM lParam, BOOL &bHandle);
	LRESULT OnMenuCommand(WPARAM wParam, LPARAM lParam, BOOL &bHandle);

	LRESULT OnMenuCommand(WORD nMenuId, BOOL &bHandle);
	LRESULT OnMenuCommand(int nMenuIndex, HMENU hMenu, BOOL &bHandle);

private:
	CConnectionPointHelper m_TrayMsgConnectPoint;	//实现连接点ITrayMsgConnectPoint
	CConnectionPointHelper m_TrayMenuConnectPoint;	//实现连接点ITrayMenuConnectPoint

	//连接Tray的连接点实现
	UseConnectPoint<IWndMessageConnectPoint> m_CPWndMessageConnectPoint; //主窗口的连接点

private:	
	UTIL::com_ptr<IMscomRunningObjectTable>	m_pRot;			//ROT
	UTIL::com_ptr<ITrayWnd>		m_pTrayWnd;
	CCreateObjectHelper<ITraySrvPlugin> m_pTraySrvPlugin;

	CSysTray	m_sysTray;
};
