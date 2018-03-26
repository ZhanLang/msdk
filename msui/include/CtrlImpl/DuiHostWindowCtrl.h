#pragma once

#include <WindowsX.h>
#include <DuiCtrl\IDuiWindowCtrl.h>
#include <DuiImpl\DuiVector.h>
#include <DuiImpl\DuiString.h>
#include "DuiContainerCtrl.h"
#include <time.h>
#include <commctrl.h>
#include <olectl.h>

#pragma comment(lib,"comctl32.lib")
namespace DuiKit{;


typedef struct tagTIMERINFO
{
	IDuiControlCtrl* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
	BOOL bKilled;
} TIMERINFO;

class CDuiHostWindowCtrl : 
	public IDuiWindowCtrl,
	public CDuiContainerCtrl
{
public:
	CDuiHostWindowCtrl():
		m_nTransparent(255),
		m_hDcPaint(NULL),
		m_hDcOffscreen(NULL),
		m_hDcBackground(NULL),
		m_hbmpOffscreen(NULL),
		m_hbmpBackground(NULL),
		m_bShowDirtyRect(FALSE),
		m_OldWndProc(::DefWindowProc),
		m_bAlphaBackground(FALSE),
		m_bNeedUpdata(TRUE),
		m_bMouseTracking(FALSE),
		
		m_uTimerID(0x1000)
	  {
		  m_hWnd = NULL;
		  m_hParentWnd = NULL;
		  m_bSubclassed = FALSE;
		  m_dwStyle	= UI_WNDSTYLE_FRAME;
		  m_dwStyleEx =  WS_EX_APPWINDOW ;
	  }

	  ~CDuiHostWindowCtrl()
	  {
		  m_hWnd = NULL;
	  }

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiWindowCtrl, CTRL_WINDOW)
		DUI_DEFINE_INTERFACE(IDuiWindowCtrl, IIDuiWindowCtrl)
		DUI_DEFINE_INTERFACE(IDuiWindowCtrl, IIDuiHostWindowCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP

	DUI_BEGIN_SETATTRIBUTE(CDuiHostWindowCtrl)
		DUI_SETATTRIBUTE_STRING(super, m_strSuperClassName);
		DUI_SETATTRIBUTE_STRING(class, m_strWindowClassName);
		DUI_SETATTRIBUTE_STRING(title, m_strTitle);
		DUI_SETATTRIBUTE_SIZE(mininfo, m_minSize);
		DUI_SETATTRIBUTE_SIZE(maxinfo, m_maxSize);
		DUI_SETATTRIBUTE_SIZE(roundcorner, m_szRoundCorner);
		DUI_SETATTRIBUTE_RECT_FUNCTION(caption, SetCaptionRect);
		DUI_SETATTRIBUTE_RECT_FUNCTION(sizebox, SetSizeBox);
		DUI_SETATTRIBUTE_INT(alpha, m_nTransparent);
		DUI_SETATTRIBUTE_BOOL_FUNCTION(bktrans, SetBackgroundTransparent)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl);
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiHostWindowCtrl)
		DUI_MESSAGE_HANDLER(WM_CREATE, OnCreate)
		DUI_MESSAGE_HANDLER(WM_PAINT, OnPaint)
		DUI_MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		DUI_MESSAGE_HANDLER(WM_SIZE, OnSize)
		DUI_MESSAGE_HANDLER(WM_DESTROY, OnDestory)
		DUI_MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		DUI_MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		DUI_MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		DUI_MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	//	DUI_MESSAGE_HANDLER(WM_NCMOUSEMOVE, OnNcMouseMove)
		DUI_MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		DUI_MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		DUI_MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		DUI_MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		DUI_MESSAGE_HANDLER(WM_TIMER, OnTimer)
		DUI_MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		DUI_MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColorEdit)
		DUI_MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStaic)
		DUI_MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
		DUI_MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		DUI_MESSAGE_HANDLER(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_Z_OrderChanged, OnZ_OrderChanged)
		DUI_MESSAGE_HANDLER(DuiMsg_SizeChanged, OnSizeChanged)
	DUI_END_MSG_MAP();




	virtual HRESULT DoCreate(IDuiObject* Parent, IDuiCore* Core, IDuiBuilder* pBuilder)
	{
		if ( pBuilder && pBuilder->GetCallBack())
		{
			HWND hWnd = (HWND)pBuilder->GetCallBack()->OnBuilderCallBack(QueryInterface(IIDuiObject), BuilderCallBack_GetHostWindow);
			if ( hWnd )
			{
				Subclass(hWnd);
			}
		}

		if ( S_OK != CDuiContainerCtrl::DoCreate(Parent, Core,pBuilder))
		{
			return E_FAIL;
		}
		
		
		
// 		Create
// 		(
// 			m_hParentWnd, 
// 			m_strTitle, 
// 			GetWindowClassName(), 
// 			GetSuperClassName(), 
// 			CS_DBLCLKS,
// 			m_pos.top,
// 			m_pos.left,
// 			GetFixedWidth(),
// 			GetFixedHeight(),
// 			NULL
// 			);

		DoInitMessage();
		if( m_nTransparent != 255)
		{
			SetTransparent(m_nTransparent);
		}
		
		return  S_OK ;
	}


