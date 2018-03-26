#include "stdafx.h"
#include "dsUIBase.h"

#include "dsUIClassMgr.h"
#include "dsGifAnimateMgr.h"


dsUIBase::dsUIBase() :
m_wID(0)
,m_nState(DSUI_STATE_NORMAL)
,m_bVisible(TRUE)
,m_hwnd(NULL)
,m_pbaseParent(NULL)
,m_procLayout(NULL)
,m_bAutoGrow(FALSE)
,m_lParam(0)
,m_wSysCursorID(32512)
{
	
}

dsUIBase::~dsUIBase()
{
	FreeGifAnimate();
	DeleteAllUIElement();
}
BOOL dsUIBase::GetClassInfo( __out PDSUICLASSINFO pci )
{
	if(NULL == pci)
		return FALSE;

	pci->procNewUI = NewDsuiElesInst;
	pci->strClassName = L"dsBase";

	return TRUE;
}

void dsUIBase::SetRect( LPRECT lprc )
{
	m_rcUI.CopyRect(lprc);
}

int dsUIBase::SetState( int nState )
{
	if(DSUI_STATE_DISABLED & m_nState)
		return -1;

	int nOldState = m_nState;
	m_nState = nState;

	if(nOldState != m_nState)
		RedrawUI();

	return nOldState;
}

BOOL dsUIBase::EnableUI( BOOL bEnabled )
{

	BOOL bOldEnabled =  !(DSUI_STATE_DISABLED & m_nState);
	if(!bEnabled)
	{
		m_nState |= DSUI_STATE_DISABLED;
		m_nState = m_nState & ~(DSUI_STATE_HOT | DSUI_STATE_PUSHED);

		dsGlobalStateMgr::getInstance()->CheckDisabledUI(this);
	}
	else
	{
		m_nState = m_nState & ~DSUI_STATE_DISABLED;
		if(DSUI_STATE_NULL == m_nState)
			m_nState = DSUI_STATE_NORMAL;
	}

	if(bOldEnabled != bEnabled)
		RedrawUI();

	return bOldEnabled;
}

void dsUIBase::SetVisible( BOOL bVisible )
{
	if(m_bVisible != bVisible)
		RedrawUI();
	m_bVisible = bVisible;
}

void dsUIBase::SetID( WORD wID )
{
	m_wID = wID;
}

IdsPaintMission * dsUIBase::GetPaintMisson( LPARAM lParamForFind )
{
	if (0 == lParamForFind)
		return NULL;

	LIST_DSPM::iterator it = m_listDSPM.begin();
	while (it != m_listDSPM.end())
	{
		if ((*it).m_lParam == lParamForFind)
		{
			return &(*it);
			break;
		}
		it ++;
	}

	return NULL;
}

BOOL dsUIBase::AddPaintMisson( IdsPaintMission *pdspm )
{
	if(NULL == pdspm)
		return FALSE;

	dsPaintMission pm = * ((dsPaintMission*)pdspm);

	//if(0 != ((dsPaintMission*)pdspm)->m_lParam && NULL != GetPaintMisson(((dsPaintMission*)pdspm)->m_lParam))
	//	return FALSE;

	if(NULL != GetPaintMisson(pm.m_lParam))
		return FALSE;

	m_listDSPM.push_back(pm);

	return TRUE;
}

int dsUIBase::GetState()
{
	return m_nState;
}

BOOL dsUIBase::CopyPaint( IdsUIBase *psrc )
{
	if(NULL == psrc)
		return FALSE;

	m_listDSPM = ((dsUIBase*)psrc)->m_listDSPM;
	return TRUE;
}

BOOL dsUIBase::ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//switch (uMsg)
	//{
	//case WM_MOUSEMOVE:



	//	break;

	//case WM_LBUTTONDOWN:

	//	break;

	//case WM_LBUTTONUP:

	//	break;

	//case WM_MOUSELEAVE:

	//	break;

	//default:

	//	break;
	//}


	return FALSE;
}

