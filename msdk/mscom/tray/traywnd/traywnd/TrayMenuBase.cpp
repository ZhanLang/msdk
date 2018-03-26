#include "StdAfx.h"
#include "TrayMenuBase.h"
#include "tray\traymenudef.h"

CTrayMenuBase::CTrayMenuBase(void)
{
}

CTrayMenuBase::~CTrayMenuBase(void)
{
}

STDMETHODIMP CTrayMenuBase::init_class(IMSBase* prot, IMSBase* punkOuter)
{
	m_pRot = prot;	//把rot缓存下来
	RASSERT(m_pRot, E_FAIL);

	m_pRot->GetObject(ClSID_CTrayWnd, __uuidof(ITrayWnd), (VOID**)&m_pTrayWnd.m_p);

	return S_OK;
}

STDMETHODIMP CTrayMenuBase::Init(void*)
{
	AddConnectionPoint(__uuidof(ITrayMenuBaseConnectPoint), m_TrayMenuBaseConnectPoint);

	return S_OK;
}

STDMETHODIMP CTrayMenuBase::Uninit()
{
	m_useMenuplugin.ReleaseMenu();

	m_UseTrayMenuConnectPoint.DisConnect();	//断开子连接点

	DelConnectionPoint(__uuidof(ITrayMenuBaseConnectPoint), m_TrayMenuBaseConnectPoint);
	m_TrayMenuBaseConnectPoint.EmptyConnection();
	ReleaseTraySrv();

	if(m_pTrayWnd)
		m_pTrayWnd = INULL;

	if(m_pRot)
		m_pRot = INULL;

	return S_OK;
}

STDMETHODIMP CTrayMenuBase::SetTraySrv(ITraySrv* pTraySrv)
{
	RFAILED(CTraySrvPluginHelper<CTrayMenuBase>::SetTraySrv(pTraySrv));

	UTIL::com_ptr<IUnknown> pUnknown(this);
	RFAILED(m_UseTrayMenuConnectPoint.Connect(m_pTraySrv, pUnknown));

	return S_OK;
}

//左键菜单
LRESULT	CTrayMenuBase::OnShowLeftButtonMenu(INT x, INT y, BOOL& bHandle)
{
	return ShowTrayMenu(MenuType_LeftButton, x, y, bHandle);
}

//右键菜单
LRESULT CTrayMenuBase::OnShowRightButtonMenu(INT x, INT y, BOOL& bHandle)
{	
	return ShowTrayMenu(MenuType_RightButton, x, y, bHandle);
}

LRESULT CTrayMenuBase::ShowTrayMenu(int nMenuType, INT x, INT y, BOOL& bHandle, BOOL bShow/* = TRUE*/)
{
	RASSERT(SUCCEEDED(m_useMenuplugin.CreateMenu()), 0);

	//先插入自己的菜单 空

	//让所有相关连接点插入自己的菜单项
	UTIL::com_ptr<IMsEnumConnections> pEnum;
	RASSERT(SUCCEEDED(m_TrayMenuBaseConnectPoint.EnumConnections((IMsEnumConnections**)&pEnum)), 0);
	RASSERT(pEnum, 0);

	UINT idCmdFirst = 1;	//可用菜单命令ID从1开始
	bool bQueryDefaultMenu = false; //默认菜单请求成功就不请求了
	while(1)
	{
		MSCONNECTDATA data[1];
		if(S_OK != pEnum->Next(1, data, NULL)) /*一个一个遍历的，S_FALSE不行	*/
			break;

		UTIL::com_ptr<ITrayMenuBaseConnectPoint> p = data[0].pUnk;
		if(!p)
			continue;

		DWORD dwCookie = data[0].dwCookie;
		UINT uCmdOffset = 0;
		if(!bQueryDefaultMenu) //先请求默认菜单项,已有就不请求了
		{
			uCmdOffset = m_useMenuplugin.QueryPluginMenu(p, dwCookie, idCmdFirst, MenuFlag_DefaultOnly, nMenuType);
			if(uCmdOffset > 0) //成功
			{
				bQueryDefaultMenu = true;
				idCmdFirst += uCmdOffset;
			}
		}

		//请求普通菜单项
		uCmdOffset = m_useMenuplugin.QueryPluginMenu(p, dwCookie, idCmdFirst, MenuFlag_Normal, nMenuType);
		if(uCmdOffset > 0) //成功
			idCmdFirst += uCmdOffset;
	}

	if( bShow && (GetMenuItemCount(m_useMenuplugin) > 0) )
	{
		bHandle = TRUE;
		::TrackPopupMenu(m_useMenuplugin, TPM_LEFTALIGN, x, y, 0, m_pTrayWnd->GetWndHwnd(), NULL);
	}

	return 0;
}

LRESULT CTrayMenuBase::OnMenuCommand(WORD nMenuId, BOOL &bHandle)
{
	return 0;
}

