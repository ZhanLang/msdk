#include "stdafx.h"
#include "002.h"
#include "dlg_effect_shadow.h"

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DlgEffectShadow, DlgModalBase)
    ON_EN_CHANGE(IDC_X_OFFSET, OnInputOffset)
    ON_EN_CHANGE(IDC_Y_OFFSET, OnInputOffset)
    ON_WM_HSCROLL()
    ON_MESSAGE(WM_PHOXO_TRACK_PALETTE_SLIDER, OnTrackPaletteSlider)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
DlgEffectShadow::DlgEffectShadow (SHADOWDATA* sd) : DlgModalBase(L"IDD_EFFECT_SHADOW", IDD_EFFECT_SHADOW),
                                                    m_slider_opaque(1, 100, (sd ? sd->m_opacity : 75))
{
    SetTitleFromMenu(ID_EFFECT_SHADOW) ;

    m_pic.m_this = this ;

    if (sd)
        m_shadow_data = *sd ;

    SetSlider_1 (2, 32, m_shadow_data.m_smooth, 0) ;
    SetSlider_2 (1, 100, (0xFF-m_shadow_data.m_color.rgbBlue)*100/0xFF, 1) ;

    m_img.LoadResource (IDR_PNG_SHADOW, L"PNG", IMG_PNG) ;
}
//-------------------------------------------------------------------------------------
void DlgEffectShadow::UpdatePreview()
{
    UpdateData() ;

    m_shadow_data.m_smooth = GetSlider_1() ;
    int   n = 0xFF * (100 - GetSlider_2()) / 100 ;
    m_shadow_data.m_color = FCColor(n,n,n) ;

    m_shadow = m_img ;
    m_shadow.ApplyEffect(CFillShadowImage(m_shadow_data.m_color, m_shadow_data.m_opacity)) ;
    m_shadow.ApplyEffect(FCEffectBlur_Gauss(m_shadow_data.m_smooth, false)) ;

    m_pic.Invalidate() ;
}
//-------------------------------------------------------------------------------------
BOOL DlgEffectShadow::OnInitDialog()
{
    SetControlText(IDC_STR_OFFSET, 3) ;
    SetControlText(IDC_SLIDER_NAME_3, 2) ;

    BOOL   b = DlgModalBase::OnInitDialog() ;

    CRect   rc ;
    m_pic.GetClientRect(rc) ;
    m_img_rect = FCObjGraph::CalcFitWindowSize(CSize(m_img.Width(),m_img.Height()), rc) ;

    UpdatePreview() ;
    return b ;
}
//-------------------------------------------------------------------------------------
void DlgEffectShadow::DoDataExchange(CDataExchange* pDX)
{
    DlgModalBase::DoDataExchange(pDX) ;
    DDX_Control(pDX, IDC_PREVIEW_THUMB, m_pic) ;
    DDX_Text_No_Tip(pDX, IDC_X_OFFSET, m_shadow_data.m_offset_x) ;
    DDX_Text_No_Tip(pDX, IDC_Y_OFFSET, m_shadow_data.m_offset_y) ;
    DDX_Control (pDX, IDC_SLIDER_3, m_slider_opaque) ;
    DDX_Text (pDX, IDC_SLIDER_VALUE_3, m_shadow_data.m_opacity) ;
    CWndPaletteSlider::DDX_Pos (pDX, m_slider_opaque, m_shadow_data.m_opacity) ;

    m_shadow_data.m_offset_x = FClamp(m_shadow_data.m_offset_x, -50, 50) ;
    m_shadow_data.m_offset_y = FClamp(m_shadow_data.m_offset_y, -50, 50) ;
}
//-------------------------------------------------------------------------------------
void DlgEffectShadow::OnInputOffset()
{
    UpdatePreview() ;
}
//-------------------------------------------------------------------------------------
void DlgEffectShadow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    DlgModalBase::OnHScroll(nSBCode, nPos, pScrollBar) ;
    UpdatePreview() ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectShadow::OnTrackPaletteSlider(WPARAM wParam, LPARAM lParam)
{
    UpdateData() ;
    UpdateData(FALSE) ;
    UpdatePreview() ;
    return 0 ;
}
//-------------------------------------------------------------------------------------