void dsUIBase::DoPaint( CDCHandle dc, LPPAINTSTRUCT lpps )
{
	LIST_DSPM::iterator it = m_listDSPM.begin();

	while (it != m_listDSPM.end()){

		dsPaintMission *pdspm = &(*it);

		if(pdspm->m_bEnabled && (m_nState & pdspm->GetState()) ){
			//draw 
			DealPaintMisson(pdspm, dc, lpps);

		}

		it ++;
	}
}

BOOL dsUIBase::DrawImage( CDCHandle dc, __in Graphics *pgrah, __in Image *pimg, __in LPRECT lprcDst, __in LPRECT lprcSrc, __in LPRECT lprcPaint, int nPaintMode, BOOL bDrawAsIcon/* = FALSE*/ )
{
	if(NULL == pgrah || NULL == pimg || NULL == lprcDst || NULL == lprcSrc || NULL == lprcPaint)
		return FALSE;

	CRect rcDst(lprcDst),rcSrc(lprcSrc),rcPaint(lprcPaint);


	if(!rcPaint.IntersectRect(lprcDst, lprcPaint))
		return TRUE;


	float xRadio = (float)rcDst.Width()/(float)rcSrc.Width();
	float yRadio = (float)rcDst.Height()/(float)rcSrc.Height();

	int cxDst = rcDst.Width();
	int cyDst = rcDst.Height();
	int cxSrc = rcSrc.Width();
	int cySrc = rcSrc.Height();



	int nXOffset = rcPaint.left - rcDst.left;
	int nYOffset = rcPaint.top - rcDst.top;

	rcDst.CopyRect(rcPaint);

	if(DSUI_PAINTMODE_NORMAL == nPaintMode)
	{
		rcSrc.OffsetRect(nXOffset, nYOffset);
		
		if(rcSrc.Width() > rcDst.Width())
			rcSrc.right = rcSrc.left + rcDst.Width();
		else
			rcDst.right = rcDst.left + rcSrc.Width();

		if(rcSrc.Height() > rcDst.Height())
			rcSrc.bottom = rcSrc.top + rcDst.Height();
		else
			rcDst.bottom = rcDst.top + rcSrc.Height(); 
	}
	else if(0 != xRadio && 0 != yRadio /*&& !(1 == xRadio && 1 == yRadio)*/)
	{
		rcSrc.OffsetRect(dsTools::GetApproximation(nXOffset / xRadio), dsTools::GetApproximation(nYOffset / yRadio));
		//rcSrc.bottom = rcSrc.top + dsTools::GetApproximation(rcDst.Height() / yRadio);
		//rcSrc.right = rcSrc.left + dsTools::GetApproximation(rcDst.Width() / xRadio);

		int cxImage = pimg->GetWidth();
		int cyImage = pimg->GetHeight();

	
		rcSrc.right = rcSrc.left + dsTools::GetApproximation(rcDst.Width() / xRadio);
		rcSrc.right = min(cxImage, rcSrc.right);
		if(rcSrc.Width() <= 0)
			rcSrc.right = rcSrc.left + 1;

		rcSrc.bottom = rcSrc.top + dsTools::GetApproximation(rcDst.Height() / yRadio);
		rcSrc.bottom = min(cyImage, rcSrc.bottom);
		if(rcSrc.Height() <= 0)
			rcSrc.bottom = rcSrc.top + 1;

		if(1 == cxImage)
		{
			rcSrc.left = 0;
			rcSrc.right = 1;
		}

		if(1 == cyImage)
		{
			rcSrc.top = 0;
			rcSrc.bottom = 1;
		}

		if(rcSrc.Width() > cxImage)
		{
			rcSrc.left = 0;
			rcSrc.right = cxImage;
		}

		if(rcSrc.Height() > cyImage)
		{
			rcSrc.left = 0;
			rcSrc.right = cyImage;
		}

		if(rcSrc.left < 0)
		{
			rcSrc.OffsetRect(- rcSrc.left, 0);
		}

		if (rcSrc.right > cxImage)
		{
			rcSrc.OffsetRect(cxImage - rcSrc.right, 0);
		}


		if(rcSrc.top < 0)
		{
			rcSrc.OffsetRect(0, - rcSrc.top);
		}

		if (rcSrc.bottom > cyImage)
		{
			rcSrc.OffsetRect(0, cyImage - rcSrc.bottom);
		}

		//rcSrc.OffsetRect((nXOffset * cxSrc)/ cxDst, (nYOffset * cySrc)/ cyDst );
		//rcSrc.right = rcSrc.left + (rcDst.Width() * cxSrc)/ cxDst ;
		//rcSrc.bottom = rcSrc.top +(rcDst.Height() * cySrc)/ cyDst ;
	}
	else
	{
		rcSrc.right = rcSrc.left + rcDst.Width();
		rcSrc.bottom = rcSrc.top + rcDst.Height();
	}

	if(bDrawAsIcon){
		pgrah->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic); 
		pgrah->SetSmoothingMode(SmoothingModeAntiAlias);
	}
	else{
		pgrah->SetInterpolationMode(InterpolationModeNearestNeighbor);
		pgrah->SetPixelOffsetMode(PixelOffsetModeHalf);
	}

	pgrah->DrawImage(pimg, 
		dsTools::GdiplusRectFromGdiRect(rcDst),
		rcSrc.left,
		rcSrc.top,
		rcSrc.Width(),
		rcSrc.Height(),
		UnitPixel);

	return TRUE;
}

