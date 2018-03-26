/*
    Copyright (C) =USTC= Fu Li

    Author   :  Fu Li
    Create   :  2005-3-11
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
#include "ctrl_list_Layout.h"

//-------------------------------------------------------------------------------------
/**
    List window.
*/
class FCListWindow : public CWnd,
                     public FCTrackMouseHover
{
private:
    CToolTipCtrl   m_tip_ctrl ;
    std::auto_ptr<FCLayout_HandlerBase>   m_layout ;
    std::deque<FCListItem_Base*>   m_item_list ;

public:
    virtual ~FCListWindow()
    {
        DestroyWindow() ;

        RemoveAllListItem() ;
    }

    /// Create list ctrl, WS_VISIBLE is not set by default.
    BOOL Create (CRect rect_on_parent, CWnd* pParent, UINT nID, DWORD dwStyle=0)
    {
        return CWnd::Create (NULL, NULL, WS_CHILD|WS_TABSTOP|WS_CLIPCHILDREN|dwStyle, rect_on_parent, pParent, nID) ;
    }

    /// Set layout handler, user can't delete this handler.
    void SetLayoutHandler (FCLayout_HandlerBase* pLayout)
    {
        m_layout.reset(pLayout) ;
    }

    /// @name Manipulate items.
    //@{
    /// Add a child item into window.
    void AddListItem (FCListItem_Base* pItem, int nIndex=-1)
    {
        if (pItem)
        {
            pItem->m_parent = this ;
            if (nIndex == -1)
            {
                m_item_list.push_back (pItem) ;
            }
            else
            {
                if ((nIndex >= 0) && (nIndex <= (int)m_item_list.size()))
                {
                    m_item_list.insert (m_item_list.begin()+nIndex, pItem) ;
                }
                else
                {
                    assert(false) ;
                    delete pItem ;
                }
            }
        }
    }

    /// Get list item.
    FCListItem_Base* GetListItem (int nIndex) const
    {
        if ((nIndex >= 0) && (nIndex < (int)m_item_list.size()))
            return m_item_list[nIndex] ;
        else
            {assert(false); return NULL;}
    }

    /// Get count of item.
    int GetListItemCount() const {return (int)m_item_list.size();}

    /// Get index of selected item.
    void GetSelectedItemIndex (std::deque<int>& ls) const
    {
        ls.clear() ;
        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            if (GetListItem(i)->IsSelected())
                ls.push_back(i) ;
        }
    }

    /// Test ptWnd in item.
    FCListItem_Base* HitTestListItem (CPoint ptOnWnd, int* pIndex=NULL) const
    {
        // on window --> on canvas
        CPoint   ptOnCanvas = ptOnWnd ;
        ptOnCanvas.Offset ( (GetStyle() & WS_HSCROLL) ? GetScrollPos(SB_HORZ) : 0,
                            (GetStyle() & WS_VSCROLL) ? GetScrollPos(SB_VERT) : 0) ;

        if (pIndex)
            *pIndex = -1 ;
        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            FCListItem_Base   * p = GetListItem(i) ;
            if (p->GetRectOnCanvas().PtInRect(ptOnCanvas))
            {
                if (pIndex)
                    *pIndex = i ;
                return p ;
            }
        }
        return NULL ;
    }

    /// Throw item's ownership, 
    void ThrowListItem (std::deque<FCListItem_Base*>& item_list)
    {
        item_list = m_item_list ;
        m_item_list.clear() ;
    }

    /// Remove item from list.
    void RemoveListItem (FCListItem_Base* pItem)
    {
        for (size_t i=0 ; i < m_item_list.size() ; i++)
        {
            if (m_item_list[i] == pItem)
            {
                RemoveListItem ((int)i) ;
                return ;
            }
        }
    }

    /// Remove item from list by its index.
    void RemoveListItem (int nIndex)
    {
        if ((nIndex >= 0) && (nIndex < (int)m_item_list.size()))
        {
            delete m_item_list[nIndex] ;
            m_item_list.erase (m_item_list.begin() + nIndex) ;
        }
    }

    /// Remove all items from list.
    void RemoveAllListItem()
    {
        std::for_each (m_item_list.begin(), m_item_list.end(), FCDeleteEachObject()) ;
        m_item_list.clear() ;
    }

    /// Re-Layout all item in window.
    void ReLayoutListItem()
    {
        // layout items.
        if (!m_layout.get())
        {
            assert(FALSE) ;
            return ;
        }

        if (!m_hWnd)
            return ;

        std::deque<FCListItem_Base*>   itemList ;
        for (int i=0 ; i < GetListItemCount() ; i++)
            itemList.push_back (GetListItem(i)) ;

        m_layout->Layout_ListItem (this, itemList) ;
        ModifyStyle (0, 0, SWP_FRAMECHANGED) ; // force to calculate client area
        ReRegisterToolTip() ;
        Invalidate() ;
    }
    //@}

