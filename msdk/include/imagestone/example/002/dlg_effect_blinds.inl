#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectBlinds : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_START = 2000,
        IDC_BLINDS_COLOR = 2100,
    };

    static COLORREF   m_color ;

    int   m_type ;
    CWndColorButton  m_color_button ;

public:
    DlgEffectBlinds (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_BLINDS", IDD_EFFECT_BLINDS, pDoc)
    {
        CSize   sz = GetProcessImageSize() ;
        int     nMax = __max(3,__min(sz.cx,sz.cy)) ;
        int     nInit = __max(2,__min(sz.cx,sz.cy)/8) ;
        SetSlider_1 (2, nMax, nInit, 0) ;
        SetSlider_2 (1, 100, 100, 1) ;
        m_type = 0 ;
        m_color_button.SetColor (m_color) ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectBlinds (m_type ? FCEffectBlinds::BLIND_Y : FCEffectBlinds::BLIND_X, GetSlider_1(), GetSlider_2(), FCColor(m_color)) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_TYPE_START, 2) ;
        SetControlText(IDC_TYPE_START+1, 3) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Radio(pDX, IDC_TYPE_START, m_type) ;
        DDX_Control(pDX, IDC_BLINDS_COLOR, m_color_button) ;
    }

    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_COMMAND) && (LOWORD(wParam) == ID_COLOR_BUTTON_CHANGE))
        {
            m_color = m_color_button.GetColor() ;
            ApplyEffect() ;
            return 0 ;
        }
        return DlgEffectBase::WindowProc(msg, wParam, lParam) ;
    }
};

COLORREF DlgEffectBlinds::m_color = RGB(0,0,0) ;
