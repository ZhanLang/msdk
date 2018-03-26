#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectColorTone : public DlgEffectBase
{
    class CHueSlider : public CWndPaletteSlider
    {
        FCObjImage   m_img ;
    public:
        CHueSlider() : CWndPaletteSlider(0, 359, 45)
        {
            m_img.Create(360, 1, 32) ;
            for (int x=0 ; x < m_img.Width() ; x++)
            {
                *(RGBQUAD*)m_img.GetBits(x,0) = GetColor(x) ;
            }
        }

        RGBQUAD GetColor (int nPos) const
        {
            return FCColor::HSVtoRGB (nPos/360.0, 1.0, 1.0) ;
        }
        RGBQUAD GetColor() const {return GetColor(GetPos());}

    private:
        virtual void OnDrawSliderImage (CDC* pDC, CRect rc)
        {
            FCImageDrawDC::DrawBitmap(*pDC, rc, m_img) ;
        }
    };

    class CAmountSlider : public CWndPaletteSlider
    {
        Gdiplus::Color   m_color ;
    public:
        CAmountSlider() : CWndPaletteSlider(0, 0xFF, 160) {}
        void SetColor (RGBQUAD cr)
        {
            m_color = Gdiplus::Color(cr.rgbRed,cr.rgbGreen,cr.rgbBlue) ;
            if (m_hWnd)
                Invalidate() ;
        }
    private:
        virtual void OnDrawSliderImage (CDC* pDC, CRect rc)
        {
            Gdiplus::LinearGradientBrush   br (UIStone::ToGdiplusRect(rc), Gdiplus::Color(255,255,255), m_color, Gdiplus::LinearGradientModeHorizontal) ;
            Gdiplus::Graphics(*pDC).FillRectangle (&br, UIStone::ToGdiplusRect(rc)) ;
        }
    };

    CHueSlider     m_hue ;
    CAmountSlider  m_amount ;

public:
    DlgEffectColorTone (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_COLOR_TONE", IDD_EFFECT_COLOR_TONE, pDoc)
    {
        m_amount.SetColor (m_hue.GetColor()) ;
    }

private:
    enum
    {
        IDC_STR_HUE = 1000,
        IDC_STR_AMOUNT = 1001,
        IDC_HUE_SLIDER = 1200,
        IDC_AMOUNT_SLIDER = 1201,
    };

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Control(pDX, IDC_HUE_SLIDER, m_hue) ;
        DDX_Control(pDX, IDC_AMOUNT_SLIDER, m_amount) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_HUE, 1) ;
        SetControlText(IDC_STR_AMOUNT, 2) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectColorTone (m_hue.GetColor(), m_amount.GetPos()) ;
    }

    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_PHOXO_TRACK_PALETTE_SLIDER)
        {
            if (wParam == IDC_HUE_SLIDER)
            {
                m_amount.SetColor (m_hue.GetColor()) ;
            }
            ApplyEffect() ;
            return 0 ;
        }
        return DlgEffectBase::WindowProc(msg, wParam, lParam) ;
    }
};