void dsUIBase::DealPaintMisson( dsPaintMission *pdspm, CDCHandle dc, LPPAINTSTRUCT lpps )
{
	if(NULL == pdspm || NULL == lpps)
		return;

	int nDrawCount = 0;

	CRect rcDst;
	pdspm->GetRealDstRect(m_rcUI, rcDst);

	CRect rcSrc;
	pdspm->GetRealSrcRect(rcSrc);

	CRect rcPaint(lpps->rcPaint);

	if(!rcPaint.IntersectRect(rcPaint, rcDst)){
		return;
	}

	//if(pdspm->m_bMarkAsGif)
	//{
	//	if(!pdspm->m_gif.IsAnimating())
	//	{
	//		if(pdspm->m_gif.StartAnimation(this, rcDst, rcSrc));
	//	}

	//	pdspm->m_gif.UpdateRect(rcDst, rcSrc);

	//	//return;
	//}


	BOOL bLWMarked = dsNoPaintMsgMgr::getInstance()->IsMarked(m_hwnd);

	if(2 == pdspm->m_lParam)
		int a = 0 ;
	
	Graphics grah(dc);

	Image *pimg = pdspm->GetImagePtr();
	if(NULL != pimg){

		dsGifAnimateMgr *pgam = dsGifAnimateMgr::getInstance();

		//if(0 != pdspm->m_dwOldGifKey){
		//	pgam->DestroyAnimate(pdspm->m_dwOldGifKey);
		//	pdspm->m_dwOldGifKey = 0;
		//}

		if(0 != pdspm->m_dwGifKey)
		{
			GUID   pageGuid = FrameDimensionTime;

			

			if(!pgam->CheckGifAnimate(pdspm->m_dwGifKey))
				pgam->StartAnimate(this, pdspm->m_dwGifKey, pimg);
			pimg->SelectActiveFrame(&pageGuid, pgam->GetImgFrmPos(pdspm->m_dwGifKey));
		}

		nDrawCount ++;

		NSST nsst;

		if(DSUI_PAINTMODE_STRETCH == pdspm->m_nPaintMode){

			if(ds9SquaresStretch::Analysis(rcDst, rcSrc, pdspm->m_rcSquares9, &nsst)){

				for (int i=0; i<9; i++)
				{
					DrawImage(dc, &grah, pimg, &nsst.tables[i].rcDst, &nsst.tables[i].rcSrc, rcPaint, nsst.tables[i].nPaintMode, pdspm->m_bMarkAsIcon);
				}
			}
			else{
				DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_STRETCH, pdspm->m_bMarkAsIcon);
			}

		}
		else{
			DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_NORMAL, pdspm->m_bMarkAsIcon);
		}

	}

	HICON hIcon = pdspm->GetIcon();
	if(NULL != hIcon){

		nDrawCount ++;

		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcPaint);
		::ExtSelectClipRgn(dc, rgn, RGN_AND);

		DrawIconEx(dc, rcDst.left, rcDst.top, hIcon, rcDst.Width(), rcDst.Height(), 0, NULL, DI_NORMAL);

		dc.SelectClipRgn(NULL);

	}

	CString &strText = pdspm->GetTextStrQuote();

	int nCountOfSuperText = pdspm->m_vecSuperText.size();

	BOOL bSuperText = nCountOfSuperText > 0;

	if(bSuperText)
	{
		strText = m_strText;

		CSize sizeOffset;

		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcPaint);

		if(bLWMarked)
			grah.SetClip(rgn);
		else
			::ExtSelectClipRgn(dc, rgn, RGN_AND);

		

		dc.SetBkMode(TRANSPARENT);

		for (int i=0; i<nCountOfSuperText; i++)
		{

			CRect rcDraw(rcDst);

			SUPERTEXT &st = pdspm->m_vecSuperText[i];

			strText = st.strText;

			if(strText.IsEmpty() && NULL == pimg)
				strText = m_strText;

			if(strText.GetLength() > 0){

				nDrawCount ++;

				HFONT hFontOld = dc.SelectFont(st.hFontText);
				COLORREF crOld = dc.SetTextColor(st.crText);

				rcDraw.OffsetRect(sizeOffset);

				//dc.DrawText(strText, strText.GetLength(), rcDraw, pdspm->m_uFormat);


				if(bLWMarked)
				{
					Color cr(254, GetRValue(st.crText), GetGValue(st.crText), GetBValue(st.crText));
					SolidBrush br(cr);

					RectF rct(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());

					StringFormat sf;

					if(DT_CENTER & pdspm->m_uFormat)
						sf.SetAlignment(StringAlignmentCenter);
					if(DT_VCENTER & pdspm->m_uFormat)
						sf.SetLineAlignment(StringAlignmentCenter);

					Font f(dc,st.hFontText);

					//grah.SetTextRenderingHint(TextRenderingHintAntiAlias);

					if(Ok != grah.DrawString(strText, strText.GetLength(), &f, rct, &sf, &br)){

						
					}

				}
				else
					dc.DrawText(strText, strText.GetLength(), rcDraw, pdspm->m_uFormat);

			
				CSize sizeTemp;
				dc.GetTextExtent(strText, strText.GetLength(), &sizeTemp);

				if('\n'  == strText.Find(strText.GetLength() - 1) ||
					('\n' == strText.Find(strText.GetLength() - 2) && '\r' == strText.Find(strText.GetLength() - 1)))
				{
					sizeOffset.cx = 0;
					sizeOffset.cy += sizeTemp.cy + pdspm->m_sizeSuperTextScale.cy;
				}
				else
				{
					sizeOffset.cx += sizeTemp.cx + pdspm->m_sizeSuperTextScale.cx;
				}

				dc.SelectFont(hFontOld);
				dc.SetTextColor(crOld);	
			}

		}

		grah.ResetClip();

		if(bLWMarked)
			grah.ResetClip();
		else
			dc.SelectClipRgn(NULL);

		
	}
	else
	{
		if(strText.IsEmpty() && NULL == pimg && pdspm->m_bUseUIText)
			strText = m_strText;

		if(strText.GetLength() > 0){

			nDrawCount ++;

			CRgn rgn;
			rgn.CreateRectRgnIndirect(rcPaint);

			if(bLWMarked)
				grah.SetClip(rgn);
			else
				::ExtSelectClipRgn(dc, rgn, RGN_AND);
			

			dc.SetBkMode(TRANSPARENT);

			HFONT hFontOld = dc.SelectFont(pdspm->m_hFont);
			COLORREF crOld = dc.SetTextColor(pdspm->m_crText);


			if(bLWMarked)
			{

				Color cr(254, GetRValue(pdspm->m_crText), GetGValue(pdspm->m_crText), GetBValue(pdspm->m_crText));
				SolidBrush br(cr);

				RectF rct(rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height());

				StringFormat sf;
				if(DT_CENTER & pdspm->m_uFormat)
					sf.SetAlignment(StringAlignmentCenter);
				if(DT_VCENTER & pdspm->m_uFormat)
					sf.SetLineAlignment(StringAlignmentCenter);

				Font f(dc, pdspm->m_hFont);


				//grah.SetTextRenderingHint(TextRenderingHintAntiAlias);
				grah.DrawString(strText, strText.GetLength(), &f, rct, &sf, &br);

			}
			else
				dc.DrawText(strText, strText.GetLength(), rcDst, pdspm->m_uFormat);


			
			
			dc.SelectFont(hFontOld);
			dc.SetTextColor(crOld);

			if(bLWMarked)
				grah.ResetClip();
			else
				dc.SelectClipRgn(NULL);
		}
	}


	


	if (0 == nDrawCount && NULL == pdspm->m_hFont)
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcPaint);
		::ExtSelectClipRgn(dc, rgn, RGN_AND);

		if(bLWMarked)
		{

			Graphics grah(dc);

			GraphicsState gs = grah.Save();

			Color cr(GetRValue(pdspm->m_crText), GetGValue(pdspm->m_crText), GetBValue(pdspm->m_crText));

			Pen pen(cr, 2 * rcPaint.Height());

			grah.DrawLine(&pen, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.top);
			//grah.DrawRectangle(&pen, rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height());
			grah.Restore(gs);
		}
		else
			dc.FillSolidRect(rcDst, pdspm->m_crText);

		dc.SelectClipRgn(NULL);
	}
}

