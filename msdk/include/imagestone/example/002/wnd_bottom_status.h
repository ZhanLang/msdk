#pragma once
#include "Resource.h"

//-------------------------------------------------------------------------------------
class CWndBottomStatus : public CStatusBar
{
    enum
    {
        PROGRESS_INDEX = 0,
        POSITION_INDEX = 1,
        ZOOM_INDEX = 2,
        TRACK_SIZE_INDEX = 3,

        IDC_PROGRESS_BAR = 2000,
    };

    CProgressCtrl   m_progress ;

    static CWndBottomStatus   * g_obj ;

public:
    CWndBottomStatus()
    {
        g_obj = this ;
    }
    ~CWndBottomStatus()
    {
        g_obj = NULL ;
    }

    // ptOnCanvas start from 0, but we show from 1, so we need add 1
    static void SetPosition (CPoint ptOnCanvas)
    {
        CWndBottomStatus   * p = g_obj ;
        if (!p)
            return ;

        static CString   sName = L"Position" ;

        CString   s ;
        if (ptOnCanvas != CPoint(-1,-1))
        {
            ptOnCanvas += CPoint(1,1) ; // start from 1

            CString   t ;
            t.Format (L" ( %d , %d )", ptOnCanvas.x, ptOnCanvas.y) ;
            s = sName + t ;
        }
        p->SetPaneText (POSITION_INDEX, s) ;
    }

    static void SetZoom (int nScale)
    {
        CWndBottomStatus   * p = g_obj ;
        if (p)
        {
            CString   s ;
            if (abs(nScale) == 1)
            {
                // no zoom
                s = L"Actual View" ;
            }
            else
            {
                CString   fs = ((nScale > 1) ? L"x %d View" : L"1 / %d View") ;
                s.Format (fs, abs(nScale)) ;
            }
            p->SetPaneText (ZOOM_INDEX, s) ;
        }
    }

    static void SetTrackSize (int w, int h)
    {
        CWndBottomStatus   * p = g_obj ;
        if (!p)
            return ;

        static CString   sName = L"Size" ;

        CString   s ;
        s.Format (L" ( %d , %d )", w, h) ;
        p->SetPaneText (TRACK_SIZE_INDEX, sName + s) ;
    }

    static void SetMoveSize (CSize sz)
    {
        CWndBottomStatus   * p = g_obj ;
        if (!p)
            return ;

        static CString   sName = L"Move" ;

        CString   s ;
        s.Format (L" ( %d , %d )", sz.cx, sz.cy) ;
        p->SetPaneText (TRACK_SIZE_INDEX, sName + s) ;
    }

    static void SetProgress (int nProgress)
    {
        CWndBottomStatus   * p = g_obj ;
        if (p)
        {
            if (nProgress == 100)
            {
                p->m_progress.ShowWindow(SW_HIDE) ;
            }
            else
            {
                if (!p->m_progress.IsWindowVisible())
                {
                    CRect   rc ;
                    p->GetItemRect (PROGRESS_INDEX, rc) ;
                    p->m_progress.SetWindowPos (NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_NOACTIVATE) ;
                    p->m_progress.SetPos(0) ;
                    p->m_progress.ShowWindow(SW_SHOWNA) ;
                }

                p->m_progress.SetPos(nProgress) ;
            }
        }
    }

    static void HideProgress()
    {
        if (g_obj)
        {
            g_obj->m_progress.ShowWindow(SW_HIDE) ;
        }
    }

    void CreateBar (CWnd* pParentWnd)
    {
        const UINT indicators[] =
        {
            ID_INDICATOR1,
            ID_INDICATOR2,           // status line indicator
            ID_INDICATOR_ZOOM,
            ID_SEPARATOR,
        };

        Create (pParentWnd) ;
        SetIndicators (indicators, sizeof(indicators)/sizeof(UINT)) ;

        // create progress control
        m_progress.Create (WS_CHILD, CRect(0,0,5,5), this, IDC_PROGRESS_BAR) ;
        m_progress.SetRange (0, 100) ;
        m_progress.SetStep(1) ;
    }
};

__declspec(selectany) CWndBottomStatus* CWndBottomStatus::g_obj = NULL ;
