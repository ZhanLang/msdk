#include "StdAfx.h"
#include "resource.h"
#include "TraySrv.h"

CTraySrv::CTraySrv(void)
:m_sysTray(g_hinstance, IDI_ICON_EMPTY)
{
}

CTraySrv::~CTraySrv(void)
{
}

STDMETHODIMP CTraySrv::init_class(IMSBase* prot, IMSBase* punkOuter)
{
	m_pRot = prot;	//把rot缓存下来
	RASSERT(m_pRot, E_FAIL);
	
	RFAILED(m_pRot->GetObject(ClSID_CTrayWnd, __uuidof(ITrayWnd), (VOID**)&m_pTrayWnd.m_p));
	RASSERT(m_pTrayWnd, E_FAIL);
	
	return S_OK;
}

STDMETHODIMP CTraySrv::Init(void*)
{
	RASSERT(m_pTrayWnd, E_FAIL);

	HWND hWnd = m_pTrayWnd->GetWndHwnd();
	RASSERT(hWnd, E_FAIL);

	UTIL::com_ptr<IUnknown> pUnknown(this);
	RFAILED(m_CPWndMessageConnectPoint.Connect(m_pTrayWnd, pUnknown));
	//////////////////////////////////////////////////////////////////////////
	AddConnectionPoint(__uuidof(ITrayMsgConnectPoint), m_TrayMsgConnectPoint);		//增加ITrayMsgConnectPoint的连接点支持
	AddConnectionPoint(__uuidof(ITrayMenuConnectPoint), m_TrayMenuConnectPoint);	//增加ITrayMenuConnectPoint的连接点支持

	return S_OK;
}

STDMETHODIMP CTraySrv::Uninit()
{
	DelConnectionPoint(__uuidof(ITrayMenuConnectPoint), m_TrayMenuConnectPoint);	//从连接容器里删除
	DelConnectionPoint(__uuidof(ITrayMsgConnectPoint), m_TrayMsgConnectPoint);		//从连接容器里删除
	m_TrayMsgConnectPoint.EmptyConnection();
	m_TrayMenuConnectPoint.EmptyConnection();
	m_CPWndMessageConnectPoint.DisConnect();
	
	if(m_pTrayWnd)
		m_pTrayWnd = INULL;
	
	if(m_pRot)
		m_pRot = INULL;

	return S_OK;
}

STDMETHODIMP_(LRESULT) CTraySrv::OnWndMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	return m_sysTray.WinProc(msg, wParam, lParam, bHandle);
}

//ITraySrv
//InitTray的clsOnTrayMsg组件应该是要实现ITrayMsg方法
STDMETHODIMP CTraySrv::CreateTray(UINT nMsgID, UINT nUID, CLSID clsTraySrvPlugin/* = ClSID_COnTrayMsg*/)
{
	if(clsTraySrvPlugin != GUID_NULL)
	{
		RFAILED(m_pTraySrvPlugin.CreateInstance(m_pRot, clsTraySrvPlugin));
	}

	return CreateTray(nMsgID, nUID, m_pTraySrvPlugin);
}

STDMETHODIMP CTraySrv::CreateTray(UINT nMsgID, UINT nUID, ITraySrvPlugin* pTraySrvPlugin/* = NULL*/)
{
	HWND hWnd = m_pTrayWnd->GetWndHwnd();
	RASSERT(hWnd, E_FAIL);

	if(!m_pTraySrvPlugin && pTraySrvPlugin)
	{
		m_pTraySrvPlugin = pTraySrvPlugin;
	}

	if(m_pTraySrvPlugin)
	{
		UTIL::com_ptr<ITraySrv> pTraySrv = this;
		m_pTraySrvPlugin->SetTraySrv(pTraySrv);
	}

	//创建真正的托盘
	BOOL bCreate = m_sysTray.CreateTray(hWnd, nMsgID, nUID, this);
	RASSERT(bCreate, E_FAIL);

	return S_OK;
}

STDMETHODIMP CTraySrv::DestroyTray()
{
	if(m_pTraySrvPlugin)
	{
		UTIL::com_ptr<ITraySrv> pTraySrv = this;
		m_pTraySrvPlugin->SetTraySrv(NULL);
	}

	m_sysTray.Destroy();
	m_pTraySrvPlugin.Release();

	return S_OK;
}

STDMETHODIMP CTraySrv::GetTraySrvPlugin(ITraySrvPlugin** pTraySrvPlugin)
{
	RASSERT(pTraySrvPlugin, E_POINTER);
	RASSERT(m_pTraySrvPlugin, E_INVALIDARG);

	return m_pTraySrvPlugin->QueryInterface(__uuidof(ITraySrvPlugin), (void**)pTraySrvPlugin);
}

STDMETHODIMP CTraySrv::UpdateTray(LPCTSTR lpcszIcon, LPCTSTR lpcszTips, BOOL bOfficial/* = TRUE*/)
{
	RASSERT(m_sysTray.UpdateTray(lpcszIcon, lpcszTips, bOfficial), E_FAIL);
	return S_OK;
}

STDMETHODIMP CTraySrv::UpdateTray(HICON hIcon, LPCTSTR lpcszTips, BOOL bOfficial/* = TRUE*/)
{
	RASSERT(m_sysTray.UpdateTray(hIcon, lpcszTips, bOfficial), E_FAIL);
	return S_OK;
}

STDMETHODIMP CTraySrv::UpdateTray(UINT nIcon, LPCTSTR lpcszTips, BOOL bOfficial, IN HINSTANCE hInstance/* = NULL*/)
{
	RASSERT(m_sysTray.UpdateTray(nIcon, lpcszTips, bOfficial, hInstance), E_FAIL);
	return S_OK;
}