protected:

	BOOL Init()
	{
		LONG_PTR Style = ::GetWindowLongPtr(m_hWnd,GWL_STYLE);
		Style = Style &~WS_CAPTION &~WS_SYSMENU &~WS_SIZEBOX & ~WS_BORDER;;
		::SetWindowLongPtr(m_hWnd, GWL_STYLE, Style);

		::SetWindowPos(m_hWnd, NULL, m_pos.left, m_pos.top, m_pos.right - m_pos.left, \
			m_pos.bottom - m_pos.top, SWP_FRAMECHANGED);

		m_hDcPaint = ::GetDC(m_hWnd);

		return TRUE;
	}
protected:


	virtual INT GetTransparent()
	{
		return m_nTransparent;
	}

	virtual VOID SetTransparent(INT nOpacity)
	{
		if (nOpacity<0)
			m_nTransparent = 0;
		else if (nOpacity>255)
			m_nTransparent = 255;
		else
			m_nTransparent = nOpacity;
		if( m_hWnd != NULL ) {
			typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
			PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32)
			{
				fSetLayeredWindowAttributes = 
					(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				if( fSetLayeredWindowAttributes == NULL ) return;
			}

			DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
			DWORD dwNewStyle = dwStyle;
			if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
			else dwNewStyle &= ~WS_EX_LAYERED;
			if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
			fSetLayeredWindowAttributes(m_hWnd, 0, nOpacity, LWA_ALPHA);
		}
	}

	virtual VOID SetParentCtrl(IDuiControlCtrl* tCtrl)
	{
		if ( tCtrl)
		{
			for ( IDuiControlCtrl* pLoop = tCtrl ; pLoop ; pLoop = pLoop->GetParentCtrl())
			{
				IDuiWindowCtrl* tWndCtrl = (IDuiWindowCtrl*)pLoop->QueryInterface(IIDuiWindowCtrl);
				if ( tWndCtrl )
				{
					HWND hWnd = tWndCtrl->GetHWND();
					if ( IsWindow(hWnd) )
					{
						m_hParentWnd = hWnd;
						if ( m_hWnd && IsWindow(m_hWnd))
						{
							::SetParent(m_hWnd, hWnd);
						}
						
						break;
					}
				}
			}
		}
		CDuiContainerCtrl::SetParentCtrl(tCtrl);
	}

	virtual DWORD GetStyle()
	{
		return m_dwStyle;
	}

	virtual DWORD GetStyleEx()
	{
		return m_dwStyleEx;
	}

	virtual VOID SetStyle(DWORD dwClassStyle)
	{
		m_dwStyle = dwClassStyle;
	}

	virtual VOID SetStyleEx(DWORD dwClassStyleEx)
	{
		m_dwStyleEx = dwClassStyleEx;
	}


	virtual BOOL SetTimer(IDuiControlCtrl* Ctrl, UINT nTimerID, UINT uElapse)
	{
		ASSERT(Ctrl!=NULL);
		ASSERT(uElapse>0);
		for( int i = 0; i< m_timerInfoSet.Size(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_timerInfoSet[i]);
			if( pTimer->pSender == Ctrl
				&& pTimer->hWnd == m_hWnd
				&& pTimer->nLocalID == nTimerID ) {
					if( pTimer->bKilled == TRUE ) {
						if( ::SetTimer(m_hWnd, pTimer->uWinTimer, uElapse, NULL) ) {
							pTimer->bKilled = FALSE;
							return TRUE;
						}
						return FALSE;
					}
					return FALSE;
			}
		}

		m_uTimerID = (++m_uTimerID) % 0xFF;
		if( !::SetTimer(m_hWnd, m_uTimerID, uElapse, NULL) ) return FALSE;
		TIMERINFO* pTimer = new TIMERINFO;
		if( pTimer == NULL ) return FALSE;
		pTimer->hWnd = m_hWnd;
		pTimer->pSender = Ctrl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = m_uTimerID;
		pTimer->bKilled = FALSE;
		return m_timerInfoSet.Add(pTimer);
	}

	virtual BOOL KillTimer(IDuiControlCtrl* pControl, UINT nTimerID)
	{
		ASSERT(pControl!=NULL);
		for( int i = 0; i< m_timerInfoSet.Size(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_timerInfoSet[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWnd
				&& pTimer->nLocalID == nTimerID )
			{
				if( pTimer->bKilled == FALSE ) {
					if( ::IsWindow(m_hWnd) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = TRUE;
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	virtual VOID KillTimer(IDuiControlCtrl* pControl)
	{
		ASSERT(pControl!=NULL);
		int count = m_timerInfoSet.Size();
		for( int i = 0, j = 0; i < count; i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_timerInfoSet[i - j]);
			if( pTimer->pSender == pControl && pTimer->hWnd == m_hWnd ) {
				if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				delete pTimer;
				m_timerInfoSet.Delete(i - j);
				j++;
			}
		}
	}

	virtual VOID RemoveAllTimers()
	{
		for( int i = 0; i < m_timerInfoSet.Size(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_timerInfoSet[i]);
			if( pTimer->hWnd == m_hWnd ) {
				if( pTimer->bKilled == FALSE ) {
					if( ::IsWindow(m_hWnd) ) ::KillTimer(m_hWnd, pTimer->uWinTimer);
				}
				delete pTimer;
			}
		}

		m_timerInfoSet.Clear();
	}

	virtual VOID DoFinalMessage()
	{
		RemoveAllTimers();
		if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
		if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
		if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
		if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWnd, m_hDcPaint);

		__super::DoFinalMessage();
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

	}

	virtual VOID DoInitMessage()
	{
		if ( m_hWnd && IsWindow(m_hWnd))
		{
			
		}
	}


	virtual VOID SetPos(const CDuiRect& ThisPos)
	{
		MoveWindow(m_hWnd, ThisPos.left, ThisPos.top, ThisPos.GetWidth(), ThisPos.GetHeight(), TRUE);
	}

public://////////////////////////////////////////////////////////////////////////



	LRESULT OnCreate(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		
		Init();

		
// 		if( m_hUpdateRectPen == NULL ) {
// 			m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
// 			
// 			::InitCommonControls();
// 			::LoadLibrary(_T("msimg32.dll"));
// 		}
		
		return 0;
	}

	LRESULT OnPaint(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;

		RECT rcPaint = { 0 };
		if( !::GetUpdateRect(m_hWnd, &rcPaint, FALSE) ) 
			return true;

//		GrpMsg(GroupName, MsgLevel_Msg, _T("OnPaint:%s[%d,%d,%d,%d]"), GetName(), rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

		if( m_bNeedUpdata ) 
		{
			m_bNeedUpdata = FALSE;
			if( !::IsRectEmpty(&m_pos) ) 
			{
				if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
				if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
				if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
				if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
				m_hDcOffscreen = NULL;
				m_hDcBackground = NULL;
				m_hbmpOffscreen = NULL;
				m_hbmpBackground = NULL;
			}
		}
		
		if(m_hbmpOffscreen == NULL )
		{
			m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
			m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, m_pos.right - m_pos.left, m_pos.bottom - m_pos.top); 
			ASSERT(m_hDcOffscreen);
			ASSERT(m_hbmpOffscreen);
		}
		

		PAINTSTRUCT ps = { 0 };
		::BeginPaint(m_hWnd, &ps);
		if( TRUE )
		{
			HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
			int iSaveDC = ::SaveDC(m_hDcOffscreen);
			if( m_bAlphaBackground ) 
			{
				if( m_hbmpBackground == NULL ) 
				{
					m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);;
					m_hbmpBackground = ::CreateCompatibleBitmap(m_hDcPaint, m_pos.right - m_pos.left, m_pos.bottom - m_pos.top); 
					ASSERT(m_hDcBackground);
					ASSERT(m_hbmpBackground);
					::SelectObject(m_hDcBackground, m_hbmpBackground);
					::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				}
				else
				{
					::SelectObject(m_hDcBackground, m_hbmpBackground);
				}

				::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
			}

			DoPaint(m_hDcOffscreen, ps.rcPaint);
// 			for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
// 				CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
// 				pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
// 			}
			::RestoreDC(m_hDcOffscreen, iSaveDC);
			::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
				ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
			::SelectObject(m_hDcOffscreen, hOldBitmap);

			if( m_bShowDirtyRect ) {
				HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
				::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
				::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
				::SelectObject(ps.hdc, hOldPen);
			}
		}
		else
		{
			int iSaveDC = ::SaveDC(ps.hdc);
			DoPaint(ps.hdc, ps.rcPaint);
			::RestoreDC(ps.hdc, iSaveDC);
		}
		::EndPaint(m_hWnd, &ps);

		if( m_bNeedUpdata ) 
		{
			::InvalidateRect(m_hWnd, NULL, FALSE);
		}
		return 0;
	}

	LRESULT OnEraseBkgnd(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		return 1;
	}

	LRESULT OnSize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiRect rcClient;
		GetClientRect(m_hWnd, &rcClient);
		CDuiContainerCtrl::SetPos(rcClient);
	
		bHandle = FALSE;
		SIZE szRoundCorner = GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
		if( !::IsIconic(m_hWnd) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(m_hWnd, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(m_hWnd, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
#endif
		
		Invalidate();
		return 0;
	}

	LRESULT OnDestory(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return 0;
	}

	LRESULT OnGetMinMaxInfo(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		CDuiRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		// 计算最大化时，正确的原点坐标
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;

		if (GetMaxWidth())
			lpMMI->ptMaxTrackSize.x = GetMaxWidth();
		else
			lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
	
		if (GetMaxHeight())
			lpMMI->ptMaxTrackSize.y = GetMaxHeight();
		else
			lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();
		
		lpMMI->ptMinTrackSize.x = GetMinWidth();
		lpMMI->ptMinTrackSize.y = GetMinHeight();

		bHandle = FALSE;
		return 0;
	}

	LRESULT OnMouseHover(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		m_bMouseTracking = FALSE;
		return SendMessage(this, DuiMsg_MouseHover, wParam, lParam);
	}

	LRESULT OnMouseLeave(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		 if( m_bMouseTracking ) 
			 SendMessage(this, DuiMsg_MouseLeave, 0, (LPARAM) -1);

 		m_bMouseTracking = FALSE;
		return 0;
	}

	LRESULT OnNcMouseMove(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return ::SendMessage(m_hWnd, WM_MOUSEMOVE, wParam, lParam);
	}

	LRESULT OnMouseMove(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		//触发 OnMouseHover 与 OnMouseLeave
		bHandle = TRUE;
//		GrpMsg(GroupName, MsgLevel_Msg, _T("OnMouseMove:%s[%d,%d]"),GetName(), GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) );

		m_MouseLastPoint.x = GET_X_LPARAM(lParam);
		m_MouseLastPoint.y = GET_Y_LPARAM(lParam);
		if ( !m_bMouseTracking )
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			tme.dwHoverTime = 400UL;
			_TrackMouseEvent(&tme);
			 m_bMouseTracking = TRUE;
		}
	
		return SendMessage(this, DuiMsg_MouseMove, wParam, lParam);
	}

	LRESULT OnLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		::SetFocus(m_hWnd);
		SetCapture(m_hWnd);
		return SendMessage(this, DuiMsg_LButtonDown, wParam, lParam);
	}

	
	LRESULT OnLButtonUp(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		ReleaseCapture();
		return SendMessage(this, DuiMsg_LButtonUp, wParam, lParam);
	}

	LRESULT OnLButtonDblClk(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		::SetFocus(m_hWnd);
		SetCapture(m_hWnd);
		return SendMessage(this, DuiMsg_LButtonDblClk, wParam, lParam);
	}

	LRESULT OnNcHitTest(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		//return FALSE;
		bHandle = FALSE;
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(m_hWnd, &pt);

		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);

		if( !::IsZoomed(m_hWnd) )
		{
			RECT rcSizeBox = GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		
		RECT rcCaption = GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			IDuiControlCtrl* pControl = GetChild(pt.x, pt.y);
			if( pControl )
			{
				if( !pControl->QueryInterface(IIDuiButtonCtrl) &&
					!pControl->QueryInterface(IIDuiScrollCtrl))
				{
					return HTCAPTION;
				}
			}
		
		}
		
		return HTCLIENT;
	}

	LRESULT OnInitialize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiRect rcClient;
		CDuiRect rcPos;

		if ( (GetWindowStyle(m_hWnd) & WS_CHILD) == 0)
		{
			LPCWSTR lpszName = GetName();
			CDuiRect clientRect;
			GetClientRect(m_hWnd, &clientRect);
			CDuiContainerCtrl::SetPos(clientRect);
		}
		

		SendMessage(this,DuiMsg_Z_OrderChanged);
		ShowWindow(m_hWnd, GetVisable() ? SW_SHOW : SW_HIDE );
		return 0;
	}

	LRESULT OnSizeChanged(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		//::MoveWindow(m_hWnd, GetAxisX(), GetAxisY(), GetWidth(), GetHeight(), TRUE);
		return 0;
	}

	LRESULT OnCommand(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( lParam == 0 ) return 0;
		HWND hWndChild = (HWND) lParam;
		return ::SendMessage(hWndChild, OCM__BASE + nMsg, wParam, lParam);
	}

	LRESULT OnCtlColorEdit(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		if( lParam == 0 ) return 0;
		bHandle = FALSE;
		HWND hWndChild = (HWND) lParam;
		return ::SendMessage(hWndChild, OCM__BASE + nMsg, wParam, lParam);
	}

	LRESULT OnCtlColorStaic(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( lParam == 0 ) return 0;
		HWND hWndChild = (HWND) lParam;
		bHandle = FALSE;
		return ::SendMessage(hWndChild, OCM__BASE + nMsg, wParam, lParam);
	}

	LRESULT OnPrintClient(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		HDC hDC = (HDC) wParam;
		int save = ::SaveDC(hDC);
		DoPaint(hDC, rcClient);
		// Check for traversing children. The crux is that WM_PRINT will assume
		// that the DC is positioned at frame coordinates and will paint the child
		// control at the wrong position. We'll simulate the entire thing instead.
		if( (lParam & PRF_CHILDREN) != 0 ) {
			HWND hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
			while( hWndChild != NULL ) {
				RECT rcPos = { 0 };
				::GetWindowRect(hWndChild, &rcPos);
				::MapWindowPoints(HWND_DESKTOP, m_hWnd, reinterpret_cast<LPPOINT>(&rcPos), 2);
				::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
				// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
				//       since the latter will not print the nonclient correctly for
				//       EDIT controls.
				::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
				hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
			}
		}
		::RestoreDC(hDC, save);

		return 0;
	}

	LRESULT OnSetCursor(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( LOWORD(lParam) != HTCLIENT ) return 0;
		
		POINT pt = { 0 };
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd, &pt);
		IDuiControlCtrl* pControl = GetChild(pt.x, pt.y, TRUE);
		if( pControl == NULL ) return 0;


		if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) return 0;
		SendMessage(pControl, DuiMsg_SetCursor, wParam, lParam, NULL);
		bHandle  = FALSE;
		return 0;
	}

	LRESULT OnZ_OrderChanged(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		//暂时就放在这里了
		DuiMsg duiMsg = {tCtrl, tToCtrl, nMsg, wParam, lParam };
		__super::DoMessage(duiMsg, bHandle);

		bHandle = FALSE;
		IDuiWindowCtrl* tWndPre = NULL;
		INT nCount = m_ChildControlArray.Size();
		for ( INT nLoop = 0 ; nLoop < nCount ; nLoop++)
		{
			IDuiWindowCtrl* tWnd = (IDuiWindowCtrl*)m_ChildControlArray[nLoop]->QueryInterface(IIDuiWindowCtrl);
			if ( !tWndPre )
				tWndPre = tWnd;
			else{
				SetWindowPos(tWndPre->GetHWND(), tWnd->GetHWND(),0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_FRAMECHANGED);
				tWndPre = tWnd;
			}
		}
		return 0;
	}

	LRESULT OnTimer(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		for( int i = 0; i < m_timerInfoSet.Size(); i++ ) 
		{
			const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_timerInfoSet[i]);
			if( pTimer->hWnd == m_hWnd && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
				pTimer->pSender->SendMessage(pTimer->pSender, DuiMsg_Timer, pTimer->nLocalID);
				break;
			}
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	virtual BOOL PostMessage(const DuiMsg& duiMsg)
	{
		{
			AUTOLOCK_CS(m_PostDuiMsgArray);
			m_PostDuiMsgArray.Add(duiMsg);
		}

		return ::PostMessageA(GetHWND(), DuiMsg_RefreshPost, 0 , 0);
	}

	virtual HWND GetHWND()
	{
		return m_hWnd;
	}

	virtual HDC  GetHDC()
	{
		return m_hDcPaint;
	}

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
		case DuiMsg_RefreshPost:
			OnDuiMsgRefresh();
			return 0;
		}
	

		DuiMsg duiMsg = { this, this, uMsg, wParam, lParam, NULL};
		BOOL bHandle = TRUE;
		LRESULT lResult = DoPreMessage(duiMsg, bHandle);
		if ( !bHandle )
			return lResult;

		lResult = DoMessage(duiMsg, bHandle);
		if ( !bHandle )
			return lResult;
		

		return CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT CDuiHostWindowCtrl::OnDuiMsgRefresh()
	{
		AUTOLOCK_CS(m_PostDuiMsgArray);
		for (INT dwLoop = 0; dwLoop < m_PostDuiMsgArray.Size() ;dwLoop ++ )
		{
			DuiMsg duiMsg = m_PostDuiMsgArray[dwLoop];
			CDuiControlCtrl* tCtrl = static_cast<CDuiControlCtrl*>(duiMsg.CtrlTo);
			if ( tCtrl )
			{
				BOOL bHandle = TRUE;
				tCtrl->DoPreMessage(duiMsg,bHandle);
				if ( !bHandle )
					return 0;
			
				DoMessage(duiMsg,bHandle);
			}
		}

		return 0;
	}

	VOID Unsubclass()
	{
		ASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return;
		if( !m_bSubclassed ) return;
		SubclassWindow(m_hWnd, m_OldWndProc);
		m_OldWndProc = ::DefWindowProc;
		m_bSubclassed = FALSE;
	}

	HWND Subclass(HWND hWnd)
	{
		ASSERT(::IsWindow(hWnd));
		//ASSERT(m_hWnd==NULL);

		::SetProp(hWnd, _T("WndX_WndProc"), (HANDLE) this);
		m_OldWndProc = SubclassWindow(hWnd, __WndProc);

		if( m_OldWndProc == NULL ) 
			return NULL;

		m_bSubclassed = TRUE;
		m_hWnd = hWnd;
		Init();
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
		return m_hWnd;
	}

	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CDuiHostWindowCtrl* pThis = NULL;
		if( uMsg == WM_NCCREATE ) {
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pThis = static_cast<CDuiHostWindowCtrl*>(lpcs->lpCreateParams);
			::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
			pThis->m_hWnd = hWnd;
		} 
		else {
			pThis = reinterpret_cast<CDuiHostWindowCtrl*>(::GetProp(hWnd, _T("WndX")));
			if( uMsg == WM_NCDESTROY && pThis != NULL ) {
				LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
				if( pThis->m_bSubclassed ) pThis->Unsubclass();
				::SetProp(hWnd, _T("WndX"), NULL);
				pThis->m_hWnd = NULL;
				return lRes;
			}
		}
		if( pThis != NULL ) {
			return pThis->MessageHandler(uMsg, wParam, lParam);
		} 
		else {
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CDuiHostWindowCtrl* pThis = NULL;
		if( uMsg == WM_NCCREATE ) {
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pThis = static_cast<CDuiHostWindowCtrl*>(lpcs->lpCreateParams);
			pThis->m_hWnd = hWnd;
			::SetProp(hWnd, _T("WndX_WndProc"), (HANDLE)lpcs->lpCreateParams);
		} 
		else {

			pThis = reinterpret_cast<CDuiHostWindowCtrl*>(::GetProp(hWnd, _T("WndX_WndProc")));
			if( uMsg == WM_NCDESTROY && pThis != NULL ) {
				LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
				::SetProp(hWnd, _T("WndX_WndProc"), NULL);
				if( pThis->m_bSubclassed ) pThis->Unsubclass();
				pThis->m_hWnd = NULL;
				return lRes;
			}
		}
		if( pThis != NULL ) {
			return pThis->MessageHandler(uMsg, wParam, lParam);
		} 
		else {
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	virtual HWND Create(
		HWND hwndParent, 
		LPCTSTR pstrName, 

		LPCWSTR lpszWindowClassName,
		LPCWSTR lpszSuperClassName = NULL,
		DWORD	dwClassStyle = CS_DBLCLKS,
		int x = CW_USEDEFAULT, 
		int y = CW_USEDEFAULT, 
		int cx = CW_USEDEFAULT, 
		int cy = CW_USEDEFAULT, 
		HMENU hMenu = NULL)
	{
		m_dwClassStyle = dwClassStyle;
		if( wcslen(GetSuperClassName()) != 0 && !RegisterSuperclass() ) return NULL;
		if( wcslen(GetSuperClassName()) == 0 && !RegisterWindowClass() ) return NULL;

		IDuiWindowCtrl* tWndCtrl = (IDuiWindowCtrl*)this->QueryInterface(IIDuiWindowCtrl);

		INT nStyle = GetStyle();
 		if ( hwndParent )
 			nStyle = nStyle | WS_CHILD;
		
		m_hWnd = ::CreateWindowEx(GetStyleEx(), GetWindowClassName(), pstrName, nStyle, x, y, cx, cy, hwndParent, hMenu, GetDuiCore()->GetModuleInstace(), tWndCtrl);
		if ( m_hParentWnd )
		{
			::SetParent(m_hWnd, m_hParentWnd);
		}

		return NULL;
	}

	virtual DWORD DoModal()
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT( m_pDuiCore);

		CDuiCore* pCore = static_cast<CDuiCore*>(m_pDuiCore);

		DWORD nRet = 0;
		HWND hWndParent = GetWindowOwner(m_hWnd);
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);
		::EnableWindow(hWndParent, FALSE);
		MSG msg = { 0 };
		while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
			if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
				nRet = msg.wParam;
				::EnableWindow(hWndParent, TRUE);
				::SetFocus(hWndParent);
			}
			if( !pCore->TranslateMessage(msg) ) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if( msg.message == WM_QUIT ) break;
		}
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
		if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
		return nRet;
	}

	virtual VOID Close(DWORD dwExitCode)
	{
		ASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return;
		::PostMessage(m_hWnd, WM_CLOSE, (WPARAM)dwExitCode, 0L);
	}

	virtual BOOL Invalidate()
	{
		m_bNeedUpdata = TRUE;
		return ::InvalidateRect(m_hWnd, &GetPos(), FALSE);
	}

	virtual LPCWSTR GetSuperClassName()
	{
		return m_strSuperClassName;
	}

	virtual LPCWSTR GetWindowClassName()
	{
		return m_strWindowClassName;
	}

	virtual VOID SetSizeBox(const CDuiRect& sizeBox)
	{
		m_sizeBox = sizeBox;
	}
	virtual CDuiRect GetSizeBox()
	{
		return m_sizeBox;
	}

	virtual VOID SetCaptionRect(const CDuiRect& caption)
	{
		m_captionRect = caption;
	}

	virtual CDuiRect GetCaptionRect()
	{
		return m_captionRect;
	}

	virtual VOID SetRoundCorner(const CDuiSize& corner )
	{
		m_szRoundCorner = corner;
	}

	virtual CDuiSize GetRoundCorner( )
	{
		return m_szRoundCorner;
	}

	virtual VOID SetBackgroundTransparent(BOOL bTrans)
	{
		m_bAlphaBackground = bTrans;
	}

	virtual BOOL GetBackgroundTransparent()
	{
		return m_bAlphaBackground;
	}

	virtual VOID SetShowDirtyRect(BOOL bShowDirty)
	{
		m_bShowDirtyRect = bShowDirty;
	}

	virtual BOOL GetShowDirtyRect()
	{
		return m_bShowDirtyRect;
	}

	virtual VOID CenterWindow()
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
		RECT rcDlg = { 0 };
		::GetWindowRect(m_hWnd, &rcDlg);
		RECT rcArea = { 0 };
		RECT rcCenter = { 0 };
		HWND hWnd=m_hWnd;
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		if (hWndCenter!=NULL)
			hWnd=hWndCenter;

		// 处理多显示器模式下屏幕居中
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rcArea = oMonitor.rcWork;

		if( hWndCenter == NULL )
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if( xLeft < rcArea.left ) xLeft = rcArea.left;
		else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
		if( yTop < rcArea.top ) yTop = rcArea.top;
		else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	virtual BOOL MapPoint(IDuiWindowCtrl* WndCtrl, CDuiPoint& Point) 
	{
		RASSERT(WndCtrl, FALSE);
		return ::MapWindowPoints(this->GetHWND(), WndCtrl->GetHWND(), (LPPOINT)&Point, 1) != 0;
	}

	virtual BOOL MapRect(IDuiWindowCtrl* WndCtrl, CDuiRect& rect)
	{
		RASSERT(WndCtrl, FALSE);
		return ::MapWindowPoints(this->GetHWND(), WndCtrl->GetHWND(), (LPPOINT)&rect, 2) != 0;
	}
	/*
	virtual VOID SetWidth(DWORD dwWidth)
	{
		if ( GetWidth() != dwWidth)
		{
			__super::SetWidth(dwWidth);
			if ( m_hWnd && IsWindow(m_hWnd))
			{
				CDuiPoint ClinetPoint;
				ClientToScreen(m_hWnd, &ClinetPoint);
				::MoveWindow(m_hWnd, ClinetPoint.x, ClinetPoint.y, dwWidth, GetHeight(), TRUE);
			}
		}
	}
	*/

	/*
	virtual VOID SetHeight(DWORD dwHeight)
	{
		if ( GetHeight() != dwHeight )
		{
			__super::SetHeight(dwHeight);
			if ( m_hWnd && IsWindow(m_hWnd))
			{
				CDuiPoint ClinetPoint;
				ClientToScreen(m_hWnd, &ClinetPoint);
				::MoveWindow(m_hWnd, ClinetPoint.x, ClinetPoint.y, GetHeight(), dwHeight, TRUE);
			}
		}
	}
	*/