void dsUIBase::SetLayoutProc( __in LAYOUTPROC procLayout )
{
	m_procLayout = procLayout;
}
BOOL dsUIBase::DoUpdateLayout( __in PDSLAYOUTINFO pdsli, int nIndex )
{
	if(NULL != m_procLayout){
		if (!m_procLayout(this, pdsli, nIndex))
			return FALSE;
	}
	else
		return dsTools::UpdateLayoutRect(pdsli->rcWnd, m_rcUISrc, m_rcCoordMark, m_rcUI);

	return TRUE;
}

dsUIBase* CALLBACK dsUIBase::NewDsuiElesInst()
{
	return new dsUIBase();
}

IdsUIBase * dsUIBase::CreateDsuiElement( __in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark /*= NULL*/, BOOL bUpdateLayout /*= FALSE*/ )
{
	if(IsUIElesExist(wUIID))
		return NULL;

	DSUICLASSINFO ci;
	if(!dsUIClassMgr::getInstance()->GetClassInfo(lpszClass, &ci))
		return NULL;

	dsUIBase *pbase = ci.procNewUI();

	if(NULL == pbase)
		return pbase;

	pbase->m_pbaseParent = this;
	pbase->m_rcUISrc.CopyRect(lprc);

	pbase->SetID(wUIID);
	pbase->m_strText = lpszUIName;
	pbase->m_hwnd = m_hwnd;
	pbase->m_rcCoordMark.CopyRect(lprcCoordMark);

	m_listUIBase.push_back(pbase);

	pbase->MoveUI(lprc, TRUE);



	if(0 != wUIID){
		MAP_UIBASE::_Pairib pib = m_mapUIBase.insert(make_pair(wUIID, pbase));

		if(!pib.second){
			m_listUIBase.pop_back();
			delete pbase;
			return  NULL;
		}
	}

	return pbase;
}