LRESULT CTrayMenuBase::OnMenuCommand(UINT nMenuIndex, HMENU hMenu, BOOL &bHandle)
{
	if(hMenu)
	{
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID;

		if(GetMenuItemInfo(hMenu, nMenuIndex, TRUE, &mii))
		{
			WORD nMenuId = mii.wID;

			ST_MENUPLUGIN_INFO info;
			if(m_useMenuplugin.FindMenuIdPlugin(nMenuId, info))
			{
				bHandle = TRUE;

				UTIL::com_ptr<ITrayMenuBaseConnectPoint> p;
				HRESULT hr = m_TrayMenuBaseConnectPoint.FindConnection(info.dwPluginCookie, __uuidof(ITrayMenuBaseConnectPoint), (void**)&p);
				RASSERT(SUCCEEDED(hr), 0);
				RASSERT(p, 0);

				MENUCMD_INFO menuInfo;
				menuInfo.cbSize = sizeof(MENUCMD_INFO);
				menuInfo.hMenu	= hMenu;
				menuInfo.uMenuItem = nMenuIndex;
				menuInfo.wMenuID = (nMenuId - info.idBegin); //要减掉开始值
				return p->InvokeMenuCmd(&menuInfo);
			}
		}
	}
	return 0;
}

LRESULT CTrayMenuBase::OnDefaultMenu(BOOL &bHandle)
{
	if(!m_useMenuplugin ||  (0 == m_useMenuplugin.GetMenuPluginCount()))
	{
		BOOL b;
		ShowTrayMenu(MenuType_RightButton, 0, 0, b, FALSE);
	}

	ST_MENUPLUGIN_INFO info;
	if(m_useMenuplugin.FindDefaultMenuIdPlugin(info))
	{
		bHandle = TRUE;

		UTIL::com_ptr<ITrayMenuBaseConnectPoint> p;
		HRESULT hr = m_TrayMenuBaseConnectPoint.FindConnection(info.dwPluginCookie, __uuidof(ITrayMenuBaseConnectPoint), (void**)&p);
		RASSERT(SUCCEEDED(hr), 0);
		RASSERT(p, 0);

		MENUCMD_INFO menuInfo;
		menuInfo.cbSize = sizeof(MENUCMD_INFO);
		menuInfo.hMenu	= m_useMenuplugin;
		menuInfo.uMenuItem = info.idEnd;	//idEnd当位置用
		menuInfo.wMenuID = info.idBegin; //默认
		return p->InvokeMenuCmd(&menuInfo);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
CUseMenuPlugin::CUseMenuPlugin()
:m_hMenu(NULL)
{

}

CUseMenuPlugin::~CUseMenuPlugin()
{
	ReleaseMenu();
}

HRESULT CUseMenuPlugin::CreateMenu()
{
	ReleaseMenu();	//先Destroy之前创建的菜单

	m_hMenu = ::CreatePopupMenu();
	RASSERT(m_hMenu, 0);

	MENUINFO mi;
	mi.cbSize = sizeof(mi);
	mi.fMask = MIM_STYLE;
	GetMenuInfo(m_hMenu, &mi);
	mi.dwStyle |= MNS_NOTIFYBYPOS;
	mi.fMask |= MIM_APPLYTOSUBMENUS;
	SetMenuInfo(m_hMenu, &mi);

	return S_OK;
}

HRESULT CUseMenuPlugin::ReleaseMenu()
{
	m_listMenuPluginInfo.clear();

	if(m_hMenu)
	{
		DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}
	return S_OK;
}

UINT CUseMenuPlugin::QueryPluginMenu(ITrayMenuBaseConnectPoint* pPoint, DWORD dwPointCookie, UINT idCmdFirst, UINT uFlags, UINT uMenuType)
{
	RASSERT(m_hMenu, 0);
	UTIL::com_ptr<ITrayMenuBaseConnectPoint> p = pPoint;
	RASSERT(p, 0);

	UINT indexMenu = (UINT)GetMenuItemCount(m_hMenu) + 1;
	UINT uCmdOffset = p->QueryMenu(m_hMenu, indexMenu, idCmdFirst, uFlags, uMenuType);
	RASSERT(uCmdOffset, 0);

	if(MenuFlag_DefaultOnly == uFlags)
		m_listMenuPluginInfo.push_back(ST_MENUPLUGIN_INFO(uCmdOffset - 1, indexMenu, dwPointCookie, TRUE));
	else
		m_listMenuPluginInfo.push_back(ST_MENUPLUGIN_INFO(idCmdFirst, idCmdFirst + uCmdOffset - 1, dwPointCookie));

	return uCmdOffset;
}

BOOL CUseMenuPlugin::FindMenuIdPlugin(UINT id, ST_MENUPLUGIN_INFO& info)
{
	return FindPlugin(id, info, FALSE);
}

BOOL CUseMenuPlugin::FindDefaultMenuIdPlugin(ST_MENUPLUGIN_INFO& info)
{
	return FindPlugin(0, info, TRUE);
}

BOOL CUseMenuPlugin::FindPlugin(UINT id, ST_MENUPLUGIN_INFO& info, BOOL bFindDefault)
{
	RASSERT(m_listMenuPluginInfo.size() > 0, FALSE);

	MENUPLUGIN_INFOLIST::const_iterator it = m_listMenuPluginInfo.begin();
	for(; it != m_listMenuPluginInfo.end(); it++)
	{
		const ST_MENUPLUGIN_INFO& item = *it;

		if(
			(bFindDefault && item.bDefaultMenu) //找默认菜单项
			||  (!bFindDefault && (id >= item.idBegin && id <= item.idEnd)) //在这个命令区间里
			) 
		{
			memcpy(&info, &item, sizeof(ST_MENUPLUGIN_INFO));
			return TRUE;
		}
	}

	return FALSE;
}

UINT CUseMenuPlugin::GetMenuPluginCount()
{
	return m_listMenuPluginInfo.size();
}