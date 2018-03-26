/************************************************************************/
/* 
Author:

lourking.All rights reserved.

Create Time:

	3,12th,2014

Module Name:

	dsToolTip.h

Abstract: 

*/
/************************************************************************/


#ifndef __DSTOOLTIP_H__
#define __DSTOOLTIP_H__

#include "dsUIBase.h"

#include <atlframe.h>

class dsTip:public CWindowImpl<dsTip>,public CDoubleBufferImpl<dsTip>
{
public:

	dsUIBase *m_pui,*m_puiOld;
	CSize m_sizeExtent;

	CString m_strTip;
	BOOL m_bChanging;

	CFontHandle m_font;
	CRect m_rcMargin;

	COLORREF m_crBack;
	COLORREF m_crText;

	TIP_MODIFY_PROC m_proc;
	LPVOID m_lpParam;

public:

	DECLARE_WND_CLASS(L"dsTip")

	dsTip():m_pui(NULL),m_puiOld(NULL),m_sizeExtent(::GetSystemMetrics(SM_CXSCREEN)/2, ::GetSystemMetrics(SM_CYSCREEN)/2),
		m_bChanging(FALSE),m_rcMargin(5,3,5,3),m_crBack(RGB(230,230,230)),m_crText(0),m_proc(NULL),m_lpParam(NULL)
	{
	//	m_sizeExtent.SetSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));

	}
	~dsTip(){}

	BEGIN_MSG_MAP_EX(dsTip)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_NCACTIVATE(OnNcActivate)
		//MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_SETFOCUS(OnSetFocus)
		//MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		//MSG_WM_RBUTTONDOWN(OnRButtonDown)
		//MSG_WM_NCRBUTTONDOWN(OnNcRButtonDown)

		CHAIN_MSG_MAP(CDoubleBufferImpl<dsTip>)
	END_MSG_MAP()

public:

	void OnNcRButtonDown(UINT nHitTest, CPoint point){

	}

	void OnRButtonDown(UINT nFlags, CPoint point){

		int a= 0;
	}

