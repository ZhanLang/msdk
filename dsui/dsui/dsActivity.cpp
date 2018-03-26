#include "stdafx.h"
#include "dsActivity.h"
#include "dsLinkMgr.h"
#include "dsToolTip.h"

#include <WinUser.h>

dsActivity::dsActivity():
m_hwnd(NULL),
m_bTracking(FALSE),
m_puiCapture(NULL),
m_puiHot(NULL),
m_crBk(0),
m_procOriginal(NULL),
m_bNeedBkgnd(TRUE),
m_procBkgnd(NULL),
m_bLockPaintForAnimate(FALSE),
m_bUpdateLayeredMark(FALSE)
{
	

}
dsActivity::~dsActivity()
{
	DeleteAllUIElement();
}
BOOL dsActivity::PreTranslateMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (WM_MOUSEWHEEL == uMsg)
	{
		dsUIBase *pbase = dsGlobalStateMgr::getInstance()->GetMouseWheelCaptureUIPtr();
		if (NULL != pbase && pbase->m_bVisible && !(DSUI_STATE_DISABLED & pbase->m_nState))
		{
			pbase->ProcessMessage(hwnd, uMsg, wParam, lParam);
			return FALSE;
		}
	}
	if(WM_NCMOUSEMOVE <= uMsg && uMsg <= WM_NCMBUTTONDBLCLK)
	{
		m_tooltip.DelayMsg(hwnd, uMsg, wParam, lParam, NULL);
	} 
	if(WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST)
	{
		if(WM_MOUSEMOVE == uMsg && !m_bTracking)
		{
			TRACKMOUSEEVENT tme = {sizeof TRACKMOUSEEVENT, TME_LEAVE | TME_HOVER, m_hwnd, 200};
			m_bTracking = TrackMouseEvent(&tme);
		}
		else if(WM_LBUTTONUP == uMsg)
		{
			CPoint pt;
			::GetCursorPos(&pt);

			::ScreenToClient(hwnd, &pt);

			CRect rcWnd;
			::GetWindowRect(hwnd, rcWnd);
			
			::ScreenToClient(hwnd, &rcWnd.TopLeft());
			::ScreenToClient(hwnd, &rcWnd.BottomRight());

			if (rcWnd.PtInRect(pt))
			{
				::PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
			}
		}

		DSMOUSEINFO dsmi;
		dsmi.pbase = NULL;
		dsmi.ptMouse.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		dsGlobalStateMgr *pfmgr = dsGlobalStateMgr::getInstance();

		dsUIBase *puiCapture = pfmgr->GetCaptureUIPtr();
		dsUIBase *puiHot = pfmgr->GetHotUIPtr();


		if(NULL != puiCapture && !(DSUI_STATE_DISABLED & puiCapture->GetState()))
		{
			puiCapture->ProcessMessage(hwnd, uMsg, wParam, lParam);
			
			//m_tooltip.DelayMsg(hwnd, uMsg, wParam, lParam, puiCapture);

			if(NULL != puiHot && puiHot != puiCapture)
				puiHot->ProcessMessage(hwnd, WM_MOUSELEAVE, 0, 0);

			pfmgr->SetHot(NULL);

			

			return FALSE;
		}

		EnumUIElements(EnumUIProc_MouseHit, (LPARAM)&dsmi, TRUE);


		m_tooltip.DelayMsg(hwnd, uMsg, wParam, lParam, dsmi.pbase);

		if(NULL != dsmi.pbase && !(DSUI_STATE_DISABLED & dsmi.pbase->GetState()))
		{
			dsmi.pbase->ProcessMessage(hwnd, uMsg, wParam, lParam);
		}

		if(WM_MOUSEMOVE == uMsg){
			if(NULL != puiHot && puiHot != dsmi.pbase)
				puiHot->ProcessMessage(hwnd, WM_MOUSELEAVE, wParam, lParam);

			pfmgr->SetHot(dsmi.pbase);
		}

		if(NULL == dsmi.pbase)
		{
			if(NULL != puiHot && puiHot != dsmi.pbase)
				puiHot->ProcessMessage(hwnd, WM_MOUSELEAVE, wParam, lParam);

			pfmgr->SetHot(NULL);
		}

		
		return FALSE;

	}

	
	switch(uMsg)
	{
	case WM_PAINT:

		if(dsNoPaintMsgMgr::getInstance()->IsMarked(hwnd))
			DoPaint_UpdateLayered();
		else
			DoPaint_Normal();

		break;

	case WM_ERASEBKGND:

		return TRUE;

	case WM_SIZE:
		{
			//CRect rcWnd;

			//::GetWindowRect(hwnd, rcWnd);

			//m_uiSkin.SetRect(rcWnd);

			if(SIZE_RESTORED == wParam || SIZE_MAXIMIZED == wParam)
			{
				UpdateSize();
			}

			UpdatePaintBase(dsNoPaintMsgMgr::getInstance()->IsMarked(hwnd));
			
		}
		break;

	//case WM_KILLFOCUS:
	case WM_MOUSELEAVE:
		{


		m_tooltip.DelayMsg(hwnd, uMsg, wParam, lParam, NULL);

		//if(WM_MOUSELEAVE == uMsg)
			m_bTracking = FALSE;
		
		//uMsg = WM_MOUSELEAVE;

		dsUIBase *puiHot = dsGlobalStateMgr::getInstance()->GetHotUIPtr();

		if(NULL != puiHot)

			puiHot->ProcessMessage(hwnd, WM_MOUSELEAVE, wParam, lParam);
		}
		break;

	case WM_MOUSEHOVER:
		{
			m_bTracking = FALSE;

			dsUIBase *puiHot = dsGlobalStateMgr::getInstance()->GetHotUIPtr();

			if(NULL != puiHot)
				m_tooltip.DelayMsg(hwnd, uMsg, wParam, lParam, puiHot);
			
			break;
		}

	case WM_SETCURSOR:
		{
			dsUIBase *pui = NULL;
			pui = dsGlobalStateMgr::getInstance()->GetCaptureUIPtr();
			if(NULL == pui)
				pui = dsGlobalStateMgr::getInstance()->GetHotUIPtr();

			WORD wCursorID = 32512;

			if(NULL != pui)
				wCursorID = pui->m_wSysCursorID;

			if(32512 == wCursorID)
				return FALSE;

				
			CCursor cursor;
			cursor.LoadOEMCursor(MAKEINTRESOURCEW(wCursorID));
			::SetCursor(cursor);

			return TRUE;
		}

	case WM_SHOWWINDOW:
		//UpdatePaintBase();
		break;

	default:

		break;
	}

	return 0;
}

