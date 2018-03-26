#include "stdafx.h"
#include "wnd_palette_slider.h"


BEGIN_MESSAGE_MAP(CWndPaletteSlider, FCButtonBase)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


//-------------------------------------------------------------------------------------
void CWndPaletteSlider::UpdateMousePos (CPoint pt)
{
    CRect   rc ;
    GetClientRect(rc) ;
    rc.DeflateRect(2,2,2,2) ;
    if (rc.Width() > 1)
    {
        long   x = FClamp(pt.x, rc.left, rc.right) ;

        m_nPos = m_nMin + (m_nMax-m_nMin) * (x-rc.left) / rc.Width() ;

        if (GetParent())
            GetParent()->SendMessage (WM_PHOXO_TRACK_PALETTE_SLIDER, GetDlgCtrlID()) ;
        Invalidate() ;
    }
}
//-------------------------------------------------------------------------------------
void CWndPaletteSlider::DDX_Pos (CDataExchange* pDX, CWndPaletteSlider& rSlider, int& nPos)
{
    if (pDX->m_bSaveAndValidate)
    {
        nPos = rSlider.m_nPos ;
    }
    else
    {
        rSlider.m_nPos = nPos ;
        if (rSlider.GetSafeHwnd())
        {
            rSlider.Invalidate() ;
        }
    }
}
//-------------------------------------------------------------------------------------
BOOL CWndPaletteSlider::PreTranslateMessage (MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        switch (pMsg->wParam)
        {
            case VK_LEFT :
            case VK_DOWN :
                if (m_nPos > m_nMin)
                {
                    m_nPos-- ;
                    if (GetParent())
                        GetParent()->SendMessage (WM_PHOXO_TRACK_PALETTE_SLIDER, GetDlgCtrlID()) ;
                    Invalidate() ;
                }
                return 1;

            case VK_RIGHT :
            case VK_UP :
                if (m_nPos < m_nMax)
                {
                    m_nPos++ ;
                    if (GetParent())
                        GetParent()->SendMessage (WM_PHOXO_TRACK_PALETTE_SLIDER, GetDlgCtrlID()) ;
                    Invalidate() ;
                }
                return 1;
        }
    }
    return FCButtonBase::PreTranslateMessage(pMsg) ;
}
//-------------------------------------------------------------------------------------
void CWndPaletteSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus() ;

    UpdateMousePos (point) ;

    SetCapture() ;
    for (;;)
    {
        MSG   msg ;
        ::GetMessage (&msg, NULL, 0, 0) ;
        if (::GetCapture() != m_hWnd)
            goto capture_over ; // <== Exit.1

        switch (msg.message)
        {
            case WM_LBUTTONUP :
            case WM_MOUSEMOVE :
                UpdateMousePos (msg.lParam) ;

                if (msg.message == WM_LBUTTONUP)
                {
                    ::ReleaseCapture() ; // finish
                    goto capture_over ; // <== Exit.2
                }
                break;

            default : // just dispatch rest of the messages
                ::DispatchMessage (&msg) ;
                break;
        }
    }

capture_over:
    Invalidate() ;
}
