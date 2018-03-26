#include "stdafx.h"
#include "002.h"
#include "wnd_color_button.h"

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWndColorButton, FCButtonBase)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
void CWndColorButton::OnDrawButton (CDC* pDC, CRect rcButton)
{
    pDC->FillSolidRect(rcButton, RGB(192,192,192)) ;
    rcButton.DeflateRect(1,1,1,1) ;
    pDC->FillSolidRect(rcButton, RGB(255,255,255)) ;
    rcButton.DeflateRect(1,1,1,1) ;
    pDC->FillSolidRect(rcButton, m_color) ;
}
//-------------------------------------------------------------------------------------
void CWndColorButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    CColorDialog   dlg (m_color, CC_FULLOPEN, this) ;
    if (dlg.DoModal() == IDOK)
    {
        SetColor (dlg.GetColor()) ;
        if (GetParent())
            GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_COLOR_BUTTON_CHANGE,0)) ;
    }
}