BOOL dsActivity::PreTranslateMessage( MSG* pMsg )
{
	return PreTranslateMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
}
void dsActivity::EnumUIElements( ENUMUIPROC procEnumUI, LPARAM lParam, BOOL bReverse /*= FALSE*/ )
{
	int nIndex = 0;

	if(!bReverse)
	{
		LIST_UIBASE::iterator it = m_listUIBase.begin();

		while(it != m_listUIBase.end())
		{
			if(!procEnumUI(*it, lParam, nIndex))
				break;

			it ++; nIndex ++;
		}
	}
	else
	{
		LIST_UIBASE::reverse_iterator it = m_listUIBase.rbegin();

		while(it != m_listUIBase.rend())
		{
			if(!procEnumUI(*it, lParam, nIndex))
				break;

			it ++; nIndex ++;
		}
	}
}


BOOL dsActivity::EnumUIProc_Paint( dsUIBase *pbase, LPARAM lParam, int nIndex )
{
	LPPAINTSTRUCT lpps = (LPPAINTSTRUCT)lParam;

	if(NULL == lpps)
		return FALSE;

	if(!pbase->m_bVisible)
		return TRUE;

	CRect rcRealPaintForEle;

	if(rcRealPaintForEle.IntersectRect(pbase->m_rcUI, &lpps->rcPaint))
	{
		CRect rcWndPaint(lpps->rcPaint);
		::CopyRect(&lpps->rcPaint, rcRealPaintForEle);
		pbase->DoPaint(lpps->hdc, lpps);
		
		
		pbase->EnumUIElements(EnumUIProc_Paint, lParam);
		
		::CopyRect(&lpps->rcPaint, rcWndPaint);
	}

	//////////////////////////////////////////////////////////////////////////
	/*go away
	
	if(rcRealPaintForEle.IntersectRect(pbase->m_rcUI, &lpps->rcPaint))
	{
	CRect rcWndPaint(lpps->rcPaint);
	::CopyRect(&lpps->rcPaint, rcRealPaintForEle);
	pbase->DoPaint(lpps->hdc, lpps);

	::CopyRect(&lpps->rcPaint, rcWndPaint);
	}

	*/
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

BOOL CALLBACK dsActivity::EnumUIProc_MouseHit( dsUIBase *pbase, LPARAM lParam, int nIndex )
{
	
	PDSMOUSEINFO pdsmi = (PDSMOUSEINFO)lParam;
	if(NULL == pdsmi || NULL == pbase)
		return FALSE;

	if((DSUI_STATE_DISABLED & pbase->m_nState) || !pbase->m_bVisible)
		return TRUE;


	if(pbase->m_rcUI.PtInRect(pdsmi->ptMouse))
	{	
		pdsmi->pbase = pbase;

		pbase->EnumUIElements(EnumUIProc_MouseHit, lParam, TRUE);

		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	/*go away
	
	if(pbase->m_rcUI.PtInRect(pdsmi->ptMouse))
	{	
	pdsmi->pbase = pbase;

	return FALSE;
	}

	*/
	//////////////////////////////////////////////////////////////////////////


	return TRUE;



}
BOOL CALLBACK dsActivity::EnumUIProc_InheritDNA( __in dsUIBase *pbase, LPARAM lParam, int nIndex )
{

	PDSDNAINFO pddi = (PDSDNAINFO)lParam;

	if(NULL == pbase || NULL == pddi)
		return FALSE;

	pbase->m_hwnd = pddi->hwnd;
	pbase->m_pbaseParent = (dsUIBase*)pddi->pbaseParent;


	dsUIBase *pbaseParent = (dsUIBase*)pddi->pbaseParent;
	pddi->pbaseParent = pbase; 
	pbase->EnumUIElements(EnumUIProc_InheritDNA, lParam);
	pddi->pbaseParent = pbaseParent;


	return TRUE;
}

BOOL CALLBACK dsActivity::EnumUIProc_UpdateLayout( __in dsUIBase *pbase, LPARAM lParam, int nIndex )
{

	PDSLAYOUTINFO pdsli = (PDSLAYOUTINFO)lParam;

	if(NULL == pbase || NULL == pdsli)
		return FALSE;


	if (0 == pbase->m_strText.Compare(L"itemButton"))
		int a= 0;
	
	if(!pbase->DoUpdateLayout(pdsli, nIndex))
		return TRUE;

	CRect rcParent(pdsli->rcWnd);
	pdsli->rcWnd.CopyRect(pbase->m_rcUI);

	pbase->EnumUIElements(EnumUIProc_UpdateLayout, lParam);

	pdsli->rcWnd.CopyRect(rcParent);

	return TRUE;
}

BOOL CALLBACK dsActivity::EnumUIProc_UpdateLayout_ForceLayout( __in dsUIBase *pbase, LPARAM lParam, int nIndex )
{
	PDSLAYOUTINFO pdsli = (PDSLAYOUTINFO)lParam;

	if(NULL == pbase || NULL == pdsli)
		return FALSE;

	pbase->DoUpdateLayout(pdsli, nIndex);

	CRect rcParent(pdsli->rcWnd);
	pdsli->rcWnd.CopyRect(pbase->m_rcUI);

	pbase->EnumUIElements(EnumUIProc_UpdateLayout, lParam);

	pdsli->rcWnd.CopyRect(rcParent);

	return TRUE;
}
IdsUIBase * dsActivity::CreateDsuiElement( __in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark /*= NULL*/, BOOL bUpdateLayout /*= FALSE*/)
{
	if(IsUIElesExist(wUIID))
	{
		::MessageBoxA(NULL, "1", NULL, 0);
		return NULL;
	}

	DSUICLASSINFO ci;
	if(!dsUIClassMgr::getInstance()->GetClassInfo(lpszClass, &ci))
	{
		::MessageBoxA(NULL, "2", NULL, 0);
		return NULL;
	}

	dsUIBase *pbase = ci.procNewUI();

	if(NULL == pbase)
	{
		::MessageBoxA(NULL, "3", NULL, 0);
		return pbase;
	}

	pbase->m_pbaseParent = NULL;
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
			::MessageBoxA(NULL, "4", NULL, 0);
			return  NULL;
		}
	}

	//if(bUpdateLayout){
	//	DSLAYOUTINFO dsli;
	//	::GetWindowRect(m_hwnd, dsli.rcWnd);
	//	dsli.rcWnd.MoveToXY(0, 0);
	//	pbase->DoUpdateLayout(&dsli, -1);
	//}

	return pbase;
}

