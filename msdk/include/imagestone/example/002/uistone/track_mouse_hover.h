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
    Monitor mouse enter/leave window.
*/
class FCTrackMouseHover
{
    BOOL   m_track ;
public:
    FCTrackMouseHover() : m_track(FALSE) {}
    virtual ~FCTrackMouseHover() {}

    /// Is mouse pointer is staying on window now.
    BOOL IsMouseHovering() const {return m_track;}

    /// @name Mouse enter/leave window event.
    //@{
    /// Event mouse enter window, default do nothing.
    virtual void OnMouse_EnterWnd() {}
    /// Event mouse leave window, default do nothing.
    virtual void OnMouse_LeaveWnd() {}
    //@}

    /// Derived class must call this function to monitor mouse event.
    void FilterMouseMessage (HWND hWnd, UINT msg)
    {
        if (msg == WM_MOUSEMOVE)
        {
            if (!m_track && ::IsWindow(hWnd))
            {
                TRACKMOUSEEVENT   t = {0} ;
                t.cbSize = sizeof(t) ;
                t.hwndTrack = hWnd ;
                t.dwFlags = TME_LEAVE ;
                m_track = TrackMouseEvent(&t) ;
                OnMouse_EnterWnd() ;
                ::InvalidateRect (hWnd, NULL, TRUE) ;
            }
        }
        else if (msg == WM_MOUSELEAVE)
        {
            if (m_track && ::IsWindow(hWnd))
            {
                // it's unnecessary to override OnKillFocus,
                // because we can receive WM_MOUSELEAVE when the window deactive.
                m_track = FALSE ;
                OnMouse_LeaveWnd() ;
                ::InvalidateRect (hWnd, NULL, TRUE) ;
            }
        }
    }
};
