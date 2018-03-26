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

//-------------------------------------------------------------------------------------
/**
    List item, responsibility for draw.
*/
class FCListItem_Base
{
private:
    CRect    m_rect ; // RECT on canvas
    CString  m_tip ;
    CWnd     * m_parent ;
    BOOL     m_select ;
    BOOL     m_hoving ;

public :
    FCListItem_Base() : m_parent(NULL), m_hoving(FALSE), m_select(FALSE), m_rect(0,0,0,0) {}
    virtual ~FCListItem_Base() {}

    /// @name Position.
    //@{
    /// Set item's position on canvas.
    void SetRectOnCanvas (CRect rcItem) {m_rect = rcItem;}
    /// Get item's position on canvas.
    CRect GetRectOnCanvas() const {return m_rect;}
    /// Get item's position on list window.
    CRect GetRectOnWindow() const
    {
        CRect   rc = m_rect ;
        if (m_parent)
        {
            int   nStyle = m_parent->GetStyle() ;
            int   x = ((nStyle & WS_HSCROLL) ? -m_parent->GetScrollPos(SB_HORZ) : 0) ;
            int   y = ((nStyle & WS_VSCROLL) ? -m_parent->GetScrollPos(SB_VERT) : 0) ;
            rc.OffsetRect (x,y) ;
        }
        return rc ;
    }
    /// Get size of item.
    CSize GetItemSize() const {return m_rect.Size();}
    //@}

    /// @name Coordinate transformation.
    //@{
    /// point on window --> point on item.
    void Window_to_Item (POINT& pt) const
    {
        if (m_parent)
        {
            int   x = ((m_parent->GetStyle() & WS_HSCROLL) ? m_parent->GetScrollPos(SB_HORZ) : 0) ;
            int   y = ((m_parent->GetStyle() & WS_VSCROLL) ? m_parent->GetScrollPos(SB_VERT) : 0) ;
            pt.x += (x - m_rect.left) ;
            pt.y += (y - m_rect.top) ;
        }
    }
    //@}

    /// Get window the item on.
    CWnd* GetWindow() const {return m_parent;}

    /// @name Query state.
    //@{
    /// Is mouse cursor on item.
    BOOL IsMouseHovering() const {return m_hoving;}
    /// Is item selected.
    BOOL IsSelected() const {return m_select;}
    /// Set select flag.
    void SetSelect (BOOL bSelect) { m_select=bSelect; }
    /// Set item's tip.
    void SetTip (LPCTSTR strTip) { m_tip=strTip; }
    /// Get item's tip.
    LPCTSTR GetTip() const {return m_tip;}
    //@}

    /// @name Event.
    //@{
    /// Mouse enter item.
    virtual void OnMouse_EnterItem() {}
    /// Mouse leave item.
    virtual void OnMouse_LeaveItem() {}
    /// Draw item, the origin point of DC has been moved to top left of item.
    virtual void OnPaint_Item (CDC& dc) =0 ;
    /// The paint of current item has been skiped when list redraw.
    virtual void OnDiscardPaint() {}
    //@}

friend class FCListWindow; // access m_parent / m_hoving
};