	LRESULT DelayMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, dsUIBase *pui){

		DWORD dwErr = 0;
		if(!IsWindow())
		{
			Create(hWnd, NULL, NULL, WS_POPUPWINDOW, WS_EX_NOACTIVATE /*| WS_EX_TOPMOST*/);

			dwErr = GetLastError();
		}

		

		//ATLTRACE("pui:%x to ",m_pui);

		Attach(pui);

		//ATLTRACE("%x\n",m_pui);

		//ATLTRACE("check change\n");
		if(m_bChanging)
			Active(FALSE);
		
		//ATLTRACE("check null\n");
		if(NULL == m_pui)
		{
			Active(FALSE);

			m_puiOld = NULL;
			return 0;
		}

		

		//ATLTRACE("check msg\n");
		if(WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST){
			
			if(WM_MOUSEMOVE == uMsg){

				if(NULL != m_pui && m_pui->m_rcUI.PtInRect(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) )
				{
					//Active(TRUE);
				}
				else
				{
					Active(FALSE);
				}
			}
			if(WM_LBUTTONDOWN == uMsg){
				Active(FALSE);
			}

			/*else
			{
				Active(FALSE);
			}*/
		
		}
		else if(WM_MOUSEHOVER == uMsg){

			if(NULL != m_pui && m_pui->m_rcUI.PtInRect(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) )
			{
				if(m_pui != m_puiOld)
				Active(TRUE);
			}

			m_puiOld = m_pui;
		}
		
		//////////////////////////////////////////////////////////////////////////
		/* go away
			mousehover ?
		*/
		//////////////////////////////////////////////////////////////////////////

		
		
		return 0;
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct){

	/*	ModifyStyle(WS_CHILDWINDOW | WS_VISIBLE | WS_CAPTION , WS_POPUP);
		ModifyStyleEx(WS_EX_APPWINDOW, 0);*/

		return 0;
	}

	void DoPaint(CDCHandle dc){
		//CPaintDC dc(m_hWnd);

		CRect rcWnd;
		GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		dc.FillSolidRect(rcWnd, m_crBack);
		dc.SetBkMode(TRANSPARENT);

		rcWnd.DeflateRect(m_rcMargin);

		HFONT hFontOld = dc.SelectFont(m_font);
		dc.SetTextColor(m_crText);
		dc.DrawText(m_strTip, m_strTip.GetLength(), rcWnd, DT_LEFT | DT_WORD_ELLIPSIS | DT_WORDBREAK | DT_NOCLIP);
		dc.SelectFont(hFontOld);

		ATLTRACE("tip paint\n");

	}

	void OnPaint(CDCHandle /*dc*/){
		CPaintDC dc(m_hWnd);

		CRect rcWnd;
		GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		dc.FillSolidRect(rcWnd, m_crBack);
		dc.SetBkMode(TRANSPARENT);

		rcWnd.DeflateRect(m_rcMargin);

		HFONT hFontOld = dc.SelectFont(m_font);
		dc.SetTextColor(m_crText);
		dc.DrawText(m_strTip, m_strTip.GetLength(), rcWnd, DT_LEFT | DT_WORD_ELLIPSIS | DT_WORDBREAK | DT_NOCLIP);
		dc.SelectFont(hFontOld);

		ATLTRACE("tip paint\n");

	}

	BOOL OnEraseBkgnd(CDCHandle dc){

		return TRUE;
	}

	BOOL OnNcActivate(BOOL bActive){


		SetMsgHandled(FALSE);

		if(!bActive)
			Active(FALSE);
		return FALSE;
	}

	void OnKillFocus(CWindow wndFocus){
		//Active(FALSE);
	}

	void OnSetFocus(CWindow wndOld){
		if(NULL != wndOld.m_hWnd)
		wndOld.SetFocus();
	}

	void OnMouseMove(UINT nFlags, CPoint point){
		Active(FALSE);
	}

	void Active(BOOL bActive){

		if(!IsWindow())
			return;

		if(bActive){
			
			if(!IsWindowVisible())
			{
				CRect rcShow;
				GetShowRect(rcShow);
				
				//MoveWindow(rcShow);
				//ShowWindow(SW_SHOW);

				SetWindowPos(HWND_TOP, /*GetShowRect(rcShow)*/rcShow, SWP_SHOWWINDOW | SWP_NOACTIVATE);

				ATLTRACE("active true\n");
			}

		}
		else if(IsWindowVisible()){
			ShowWindow(SW_HIDE);

			ATLTRACE("active false\n");
		}
		
	}


	LPRECT GetShowRect(__out LPRECT lprcShow){

		if(NULL == lprcShow)
			return lprcShow;

		CDC dc(::GetDC(NULL));
		
		HFONT hFontOld = dc.SelectFont(m_font);

		CRect rcDraw(0, 0, m_sizeExtent.cx, m_sizeExtent.cy);
		dc.DrawText(m_strTip, -1, rcDraw, DT_LEFT | DT_CALCRECT | DT_WORD_ELLIPSIS | DT_WORDBREAK | DT_NOCLIP);
		
		dc.SelectFont(hFontOld);

		CURSORINFO ci = {0};
		ci.cbSize = sizeof CURSORINFO;

		GetCursorInfo(&ci);

		
		ICONINFO ii;
		GetIconInfo(ci.hCursor, &ii);

		CSize sizeBm;
		::GetBitmapDimensionEx(ii.hbmColor, &sizeBm);

		rcDraw.OffsetRect(sizeBm);
		rcDraw.OffsetRect(0, 20);
		rcDraw.OffsetRect(ci.ptScreenPos);

		rcDraw.right += m_rcMargin.left + m_rcMargin.right;
		rcDraw.bottom += m_rcMargin.top + m_rcMargin.bottom;

		::CopyRect(lprcShow, rcDraw);

		if(NULL != m_proc)
			m_proc(m_pui, lprcShow, m_lpParam);

		return lprcShow;
	}
	

	void Attach(dsUIBase *pui){

		m_bChanging = FALSE;

		if(pui != m_pui)
		{
			m_bChanging = TRUE;
		}

		m_pui = pui;

		if(NULL != m_pui)
		{
			
			if(m_pui->m_strTip.GetLength() > 0)
			{
				m_strTip = m_pui->m_strTip;
				return;
			}
		}

		m_pui = NULL;
		
	}

	void SetMaxExtent(int cx, int cy)
	{
		m_sizeExtent.SetSize(cx, cy);
	}

	void SetModifyProc(TIP_MODIFY_PROC proc, LPVOID lpParam)
	{

		m_proc = proc;
		m_lpParam = lpParam;

	}
	
};


#endif /*__DSTOOLTIP_H__*/