protected:
    /// Draw window's client.
    virtual void PaintClientArea (CPaintDC& paint_dc) ;

    /// Draw background of window, default to fill white.
    virtual void DrawListWindowBack (CDC& dc, CRect rcClient)
    {
        dc.FillSolidRect (rcClient, RGB(255,255,255)) ;
    }

    /// Change draw order of list item, default do nothing.
    virtual void ChangeListItemDrawOrder (std::deque<FCListItem_Base*>& all_item) {}

    // internal.
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if (m_tip_ctrl.GetSafeHwnd())
        {
            m_tip_ctrl.RelayEvent(pMsg) ;
        }
        return __super::PreTranslateMessage(pMsg) ;
    }

    // internal. the tooltip doesn't display after it's first show, i don't know why.
    virtual void OnMouse_EnterWnd()
    {
        ModifyStyle (0, 0, SWP_FRAMECHANGED) ;
        ReRegisterToolTip() ;
    }
    // internal. simulate mouse out of every-item.
    virtual void OnMouse_LeaveWnd()
    {
        MouseMoveItemHover (CPoint(-2008,-2008)) ;
    }

    // internal. handler window's message.
    virtual LRESULT WindowProc (UINT message, WPARAM wParam, LPARAM lParam) ;

private:
    // test need create tip
    BOOL IsNeedCreateTipCtrl() const
    {
        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            if (GetListItem(i)->m_tip.GetLength())
                return TRUE ;
        }
        return FALSE ;
    }

    // internal.
    void ReRegisterToolTip()
    {
        if (!GetSafeHwnd())
            {ASSERT(FALSE); return;}

        if (m_tip_ctrl.GetSafeHwnd())
            m_tip_ctrl.DestroyWindow() ;

        if (!IsNeedCreateTipCtrl())
            return ;

        m_tip_ctrl.Create(this, TTS_ALWAYSTIP) ;
        m_tip_ctrl.SetMaxTipWidth(500) ; // max 500-pixel width
        m_tip_ctrl.SetDelayTime(250) ;
        m_tip_ctrl.SetDelayTime(TTDT_AUTOPOP, 15000) ;
        m_tip_ctrl.Activate(TRUE) ;

        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            FCListItem_Base   * pItem = GetListItem(i) ;
            if (pItem->m_tip.GetLength())
            {
                m_tip_ctrl.AddTool (this, pItem->m_tip, pItem->GetRectOnWindow(), 100+i) ;
            }
        }
    }

    void MouseMoveItemHover (CPoint pt)
    {
        // on window --> on canvas
        pt.Offset ( (GetStyle() & WS_HSCROLL) ? GetScrollPos(SB_HORZ) : 0,
                    (GetStyle() & WS_VSCROLL) ? GetScrollPos(SB_VERT) : 0) ;

        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            FCListItem_Base   * pItem = GetListItem(i) ;
            if (pItem->m_rect.PtInRect(pt))
            {
                if (!pItem->m_hoving)
                {
                    pItem->m_hoving = TRUE ;
                    pItem->OnMouse_EnterItem() ;
                    InvalidateRect (pItem->GetRectOnWindow(), FALSE) ;
                }
            }
            else
            {
                if (pItem->m_hoving)
                {
                    pItem->m_hoving = FALSE ;
                    pItem->OnMouse_LeaveItem() ;
                    InvalidateRect (pItem->GetRectOnWindow(), FALSE) ;
                }
            }
        }
    }

    void OnScrollBarEvent (int nSBCode, int nSBType)
    {
        if (!GetListItemCount())
            return ;

        int     nPage ;
        if (nSBType == SB_HORZ)
        {
            if (!(GetStyle() & WS_HSCROLL))
                return ;
            nPage = GetListItem(0)->m_rect.Size().cx ;
        }
        else
        {
            if (!(GetStyle() & WS_VSCROLL))
                return ;
            nPage = GetListItem(0)->m_rect.Size().cy ;
        }

        // Get Current Position
        SCROLLINFO   si = {0} ;
        GetScrollInfo (nSBType, &si, SIF_ALL) ;

        int   nPos ;
        if ((nSBCode != SB_THUMBPOSITION) && (nSBCode != SB_THUMBTRACK))
        {
            nPos = si.nPos ;
        }
        else
        {
            nPos = si.nTrackPos ;
        }

        switch (nSBCode)
        {
            case SB_PAGEUP   : nPos -= nPage ; break;
            case SB_LINEUP   : nPos -= 5 ;     break;
            case SB_PAGEDOWN : nPos += nPage ; break;
            case SB_LINEDOWN : nPos += 5 ;     break;
        }

        nPos = __max(si.nMin, __min(si.nMax, nPos)) ;
        SetScrollPos (nSBType, nPos) ; // update scrollbar's position
        Invalidate() ;
    }
};
//-------------------------------------------------------------------------------------
inline void FCListWindow::PaintClientArea (CPaintDC& paint_dc)
{
    CRect   rcClient ;
    GetClientRect (rcClient) ;

    // update rect on client
    CRect   rcUpdate = paint_dc.m_ps.rcPaint ;
    if (rcUpdate.IsRectEmpty())
        return ;

    // scroll-bar position
    CSize   scroll_pos ;
    scroll_pos.cx = ((GetStyle() & WS_HSCROLL) ? GetScrollPos(SB_HORZ) : 0) ;
    scroll_pos.cy = ((GetStyle() & WS_VSCROLL) ? GetScrollPos(SB_VERT) : 0) ;

    // memory dc to avoid flash
    CBitmap   bmp ;
    bmp.CreateCompatibleBitmap (&paint_dc, rcClient.Width(), rcClient.Height()) ;
    {
        FCImageDrawDC  idd(bmp) ;
        CDC            * pMemDC = CDC::FromHandle(idd) ;
        HGDIOBJ        hOldFont = ::SelectObject(*pMemDC, FCFontManager::GetDefaultFont()) ;

        // draw client area's background
        DrawListWindowBack (*pMemDC, rcClient) ;

        std::deque<FCListItem_Base*>   draw_list ;
        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            draw_list.push_back (GetListItem(i)) ;
        }

        ChangeListItemDrawOrder (draw_list) ;

        // draw all items in list
        for (size_t i=0 ; i < draw_list.size() ; i++)
        {
            FCListItem_Base   * pItem = draw_list[i] ;

            CRect   rcItemOnDC = pItem->m_rect ;
            rcItemOnDC.MoveToXY (pItem->m_rect.TopLeft() - scroll_pos) ;

            if (CRect().IntersectRect(rcItemOnDC, rcClient) == 0)
            {
                pItem->OnDiscardPaint() ;
            }

            if (CRect().IntersectRect(rcItemOnDC, rcUpdate) == 0)
                continue ;

            // to every item, the origin of DC is point (0,0)
            CPoint   ptOldView = pMemDC->SetViewportOrg (rcItemOnDC.TopLeft()) ;
            pItem->OnPaint_Item (*pMemDC) ;
            pMemDC->SetViewportOrg (ptOldView) ;
        }

        ::SelectObject(*pMemDC, hOldFont) ;
        paint_dc.BitBlt (rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), pMemDC, rcUpdate.left, rcUpdate.top, SRCCOPY) ;
    }
}
//-------------------------------------------------------------------------------------
inline LRESULT FCListWindow::WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
{
    FCTrackMouseHover::FilterMouseMessage (m_hWnd, msg) ;

    switch (msg)
    {
        case WM_SIZE :
            // layout item object when size changed
            ReLayoutListItem() ;
            break;

        case WM_ERASEBKGND :
            return TRUE ;

        case WM_MOUSEMOVE :
            MouseMoveItemHover (CPoint(lParam)) ;
            break;

        case WM_LBUTTONDOWN :
            SetFocus() ;
            Invalidate() ;
            break;

        case WM_PAINT :
            {
                CPaintDC   dc(this) ;
                PaintClientArea(dc) ;
            }
            return TRUE ;

        case WM_HSCROLL :
        case WM_VSCROLL :
            OnScrollBarEvent ((short)LOWORD(wParam), (msg == WM_HSCROLL) ? SB_HORZ : SB_VERT) ;
            ReRegisterToolTip() ;
            break;

        case WM_MOUSEWHEEL :
            {
                UINT   uCode = ((short)HIWORD(wParam)>0) ? SB_PAGEUP : SB_PAGEDOWN ;
                PostMessage (WM_VSCROLL, MAKEWPARAM(uCode,0)) ;
                CPoint   pt (lParam) ;
                ScreenToClient (&pt) ;
                PostMessage (WM_MOUSEMOVE, wParam, MAKELPARAM(pt.x,pt.y)) ;
            }
            break;
    }
    return __super::WindowProc (msg, wParam, lParam) ;
}
//-------------------------------------------------------------------------------------