// 	virtual DWORD GetWidth()
// 	{
// 		if ( m_hWnd && IsWindow(m_hWnd))
// 		{
// 			CDuiRect rcClient;
// 			::GetClientRect(m_hWnd, &rcClient);
// 			return rcClient.GetWidth();
// 		}
// 		return __super::GetWidth();
// 	}

// 	virtual DWORD GetHeight()
// 	{
// 		if ( m_hWnd && IsWindow(m_hWnd))
// 		{
// 			CDuiRect rcClient;
// 			::GetClientRect(m_hWnd, &rcClient);
// 			return rcClient.GetHeight();
// 		}
// 		
// 		return __super::GetHeight();
// 	}


public:
	BOOL RegisterWindowClass()
	{
		if ( !m_strWindowClassName.Length() )
		{
			DWORD dwCount = GetTickCount();
			DWORD dwParent = 0;
			if ( GetParentCtrl() && GetWindowClassName() && wcslen(GetWindowClassName()))
			{
				IDuiContainerCtrl* tParent = (IDuiContainerCtrl*)(GetParentCtrl()->QueryInterface(IIDuiContainerCtrl));
				if ( tParent )
				{
					dwCount = tParent->GetChildCount();
				}

				dwParent = (DWORD)tParent;
			}
			
			srand((int)time(NULL));
			m_strWindowClassName.Format(_T("DuiKit_%d_%d_%d"),dwParent,dwCount , rand());
		}

		WNDCLASS wc		= { 0 };
		wc.style		= m_dwClassStyle;
		wc.cbClsExtra	= 0;
		wc.cbWndExtra	= 0;
		wc.hIcon		= NULL;
		wc.lpfnWndProc	= CDuiHostWindowCtrl::__WndProc;
		wc.hInstance	= GetDuiCore()->GetModuleInstace();
		wc.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = GetWindowClassName();
		ATOM atom = ::RegisterClass(&wc);

		if (WS_OVERLAPPEDWINDOW != ERROR_CLASS_ALREADY_EXISTS && atom == NULL)
		{
			return FALSE;
		}

		return TRUE;
	}

	public:
	BOOL RegisterSuperclass()
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) ) {
			if( !::GetClassInfoEx(GetDuiCore()->GetModuleInstace(), GetSuperClassName(), &wc) ) {
				ASSERT(!"Unable to locate window class");
				return NULL;
			}
		}
		m_OldWndProc = wc.lpfnWndProc;
		wc.lpfnWndProc = CDuiHostWindowCtrl::__WndProc;
		wc.hInstance = GetDuiCore()->GetModuleInstace();
		wc.lpszClassName = GetWindowClassName();
		ATOM ret = ::RegisterClassEx(&wc);
		ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