IdsUIBase * dsActivity::GetDsuiElement( WORD wUIID )
{
	if(0 == wUIID)
		return NULL;

	MAP_UIBASE::iterator it = m_mapUIBase.find(wUIID);

	if(it != m_mapUIBase.end())
		return it->second;

	return NULL;
}

IdsUIBase * dsActivity::GetDsuiElement(LPARAM lParam, LPVOID pUserInfo)
{
	DSMOUSEINFO dsmi;
	dsmi.pbase = NULL;
	dsmi.ptMouse.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	EnumUIElements(EnumUIProc_MouseHit, (LPARAM)&dsmi, TRUE);
	return dsmi.pbase;
}
BOOL dsActivity::AddSkinPaintMission( IdsPaintMission *pdspm )
{
	return m_uiSkin.AddPaintMisson(pdspm);
}
IdsPaintMission * dsActivity::GetSkinPaintMisson( LPARAM lParamForFind )
{
	return m_uiSkin.GetPaintMisson(lParamForFind);
}
void dsActivity::DrawSkin( CDCHandle dc, LPPAINTSTRUCT lpps )
{
	EnumUIProc_Paint(&m_uiSkin, (LPARAM)lpps, 0);
}	

void dsActivity::DeleteAllUIElement()
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

BOOL dsActivity::UpdateSize(BOOL bForUpdateLayout/* = FALSE*/)
{
	CRect rcWnd;
	::GetWindowRect(m_hwnd, rcWnd);

	rcWnd.MoveToXY(0, 0);

	m_uiSkin.SetRect(rcWnd);

	if(NULL != m_hwnd && ::IsWindow(m_hwnd))
	{
		::InvalidateRect(m_hwnd, NULL, TRUE);
	}

	DSLAYOUTINFO dsli;
	dsli.rcWnd.CopyRect(rcWnd);

	if(bForUpdateLayout)
		EnumUIElements(EnumUIProc_UpdateLayout_ForceLayout, (LPARAM)&dsli);
	else
		EnumUIElements(EnumUIProc_UpdateLayout, (LPARAM)&dsli);

	return TRUE;
}



void dsActivity::UpdatePaintBase(BOOL bForce /*= FALSE*/)
{
	CRect rcWnd;
	::GetClientRect(m_hwnd, rcWnd);

	

	if(rcWnd.Width() != m_rcSkin.Height() || rcWnd.Height() != m_rcSkin.Height())
		m_bNeedBkgnd = TRUE;


	if (!bForce && rcWnd.Width() <= m_rcSkin.Width() && rcWnd.Height() <= m_rcSkin.Height())
		return;

	m_rcSkin.CopyRect(rcWnd);

	CWindowDC dcWnd(m_hwnd);


	if (NULL == m_dcMem.m_hDC)
	{
		m_dcMem.CreateCompatibleDC(dcWnd);
	}

	if (NULL != m_bmMem.m_hBitmap)
	{
		m_bmMem.DeleteObject();
	}

	m_bmMem.CreateCompatibleBitmap(dcWnd, m_rcSkin.Width(), m_rcSkin.Height());


	if (NULL == m_dcSkinMem.m_hDC)
	{
		m_dcSkinMem.CreateCompatibleDC(dcWnd);
	}

	if (NULL != m_bmSkinMem.m_hBitmap)
	{
		m_bmSkinMem.DeleteObject();
	}

	m_bmSkinMem.CreateCompatibleBitmap(dcWnd, m_rcSkin.Width(), m_rcSkin.Height());
}
BOOL dsActivity::InheritDNA( PDSDNAINFO pddi )
{
	if(NULL == pddi)
		return FALSE;

	m_hwnd = pddi->hwnd;
	pddi->pbaseParent = NULL;

	EnumUIElements(EnumUIProc_InheritDNA, (LPARAM)pddi);

	return TRUE;
}

