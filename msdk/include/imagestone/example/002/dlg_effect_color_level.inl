#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectColorLevel : public DlgEffectBase
{
    //---------------------------------------------------------------------------------
    class CArrowSliderBase : public FCButtonBase
    {
    protected:
        static void DrawArrow (HDC hdc, int nTop, int nCenter, Gdiplus::Color inner_color)
        {
            Gdiplus::Point   pt[3] ;
            pt[0] = Gdiplus::Point (nCenter-5, nTop+10) ;
            pt[1] = Gdiplus::Point (nCenter+5, nTop+10) ;
            pt[2] = Gdiplus::Point (nCenter, nTop) ;

            Gdiplus::Graphics     g (hdc) ;
            Gdiplus::SolidBrush   b1(inner_color) ;
            g.FillPolygon (&b1, pt, 3) ;
            g.SetSmoothingMode (Gdiplus::SmoothingModeAntiAlias) ;
            Gdiplus::Pen   p1(Gdiplus::Color(0,0,0)) ;
            g.DrawPolygon (&p1, pt, 3) ;
        }

        enum ARROW_TYPE
        {
            ARROW_TYPE_LOW,
            ARROW_TYPE_HIGH,
            ARROW_TYPE_GAMMA,
        };

        int GetValue (int x)
        {
            CRect   rc ;
            GetClientRect(rc) ;
            rc.DeflateRect(8,0,8,0) ;

            if (!rc.Width())
                rc.right = rc.left + 1 ;

            return FClamp0255(255 * (x - rc.left) / rc.Width()) ;
        }

        virtual ARROW_TYPE GetArrowType (int x) =0 ;
        virtual void UpdateMousePos (CPoint pt, ARROW_TYPE nType) =0 ;

        void OnLButtonDown (CPoint point)
        {
            ARROW_TYPE   nt = GetArrowType(point.x) ;

            UpdateMousePos (point, nt) ;

            SetCapture() ;
            for (;;)
            {
                MSG   msg ;
                ::GetMessage (&msg, NULL, 0, 0) ;
                if (::GetCapture() != m_hWnd)
                    goto capture_over ; // <== Exit.1

                switch (msg.message)
                {
                    case WM_LBUTTONUP :
                    case WM_MOUSEMOVE :
                        UpdateMousePos (msg.lParam, nt) ;

                        if (msg.message == WM_LBUTTONUP)
                        {
                            ::ReleaseCapture() ; // finish
                            goto capture_over ; // <== Exit.2
                        }
                        break;

                    default : // just dispatch rest of the messages
                        ::DispatchMessage (&msg) ;
                        break;
                }
            }

capture_over:
            Invalidate() ;
        }

        virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_LBUTTONDOWN)
            {
                OnLButtonDown(lParam) ;
                return 0 ;
            }
            return FCButtonBase::WindowProc(msg, wParam, lParam) ;
        }
    };

    //---------------------------------------------------------------------------------
    class CInputSlider : public CArrowSliderBase
    {
    public:
        DlgEffectColorLevel   * m_dlg ;
        int   m_low, m_high, m_gamma ;

        CInputSlider() : m_low(0), m_high(255), m_gamma(100) {}

    private:
        int GetGamma (int x)
        {
            CRect   rc ;
            GetClientRect(rc) ;
            rc.DeflateRect(8,0,8,0) ;

            int   nl = rc.left + rc.Width() * m_low / 255 ;
            int   nh = rc.left + rc.Width() * m_high / 255 ;

            double   d = (nh - nl) / 2.0 ;
            if (nh == nl)
                d = 0.5 ;

            double   t = 1.0 / pow (10.0, (x - (nl + d)) / d) ;
            return (int)(FClamp(t * 100, 10.0, 999.0) + 0.5) ;
        }

        virtual ARROW_TYPE GetArrowType (int x)
        {
            CRect   rc ;
            GetClientRect(rc) ;
            rc.DeflateRect(8,0,8,0) ;

            int   nl = rc.left + rc.Width() * m_low / 255 ;
            int   nh = rc.left + rc.Width() * m_high / 255 ;

            double   d = (nh - nl) / 2.0 ;
            int   ng = (int)(nl + d + d * log10 (1.0 / (m_gamma / 100.0))) ;

            int   dl = (x-nl)*(x-nl) ;
            int   dh = (x-nh)*(x-nh) ;
            int   dg = (x-ng)*(x-ng) ;
            if ((dg <= dl) && (dg <= dh))
                return ARROW_TYPE_GAMMA ;
            if ((dl <= dh) && (dl <= dg))
                return ARROW_TYPE_LOW ;
            if ((dh <= dl) && (dh <= dg))
                return ARROW_TYPE_HIGH ;
            return ARROW_TYPE_GAMMA ;
        }

        virtual void OnDrawButton (CDC* pDC, CRect rcButton)
        {
            pDC->FillSolidRect(rcButton, ::GetSysColor(COLOR_3DFACE)) ;

            int   arrow_height = 38 ;

            // draw histogram
            {
                CRect   rc = rcButton ;
                rc.DeflateRect(6,0,6,0) ;
                rc.bottom -= arrow_height ;
                pDC->FillSolidRect(rc, RGB(192,192,192)) ;
                rc.DeflateRect(1,1,1,1) ;
                pDC->FillSolidRect(rc, RGB(255,255,255)) ;
                rc.DeflateRect(1,1,1,1) ;

                m_dlg->m_histogram[m_dlg->m_channel].Draw (*pDC, rc) ;
            }

            // draw arrow
            {
                CRect   rc = rcButton ;
                rc.DeflateRect(8,0,8,0) ;
                rc.top = rcButton.bottom - arrow_height ;

                int   nl = rc.left + rc.Width() * m_low / 255 ;
                int   nh = rc.left + rc.Width() * m_high / 255 ;

                double   d = (nh - nl) / 2.0 ;
                int   ng = (int)(nl + d + d * log10 (1.0 / (m_gamma / 100.0))) ;

                DrawArrow(*pDC, rc.top, nl, Gdiplus::Color(0,0,0)) ;
                DrawArrow(*pDC, rc.top, nh, Gdiplus::Color(255,255,255)) ;
                DrawArrow(*pDC, rc.top, ng, Gdiplus::Color(133,133,133)) ;
            }

            // draw text
            {
                CRect   rc = rcButton ;
                rc.top = rc.bottom - 23 ;
                rc.left += 6 ;

                CString   s ;
                s.Format(L"%d", m_low) ;
                pDC->DrawText(s, rc, DT_SINGLELINE) ;

                s.Format(L"%d", m_high) ;
                pDC->DrawText(s, rc, DT_SINGLELINE|DT_RIGHT) ;

                s.Format(L"%.2f", m_gamma/100.0) ;
                pDC->DrawText(s, rc, DT_SINGLELINE|DT_CENTER) ;
            }
        }

        virtual void UpdateMousePos (CPoint pt, ARROW_TYPE nType)
        {
            switch (nType)
            {
                case ARROW_TYPE_LOW :
                    m_low = GetValue(pt.x) ;
                    m_low = __min(m_low, m_high - 5) ;
                    break;

                case ARROW_TYPE_HIGH :
                    m_high = GetValue(pt.x) ;
                    m_high = __max(m_high, m_low + 5) ;
                    break;

                case ARROW_TYPE_GAMMA :
                    m_gamma = GetGamma(pt.x) ;
                    break;
            }

            Invalidate() ;
            m_dlg->ApplyEffect() ;
        }
    };

    //---------------------------------------------------------------------------------
    class COutputSlider : public CArrowSliderBase
    {
    public:
        DlgEffectColorLevel   * m_dlg ;
        int   m_low, m_high ;

        COutputSlider() : m_low(0), m_high(255) {}

    private:
        virtual ARROW_TYPE GetArrowType (int x)
        {
            CRect   rc ;
            GetClientRect(rc) ;
            rc.DeflateRect(8,0,8,0) ;

            int   n1 = rc.left + rc.Width() * m_low / 255 ;
            int   n2 = rc.left + rc.Width() * m_high / 255 ;
            if ((x-n1)*(x-n1) > (x-n2)*(x-n2))
                return ARROW_TYPE_HIGH ;
            return ARROW_TYPE_LOW ;
        }

        virtual void OnDrawButton (CDC* pDC, CRect rcButton)
        {
            pDC->FillSolidRect(rcButton, ::GetSysColor(COLOR_3DFACE)) ;

            int   slider_height = 16 ;

            // draw slider
            {
                CRect   rc = rcButton ;
                rc.bottom = rc.top + slider_height ;
                rc.DeflateRect(6,0,6,0) ;
                pDC->FillSolidRect(rc, RGB(192,192,192)) ;
                rc.DeflateRect(1,1,1,1) ;
                pDC->FillSolidRect(rc, RGB(255,255,255)) ;
                rc.DeflateRect(1,1,1,1) ;

                Gdiplus::LinearGradientBrush   br (UIStone::ToGdiplusRect(rc), Gdiplus::Color(0,0,0), Gdiplus::Color(255,255,255), Gdiplus::LinearGradientModeHorizontal) ;
                Gdiplus::Graphics(*pDC).FillRectangle (&br, UIStone::ToGdiplusRect(rc)) ;
            }

            // draw arrow
            {
                CRect   rc = rcButton ;
                rc.DeflateRect(8,slider_height,8,0) ;

                int   nl = rc.left + rc.Width() * m_low / 255 ;
                int   nh = rc.left + rc.Width() * m_high / 255 ;
                DrawArrow(*pDC, rc.top, nl, Gdiplus::Color(0,0,0)) ;
                DrawArrow(*pDC, rc.top, nh, Gdiplus::Color(255,255,255)) ;
            }

            // draw text
            {
                CRect   rc = rcButton ;
                rc.top += slider_height + 15 ;
                rc.left += 6 ;

                CString   s ;
                s.Format(L"%d", m_low) ;
                pDC->DrawText(s, rc, DT_SINGLELINE) ;

                s.Format(L"%d", m_high) ;
                pDC->DrawText(s, rc, DT_SINGLELINE|DT_RIGHT) ;
            }
        }

        virtual void UpdateMousePos (CPoint pt, ARROW_TYPE nType)
        {
            switch (nType)
            {
                case ARROW_TYPE_LOW :
                    m_low = GetValue(pt.x) ;
                    break;

                case ARROW_TYPE_HIGH :
                    m_high = GetValue(pt.x) ;
                    break;
            }

            Invalidate() ;
            m_dlg->ApplyEffect() ;
        }
    };