STDMETHODIMP CTraySrv::ShowTray(bool bShow/* = TRUE*/)
{
	RTEST(m_sysTray.ShowTray(bShow), E_FAIL);
	return S_OK;
}

// 显示Tray气球信息
STDMETHODIMP CTraySrv::ShowBalloon(LPCTSTR lpcszTips, LPCTSTR lpcszTitle/* = NULL*/, BOOL bSynchronous/* = FALSE*/
					   , UINT uTimeout/* = 5000*/, UINT uStyle/*=POPTIPSTYLE_INFORMATION*/, UINT nActionID/*=TRAY_ACTION_NONE*/)
{
	BOOL bOk = FALSE;
	if(bSynchronous)
		bOk = m_sysTray.ShowBalloonimmediately(lpcszTips, lpcszTitle, uTimeout, uStyle, nActionID);
	else
		bOk = m_sysTray.ShowBalloon(lpcszTips, lpcszTitle, uTimeout, uStyle, nActionID);
	RASSERT(bOk, E_FAIL);
	
	return S_OK;
}

//托盘闪动，nTimerElapse闪动间隔，0：停止闪动
STDMETHODIMP CTraySrv::FlickerTray(UINT nTimerElapse/* = 0*/)
{
	BOOL bOk = FALSE;
	bOk = m_sysTray.FlickerTray(nTimerElapse);
	RASSERT(bOk, E_FAIL);

	return S_OK;
}

STDMETHODIMP_(BOOL) CTraySrv::IsFlicker()
{
	return m_sysTray.IsFlicker();
}

//托盘复位，回到之前最后设定的稳定状态
STDMETHODIMP CTraySrv::ResetTray()
{
	BOOL bOk = FALSE;
	bOk = m_sysTray.ResetTray();
	RASSERT(bOk, E_FAIL);

	return S_OK;
}

//偷懒用的，免得每个函数写一样的东西
#define ONTRAY_MSG(obj, iid, func)	{	\
	UTIL::com_ptr<IMsEnumConnections> pEnum;	\
	RASSERT(SUCCEEDED(obj.EnumConnections((IMsEnumConnections**)&pEnum)), 0);	\
	RASSERT(pEnum, 0);	\
	while(1){	\
		MSCONNECTDATA data[1];	\
		/*一个一个遍历的，S_FALSE不行	*/ \
		RASSERT(S_OK == pEnum->Next(1, data, NULL), 0);	\
		UTIL::com_ptr<iid> p = data[0].pUnk;	\
		if(p){	\
			LRESULT rc = p->func;	\
			RTEST(bHandle, rc);	\
		}}	\
	}

//左键菜单
LRESULT	CTraySrv::OnShowLeftButtonMenu(INT x, INT y, BOOL &bHandle)
{
	ONTRAY_MSG(m_TrayMenuConnectPoint, ITrayMenuConnectPoint, OnShowLeftButtonMenu(x, y, bHandle));
	return 0;
}

//右键菜单
LRESULT CTraySrv::OnShowRightButtonMenu(INT x, INT y, BOOL &bHandle)
{
	ONTRAY_MSG(m_TrayMenuConnectPoint, ITrayMenuConnectPoint, OnShowRightButtonMenu(x, y, bHandle));
	return 0;
}

//托盘图标消息
LRESULT CTraySrv::OnTrayIconMsg(LPARAM lParam, INT x, INT y, BOOL &bHandle)
{
	if(WM_LBUTTONDBLCLK == lParam)
	{
		ONTRAY_MSG(m_TrayMenuConnectPoint, ITrayMenuConnectPoint, OnDefaultMenu(bHandle));
	}

	if(!bHandle)
		ONTRAY_MSG(m_TrayMsgConnectPoint, ITrayMsgConnectPoint, OnTrayIconMsg(lParam, x, y, bHandle));

	return 0;
}

//气球被点击
LRESULT CTraySrv::OnBalloonClicked(UINT nActionID, BOOL &bHandle)
{
	ONTRAY_MSG(m_TrayMsgConnectPoint, ITrayMsgConnectPoint, OnBalloonClicked(nActionID, bHandle));
	return 0;
}

#define BEGIN_MSGMAP()	switch(msg){
#define ONMSG(msg, f)	case msg:{LRESULT rc = f(wParam, lParam, bHandle);if(bHandle) return rc;}break;
#define END_MSGMAP()	default:break;}

//其他消息
LRESULT	CTraySrv::OnOtherMsg(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	BEGIN_MSGMAP()
		ONMSG(WM_COMMAND, OnCommand)
		ONMSG(WM_MENUCOMMAND, OnMenuCommand)
	END_MSGMAP()

	ONTRAY_MSG(m_TrayMsgConnectPoint, ITrayMsgConnectPoint, OnOtherMsg(msg, wParam, lParam, bHandle));
	return 0;
}

LRESULT CTraySrv::OnCommand(WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	if(0 == HIWORD(wParam) && 0 == lParam) //Menu message
	{
		return OnMenuCommand(LOWORD(wParam), bHandle);
	}

	return 0;
}

LRESULT CTraySrv::OnMenuCommand(WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	return OnMenuCommand((int)wParam, (HMENU)lParam, bHandle);
}

LRESULT CTraySrv::OnMenuCommand(WORD nMenuId, BOOL &bHandle)
{
	ONTRAY_MSG(m_TrayMenuConnectPoint, ITrayMenuConnectPoint, OnMenuCommand(nMenuId, bHandle));
	return 0;
}

LRESULT CTraySrv::OnMenuCommand(int nMenuIndex, HMENU hMenu, BOOL &bHandle)
{
	ONTRAY_MSG(m_TrayMenuConnectPoint, ITrayMenuConnectPoint, OnMenuCommand(nMenuIndex, hMenu, bHandle));
	return 0;
}