BOOL dsActivity::IsUIElesExist( int wUIID )
{
	return (NULL != GetDsuiElement(wUIID));
}

COLORREF dsActivity::SetBkColor( COLORREF crBk )
{
	return dsTools::Exchange2Objects(m_crBk, crBk);
}

BOOL dsActivity::LinkWindow( HWND hwnd )
{
	if(!::IsWindow(hwnd))
		return FALSE;

	m_uiSkin.m_hwnd = hwnd;

	dsActivity *pactOld = dsLinkMgr::getInstance()->GetLinkActivity(hwnd);

	if(NULL == pactOld)
		m_procOriginal = (WNDPROC)::GetWindowLongPtr(hwnd, -4/*GWL_WNDPROC*/);
	else
		m_procOriginal = pactOld->m_procOriginal;

	::SetWindowLongPtr(hwnd, -4, (LONG_PTR)UIProc);

	DSDNAINFO ddi;
	ddi.hwnd = hwnd;

	InheritDNA(&ddi);
	UpdateSize();
	UpdatePaintBase();

	dsLinkMgr::getInstance()->AddLink(hwnd, this);

	return TRUE;
}

LRESULT CALLBACK dsActivity::UIProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	dsActivity *pact = dsLinkMgr::getInstance()->GetLinkActivity(hwnd);

	if(NULL == pact)
	{
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else if(WM_SETCURSOR == uMsg){

		
		int nRet = pact->m_procOriginal(hwnd, uMsg, wParam, lParam);
		pact->PreTranslateMessage(hwnd, uMsg, wParam, lParam);

		return nRet;

	}
	else
	{
		

		int nRet = pact->PreTranslateMessage(hwnd, uMsg, wParam, lParam);

		if(WM_ERASEBKGND == uMsg)
			return nRet;
		
		/*if(0 != nRet)
			return nRet;
		else*/
			return pact->m_procOriginal(hwnd, uMsg, wParam, lParam);

		//////////////////////////////////////////////////////////////////////////
		/*go away

		int nRet = pact->PreTranslateMessage(hwnd, uMsg, wParam, lParam)
		
		if(0 != nRet)
			return nRet;
		else
			return pact->m_procOriginal(hwnd, uMsg, wParam, lParam);

		*/
		//////////////////////////////////////////////////////////////////////////
		

	}

	return 0;
}

BOOL dsActivity::SetTipFont( int nFontID )
{
	m_tooltip.m_font.Attach(dsFontMgr::getInstance()->GetFont(nFontID));

	if(NULL == m_tooltip.m_font.m_hFont)
		return FALSE;
	else
		return TRUE;
}

BOOL dsActivity::SetTipMargin( __in LPRECT lprcMargin )
{
	if(NULL == lprcMargin)
		return FALSE;

	if(lprcMargin->left < 0 || lprcMargin->top < 0 || lprcMargin->right < 0 || lprcMargin->bottom < 0)
		return FALSE;


	m_tooltip.m_rcMargin.CopyRect(lprcMargin);

	return 0;
}

void dsActivity::SetTipExtent( int cxMax, int cyMax)
{
	m_tooltip.m_sizeExtent.SetSize(cxMax, cyMax);
}

COLORREF dsActivity::SetTipBkColor( COLORREF crBk )
{
	return dsTools::Exchange2Objects(m_tooltip.m_crBack, crBk);
}

COLORREF dsActivity::SetTipTextColor( COLORREF crText )
{
	return dsTools::Exchange2Objects(m_tooltip.m_crText, crText);
}

void dsActivity::ForceRefreshBkgnd()
{
	m_bNeedBkgnd = TRUE;
	if(m_bUpdateLayeredMark){
		UpdatePaintBase(TRUE);
		DoPaint_UpdateLayered();
	}
	else
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

BOOL dsActivity::SetBkgndProc( BKGNDPROC procBkgnd, LPVOID lpParam )
{
	if(NULL == procBkgnd)
		return FALSE;

	m_procBkgnd = procBkgnd;
	m_lpBkgndParam = lpParam;

	return TRUE;
}
//

BOOL dsActivity::SetTransparentAttribute(BOOL bTransparent, COLORREF cr, BYTE bAlpha){

	DWORD dwExStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);

	SetWindowLong(m_hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED);

	return SetLayeredWindowAttributes(m_hwnd, cr, bAlpha, LWA_COLORKEY | LWA_ALPHA);
}

HBITMAP dsActivity::GetPrintBitmap()
{

	CBitmapHandle bm;

	try{
		CPaintDC dc(m_hwnd);

		CRect rcPaint;
		::GetClientRect(m_hwnd, rcPaint);
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		CDC dcMem;
		dcMem.CreateCompatibleDC(dc);

		bm.CreateCompatibleBitmap(dc, rcPaint.Width(), rcPaint.Height());

		HBITMAP hBmOld = dcMem.SelectBitmap(bm);

		dc.m_ps.hdc = dcMem.m_hDC;


		dcMem.FillSolidRect(rcPaint, m_crBk);

		if(NULL != m_procBkgnd)
			m_procBkgnd(dcMem, rcPaint, m_lpBkgndParam);

		DrawSkin(dcMem.m_hDC, &dc.m_ps);

		EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

		dcMem.SelectBitmap(hBmOld);

	}
	catch(...){

	}
		
	return bm.m_hBitmap;
}