private:
    enum
    {
        IDC_CHANNEL_COMBO = 1200,
        IDC_SHOW_HISTOGRAM = 1201,
        IDC_OUTPUT_SLIDER = 1202,
        IDC_STR_INPUT = 1210,
        IDC_STR_OUTPUT = 1211,
        IDC_STR_CHANNEL = 1212,
    };

    CInputSlider   m_input ;
    COutputSlider  m_output ;
    int   m_channel ;
    FCObjImage   m_histogram[4] ;
    FCObjImage   m_img ;

public:
    DlgEffectColorLevel (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_COLOR_LEVEL", IDD_EFFECT_COLOR_LEVEL, pDoc),
                                            m_img(pDoc->m_img)
    {
        m_input.m_dlg = this ;
        m_output.m_dlg = this ;
        m_channel = 0 ;
        CreateHistogramImage() ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        FCEffectColorLevel::COLOR_CHANNEL   ic = FCEffectColorLevel::CHANNEL_RGB ;
        switch (m_channel)
        {
            case 0 : ic = FCEffectColorLevel::CHANNEL_RGB ; break;
            case 1 : ic = FCEffectColorLevel::CHANNEL_RED ; break;
            case 2 : ic = FCEffectColorLevel::CHANNEL_GREEN ; break;
            case 3 : ic = FCEffectColorLevel::CHANNEL_BLUE ; break;
        }
        return new FCEffectColorLevel (m_input.m_low, m_input.m_high, m_output.m_low, m_output.m_high, m_input.m_gamma/100.0, ic) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_INPUT, 5) ;
        SetControlText(IDC_STR_OUTPUT, 6) ;
        SetControlText(IDC_STR_CHANNEL, 7) ;

        CComboBox   * p = (CComboBox*)GetDlgItem(IDC_CHANNEL_COMBO) ;
        if (p)
        {
            p->AddString(QueryLanguageText(10)) ;
            p->AddString(QueryLanguageText(11)) ;
            p->AddString(QueryLanguageText(12)) ;
            p->AddString(QueryLanguageText(13)) ;
        }
        return DlgEffectBase::OnInitDialog() ;
    };

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_CBIndex(pDX, IDC_CHANNEL_COMBO, m_channel) ;
        DDX_Control(pDX, IDC_SHOW_HISTOGRAM, m_input) ;
        DDX_Control(pDX, IDC_OUTPUT_SLIDER, m_output) ;
    }

    void CreateHistogramImage()
    {
        for (int i=0 ; i < 4 ; i++)
        {
            m_histogram[i].Create(256, 100, 24) ;

            FCEffectFillColor   c (FCColor(255,255,255)) ;
            m_histogram[i].ApplyEffect(c) ;
        }

        if (!m_img.IsValidImage())
            return ;

        FCEffectGetHistogram   c ;
        m_img.ApplyEffect(c) ;

        int   buf[3] ;
        buf[0] = *std::max_element(c.m_blue.begin(), c.m_blue.end()) ;
        buf[1] = *std::max_element(c.m_green.begin(), c.m_green.end()) ;
        buf[2] = *std::max_element(c.m_red.begin(), c.m_red.end()) ;

        int   nMax = *std::max_element(buf, buf+3) ;
        if (!nMax)
            return ;

        FCImageDrawDC   mb(m_histogram[3]) ;
        FCImageDrawDC   mg(m_histogram[2]) ;
        FCImageDrawDC   mr(m_histogram[1]) ;
        FCImageDrawDC   mrgb(m_histogram[0]) ;
        HBRUSH   br = (HBRUSH)GetStockObject(BLACK_BRUSH) ;
        for (int i=0 ; i < 256 ; i++)
        {
            CRect   rc (i, 0, i+1, 0) ;

            rc.bottom = 100 * c.m_blue[i] / nMax ;
            FillRect(mb, rc, br) ;
            FillRect(mrgb, rc, br) ;

            rc.bottom = 100 * c.m_green[i] / nMax ;
            FillRect(mg, rc, br) ;
            FillRect(mrgb, rc, br) ;

            rc.bottom = 100 * c.m_red[i] / nMax ;
            FillRect(mr, rc, br) ;
            FillRect(mrgb, rc, br) ;
        }

        for (int i=0 ; i < 4 ; i++)
        {
            FCEffectFlip   fc ;
            m_histogram[i].ApplyEffect(fc) ;
        }
    }

    afx_msg void OnChangeChannel() ;
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(DlgEffectColorLevel, DlgEffectBase)
    ON_CBN_SELCHANGE(IDC_CHANNEL_COMBO, OnChangeChannel)
END_MESSAGE_MAP()

void DlgEffectColorLevel::OnChangeChannel()
{
    UpdateData() ;

    m_input.Invalidate() ;
    ApplyEffect() ;
}
