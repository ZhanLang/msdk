#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectColorBalance : public DlgEffectBase
{
    class CColorSlider : public CWndPaletteSlider
    {
    public:
        Gdiplus::Color   m_left, m_right ;

        CColorSlider() : CWndPaletteSlider(-100, 100, 0) {}

    private:
        virtual void OnDrawSliderImage (CDC* pDC, CRect rc)
        {
            Gdiplus::LinearGradientBrush   br (UIStone::ToGdiplusRect(rc), m_left, m_right, Gdiplus::LinearGradientModeHorizontal) ;
            Gdiplus::Graphics(*pDC).FillRectangle (&br, UIStone::ToGdiplusRect(rc)) ;
        }
    };

private:
    CColorSlider   m_slider[3] ;
    int   m_value[3] ;
    int   m_type ;
    int   m_keep_luminosity ;

public:
    DlgEffectColorBalance (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_COLOR_BALANCE", IDD_EFFECT_COLOR_BALANCE, pDoc)
    {
        m_type = 1 ;
        m_keep_luminosity = 1 ;

        m_slider[0].m_left = Gdiplus::Color(0,255,255) ;
        m_slider[0].m_right = Gdiplus::Color(255,0,0) ;
        m_slider[1].m_left = Gdiplus::Color(255,0,255) ;
        m_slider[1].m_right = Gdiplus::Color(0,255,0) ;
        m_slider[2].m_left = Gdiplus::Color(255,255,0) ;
        m_slider[2].m_right = Gdiplus::Color(0,0,255) ;

        m_value[0]=m_value[1]=m_value[2]=0 ;
    }

private:
    enum
    {
        IDC_TYPE_START = 2000,
        IDC_KEEP_LUMINOSITY = 2100,
        IDC_STR_COLOR = 2200,
        IDC_COLOR_SLIDER = 3000,
        IDC_VALUE_TXT = 3500,
    };

    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectColorBalance (m_keep_luminosity ? true : false, (FCEffectColorBalance::TONE_REGION)m_type, m_value[0], m_value[1], m_value[2]) ;
    }

    virtual BOOL OnInitDialog()
    {
        for (int i=0 ; i < 6 ; i++)
        {
            SetControlText(IDC_STR_COLOR+i, i) ;
        }

        for (int i=0 ; i < 3 ; i++)
        {
            SetControlText(IDC_TYPE_START+i, 10+i) ;
        }

        SetControlText(IDC_KEEP_LUMINOSITY, 20) ;

        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Radio(pDX, IDC_TYPE_START, m_type) ;
        DDX_Check(pDX, IDC_KEEP_LUMINOSITY, m_keep_luminosity) ;

        for (int i=0 ; i < 3 ; i++)
        {
            DDX_Control(pDX, IDC_COLOR_SLIDER+i, m_slider[i]) ;
            DDX_Text(pDX, IDC_VALUE_TXT+i, m_value[i]) ;
            CWndPaletteSlider::DDX_Pos(pDX, m_slider[i], m_value[i]) ;
        }
    }

    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_PHOXO_TRACK_PALETTE_SLIDER)
        {
            UpdateData() ;
            UpdateData(FALSE) ;
            ApplyEffect() ;
            return 0 ;
        }
        return DlgEffectBase::WindowProc(msg, wParam, lParam) ;
    }
};