int dsActivity::SetState( int nState )
{
	int nRet = m_uiSkin.SetState(nState);
	if(nState !=  nRet){
		ForceRefreshBkgnd();
	}
	return nRet;
}

void dsActivity::SetTipModifyProc( TIP_MODIFY_PROC proc, LPVOID lpParam )
{
	m_tooltip.SetModifyProc(proc, lpParam);


}

BOOL dsActivity::SetUpdateLayeredMark( BOOL bMark )
{
	m_bUpdateLayeredMark = bMark;
	return dsNoPaintMsgMgr::getInstance()->SetMark(m_hwnd, bMark);
}

void dsActivity::DoPaint_Normal()
{
	
		CPaintDC dc(m_hwnd);

		HBITMAP hBmOld = m_dcMem.SelectBitmap(m_bmMem);
		dc.m_ps.hdc = m_dcMem.m_hDC;

		CRect rcPaint(dc.m_ps.rcPaint);


		HBITMAP hBmOldSkin = NULL;



		if(m_bNeedBkgnd){
			hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

			CRect rcClient;
			::GetClientRect(m_hwnd, rcClient);
			::CopyRect(&dc.m_ps.rcPaint, rcClient);

			m_dcSkinMem.FillSolidRect(rcClient, m_crBk);

			if(NULL != m_procBkgnd)
				m_procBkgnd(m_dcSkinMem, rcClient, m_lpBkgndParam);

			dc.m_ps.hdc = m_dcSkinMem.m_hDC;
			DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
			dc.m_ps.hdc = m_dcMem.m_hDC;
			::CopyRect(&dc.m_ps.rcPaint, rcPaint);

			m_bNeedBkgnd = FALSE;
		}


		//////////////////////////////////////////////////////////////////////////
		/*go away

		if(m_bNeedBkgnd){
		hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

		CRect rcClient;
		::GetClientRect(hwnd, rcClient);
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		m_dcSkinMem.FillSolidRect(rcPaint, m_crBk);
		dc.m_ps.hdc = m_dcSkinMem.m_hDC;
		DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
		dc.m_ps.hdc = m_dcMem.m_hDC;
		::CopyRect(&dc.m_ps.rcPaint, rcClient);

		m_bNeedBkgnd = FALSE;
		}

		*/
		//////////////////////////////////////////////////////////////////////////


		m_dcMem.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
			m_dcSkinMem, rcPaint.left, rcPaint.top, SRCCOPY);



		//DrawSkin(m_dcMem.m_hDC, &dc.m_ps);
		EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

		dc.m_ps.hdc = dc.m_hDC;

		dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
			m_dcMem, rcPaint.left, rcPaint.top, SRCCOPY);

		//Graphics gph(dc);
		//Color cr(50 , 255, 255, 255);
		//SolidBrush sb(cr);
		//gph.FillRectangle(&sb, 0, 0, rcPaint.Width(), rcPaint.Height());

		//dc.FillSolidRect(rcPaint, RGB(255,0,255));

		m_dcMem.SelectBitmap(hBmOld);

		if(m_bNeedBkgnd)
		{
			m_dcSkinMem.SelectBitmap(hBmOldSkin);
		}
	

}

void dsActivity::DoPaint_UpdateLayered()
{

	if(m_bLockPaintForAnimate)
		return;

	if(NULL == m_dcMem.m_hDC)
		return;

	BLENDFUNCTION m_Blend;

	//初始化GDI+.
	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//窗体样式为0x80000为层级窗体

	CRect rct;
	GetWindowRect(m_hwnd, rct);
	POINT ptWinPos={rct.left,rct.top};

	////绘制内存位图
	//CDC hdcTemp = ::GetDC(m_hwnd);
	//CDC hdcMemory = CreateCompatibleDC(hdcTemp);
	//CBitmap hBitMap=CreateCompatibleBitmap(hdcTemp,rct.Width(),rct.Height());

	HBITMAP hBmOld = (HBITMAP)SelectObject(m_dcMem,m_bmMem);

	//使用GDI+载入PNG图片
	CDC hdcScreen=::GetDC (m_hwnd);


	CPaintDC dc(m_hwnd);

	//HBITMAP hBmOld = hdcMemory.SelectBitmap(hBitMap);
	dc.m_ps.hdc = m_dcMem.m_hDC;
	SetRect(&dc.m_ps.rcPaint, 0, 0, rct.Width(), rct.Height());

	CRect rcPaint(dc.m_ps.rcPaint);

	HBITMAP hBmOldSkin = NULL;

	if(m_bNeedBkgnd){
		hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

		CRect rcClient;
		::GetClientRect(m_hwnd, rcClient);
		::CopyRect(&dc.m_ps.rcPaint, rcClient);

		//m_dcSkinMem.FillSolidRect(rcClient, m_crBk);

		if(NULL != m_procBkgnd)
			m_procBkgnd(m_dcSkinMem, rcClient, m_lpBkgndParam);

		dc.m_ps.hdc = m_dcSkinMem.m_hDC;
		DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
		dc.m_ps.hdc = m_dcMem.m_hDC;
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		m_bNeedBkgnd = FALSE;
	}

	m_dcMem.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
		m_dcSkinMem, rcPaint.left, rcPaint.top, SRCCOPY);

	//DrawSkin(m_dcMem.m_hDC, &dc.m_ps);
	EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

	dc.m_ps.hdc = dc.m_hDC;

	//dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
	//	m_dcMem, rcPaint.left, rcPaint.top, SRCCOPY);


	//Color cr(255, 0, 0, 0);

	//Graphics graph(m_dcMem);
	//graph.Clear(cr);


	//////////////////////////////////////////////////////////////////////////
	//UpdateWindow
	SIZE sizeWindow={rct.Width(), rct.Height()};
	POINT ptSrc={0,0};
	UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,m_dcMem,&ptSrc, 0,&m_Blend,ULW_ALPHA);

	m_dcMem.SelectBitmap(hBmOld);

	if(m_bNeedBkgnd)
	{
		m_dcSkinMem.SelectBitmap(hBmOldSkin);
	}

	//SetForegroundWindow(m_hwnd);
}