dsUIBase * dsUIBase::InsertDsuiElement( __in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, int nIndex /*= -1*/, LPARAM lParam /*= 0*/ )
{
	int nSize = m_listUIBase.size();

	if (-1 != nIndex)
	{
		if (nIndex > nSize || nIndex < 0)
			return NULL;
	}

	DSUICLASSINFO ci;
	if(!dsUIClassMgr::getInstance()->GetClassInfo(lpszClass, &ci))
		return NULL;

	dsUIBase *pbase = ci.procNewUI();

	if(NULL == pbase)
		return pbase;

	pbase->m_pbaseParent = this;
	pbase->m_strText = lpszUIName;
	pbase->m_hwnd = m_hwnd;
	pbase->m_lParam = lParam;

	if (-1 == nIndex || nSize == nIndex){
		m_listUIBase.push_back(pbase);
	}
	else
	{
		LIST_UIBASE::iterator it = m_listUIBase.begin();

		for (int i=0; i<nSize; i++)
		{
			it ++;
		}
			
		m_listUIBase.insert(it, pbase);
	}

	return pbase;
}

IdsUIBase * dsUIBase::GetDsuiElement( WORD wUIID )
{
	if(0 == wUIID)
		return NULL;

	MAP_UIBASE::iterator it = m_mapUIBase.find(wUIID);

	if(it != m_mapUIBase.end())
		return it->second;

	return NULL;
}

