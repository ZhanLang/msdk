#include "stdafx.h"
#include "002.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)
//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
    ON_WM_INITMENUPOPUP()
    ON_WM_MEASUREITEM()
    ON_WM_DRAWITEM()
    ON_WM_EXITMENULOOP()
    ON_COMMAND_EX(ID_EFFECT_LIB_VISIBLE, OnBarCheck)
    ON_UPDATE_COMMAND_UI(ID_EFFECT_LIB_VISIBLE, OnUpdateControlBarMenu)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    // create top toolbar
    m_top_bar.Create(this) ;

    // add top toolbar on rebar
    m_rebar.Create(this) ;
    m_rebar.AddBar(&m_top_bar) ;

    // create bottom status bar
    m_bottom_status.CreateBar(this) ;

    EnableDocking (CBRS_ALIGN_ANY) ;

    // effect list, create it before layer manager to dock it on left of layer manager
    m_effect_lib.Create(this) ;

    PostMessage(WM_COMMAND, MAKEWPARAM(ID_EFFECT_LIB_VISIBLE,0)) ;
    return 0 ;
}
//-------------------------------------------------------------------------------------
void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
    CString   strDocMaxIco ;
    pPopupMenu->GetMenuString (SC_CLOSE, strDocMaxIco, MF_BYCOMMAND) ;

    // not sys menu and not doc menu
    if (!bSysMenu && !strDocMaxIco.GetLength())
    {
        // set menu bitmap style
        MENUINFO   mi = {0} ;
        mi.cbSize = sizeof(mi) ;
        mi.fMask = MIM_STYLE ;
        pPopupMenu->GetMenuInfo(&mi) ;
        mi.dwStyle |= MNS_CHECKORBMP ;
        pPopupMenu->SetMenuInfo(&mi) ;

        // custom draw menu item bitmap
        for (UINT i=0 ; i < pPopupMenu->GetMenuItemCount() ; i++)
        {
            MENUITEMINFO   mii ;
            mii.cbSize = sizeof(mii) ;
            mii.fMask = MIIM_BITMAP ;

            if (IsWinVistaAndLater())
            {
                mii.hbmpItem = m_vista_menu_icon.QueryIcon (pPopupMenu->GetMenuItemID(i)) ;
                if (mii.hbmpItem)
                {
                    pPopupMenu->SetMenuItemInfo(i, &mii, TRUE) ;
                }
            }
            else
            {
                mii.hbmpItem = HBMMENU_CALLBACK ;
                pPopupMenu->SetMenuItemInfo(i, &mii, TRUE) ;
            }
        }
    }
    CMDIFrameWnd::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu) ;
}
//-------------------------------------------------------------------------------------
void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS)
{
    if (lpMIS && (lpMIS->CtlType == ODT_MENU))
    {
        lpMIS->itemWidth = 20 ;
        lpMIS->itemHeight = 20 ;
        return ;
    }
    CMDIFrameWnd::OnMeasureItem (nIDCtl, lpMIS) ;
}
//-------------------------------------------------------------------------------------
void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS && (lpDIS->CtlType == ODT_MENU))
    {
        FCObjImage   img ;
        img.LoadResource (lpDIS->itemID, L"PNG", IMG_PNG) ;
        if (img.IsValidImage())
        {
            if (lpDIS->itemState & ODS_DISABLED)
            {
                img.ApplyEffect (FCEffectGrayscale()) ;
            }

            std::auto_ptr<Gdiplus::Bitmap>   gb (img.CreateBitmap()) ;

            CRect   rc = FCObjGraph::CalcFitWindowSize (CSize(img.Width(),img.Height()), lpDIS->rcItem) ;
            Gdiplus::Graphics(lpDIS->hDC).DrawImage (gb.get(), rc.left, rc.top, rc.Width(), rc.Height()) ;
        }
        return ;
    }
    CMDIFrameWnd::OnDrawItem (nIDCtl, lpDIS) ;
}
//-------------------------------------------------------------------------------------
void CMainFrame::OnExitMenuLoop (BOOL bIsTrackPopupMenu)
{
    m_vista_menu_icon.DeleteAll() ;
    CMDIFrameWnd::OnExitMenuLoop(bIsTrackPopupMenu) ;
}
//-------------------------------------------------------------------------------------
HBITMAP CMainFrame::CVistaMenuIcon::QueryIcon (UINT nID)
{
    std::map<UINT, FCObjImage*>::iterator   i = m_tab.find(nID) ;
    if (i != m_tab.end())
        return *i->second ;

    FCObjImage   * pImg = new FCObjImage ;
    pImg->LoadResource (nID, L"PNG", IMG_PNG) ;
    if (pImg->IsValidImage() && (pImg->ColorBits() == 32))
    {
        pImg->ApplyEffect (FCEffectPremultipleAlpha()) ;
    }
    m_tab[nID] = pImg ;
    return *pImg ;
}
//-------------------------------------------------------------------------------------
void CMainFrame::CVistaMenuIcon::DeleteAll()
{
    for (std::map<UINT, FCObjImage*>::iterator i=m_tab.begin() ; i != m_tab.end() ; i++)
    {
        delete i->second ;
    }
    m_tab.clear() ;
}