void dsActivity::DoPaint_UpdateLayered2()
{

	BLENDFUNCTION m_Blend;

	//初始化GDI+.
	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//窗体样式为0x80000为层级窗体

	CRect rct;
	GetWindowRect(m_hwnd, rct);
	POINT ptWinPos={rct.left,rct.top};

	//绘制内存位图
	CDC hdcTemp = ::GetDC(m_hwnd);
	CDC hdcMemory = CreateCompatibleDC(hdcTemp);
	CBitmap hBitMap=CreateCompatibleBitmap(hdcTemp,rct.Width(),rct.Height());

	HBITMAP hBmOld = (HBITMAP)SelectObject(hdcMemory,hBitMap);

	//使用GDI+载入PNG图片
	CDC hdcScreen=::GetDC (m_hwnd);


	CPaintDC dc(m_hwnd);

	//HBITMAP hBmOld = hdcMemory.SelectBitmap(hBitMap);
	dc.m_ps.hdc = hdcMemory.m_hDC;
	SetRect(&dc.m_ps.rcPaint, 0, 0, rct.Width(), rct.Height());

	CRect rcPaint(dc.m_ps.rcPaint);

	HBITMAP hBmOldSkin = NULL;

	if(m_bNeedBkgnd){
		hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

		CRect rcClient;
		::GetClientRect(m_hwnd, rcClient);
		::CopyRect(&dc.m_ps.rcPaint, rcClient);

		//m_dcSkinMem.FillSolidRect(rcClient, m_crBk);

		if(NULL != m_procBkgnd)
			m_procBkgnd(m_dcSkinMem, rcClient, m_lpBkgndParam);

		dc.m_ps.hdc = m_dcSkinMem.m_hDC;
		DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
		dc.m_ps.hdc = hdcMemory.m_hDC;
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		m_bNeedBkgnd = FALSE;
	}

	hdcMemory.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
		m_dcSkinMem, rcPaint.left, rcPaint.top, SRCCOPY);

	//DrawSkin(m_dcMem.m_hDC, &dc.m_ps);
	EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

	dc.m_ps.hdc = dc.m_hDC;

	//dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
	//	m_dcMem, rcPaint.left, rcPaint.top, SRCCOPY);



	//////////////////////////////////////////////////////////////////////////
	//UpdateWindow
	SIZE sizeWindow={rct.Width(), rct.Height()};
	POINT ptSrc={0,0};
	UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,hdcMemory,&ptSrc, 0,&m_Blend,ULW_ALPHA);

	hdcMemory.SelectBitmap(hBmOld);

	if(m_bNeedBkgnd)
	{
		m_dcSkinMem.SelectBitmap(hBmOldSkin);
	}

	//SetForegroundWindow(m_hwnd);
}

