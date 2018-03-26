#pragma once
#include "002Doc.h"
#include "interface_zoom.h"

//-------------------------------------------------------------------------------------
class CMy002View : public CScrollView,
                   public PCL_Interface_ZoomScale
{
public:
    // pt is coordinate on view
    void ShowMousePos (CPoint ptOnView) ;

    // coordinates on client of window --> on canvas
    void ViewToCanvas (CPoint& pt) ;

    CMy002Doc* GetDocument() {return (CMy002Doc*)m_pDocument;}

    // try to put ptCanvas at center of new view ratio.
    void SetViewRatio (int nNewRatio, CPoint ptCanvas) ;

private:
    void DrawAllLayer (FCObjImage& view_img, const FCObjImage& imgLayer, CRect canvas_on_img) ;
    void DrawCanvas (CDC* pDC) ;

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    // get canvas offset when image center in view
    CPoint GetCanvasOffset() ;
    CSize GetCanvasScaledSize() ;

private:
    virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) ;
    virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll) ;

    afx_msg void OnViewZoomInStep();
    afx_msg void OnViewZoomOutStep();
    afx_msg void OnViewActual();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC) {return FALSE;}
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

    DECLARE_DYNCREATE(CMy002View) // create from serialization only
};