void dsUIBase::DeleteAllUIElement()
{
	LIST_UIBASE::iterator it = m_listUIBase.begin();

	while(it != m_listUIBase.end())
	{
		dsGlobalStateMgr::getInstance()->CheckDeletedUIPtr(*it);

		delete *it;
		*it = NULL;
		it ++;
	}

	m_listUIBase.clear();
	m_mapUIBase.clear();
}

BOOL dsUIBase::IsUIElesExist( int wUIID )
{
	return (NULL != GetDsuiElement(wUIID));
}


dsUIBase *dsUIBase::EnumUIElements(ENUMUIPROC procEnumUI, LPARAM lParam, BOOL bReverse /*= FALSE*/, int nFirst, int nCount)
{

	int nIndex = 0;

	if(!bReverse)
	{
		LIST_UIBASE::iterator it = m_listUIBase.begin();

		while(it != m_listUIBase.end())
		{
			if(!procEnumUI(*it, lParam, nIndex))
				return *it;

			it ++; nIndex++;
		}
	}
	else
	{
		LIST_UIBASE::reverse_iterator it = m_listUIBase.rbegin();

		while(it != m_listUIBase.rend())
		{
			if(!procEnumUI(*it, lParam, nIndex))
				return *it;

			it ++; nIndex++;
		}
	}


	return NULL;
}

IdsUIBase * dsUIBase::SetMouseWheelCapture()
{
	return dsGlobalStateMgr::getInstance()->SetMouseWheelCaptureUIPtr(this);
}

void dsUIBase::ReleaseMouseWheelCapture()
{
	dsGlobalStateMgr::getInstance()->ReleaseMouseWheelCaptureUIPtr();
}

IdsUIBase * dsUIBase::GetParent()
{
	return m_pbaseParent;
}

BOOL dsUIBase::MoveUI( __in LPRECT lprcDst, BOOL bRedraw )
{
	if(NULL == lprcDst)
		return FALSE;

	CRect rcOld(m_rcUI);

	m_rcUISrc.CopyRect(lprcDst);
	DSLAYOUTINFO dsli;

	if(NULL == m_pbaseParent)
		::GetClientRect(m_hwnd, dsli.rcWnd);
	else
		dsli.rcWnd.CopyRect(m_pbaseParent->m_rcUI);

	DoUpdateLayout(&dsli, -1);

	if(bRedraw)
	{
		dsuiInvalidateRect(m_hwnd, rcOld, FALSE);
		dsuiInvalidateRect(m_hwnd, m_rcUI, FALSE);
	}

	return TRUE;
}



