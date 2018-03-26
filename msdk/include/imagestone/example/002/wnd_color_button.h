#pragma once

//-------------------------------------------------------------------------------------
class CWndColorButton : public FCButtonBase
{
private:
    COLORREF   m_color ;

public:
    CWndColorButton()
    {
        m_color = RGB(0,0,0) ;
        SetButtonCursor(::LoadCursor(NULL,IDC_HAND)) ;
    }

    void SetColor (COLORREF cr)
    {
        m_color = cr ;
        if (m_hWnd)
            Invalidate() ;
    }

    COLORREF GetColor() const {return m_color;}

private:
    virtual void OnDrawButton (CDC* pDC, CRect rcButton);

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};
