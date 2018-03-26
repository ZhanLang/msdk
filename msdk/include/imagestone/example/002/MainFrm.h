#pragma once
#include "wnd_top_tool.h"
#include "wnd_bottom_status.h"
#include "dlg_effect_lib.h"

//-------------------------------------------------------------------------------------
class CMainFrame : public CMDIFrameWnd
{
public:

private:
    CWndBottomStatus    m_bottom_status ;
    CWndTopTool         m_top_bar ; // top toolbar
    CReBar              m_rebar ;
    DlgEffectLib        m_effect_lib ;

private:
    class CVistaMenuIcon
    {
        std::map<UINT, FCObjImage*>   m_tab ; // menu command <--> menu icon
    public:
        ~CVistaMenuIcon() { DeleteAll(); }
        void DeleteAll() ;
        HBITMAP QueryIcon (UINT nID) ;
    };

    CVistaMenuIcon   m_vista_menu_icon ;

private:
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS);
    afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

    DECLARE_DYNAMIC(CMainFrame)
};