BOOL dsUIBase::MoveSrcUI( __in LPRECT lprcDst, BOOL bRedraw )
{
	if(NULL == lprcDst)
		return FALSE;

	CRect rcOld(m_rcUI);

	m_rcUISrc.CopyRect(lprcDst);

	DSLAYOUTINFO dsli;

	if(NULL == m_pbaseParent)
		::GetClientRect(m_hwnd, dsli.rcWnd);
	else
		dsli.rcWnd.CopyRect(m_pbaseParent->m_rcUI);

	UpdateLayoutRect(dsli.rcWnd, m_rcUISrc, m_rcCoordMark, m_rcUI);

	DoUpdateLayout(&dsli, -1);

	if(bRedraw)
	{
		dsuiInvalidateRect(m_hwnd, rcOld, FALSE);
		dsuiInvalidateRect(m_hwnd, m_rcUI, FALSE);
	}

	return TRUE;
}

BOOL dsUIBase::GetText( __out LPWSTR lpszText, int nMaxCount )
{
	if(NULL == lpszText)
		return FALSE;
	
	wcscpy_s(lpszText, nMaxCount, m_strText);

	return TRUE;
}

void dsUIBase::SetParam( LPARAM lParam )
{
	m_lParam = lParam;
}

LPARAM dsUIBase::GetParam()
{
	return m_lParam;
}

BOOL dsUIBase::SetAutoGrow( BOOL bAutoGrow )
{
	return dsTools::Exchange2Objects(m_bAutoGrow, bAutoGrow);
}

BOOL dsUIBase::IsAutoGrow()
{
	return m_bAutoGrow;
}

int dsUIBase::GetChildCount()
{
	return m_listUIBase.size();
}

void dsUIBase::RedrawUI()
{
	if(m_bVisible && IsWindow(m_hwnd))
		dsuiInvalidateRect(m_hwnd, m_rcUI, FALSE);
}

BOOL dsUIBase::GetRect( __out LPRECT lprcRect )
{
	if(NULL == lprcRect)
		return FALSE;

	::CopyRect(lprcRect, m_rcUI);

	return TRUE;
}

BOOL dsUIBase::GetSrcRect( __out LPRECT lprcRect )
{
	if(NULL == lprcRect)
		return FALSE;

	::CopyRect(lprcRect, m_rcUISrc);

	return TRUE;
}
IdsUIBase *dsUIBase::EnumUI( __in ENUMPROC procFind, __in LPARAM lParam )
{
	if(NULL == procFind)
		return FALSE;

	return EnumUIElements((ENUMUIPROC)procFind, lParam);
}

BOOL dsUIBase::dsuiInvalidateRect( __in_opt HWND hWnd, __in_opt CONST RECT *lpRect, __in BOOL bErase )
{

	if(NULL == hWnd)
		return FALSE;

	if(::IsWindow(hWnd)){

		if(dsNoPaintMsgMgr::getInstance()->IsMarked(hWnd)){

			SendMessage(hWnd, WM_PAINT, 0, 0);
			return TRUE;

		}else{
			::InvalidateRect(hWnd, lpRect, bErase);
			//::UpdateWindow(hWnd);
			return	::RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		}

		
	}

	return FALSE;
}

CRect dsUIBase::GetVisibleRect()
{

	dsUIBase *puiParent = m_pbaseParent;

	CRect rcVisible(m_rcUI);

	while(NULL != puiParent)
	{
		rcVisible.IntersectRect(rcVisible, puiParent->m_rcUI);
		if(rcVisible.IsRectEmpty())
			break;
	
		puiParent = (dsUIBase*)puiParent->GetParent();
	} 

	return rcVisible;
}

void dsUIBase::SetTooltipText( __in LPCWSTR lpszTooltip )
{
	m_strTip = lpszTooltip;
}

BOOL dsUIBase::GetTooltipText( __out LPWSTR lpszText, int nMaxCount )
{
	if(NULL == lpszText)
		return FALSE;

	wcscpy_s(lpszText, nMaxCount, m_strTip);

	return TRUE;
}
