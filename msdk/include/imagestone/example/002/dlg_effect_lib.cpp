#include "stdafx.h"
#include "002.h"
#include "dlg_effect_lib.h"

//-------------------------------------------------------------------------------------
void DlgEffectLib::CEffectThumb::SetImage (HBITMAP hBmp)
{
    BITMAP   bm = {0} ;
    if (GetObject (hBmp, sizeof(bm), &bm))
    {
        m_img.Attach (CreateCompatibleBitmap (CClientDC(NULL), bm.bmWidth, bm.bmHeight)) ;
        FCImageDrawDC::DrawBitmap (FCImageDrawDC(m_img), CRect(0, 0, bm.bmWidth, bm.bmHeight), hBmp) ;
    }
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::CEffectThumb::DrawTitle (CDC& dc, CRect rcItemOnDC)
{
    dc.FillSolidRect (rcItemOnDC, ::GetSysColor(COLOR_3DFACE)) ;
    dc.FillSolidRect (CRect(rcItemOnDC.TopLeft(), CSize(rcItemOnDC.Width(),1)), RGB(160,160,160)) ;
    dc.DrawText (m_text, rcItemOnDC, DT_SINGLELINE|DT_VCENTER|DT_CENTER) ;
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::CEffectThumb::DrawThumb (CDC& dc, CRect rcItemOnDC)
{
    if (IsMouseHovering())
    {
        dc.FillSolidRect (rcItemOnDC, RGB(220,241,253)) ;
        FrameRect (dc, rcItemOnDC, CBrush(RGB(51,153,255))) ;
    }

    CRect   rcTxt = rcItemOnDC ;
    rcTxt.top = rcTxt.bottom - 20 ;

    CRect   rcImg = rcItemOnDC ;
    rcImg.bottom = rcTxt.top ;
    rcImg.DeflateRect(8,8,8,0) ;

    if (m_img.GetSafeHandle())
    {
        FCImageDrawDC::DrawBitmap (dc, rcImg, m_img) ;
    }
    dc.DrawText (m_text, rcTxt, DT_SINGLELINE|DT_VCENTER|DT_CENTER) ;
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::CEffectThumb::OnPaint_Item (CDC& dc)
{
    CRect   rcItemOnDC (CPoint(0,0), GetItemSize()) ;
    if (m_command_id)
    {
        DrawThumb(dc, rcItemOnDC) ;
    }
    else
    {
        DrawTitle(dc, rcItemOnDC) ;
    }
}


//-------------------------------------------------------------------------------------
void DlgEffectLib::CThumbLayout::Layout_ListItem (CWnd* pWnd, std::deque<FCListItem_Base*>& item_list)
{
    CRect   rcClient ;
    pWnd->GetClientRect(rcClient) ;

    CSize   item_size ;
    item_size.cx = rcClient.Width() / 2 ;
    item_size.cy = 100 ;

    int    nCurrY = 0 ;
    BOOL   nLineIndex = 0 ;
    CRect  rcBound (0,0,0,0) ;

    for (size_t i=0 ; i < item_list.size() ; i++)
    {
        CEffectThumb   * pItem = (CEffectThumb*)item_list[i] ;

        CRect   rc ;
        BOOL    bNewLine = FALSE ;
        if (pItem->m_command_id)
        {
            rc = CRect(CPoint(nLineIndex * item_size.cx,nCurrY), item_size) ;

            nLineIndex++ ;

            if (nLineIndex == 2)
                bNewLine = TRUE ;

            if (i < item_list.size()-1)
            {
                if (!((CEffectThumb*)item_list[i+1])->m_command_id)
                    bNewLine = TRUE ;
            }
        }
        else
        {
            // title item
            rc = CRect(CPoint(0,nCurrY), CSize(rcClient.Width(),26)) ;
            bNewLine = TRUE ;
        }

        if (bNewLine)
        {
            nCurrY += rc.Height() ;
            nLineIndex = 0 ;
        }

        pItem->SetRectOnCanvas (rc) ;
        rcBound.UnionRect (rcBound, rc) ;
    }
    SetVScrollRange (pWnd, rcBound.Height()) ;
}


//-------------------------------------------------------------------------------------
DlgEffectLib::CEffectList::CEffectList()
{
    m_thread = NULL ;

    SetLayoutHandler(new CThumbLayout()) ;

    AddThumb (ID_EFFECT_COLORTONE, NULL) ;
    AddThumb (ID_EFFECT_GRAYSCALE, NULL) ;
    AddThumb (ID_EFFECT_SOFTGLOW, NULL) ;
    AddThumb (ID_EFFECT_SOFT_PORTRAIT, NULL) ;
    AddThumb (ID_EFFECT_MOSAIC, NULL) ;
    AddThumb (ID_EFFECT_INVERT, NULL) ;
    AddThumb (ID_EFFECT_SMOOTHEDGE, NULL) ;
    AddThumb (ID_EFFECT_BLURMOTION, NULL) ;
    AddThumb (ID_EFFECT_BLURRADIAL, NULL) ;
    AddThumb (ID_EFFECT_BLURZOOM, NULL) ;
    AddThumb (0, GetPureMenuText(L"s_3_0")) ;
    AddThumb (ID_EFFECT_NOISIFY, NULL) ;
    AddThumb (ID_EFFECT_OILPAINT, NULL) ;
    AddThumb (ID_EFFECT_EMBOSS, NULL) ;
    AddThumb (ID_EFFECT_SPLASH, NULL) ;
    AddThumb (ID_EFFECT_POSTERIZE, NULL) ;
    AddThumb (ID_EFFECT_TEXTURE, NULL) ;
    AddThumb (ID_EFFECT_3DGRID, NULL) ;
    AddThumb (ID_EFFECT_PENCILSKETCH, NULL) ;
    AddThumb (0, GetPureMenuText(L"s_3_1")) ;
    AddThumb (ID_EFFECT_SHIFT, NULL) ;
    AddThumb (ID_EFFECT_RIBBON, NULL) ;
    AddThumb (ID_EFFECT_BULGE, NULL) ;
    AddThumb (ID_EFFECT_TWIST, NULL) ;
    AddThumb (ID_EFFECT_ILLUSION, NULL) ;
    AddThumb (ID_EFFECT_WAVE, NULL) ;
    AddThumb (ID_EFFECT_RIPPLE, NULL) ;
    AddThumb (0, GetPureMenuText(L"s_3_2")) ;
    AddThumb (ID_EFFECT_LENSFLARE, NULL) ;
    AddThumb (ID_EFFECT_SUPERNOVA, NULL) ;
    AddThumb (ID_EFFECT_GLASSTILE, NULL) ;
    AddThumb (ID_EFFECT_BLINDS, NULL) ;
    AddThumb (ID_EFFECT_RAISEFRAME, NULL) ;
    AddThumb (0, GetPureMenuText(L"s_3_3")) ;
    AddThumb (ID_EFFECT_HALFTONE, NULL) ;
    AddThumb (ID_EFFECT_THRESHOLD, NULL) ;
    AddThumb (ID_EFFECT_SOLARIZE, NULL) ;
    AddThumb (ID_EFFECT_OLDPHOTO, NULL) ;
    AddThumb (0, GetPureMenuText(L"s_3_4")) ;
    AddThumb (ID_EFFECT_FILLGRID, NULL) ;
    AddThumb (ID_EFFECT_VIDEO, NULL) ;
}
//-------------------------------------------------------------------------------------
DlgEffectLib::CEffectList::~CEffectList()
{
    // first destroy window to cancel SendMessage, wait thread finish
    DestroyWindow() ;

    if (m_thread)
    {
        WaitForSingleObject (m_thread, INFINITE) ;
        CloseHandle(m_thread) ;
    }
}
//-------------------------------------------------------------------------------------
CString DlgEffectLib::CEffectList::GetPureMenuText (LPCWSTR sName)
{
    CString   s = theApp.GetText(L"MENU_TEXT", sName) ;

    int   n = s.Find(L"(&") ;
    if (n != -1)
    {
        s = s.Left(n) ;
    }
    s.Replace (L"&", L"") ;
    return s ;
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::CEffectList::AddThumb (int nCommand, LPCWSTR sText)
{
    CString   s ;
    if (nCommand)
    {
        s = theApp.GetMenuPureText(nCommand) ;
    }
    else
    {
        s = (sText ? sText : L"") ;
    }
    AddListItem (new CEffectThumb(nCommand, s)) ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectLib::CEffectList::WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT)
    {
        // create sample when paint
        if (!m_thread)
        {
            THREAD_PARAM   * p = new THREAD_PARAM ;
            p->m_wnd = m_hWnd ;
            for (int i=0 ; i < GetListItemCount() ; i++)
            {
                p->m_cmd.push_back( ((CEffectThumb*)GetListItem(i))->m_command_id ) ;
            }
            m_thread = CreateThread (NULL, 0, MakeThumbThread, p, 0, NULL) ;
            if (!m_thread)
            {
                ASSERT(FALSE) ;
                delete p ;
            }
        }
    }
    if (msg == WM_OXO_EFFECT_THUMB_FINISH)
    {
        for (int i=0 ; i < GetListItemCount() ; i++)
        {
            CEffectThumb   * pItem = (CEffectThumb*)GetListItem(i) ;
            if (pItem->m_command_id == (int)wParam)
            {
                pItem->SetImage((HBITMAP)lParam) ;
                break;
            }
        }
        Invalidate() ;
        return 0x11228899 ;
    }
    if (msg == WM_LBUTTONDOWN)
    {
        CEffectThumb   * pItem = (CEffectThumb*)HitTestListItem(lParam) ;
        if (pItem && pItem->m_command_id)
        {
            AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(pItem->m_command_id,0)) ;
        }
    }
    return FCListWindow::WindowProc(msg, wParam, lParam) ;
}
//-------------------------------------------------------------------------------------
FCImageEffect* DlgEffectLib::CEffectList::CreateEffect (int nCommandID)
{
    switch (nCommandID)
    {
        case ID_EFFECT_COLORTONE : return new FCEffectColorTone(FCColor(255,187,0),160) ;
        case ID_EFFECT_GRAYSCALE : return new FCEffectGrayscale ;
        case ID_EFFECT_SOFTGLOW : return new FCEffectSoftGlow(10,10,10) ;
        case ID_EFFECT_SOFT_PORTRAIT : return new FCEffectSoftPortrait(5,0,20) ;
        case ID_EFFECT_MOSAIC : return new FCEffectMosaic(6) ;
        case ID_EFFECT_INVERT : return new FCEffectInvert ;
        case ID_EFFECT_SMOOTHEDGE : return new FCEffectSmoothEdge(15) ;
        case ID_EFFECT_BLURMOTION : return new FCEffectBlur_Motion(20,0) ;
        case ID_EFFECT_BLURRADIAL : return new FCEffectBlur_Radial(10) ;
        case ID_EFFECT_BLURZOOM : return new FCEffectBlur_Zoom(100) ;

        case ID_EFFECT_NOISIFY : return new FCEffectNoisify(50,false) ;
        case ID_EFFECT_OILPAINT : return new FCEffectOilPaint(3,30) ;
        case ID_EFFECT_EMBOSS : return new FCEffectEmboss(0) ;
        case ID_EFFECT_SPLASH : return new FCEffectSplash(10) ;
        case ID_EFFECT_POSTERIZE : return new FCEffectPosterize(2) ;
        case ID_EFFECT_TEXTURE :
            {
                FCObjImage   * pImg = new FCObjImage ;
                pImg->LoadResource (IDR_PNG_TEXTURE_CANVAS_2, L"PNG", IMG_PNG) ;
                pImg->ConvertTo24Bit() ;
                return new FCEffectFillPattern(pImg, 0xFF, true) ;
            }
        case ID_EFFECT_3DGRID : return new FCEffect3DGrid(16,128) ;
        case ID_EFFECT_PENCILSKETCH : return new FCEffectPencilSketch(2,7) ;

        case ID_EFFECT_SHIFT : return new FCEffectShift(10) ;
        case ID_EFFECT_RIBBON : return new FCEffectRibbon(30,30) ;
        case ID_EFFECT_BULGE : return new FCEffectBulge(100) ;
        case ID_EFFECT_TWIST : return new FCEffectTwist(20,200) ;
        case ID_EFFECT_ILLUSION : return new FCEffectIllusion(3) ;
        case ID_EFFECT_WAVE : return new FCEffectWave(6,6) ;
        case ID_EFFECT_RIPPLE : return new FCEffectRipple(38,15,true) ;

        case ID_EFFECT_LENSFLARE : return new FCEffectLensFlare(CPoint(50,30)) ;
        case ID_EFFECT_SUPERNOVA : return new FCEffectSupernova(CPoint(50,30),FCColor(255,255,60),10,100) ;
        case ID_EFFECT_GLASSTILE : return new FCEffectTileReflection(20,10) ;
        case ID_EFFECT_BLINDS : return new FCEffectBlinds(FCEffectBlinds::BLIND_X,10,100,FCColor(255,255,255)) ;
        case ID_EFFECT_RAISEFRAME : return new FCEffectRaiseFrame(10) ;

        case ID_EFFECT_HALFTONE : return new FCEffectHalftoneM3 ;
        case ID_EFFECT_THRESHOLD : return new FCEffectThreshold(128) ;
        case ID_EFFECT_SOLARIZE : return new FCEffectSolarize(128) ;
        case ID_EFFECT_OLDPHOTO : return new FCEffectOldPhoto(2) ;

        case ID_EFFECT_FILLGRID : return new FCEffectFillGrid(FCColor(0,128,255),FCColor(255,255,255),16,128) ;
        case ID_EFFECT_VIDEO : return new FCEffectVideo(FCEffectVideo::VIDEO_TRIPED) ;
    }
    return NULL ;
}
//-------------------------------------------------------------------------------------
DWORD WINAPI DlgEffectLib::CEffectList::MakeThumbThread (LPVOID lpParameter)
{
    FCObjImage   demo_img ;
    demo_img.LoadResource (IDR_PNG_EXAMPLE, L"PNG", IMG_PNG) ;
    demo_img.ConvertTo32Bit() ;

    std::auto_ptr<THREAD_PARAM>   r ((THREAD_PARAM*)lpParameter) ;

    for (size_t i=0 ; i < r->m_cmd.size() ; i++)
    {
        int   nCommandID = r->m_cmd[i] ;

        std::auto_ptr<FCImageEffect>   eff (CreateEffect(nCommandID)) ;
        if (!eff.get())
            continue ;

        FCObjImage   img = demo_img ;
        img.ApplyEffect (*eff) ;
        img.ApplyEffect (FCEffectFillBackGround(FCColor(0xFF,0xFF,0xFF,0xFF))) ;

        LRESULT   l = ::SendMessage (r->m_wnd, WM_OXO_EFFECT_THUMB_FINISH, (WPARAM)nCommandID, (LPARAM)(HBITMAP)img) ;
        if (l != 0x11228899)
            break;
    }
    return 0 ;
}


//-------------------------------------------------------------------------------------
void DlgEffectLib::Create (CMDIFrameWnd* pMainFrame)
{
    CDialogBar::Create (pMainFrame, IDD_EFFECT_LIB, CBRS_LEFT|CBRS_RIGHT|CBRS_SIZE_DYNAMIC, ID_EFFECT_LIB_VISIBLE) ;
    m_sizeDefault.cx = 200 ;
    m_sizeDefault.cy = GetSystemMetrics(SM_CYFULLSCREEN) - 200 ;

    SetWindowText(L"Effect Lib") ;

    pMainFrame->ShowControlBar (this, FALSE, FALSE) ; // hide at first

    EnableDocking (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT) ;
    pMainFrame->DockControlBar (this, AFX_IDW_DOCKBAR_RIGHT) ;
}
//-------------------------------------------------------------------------------------
CSize DlgEffectLib::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    if (dwMode & LM_VERTDOCK)
    {
        CSize   sz = m_sizeDefault ;
        sz.cy = GetSystemMetrics(SM_CYFULLSCREEN) ;
        return sz ;
    }
    return CDialogBar::CalcDynamicLayout (nLength, dwMode) ;
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::ShowList()
{
    if (!m_list.get())
    {
        m_list.reset(new CEffectList) ;

        CRect   rc (CPoint(0,0), m_sizeDefault) ;
        rc.DeflateRect(4,5,3,10) ;
        m_list->Create(rc, this, 0, WS_VISIBLE) ;
    }
}
//-------------------------------------------------------------------------------------
void DlgEffectLib::HideList()
{
    m_list.reset() ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectLib::WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ((msg == WM_WINDOWPOSCHANGED) && lParam)
    {
        WINDOWPOS   * p = (WINDOWPOS*)lParam ;
        if (p->flags & SWP_SHOWWINDOW)
        {
            ShowList() ;
        }
        if (p->flags & SWP_HIDEWINDOW)
        {
            HideList() ;
        }
    }
    return CDialogBar::WindowProc(msg, wParam, lParam) ;
}
