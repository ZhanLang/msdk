#include "stdafx.h"
#include "002.h"
#include "002View.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMy002View, CScrollView)

BEGIN_MESSAGE_MAP(CMy002View, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()

    ON_COMMAND(ID_VIEW_ZOOMIN_STEP, OnViewZoomInStep)
    ON_COMMAND(ID_VIEW_ZOOMOUT_STEP, OnViewZoomOutStep)
    ON_COMMAND(ID_VIEW_ACTUAL, OnViewActual)

	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
void CMy002View::DrawAllLayer (FCObjImage& view_img, const FCObjImage& imgLayer, CRect canvas_on_img)
{
    CRect   rc (0, 0, view_img.Width(), view_img.Height()) ;
    rc.IntersectRect (rc, canvas_on_img) ;

    std::vector<int>   xt (view_img.Width()),
                       yt (view_img.Height()) ;

    for (int x=0 ; x < view_img.Width() ; x++)
    {
        CPoint   pt(x,x) ;
        ViewToCanvas(pt) ;
        xt[x] = pt.x ;
    }

    for (int y=0 ; y < view_img.Height() ; y++)
    {
        CPoint   pt(y,y) ;
        ViewToCanvas(pt) ;
        yt[y] = pt.y ;
    }

    for (int y=rc.top ; y < rc.bottom ; y++)
    {
        int   sy = yt[y] ;
        for (int x=rc.left ; x < rc.right ; x++)
        {
            int   sx = xt[x] ;

            if (imgLayer.IsInside (sx,sy))
            {
                RGBQUAD   cr = *(RGBQUAD*)imgLayer.GetBits (sx,sy) ;
                FCColor::AlphaBlendPixel (view_img.GetBits(x,y), cr) ;
            }
        }
    }
}
//-------------------------------------------------------------------------------------
void CMy002View::DrawCanvas (CDC* pDC)
{
    CRect   rcView ;
    GetClientRect(rcView) ;
    if (rcView.IsRectEmpty())
        return ;

    const CPoint   canvas_offset = GetCanvasOffset() ;

    FCObjImage   img ;
    img.Create (rcView.Width(), rcView.Height(), 32) ;

    // get canvas on image rect
    CRect   canvas_on_img (CPoint(0,0), GetCanvasScaledSize()) ;
    canvas_on_img.OffsetRect ((GetStyle() & WS_HSCROLL) ? -GetScrollPos(SB_HORZ) : canvas_offset.x, 0) ;
    canvas_on_img.OffsetRect (0, (GetStyle() & WS_VSCROLL) ? -GetScrollPos(SB_VERT) : canvas_offset.y) ;

    {
        FCImageDrawDC   memDC (img) ;

        // fill view back
        FillRect (memDC, CRect(0,0,img.Width(),img.Height()), CBrush(RGB(0xC0,0xC0,0xC0))) ;

        // draw back and border of canvas
        {
            CPoint   ptBrOri = canvas_on_img.TopLeft() ;
            SetBrushOrgEx (memDC, ptBrOri.x, ptBrOri.y, NULL) ;

            HBRUSH   br = CreatePatternBrush(theApp.m_canvas_back) ;
            FillRect (memDC, canvas_on_img, br) ;
            DeleteObject(br) ;

            CRect   rc = canvas_on_img ;
            rc.InflateRect(1,1,1,1) ;
            FrameRect (memDC, rc, CBrush(RGB(0,0,0))) ;
        }

        // draw layers
        DrawAllLayer (img, GetDocument()->m_img, canvas_on_img) ;

        // draw to view
        CPoint   ptOldOri = pDC->SetViewportOrg(0,0) ;
        ::BitBlt (*pDC, 0, 0, img.Width(), img.Height(), memDC, 0, 0, SRCCOPY) ;
        pDC->SetViewportOrg(ptOldOri) ;
    } // <-- select out bitmap
}
//-------------------------------------------------------------------------------------
void CMy002View::OnDraw(CDC* pDC)
{
	CMy002Doc   * pDoc = GetDocument(); ASSERT_VALID(pDoc);

    // printer DC
    if (pDC->IsPrinting())
    {
        const FCObjImage   & img = pDoc->m_img ;

        // get size of printer page (in pixels)
        CRect    rcPage (0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES)) ;
        rcPage.DeflateRect (rcPage.Width()/10, rcPage.Height()/10, rcPage.Width()/10, rcPage.Height()/10) ;

        double   fScaleX = rcPage.Width() / (double)img.Width(),
                 fScaleY = rcPage.Height() / (double)img.Height() ;
        double   fScale = __min (fScaleX, fScaleY) ;
        CRect    rcImg (0, 0, 0, 0) ;
        rcImg.right = __max (1, (int)(img.Width() * fScale)) ;
        rcImg.bottom = __max (1, (int)(img.Height() * fScale)) ;
        rcImg.OffsetRect (rcPage.left, (pDC->GetDeviceCaps(VERTRES)-rcImg.Height())/2) ;

        int   nOldMode = pDC->SetStretchBltMode(COLORONCOLOR) ;
        StretchBlt (*pDC, rcImg.left, rcImg.top, rcImg.Width(), rcImg.Height(), FCImageDrawDC(img), 0, 0, img.Width(), img.Height(), SRCCOPY) ;
        pDC->SetStretchBltMode(nOldMode) ;
    }
    else
    {
        DrawCanvas(pDC) ;
    }
}
//-------------------------------------------------------------------------------------
BOOL CMy002View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy002View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy002View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
//-------------------------------------------------------------------------------------
void CMy002View::ShowMousePos(CPoint ptOnView)
{
    CMy002Doc   * pDoc = GetDocument(); ASSERT_VALID(pDoc);

    CRect   rc (CPoint(0,0), CSize(pDoc->m_img.Width(), pDoc->m_img.Height())) ;
    ViewToCanvas (ptOnView) ;
    if (!rc.PtInRect(ptOnView))
    {
        ptOnView = CPoint(-1,-1) ;
    }
    CWndBottomStatus::SetPosition(ptOnView) ;
}
//-------------------------------------------------------------------------------------
void CMy002View::OnMouseMove(UINT nFlags, CPoint point)
{
    // update mouse position
	ShowMousePos (point) ;
}
//-------------------------------------------------------------------------------------
void CMy002View::ViewToCanvas (CPoint& pt)
{
    CMy002Doc   * pDoc = GetDocument() ;
    if (pDoc)
    {
        CPoint   ptOffset = GetCanvasOffset() ;

        pt.x += ((GetStyle() & WS_HSCROLL) ? GetScrollPos(SB_HORZ) : -ptOffset.x) ;
        pt.y += ((GetStyle() & WS_VSCROLL) ? GetScrollPos(SB_VERT) : -ptOffset.y) ;

        Scaled_to_Actual (pt) ;
    }
}
//-------------------------------------------------------------------------------------
void CMy002View::OnInitialUpdate()
{
    CFrameWnd     * pFrame = GetParentFrame() ;
    CMDIFrameWnd  * pMainWnd = (CMDIFrameWnd*)AfxGetMainWnd() ;
    if (pFrame && pMainWnd && GetDocument())
    {
        // get max region of MDI client
        CRect   rcMDI ;
        ::GetClientRect (pMainWnd->m_hWndMDIClient, rcMDI) ;

        // test fit scale ratio
        int   n = -15 ;
        for ( ; n <= -1 ; n++)
		{
            SetZoomScale(n) ;
            CSize   sizeScaled = GetCanvasScaledSize() ;

			if ((sizeScaled.cx > rcMDI.Width()) || (sizeScaled.cy > rcMDI.Height()))
				break ;
		}
        n-- ;

        SetZoomScale (n) ;
        CWndBottomStatus::SetZoom (n) ;

        // calculate parent frame size
        CRect   rcOut (CPoint(0,0), GetCanvasScaledSize()) ;
        ::AdjustWindowRectEx (rcOut, pFrame->GetStyle(), FALSE, WS_EX_CLIENTEDGE) ;
        pFrame->SetWindowPos (NULL, 0, 0, rcOut.Width(), rcOut.Height(), SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE) ;
	}
	CScrollView::OnInitialUpdate(); // default to call OnUpdate to set scroll size
}
//-------------------------------------------------------------------------------------
void CMy002View::OnViewZoomInStep()
{
    CRect   rc ;
    GetClientRect(rc) ;

    CPoint   pt = rc.CenterPoint() ;
    ViewToCanvas(pt) ;

    int   nScale = GetZoomScale() ;
    int   nOldScale = nScale ;

    if (nScale == -1)
        nScale = 1 ;
    ++nScale ;

    nScale = __max(-16, __min(16,nScale)) ;
    if (nScale != nOldScale)
    {
        SetViewRatio (nScale, pt) ;
    }
}
//-------------------------------------------------------------------------------------
void CMy002View::OnViewZoomOutStep()
{
    CRect   rc ;
    GetClientRect(rc) ;

    CPoint   pt = rc.CenterPoint() ;
    ViewToCanvas(pt) ;

    int   nScale = GetZoomScale() ;
    int   nOldScale = nScale ;

    if (nScale == 1)
        nScale = -1 ;
    --nScale ;

    nScale = __max(-16, __min(16,nScale)) ;
    if (nScale != nOldScale)
    {
        SetViewRatio (nScale, pt) ;
    }
}
//-------------------------------------------------------------------------------------
BOOL CMy002View::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
    // scroll pos > 32K
    SCROLLINFO   info = {0} ;
    info.cbSize = sizeof(info) ;

    if (LOBYTE(nScrollCode) == SB_THUMBTRACK)
    {
        GetScrollInfo (SB_HORZ, &info, SIF_TRACKPOS) ;
        nPos = info.nTrackPos ;
    }

    if (HIBYTE(nScrollCode) == SB_THUMBTRACK)
    {
        GetScrollInfo (SB_VERT, &info, SIF_TRACKPOS) ;
        nPos = info.nTrackPos ;
    }

    return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll) ;
}
//-------------------------------------------------------------------------------------
BOOL CMy002View::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
    int xOrig, x;
    int yOrig, y;

    // don't scroll if there is no valid scroll range (ie. no scroll bar)
    CScrollBar* pBar;
    DWORD dwStyle = GetStyle();
    pBar = GetScrollBarCtrl(SB_VERT);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
        (pBar == NULL && !(dwStyle & WS_VSCROLL)))
    {
        // vertical scroll bar not enabled
        sizeScroll.cy = 0;
    }
    pBar = GetScrollBarCtrl(SB_HORZ);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
        (pBar == NULL && !(dwStyle & WS_HSCROLL)))
    {
        // horizontal scroll bar not enabled
        sizeScroll.cx = 0;
    }

    // adjust current x position
    xOrig = x = GetScrollPos(SB_HORZ);
    int xMax = GetScrollLimit(SB_HORZ);
    x += sizeScroll.cx;
    if (x < 0)
        x = 0;
    else if (x > xMax)
        x = xMax;

    // adjust current y position
    yOrig = y = GetScrollPos(SB_VERT);
    int yMax = GetScrollLimit(SB_VERT);
    y += sizeScroll.cy;
    if (y < 0)
        y = 0;
    else if (y > yMax)
        y = yMax;

    // did anything change?
    if (x == xOrig && y == yOrig)
        return FALSE;

    if (bDoScroll)
    {
        if (x != xOrig)
            SetScrollPos(SB_HORZ, x);
        if (y != yOrig)
            SetScrollPos(SB_VERT, y);
    }
    Invalidate() ;
    return FALSE;
}
//-------------------------------------------------------------------------------------
void CMy002View::OnViewActual()
{
    CRect    rc ;
    GetClientRect(rc) ;
    CPoint   pt = rc.CenterPoint() ;
    ViewToCanvas (pt) ;

    SetViewRatio (1, pt) ;
}
//-------------------------------------------------------------------------------------
void CMy002View::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    // calculate the total size of this view
    SetScrollSizes (MM_TEXT, GetCanvasScaledSize()) ;
    CScrollView::OnUpdate (pSender, lHint, pHint) ;
}
//-------------------------------------------------------------------------------------
void CMy002View::SetViewRatio (int nNewRatio, CPoint ptCanvas)
{
    CMy002Doc   * pDoc = GetDocument() ;
    if (!pDoc)
        return ;

    // set new ratio and scroll size
    SetZoomScale (nNewRatio) ;
    CWndBottomStatus::SetZoom (nNewRatio) ;
    pDoc->UpdateAllViews(NULL) ; // <-- call SetScrollSize in OnUpdate

    // position on new view
    Actual_to_Scaled (ptCanvas) ;

    // try to put point ptCanvas at center
    int     nPos = 0 ;
    CRect   rcView ;
    GetClientRect(rcView) ;
    if (GetTotalSize().cx > rcView.Width())
    {
        int   nMax, nMin ;
        GetScrollRange (SB_HORZ, &nMin, &nMax) ;
        nPos = FClamp ((int)(ptCanvas.x - rcView.Width()/2), nMin, nMax) ;
    }
    SetScrollPos (SB_HORZ, nPos) ;

    nPos = 0 ;
    if (GetTotalSize().cy > rcView.Height())
    {
        int   nMax, nMin ;
        GetScrollRange (SB_VERT, &nMin, &nMax) ;
        nPos = FClamp ((int)(ptCanvas.y - rcView.Height()/2), nMin, nMax) ;
    }
    SetScrollPos (SB_VERT, nPos) ;
    pDoc->UpdateAllViews(NULL) ;
}
//-------------------------------------------------------------------------------------
CSize CMy002View::GetCanvasScaledSize()
{
    CPoint   sz (1,1) ;
    if (GetDocument())
    {
        const FCObjImage   & img = GetDocument()->m_img ;

        if (GetZoomScale() > 1)
        {
            sz.SetPoint (img.Width(), img.Height()) ;
            Actual_to_Scaled(sz) ;
        }
        else
        {
            sz = CSize(img.Width(), img.Height()) - CSize(1,1) ;
            Actual_to_Scaled(sz) ;
            sz.x++ ;
            sz.y++ ;
        }
    }
    return sz ;
}
//-------------------------------------------------------------------------------------
// get canvas offset when image center in view
CPoint CMy002View::GetCanvasOffset()
{
    CRect   rc ;
    GetClientRect(rc) ;

    CPoint   pt (0,0) ;
    CSize    sizeScaled = GetCanvasScaledSize() ;

    if (!(GetStyle() & WS_HSCROLL))
        pt.x = (rc.Width() - sizeScaled.cx) / 2 ;
    if (!(GetStyle() & WS_VSCROLL))
        pt.y = (rc.Height() - sizeScaled.cy) / 2 ;
    return pt ;
}
