#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectFillGrid : public DlgEffectBase
{
    enum
    {
        IDC_STR_TRANSPARENCY = 1200,
        IDC_SLIDER_TEXT = 1201,
        IDC_SLIDER_BAR = 1202,

        IDC_COLOR_1 = 1000,
        IDC_COLOR_2 = 1001,
        IDC_SIZE_EDIT = 1002,
        IDC_STR_SIZE = 1005,
    };

    int   m_size ;
    int   m_opaque ;
    CWndColorButton   m_pick_1 ;
    CWndColorButton   m_pick_2 ;

    static COLORREF   m_color_1 ;
    static COLORREF   m_color_2 ;

    CWndTransparencySlider   m_slider ;

public:
    DlgEffectFillGrid (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_FILLGRID", IDD_EFFECT_FILLGRID, pDoc),
                                          m_slider(1, 100, 100)
    {
        m_opaque = 100 ;
        m_pick_1.SetColor (m_color_1) ;
        m_pick_2.SetColor (m_color_2) ;
        m_size = 16 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        int   a = 255 * m_opaque / 100 ;
        return new FCEffectFillGrid (FCColor(m_color_1), FCColor(m_color_2), m_size, a) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_TRANSPARENCY, 1) ;
        SetControlText(IDC_STR_SIZE, 0) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Text_No_Tip(pDX, IDC_SIZE_EDIT, m_size) ;
        DDX_Control(pDX, IDC_COLOR_1, m_pick_1) ;
        DDX_Control(pDX, IDC_COLOR_2, m_pick_2) ;
        DDX_Control (pDX, IDC_SLIDER_BAR, m_slider) ;
        DDX_Text (pDX, IDC_SLIDER_TEXT, m_opaque) ;
        CWndPaletteSlider::DDX_Pos (pDX, m_slider, m_opaque) ;
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

    afx_msg void OnChangeGridColor() ;
    afx_msg void OnInputGridSize() ;
    DECLARE_MESSAGE_MAP()
};

COLORREF DlgEffectFillGrid::m_color_1 = RGB(0,160,255) ;
COLORREF DlgEffectFillGrid::m_color_2 = RGB(0xFF,0xFF,0xFF) ;

BEGIN_MESSAGE_MAP(DlgEffectFillGrid, DlgEffectBase)
    ON_EN_CHANGE(IDC_SIZE_EDIT, OnInputGridSize)
    ON_COMMAND(ID_COLOR_BUTTON_CHANGE, OnChangeGridColor)
END_MESSAGE_MAP()

void DlgEffectFillGrid::OnInputGridSize()
{
    ApplyEffect() ;
}

void DlgEffectFillGrid::OnChangeGridColor()
{
    m_color_1 = m_pick_1.GetColor() ;
    m_color_2 = m_pick_2.GetColor() ;
    ApplyEffect() ;
}
