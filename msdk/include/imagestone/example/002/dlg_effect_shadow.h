#pragma once
#include "wnd_transparency_slider.h"

//-------------------------------------------------------------------------------------
class DlgEffectShadow : public DlgModalBase
{
public:
    DlgEffectShadow (SHADOWDATA* sd=NULL) ;

    SHADOWDATA   m_shadow_data ;

private:
    enum
    {
        IDC_STR_OFFSET = 1005,
        IDC_X_OFFSET = 2000,
        IDC_Y_OFFSET = 2001,
        IDC_PREVIEW_THUMB = 2100,
    };

    struct CThumbPreview : public FCButtonBase
    {
        DlgEffectShadow   * m_this ;

        virtual void OnDrawButton (CDC* pDC, CRect rcButton)
        {
            pDC->FillSolidRect(rcButton, RGB(192,192,192)) ;
            rcButton.DeflateRect(1,1,1,1) ;
            pDC->FillSolidRect(rcButton, RGB(255,255,255)) ;

            {
                CRect   rc = m_this->m_img_rect ;
                rc.OffsetRect (m_this->m_shadow_data.m_offset_x, m_this->m_shadow_data.m_offset_y) ;

                std::auto_ptr<Gdiplus::Bitmap>   gb (m_this->m_shadow.CreateBitmap()) ;
                Gdiplus::Graphics(*pDC).DrawImage (gb.get(), rc.left, rc.top, m_this->m_shadow.Width(), m_this->m_shadow.Height()) ;
            }

            {
                CRect   rc = m_this->m_img_rect ;

                std::auto_ptr<Gdiplus::Bitmap>   gb (m_this->m_img.CreateBitmap()) ;
                Gdiplus::Graphics(*pDC).DrawImage (gb.get(), rc.left, rc.top, m_this->m_img.Width(), m_this->m_img.Height()) ;
            }
        }
    };

    CThumbPreview  m_pic ;
    FCObjImage     m_img ;
    FCObjImage     m_shadow ;
    CRect          m_img_rect ;

    CWndTransparencySlider   m_slider_opaque ;

private:
    class CFillShadowImage : public FCImageEffect
    {
        RGBQUAD  m_color ;
        int      m_opacity ;
    public:
        CFillShadowImage (RGBQUAD cr, int nOpacity) : m_color(cr), m_opacity(nOpacity) {}
    private:
        virtual void ProcessPixel (FCObjImage& img, int x, int y, BYTE* pPixel)
        {
            RGBQUAD   c = m_color ;
            PCL_A(&c) = FClamp0255(PCL_A(pPixel) * m_opacity / 100) ;
            *(RGBQUAD*)pPixel = c ;
        }
    };

private:
    void UpdatePreview() ;

    virtual void DoDataExchange(CDataExchange* pDX) ;
    virtual BOOL OnInitDialog() ;
    virtual void OnOK()
    {
        UpdatePreview() ;
        DlgModalBase::OnOK() ;
    }

    afx_msg LRESULT OnTrackPaletteSlider(WPARAM wParam, LPARAM lParam);
    afx_msg void OnInputOffset() ;
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) ;
    DECLARE_MESSAGE_MAP()
};
