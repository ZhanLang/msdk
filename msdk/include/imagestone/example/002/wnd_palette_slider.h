#pragma once

//-------------------------------------------------------------------------------------
class CWndPaletteSlider : public FCButtonBase
{
    int   m_nMin, m_nMax, m_nPos ;
public:
    // nMin <= range <= nMax
    CWndPaletteSlider (int nMin, int nMax, int nInit)
    {
        m_nMin = nMin ;
        m_nMax = nMax ;
        m_nPos = nInit ;
    }

    int GetPos() const {return m_nPos;}
    int GetMax() const {return m_nMax;}

    static void DDX_Pos (CDataExchange* pDX, CWndPaletteSlider& rSlider, int& nPos) ;

protected:
    virtual void OnDrawSliderImage (CDC* pDC, CRect rc) {}

private:
    void DrawArrow (CDC* pDC, CRect rc)
    {
        int   nCenter = rc.left + (m_nPos-m_nMin) * (rc.Width()-1) / (m_nMax-m_nMin) ;
        Gdiplus::Point   pt[3] ;
        pt[0] = Gdiplus::Point (nCenter-4, rc.bottom) ;
        pt[1] = Gdiplus::Point (nCenter+4, rc.bottom) ;
        pt[2] = Gdiplus::Point (nCenter, rc.bottom-9) ;
        Gdiplus::Graphics   g (*pDC) ;
        g.SetSmoothingMode (Gdiplus::SmoothingModeAntiAlias) ;

        Gdiplus::SolidBrush   b1 (Gdiplus::Color(0,0,0)) ;
        g.FillPolygon (&b1, pt, 3) ;

        pt[0].X-- ;
        pt[1].X++ ;
        pt[2].Y-- ;

        Gdiplus::Pen   p1 (Gdiplus::Color(255,255,255)) ;
        g.DrawPolygon (&p1, pt, 3) ;
    }

    virtual void OnDrawButton (CDC* pDC, CRect rcButton)
    {
        rcButton.DeflateRect(2,2,2,2) ;
        OnDrawSliderImage (pDC, rcButton) ;
        DrawArrow (pDC, rcButton) ;
        rcButton.InflateRect(1,1,1,1) ;
        FrameRect (*pDC, rcButton, CBrush(RGB(255,255,255))) ;
        rcButton.InflateRect(1,1,1,1) ;
        FrameRect (*pDC, rcButton, CBrush(RGB(192,192,192))) ;
    }

    void UpdateMousePos (CPoint pt) ;

    virtual BOOL PreTranslateMessage (MSG* pMsg);

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};
