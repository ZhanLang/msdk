#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectLensFlare : public DlgEffectBase
{
private:
    //---------------------------------------------------------------------------------
    class CWndPreviewCtrl : public FCButtonBase
    {
    public:
        FCObjImage   m_img ;
        DlgEffectLensFlare   * m_parent ;

    private:
        virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_LBUTTONDOWN)
            {
                SetTooltipText(L"") ;
                m_parent->ClickPreviewImage(lParam) ;
                return 0 ;
            }
            return FCButtonBase::WindowProc(msg, wParam, lParam) ;
        }

        virtual void OnDrawButton (CDC* pDC, CRect rcButton)
        {
            pDC->FillSolidRect(rcButton, RGB(192,192,192)) ; // border color
            rcButton.DeflateRect(1,1,1,1) ;
            pDC->FillSolidRect(rcButton, RGB(191,203,251)) ;

            if (m_img.IsValidImage())
            {
                CRect   rc = FCObjGraph::CalcFitWindowSize(CSize(m_img.Width(),m_img.Height()), rcButton) ;
                m_img.Draw (*pDC, rc) ;
            }
        }
    };

private:
    enum
    {
        IDC_PREVIEW_THUMB = 2100,
    };

    FCObjImage      m_bak ;
    double          m_scale ;
    CPoint          m_pt ;
    CWndPreviewCtrl m_preview ;

public:
    DlgEffectLensFlare (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_LENSFLARE", IDD_EFFECT_LENSFLARE, pDoc)
    {
        m_preview.m_parent = this ;

        m_bak = pDoc->m_img ;

        if (!m_bak.IsValidImage())
        {
            m_bak.Create(1,1,32) ;
        }

        m_scale = 1 ;
        m_pt = CPoint(m_bak.Width()/10,m_bak.Height()/10) ;
    }

private:
    void UpdatePreviewImage()
    {
        // process effect
        FCObjImage   img = m_bak ;
        POINT        pt = {(int)(m_pt.x * m_scale), (int)(m_pt.y * m_scale)} ;
        img.ApplyEffect (FCEffectLensFlare(pt)) ;
        img.ApplyEffect (FCEffectPremultipleAlpha()) ;

        m_preview.m_img.Create (m_bak.Width(), m_bak.Height(), 24) ;
        {
            FCImageDrawDC   memDC (m_preview.m_img) ;

            // draw canvas back
            HBRUSH   br = CreatePatternBrush(theApp.m_canvas_back) ;
            FillRect (memDC, CRect(0, 0, m_bak.Width(), m_bak.Height()), br) ;
            DeleteObject(br) ;

            // draw effect image
            img.Draw (memDC, 0, 0) ;
        }
        m_preview.Invalidate() ;
    }

    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectLensFlare(m_pt) ;
    }

    virtual BOOL OnInitDialog()
    {
        BOOL   b = DlgEffectBase::OnInitDialog() ;

        m_preview.SetTooltipText(QueryLanguageText(0)) ;
        m_preview.SetButtonCursor(LoadCursor(NULL,IDC_CROSS)) ;

        CRect   rc ;
        m_preview.GetClientRect(rc) ;
        rc.DeflateRect(1,1,1,1) ;
        rc = FCObjGraph::CalcFitWindowSize (CSize(m_bak.Width(),m_bak.Height()), rc) ;
        m_scale = rc.Width() / (double)m_bak.Width() ;

        m_bak.Stretch (rc.Width(), rc.Height()) ;
        UpdatePreviewImage() ;
        return b ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Control(pDX, IDC_PREVIEW_THUMB, m_preview) ;
    }

    void ClickPreviewImage (CPoint ptOnPreview)
    {
        CRect   rcWnd ;
        m_preview.GetClientRect(rcWnd) ;
        rcWnd.DeflateRect(1,1,1,1) ;

        CRect   rc = FCObjGraph::CalcFitWindowSize (CSize(m_bak.Width(),m_bak.Height()), rcWnd) ;

        m_pt = ptOnPreview ;
        m_pt.x = FClamp(m_pt.x, rc.left, rc.right) ;
        m_pt.y = FClamp(m_pt.y, rc.top, rc.bottom) ;
        m_pt = m_pt - rc.TopLeft() ;

        m_pt.x = (int)(m_pt.x / m_scale) ;
        m_pt.y = (int)(m_pt.y / m_scale) ;

        UpdatePreviewImage() ;
        ApplyEffect() ;
    }
};