void dsActivity::Animate( int nFlag,  LPRECT lprcStart, LPRECT lprcEnd )
{
	if(m_dcMem.m_hDC == NULL)
		return;

	if(NULL == lprcStart || NULL == lprcEnd)
		return;

	BLENDFUNCTION m_Blend;

	//初始化GDI+.
	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//窗体样式为0x80000为层级窗体

	CRect rct;
	GetWindowRect(m_hwnd, rct);
	POINT ptWinPos={rct.left,rct.top};

	HBITMAP hBmOld = (HBITMAP)SelectObject(m_dcMem,m_bmMem);

	CDC hdcScreen=::GetDC (m_hwnd);
	CPaintDC dc(m_hwnd);

	

	dc.m_ps.hdc = m_dcMem.m_hDC;
	SetRect(&dc.m_ps.rcPaint, 0, 0, rct.Width(), rct.Height());

	CRect rcPaint(dc.m_ps.rcPaint);

	HBITMAP hBmOldSkin = NULL;

	if(m_bNeedBkgnd && NULL != m_dcSkinMem.m_hDC){
		hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

		CRect rcClient;
		::GetClientRect(m_hwnd, rcClient);
		::CopyRect(&dc.m_ps.rcPaint, rcClient);

		//m_dcSkinMem.FillSolidRect(rcClient, m_crBk);

		if(NULL != m_procBkgnd)
			m_procBkgnd(m_dcSkinMem, rcClient, m_lpBkgndParam);

		dc.m_ps.hdc = m_dcSkinMem.m_hDC;
		DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
		dc.m_ps.hdc = m_dcMem.m_hDC;
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		m_bNeedBkgnd = FALSE;
	}

	if(!(ANIMATE_CONTENT_USEOLD & nFlag))
	m_dcMem.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
		m_dcSkinMem, rcPaint.left, rcPaint.top, SRCCOPY);

	if(!(ANIMATE_CONTENT_USEOLD & nFlag))
	EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

	dc.m_ps.hdc = dc.m_hDC;


	//Color cr(255, 0, 0, 0);

	//Graphics graph(m_dcMem);
	//graph.Clear(cr);


	//////////////////////////////////////////////////////////////////////////
	//UpdateWindow
	SIZE sizeWindow={rct.Width(), rct.Height()};
	POINT ptSrc={0,0};


	CRect rcStart(lprcStart),rcEnd(lprcEnd);


	int cxChange = rcEnd.Width() - rcStart.Width();
	int cyChange = rcEnd.Height() - rcStart.Height();

	int xOffset = rcEnd.left - rcStart.left;
	int yOffset = rcEnd.top - rcStart.top;

	int nCount = 10;

	//UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,NULL,NULL, 0,&m_Blend,ULW_ALPHA);

	
	

	//UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,m_dcMem,&ptSrc, 0,&m_Blend,ULW_ALPHA);


	Color cr(255, 0, 0, 0);

	CSize size(rcEnd.left - rcStart.left, rcEnd.top - rcStart.top);

	CPoint ptCopy(rcStart.left, rcStart.top);

	if(ANIMATE_MOVE_SRC & nFlag){

		for (int i=0; i<nCount; i++)
		{
			//
			ptCopy.x += xOffset/nCount;
			ptCopy.y += yOffset/nCount;

			//ptWinPos.y += 10;

			//sizeWindow.cy -= 10;
			

			CDC dcTemp;
			dcTemp.CreateCompatibleDC(dc);
			CBitmap bmTemp;
			bmTemp.CreateCompatibleBitmap(dc, rct.Width(), rct.Height());

			HBITMAP hBmOldTemp = dcTemp.SelectBitmap(bmTemp);

			Graphics graphTemp(dcTemp);
			dcTemp.BitBlt(ptCopy.x, ptCopy.y, rct.Width(), rct.Height(), m_dcMem, ptCopy.x, ptCopy.y, SRCCOPY);
			
			Sleep(7);

			//SetViewportOrgEx(m_dcMem, ptCopy.x, ptCopy.y, NULL);

			//SetWindowOrgEx(m_dcMem, ptCopy.x, ptCopy.y, NULL);

			UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,dcTemp,&ptSrc, 0,&m_Blend,ULW_ALPHA);

			dcTemp.SelectBitmap(hBmOldTemp);

		}


		ptCopy.x += xOffset - (xOffset/nCount) * nCount;
		ptCopy.y += yOffset - (yOffset/nCount) * nCount;

		CDC dcTemp;
		dcTemp.CreateCompatibleDC(dc);
		CBitmap bmTemp;
		bmTemp.CreateCompatibleBitmap(dc, rct.Width(), rct.Height());

		HBITMAP hBmOldTemp = dcTemp.SelectBitmap(bmTemp);

		Graphics graphTemp(dcTemp);
		dcTemp.BitBlt(ptCopy.x, ptCopy.y, rct.Width(), rct.Height(), m_dcMem, ptCopy.x, ptCopy.y, SRCCOPY);

		UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,dcTemp,&ptSrc, 0,&m_Blend,ULW_ALPHA);

		dcTemp.SelectBitmap(hBmOldTemp);


		
	}
	else if(ANIMATE_MOVE_DST & nFlag){

		for (int i=0; i<nCount; i++)
		{

			ptWinPos.x += xOffset/nCount;
			ptWinPos.y += yOffset/nCount;

			UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,m_dcMem,&ptSrc, 0,&m_Blend,ULW_ALPHA);
		}

	}
	

	

	m_dcMem.SelectBitmap(hBmOld);

	if(m_bNeedBkgnd)
	{
		m_dcSkinMem.SelectBitmap(hBmOldSkin);
	}

}

