#pragma once
#include <rscom\rsplugin.h>
#include <tray\trayplugin.h>
#include <map>
#include <rscomhelper/connectionpointhelper.h>

class COnTrayMsg : public ITrayMsgConnectPoint
	, public IRsPlugin
	, public ITrayMsg
	, public CConnectionPointContainerHelper<COnTrayMsg> //实现连接点
	, public CUnknownImp
{
public:
	COnTrayMsg(void);
	~COnTrayMsg(void);

	UNKNOWN_IMP4_(ITrayMsgConnectPoint, IRsPlugin, ITrayMsg, IRsConnectionPointContainer);

	STDMETHOD(init_class)(IRSBase* prot, IRSBase* punkOuter);

	//IRsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	//ITrayMsg
	STDMETHOD(SetTraySrv)(ITraySrv* pTraySrv);  //这个方法仅仅给TraySrv组件用
	STDMETHOD(GetTraySrv)(ITraySrv** pTraySrv); //其被TrayMsg组件使用的组件通过这个方法拿到ITraySrv

	//ITrayMenuConnectPoint
	STDMETHOD_(LRESULT, OnShowLeftButtonMenu)(INT x, INT y, BOOL& bHandle);			//左键菜单
	STDMETHOD_(LRESULT, OnShowRightButtonMenu)(INT x, INT y, BOOL& bHandle);		//右键菜单
	STDMETHOD_(LRESULT, OnMenuCommand)(WORD nMenuId, BOOL &bHandle);
	STDMETHOD_(LRESULT, OnMenuCommand)(int nMenuIndex, HMENU hMenu, BOOL &bHandle);

	//ITrayMsgConnectPoint
	STDMETHOD_(LRESULT, OnTrayIconMsg)(LPARAM lParam, INT x, INT y, BOOL& bHandle);	//托盘图标消息
	STDMETHOD_(LRESULT, OnBalloonClicked)(UINT nActionID, BOOL& bHandle);				//气球被点击
	STDMETHOD_(LRESULT, OnOtherMsg)(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle); //其他消息

private:
	//实现连接点IOnMenuMsgConnectPoint
	CConnectionPointHelper m_OnMenuMsgConnectPoint;

	//连接TraySrv的连接点实现
	UTIL::com_ptr<IRsConnectionPoint> m_pOnTrayMsgConnectPoint;	//主窗口的连接点
	DWORD						m_dwOnTrayMsgConnectPointCookie;

private:
	UTIL::com_ptr<IRscomRunningObjectTable>	m_pRot;			//ROT
	UTIL::com_ptr<ITrayWnd>		m_pTrayWnd;		//主窗口
	UTIL::com_ptr<ITraySrv>		m_pTraySrv;		//托盘服务
};
