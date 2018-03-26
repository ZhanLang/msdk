/*
    Copyright (C) =USTC= Fu Li

    Author   :  Fu Li
    Create   :  2008-3-23
    Home     :  http://www.phoxo.com
    Mail     :  crazybitwps@hotmail.com

    This file is part of UIStone

    The code distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Redistribution and use the source code, with or without modification,
    must retain the above copyright.
*/
#pragma once
#include "track_mouse_hover.h"
#include "font.h"

//-------------------------------------------------------------------------------------
/**
    Custom draw button.
*/
class FCButtonBase : public CButton,
                     public FCTrackMouseHover
{
public:
    FCButtonBase()
    {
        m_tooltip_style = TTS_ALWAYSTIP ;
        m_check = BST_UNCHECKED ;
        m_nDelayTimeMS = 250 ;
        m_cursor = NULL ;
    }

    /// Create button, WS_VISIBLE is not set by default.
    BOOL Create (CRect rcWnd, CWnd* pParent, UINT nID, DWORD dwStyle=0)
    {
        return CButton::Create (_T(""), WS_CHILD|BS_OWNERDRAW|dwStyle, rcWnd, pParent, nID) ;
    }

    /// Is the button highlighted.
    BOOL IsButtonHighlight() const {return (GetState() & BST_PUSHED) ? TRUE : FALSE ;}

    /**
        @name Tooltip.
    */
    //@{
    /// Get tooltip text.
    CString GetTooltipText() const {return m_tip;}
    /// Set tooltip text.
    void SetTooltipText (LPCTSTR strTip) {m_tip = strTip;}
    /// Set tooltip's style, default style is TTS_ALWAYSTIP, you can set TTS_BALLOON | TTS_ALWAYSTIP
    void SetTooltipStyle (DWORD dwStyle=TTS_ALWAYSTIP) {m_tooltip_style = dwStyle;}
    /// Set button cursor, set NULL to use standard arrow cursor.
    void SetButtonCursor (HCURSOR hCursor) {m_cursor = hCursor;}
    /// Set delay milliseconds.
    void SetTooltipDelayTime (UINT nDelay) {m_nDelayTimeMS = nDelay;}
    //@}

protected:
    /// Override to draw button.
    virtual void OnDrawButton (CDC* pDC, CRect rcButton) {}

protected:
    /// If you override it, must call base function first (in order to register tool tip).
    virtual void OnMouse_EnterWnd()
    {
        ReRegisterToolTip() ;
    }

    virtual void OnMouse_LeaveWnd()
    {
        m_tip_ctrl.DestroyWindow() ;
    }

    virtual void PreSubclassWindow()
    {
        // can't create tooltip window in PreSubclassWindow
        __super::PreSubclassWindow() ;
        ModifyStyle (0, BS_OWNERDRAW) ;
    }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if (m_tip_ctrl.GetSafeHwnd())
            m_tip_ctrl.RelayEvent(pMsg) ;
        return __super::PreTranslateMessage(pMsg) ;
    }

protected:
    /// Draw background of button using system theme (it doesn't draw focus rect).
    void DrawSystemBack (CDC* pDC, CRect rcButton)
    {
        HTHEME   hTm = OpenThemeData (NULL, L"Button") ;
        if (hTm)
        {
            int   n = PBS_NORMAL ;
            if (IsButtonHighlight())
                n = PBS_PRESSED ;
            else if (IsMouseHovering())
                n = PBS_HOT ;

            DrawThemeBackground (hTm, *pDC, BP_PUSHBUTTON, n, rcButton, NULL) ;
            CloseThemeData(hTm) ;
        }
        else
        {
            UINT   t = 0 ;
            if (IsButtonHighlight() || GetCheck())
                t = DFCS_PUSHED ;
            pDC->DrawFrameControl (rcButton, DFC_BUTTON, DFCS_BUTTONPUSH | t) ;
        }
    }

    virtual void DrawItem (LPDRAWITEMSTRUCT lpDIS)
    {
        CRect   rcClient ;
        GetClientRect(rcClient) ;

        CBitmap   bmp ;
        bmp.Attach (CreateCompatibleBitmap(lpDIS->hDC, rcClient.Width(), rcClient.Height())) ;

        {
            FCImageDrawDC   memDC(bmp) ;

            DrawButtonToDC (memDC, rcClient) ;

            BitBlt (lpDIS->hDC, 0, 0, rcClient.Width(), rcClient.Height(), memDC, 0, 0, SRCCOPY) ;
        } // <-- select bmp out of dc
    }

    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
    {
        FCTrackMouseHover::FilterMouseMessage (m_hWnd, msg) ;

        switch (msg)
        {
            case WM_ERASEBKGND :
                return 1 ;

            case BM_GETCHECK :
                return m_check ;

            case BM_SETCHECK :
                m_check = wParam ;
                Invalidate() ;
                return 0 ;

            case WM_SETCURSOR :
                if (m_cursor)
                {
                    ::SetCursor(m_cursor) ;
                    return TRUE ;
                }
                break;
        }
        return __super::WindowProc(msg, wParam, lParam) ;
    }

private:
    DWORD     m_tooltip_style ;
    LRESULT   m_check ;
    CString   m_tip ;
    UINT      m_nDelayTimeMS ;
    HCURSOR   m_cursor ;

    CToolTipCtrl   m_tip_ctrl ;

    enum
    {
        ID_UISTONE_BUTTON_TOOLIP = 100,
    };

    void DrawButtonToDC (HDC dc, CRect rcButton)
    {
        HGDIOBJ   hOldFont = SelectObject (dc, FCFontManager::GetDefaultFont()) ;

        CDC   * pDC = CDC::FromHandle(dc) ;
        if (pDC)
        {
            OnDrawButton (pDC, rcButton) ;
        }

        SelectObject (dc, hOldFont) ;
    }

    // internal.
    void ReRegisterToolTip()
    {
        if (!GetSafeHwnd())
            {ASSERT(FALSE); return;}

        if (m_tip_ctrl.GetSafeHwnd())
            m_tip_ctrl.DestroyWindow() ;

        if (!m_tip.GetLength())
            return ;

        m_tip_ctrl.Create (this, m_tooltip_style) ;
        m_tip_ctrl.SetMaxTipWidth(500) ; // max 500-pixel width
        m_tip_ctrl.SetDelayTime(m_nDelayTimeMS) ;
        m_tip_ctrl.SetDelayTime(TTDT_AUTOPOP, 15000) ;
        m_tip_ctrl.Activate(TRUE) ;

        CRect   rc ;
        GetClientRect(rc) ;
        m_tip_ctrl.AddTool (this, m_tip, rc, ID_UISTONE_BUTTON_TOOLIP) ;
    }
};

//-------------------------------------------------------------------------------------
class UIStone
{
public:
    static UINT MSG_UPDATE_LAYERED_WINDOW()
    {
        static UINT   n = ::RegisterWindowMessage (_T("MSG_UPDATE_LAYERED_WINDOW")) ;
        return n ;
    }

    static UINT MSG_DRAW_PARENT_PANEL()
    {
        static UINT   n = ::RegisterWindowMessage (_T("MSG_DRAW_PARENT_PANEL")) ;
        return n ;
    }

    static Gdiplus::Rect ToGdiplusRect (CRect rc)
    {
        return Gdiplus::Rect(rc.left, rc.top, rc.Width(), rc.Height()) ;
    }
};