void dsActivity::Animate( int nFlag, LPRECT lprcStart, LPRECT lprcEnd, int nFrameTime, int nFrameCount )
{
	if(m_dcMem.m_hDC == NULL)
		return;

	if(NULL == lprcStart || NULL == lprcEnd)
		return;

	BLENDFUNCTION m_Blend;

	//初始化GDI+.
	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//窗体样式为0x80000为层级窗体

	CRect rct;
	GetWindowRect(m_hwnd, rct);
	POINT ptWinPos={rct.left,rct.top};

	HBITMAP hBmOld = (HBITMAP)SelectObject(m_dcMem,m_bmMem);

	CDC hdcScreen=::GetDC (m_hwnd);
	CPaintDC dc(m_hwnd);



	dc.m_ps.hdc = m_dcMem.m_hDC;
	SetRect(&dc.m_ps.rcPaint, 0, 0, rct.Width(), rct.Height());

	CRect rcPaint(dc.m_ps.rcPaint);

	HBITMAP hBmOldSkin = NULL;

	if(m_bNeedBkgnd && NULL != m_dcSkinMem.m_hDC){
		hBmOldSkin = m_dcSkinMem.SelectBitmap(m_bmSkinMem);

		CRect rcClient;
		::GetClientRect(m_hwnd, rcClient);
		::CopyRect(&dc.m_ps.rcPaint, rcClient);

		//m_dcSkinMem.FillSolidRect(rcClient, m_crBk);

		if(NULL != m_procBkgnd)
			m_procBkgnd(m_dcSkinMem, rcClient, m_lpBkgndParam);

		dc.m_ps.hdc = m_dcSkinMem.m_hDC;
		DrawSkin(m_dcSkinMem.m_hDC, &dc.m_ps);
		dc.m_ps.hdc = m_dcMem.m_hDC;
		::CopyRect(&dc.m_ps.rcPaint, rcPaint);

		m_bNeedBkgnd = FALSE;
	}

	if(!(ANIMATE_CONTENT_USEOLD & nFlag))
		m_dcMem.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
		m_dcSkinMem, rcPaint.left, rcPaint.top, SRCCOPY);

	if(!(ANIMATE_CONTENT_USEOLD & nFlag))
		EnumUIElements(EnumUIProc_Paint, (LPARAM)&dc.m_ps);

	dc.m_ps.hdc = dc.m_hDC;


	//Color cr(255, 0, 0, 0);

	//Graphics graph(m_dcMem);
	//graph.Clear(cr);


	//////////////////////////////////////////////////////////////////////////
	//UpdateWindow
	SIZE sizeWindow={rct.Width(), rct.Height()};
	POINT ptSrc={0,0};


	CRect rcStart(lprcStart),rcEnd(lprcEnd);


	int cxChange = rcEnd.Width() - rcStart.Width();
	int cyChange = rcEnd.Height() - rcStart.Height();

	int xOffset = rcEnd.left - rcStart.left;
	int yOffset = rcEnd.top - rcStart.top;

	int nCount = nFrameCount;

	//UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,NULL,NULL, 0,&m_Blend,ULW_ALPHA);




	//UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,m_dcMem,&ptSrc, 0,&m_Blend,ULW_ALPHA);


	Color cr(255, 0, 0, 0);

	CSize size(rcEnd.left - rcStart.left, rcEnd.top - rcStart.top);

	CPoint ptCopy(rcStart.left, rcStart.top);

	if(ANIMATE_MOVE_SRC & nFlag){

		for (int i=0; i<nCount; i++)
		{
			//
			ptCopy.x += xOffset/nCount;
			ptCopy.y += yOffset/nCount;

			//ptWinPos.y += 10;

			//sizeWindow.cy -= 10;


			CDC dcTemp;
			dcTemp.CreateCompatibleDC(dc);
			CBitmap bmTemp;
			bmTemp.CreateCompatibleBitmap(dc, rct.Width(), rct.Height());

			HBITMAP hBmOldTemp = dcTemp.SelectBitmap(bmTemp);

			Graphics graphTemp(dcTemp);
			dcTemp.BitBlt(ptCopy.x, ptCopy.y, rct.Width(), rct.Height(), m_dcMem, ptCopy.x, ptCopy.y, SRCCOPY);

			if(nFrameTime > 0)
				Sleep(nFrameTime);

			//SetViewportOrgEx(m_dcMem, ptCopy.x, ptCopy.y, NULL);

			//SetWindowOrgEx(m_dcMem, ptCopy.x, ptCopy.y, NULL);

			UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,dcTemp,&ptSrc, 0,&m_Blend,ULW_ALPHA);

			dcTemp.SelectBitmap(hBmOldTemp);

		}


		ptCopy.x += xOffset - (xOffset/nCount) * nCount;
		ptCopy.y += yOffset - (yOffset/nCount) * nCount;

		CDC dcTemp;
		dcTemp.CreateCompatibleDC(dc);
		CBitmap bmTemp;
		bmTemp.CreateCompatibleBitmap(dc, rct.Width(), rct.Height());

		HBITMAP hBmOldTemp = dcTemp.SelectBitmap(bmTemp);

		Graphics graphTemp(dcTemp);
		dcTemp.BitBlt(ptCopy.x, ptCopy.y, rct.Width(), rct.Height(), m_dcMem, ptCopy.x, ptCopy.y, SRCCOPY);

		UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,dcTemp,&ptSrc, 0,&m_Blend,ULW_ALPHA);

		dcTemp.SelectBitmap(hBmOldTemp);



	}
	else if(ANIMATE_MOVE_DST & nFlag){

		for (int i=0; i<nCount; i++)
		{

			ptWinPos.x += xOffset/nCount;
			ptWinPos.y += yOffset/nCount;

			UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,m_dcMem,&ptSrc, 0,&m_Blend,ULW_ALPHA);
		}

	}




	m_dcMem.SelectBitmap(hBmOld);

	if(m_bNeedBkgnd)
	{
		m_dcSkinMem.SelectBitmap(hBmOldSkin);
	}
}
void dsActivity::MakeAnimateEmptyView()
{
	BLENDFUNCTION m_Blend;

	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	CRect rct;
	GetWindowRect(m_hwnd, rct);
	POINT ptWinPos={rct.left,rct.top};

	CDC hdcScreen=::GetDC (m_hwnd);
	CPaintDC dc(m_hwnd);


	//////////////////////////////////////////////////////////////////////////
	//UpdateWindow
	SIZE sizeWindow={rct.Width(), rct.Height()};
	POINT ptSrc={0,0};


	CDC dcTemp;
	dcTemp.CreateCompatibleDC(dc);
	CBitmap bmTemp;
	bmTemp.CreateCompatibleBitmap(dc, rct.Width(), rct.Height());

	HBITMAP hBmOldTemp = dcTemp.SelectBitmap(bmTemp);

	UpdateLayeredWindow( m_hwnd,hdcScreen,&ptWinPos,&sizeWindow,dcTemp,&ptSrc, 0,&m_Blend,ULW_ALPHA);


	

}

void dsActivity::LockPaintForAnimate( BOOL bLock )
{
	m_bLockPaintForAnimate = bLock;
}

void dsActivity::ForceRefreshBkgnd_JustSetFlag()
{
	m_bNeedBkgnd = TRUE;
}

//IdsListView * dsActivity::Query_ListView( __in IdsUIBase *pbase )
//{
//	return (dsListView*)pbase;
//}


DSUI_API IdsActivity * IdsNew_Activity()
{
	return new dsActivity();
}


DSUI_API void IdsDel_Activity( IdsActivity *p )
{
	if(NULL != p)
		delete p;
}