public:
	HWND m_hWnd;
	HWND m_hParentWnd;
	WNDPROC m_OldWndProc;
	HDC m_hDcPaint;
private:
	
	HDC m_hDcBackground;
	HBITMAP m_hbmpBackground;
	HBITMAP m_hbmpOffscreen;
	LPBYTE m_pBmpBackgroundBits;
	HDC m_hDcOffscreen;
	HPEN m_hUpdateRectPen;
protected:
	
	
	
	BOOL m_bSubclassed ;
	CDuiSize m_size;
	CDuiSize m_minSize;
	CDuiSize m_maxSize;
	CDuiSize m_szRoundCorner;
	CDuiRect m_captionRect;
	CDuiString m_strTitle;
	DWORD m_dwStyle;
	DWORD m_dwStyleEx;
	DWORD m_dwClassStyle;
	INT	  m_nTransparent;
	BOOL m_bAlphaBackground;
	CDuiRect m_sizeBox;
	BOOL	m_bShowDirtyRect;
	BOOL	m_bNeedUpdata;
	BOOL	m_bMouseTracking;
	CDuiPoint m_MouseLastPoint;
private:
	typedef CDuiVector<DuiMsg> CPostDuiMsgArray;
	CPostDuiMsgArray m_PostDuiMsgArray;
	DECLARE_AUTOLOCK_CS(m_PostDuiMsgArray);

	typedef CDuiVector<TIMERINFO*> CTimerInfoSet;
	CTimerInfoSet m_timerInfoSet;
	UINT m_uTimerID;
	CDuiString m_strSuperClassName;
	CDuiString m_strWindowClassName;
};







};//namespace DuiKit{;