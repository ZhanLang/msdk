#ifndef __NF_RESIZE_H__
#define __NF_RESIZE_H__

#pragma once

#ifndef __ATLFRAME_H__
#include <altframe.h>
#endif	

#ifndef __ATLMISC_H__
#include <atlmisc.h>
#endif

#ifndef __ATLGDI_H__	
#include <atlgdi.h>
#endif

#ifndef __ATLGDIX_H__
#include "atlgdix.h" //use Bjarke Viksoe's CMemDC
#endif

/////////////////////////////////////////////////////////////////////////////
// Classes in this file
//
// CNoFlickerDialogResize<T>

namespace WTL
{

template <class T>
class CNoFlickerDialogResize : public CDialogResize<T>
{
public:
	typedef CDialogResize<T> baseclass;

	POINT m_ptMaxTrackSize;

	CNoFlickerDialogResize() : m_bDisableFlicker(true),m_bIsXP(false)
	{
		m_ptMaxTrackSize.x = -1;
		m_ptMaxTrackSize.y = -1;
	}

	// Message map and handlers
	BEGIN_MSG_MAP(CNoFlickerDialogResize)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkGnd)
		CHAIN_MSG_MAP(baseclass)
	END_MSG_MAP()

	// Operations

	//override baseclass to setup correctly for anti-flicker
	void DlgResize_Init(bool bAddGripper = true, bool bUseMinTrackSize = true, DWORD dwForceStyle = WS_THICKFRAME | WS_CLIPCHILDREN)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		SetXPStyleGroupBoxes(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion >= 1);

		//remove visible and add transparent to all groupboxes
		DisableFlicker(true);

		dwForceStyle |= WS_CLIPCHILDREN; //force WS_CLIPCHILDREN
		baseclass::DlgResize_Init(bAddGripper,bUseMinTrackSize,dwForceStyle);
	}

	//set false to use standard resize methods, add WS_CLIPCHILDREN back to window
	//returns original setting.
	//default initialisation would be an implied DisableFlicker(true)
	bool DisableFlicker(bool bDisableFlicker)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		if(bDisableFlicker)
			SetAllGroupboxStyles(WS_VISIBLE,0,WS_EX_TRANSPARENT,0);
		else
			SetAllGroupboxStyles(0,WS_VISIBLE,0,WS_EX_TRANSPARENT);

		bool bhold = m_bDisableFlicker;
		m_bDisableFlicker = bDisableFlicker;
		return bhold;
	}

	void SetXPStyleGroupBoxes(bool b)
	{
		m_bIsXP = b;
	}

	bool GetXPStyleGroupBoxes()
	{
		return m_bIsXP;
	}

	//enumerates all groupboxes, and can add/remove any styles/exstyles
	void SetAllGroupboxStyles(DWORD dwRemove,DWORD dwAdd,DWORD dwRemoveEx=0,DWORD dwAddEx=0) 
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		_EnumSetAllGroupboxStyles esgc(dwRemove,dwAdd,dwRemoveEx,dwAddEx);
		//Walk all groupboxes and set style
		EnumChildWindows(pT->m_hWnd,EnumSetAllGroupboxStyles,(LPARAM)&esgc);
	}

	enum
	{
		NOLIMIT = -1, //if used, use for both params.
		NOSIZECHANGE = -2 //use this param to set one param without affecting the other
	};

	//Set mintracksize.
	//If second parameter of DlgResize_Init is true, then initial window size will be smallest size.
	//Call this func to set/change it regardless of DlgResize_Init use.
	void SetMinTrackSize(long x, long y)
	{
		if(x == NOLIMIT || y == NOLIMIT) { x = NOLIMIT; y = NOLIMIT; }
		if(x != NOSIZECHANGE) m_ptMinTrackSize.x = x;
		if(y != NOSIZECHANGE) m_ptMinTrackSize.y = y;
	}

	//Set maximum size. If not set, window can be maximized to full screen.
	//if set window can only be dragged/maximized to this size.
	void SetMaxTrackSize(long x=NOLIMIT, long y=NOLIMIT)
	{
		if(x == NOLIMIT || y == NOLIMIT) { x = NOLIMIT; y = NOLIMIT; }
		if(x != NOSIZECHANGE) m_ptMaxTrackSize.x = x;
		if(y != NOSIZECHANGE) m_ptMaxTrackSize.y = y;
	}

protected:

	bool m_bDisableFlicker;
	bool m_bIsXP;

	//Handle maximize sys command. If we have a maxTrackSize, use it, else allow default handling.
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam == SC_MAXIMIZE && (m_ptMaxTrackSize.x != -1 && m_ptMaxTrackSize.y != -1))
		{
			T* pT = static_cast<T*>(this);

			CRect rect;
			pT->GetWindowRect(rect);

			if(rect.Width() != m_ptMaxTrackSize.x && rect.Height() != m_ptMaxTrackSize.y)
			{
				pT->SetWindowPos(NULL,0,0,m_ptMaxTrackSize.x,m_ptMaxTrackSize.y,SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
				pT->CenterWindow();
			}
			return 0;
		}

		bHandled=FALSE;
		return 1;
	}

	LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL&)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		if(m_ptMinTrackSize.x != -1 && m_ptMinTrackSize.y != -1)
			lpMMI->ptMinTrackSize =  m_ptMinTrackSize;

		if(m_ptMaxTrackSize.x != -1 && m_ptMaxTrackSize.y != -1)
			lpMMI->ptMaxTrackSize =  m_ptMaxTrackSize;

		return 0;
	}

	LRESULT OnEraseBkGnd(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
	{
		if(m_bDisableFlicker)
		{
			T* pT = static_cast<T*>(this);
			ATLASSERT(::IsWindow(pT->m_hWnd));

			CRect clientrect;
			pT->GetClientRect(clientrect);
			CMemDC memDC((HDC)wParam,clientrect);

			_EnumExcludeChildRegionData ecrd;
			ecrd.ParentWindow.Attach(pT->m_hWnd);
			ecrd.dc = memDC;
			ecrd.m_bIsXP = m_bIsXP;

			ecrd.rgn.CreateRectRgnIndirect(clientrect);

			//Walk all children, exclude from fill region, except groupboxes
			EnumChildWindows(pT->m_hWnd,EnumExcludeChildRegion,(LPARAM)&ecrd);

			HBRUSH backgroundbrush = (HBRUSH)GetClassLong(pT->m_hWnd,GCL_HBRBACKGROUND);
			if(!backgroundbrush) backgroundbrush = GetSysColorBrush(COLOR_BTNFACE);
			ecrd.dc.FillRgn(ecrd.rgn, backgroundbrush);

			//Walk controls again, and paint all groupboxes
			EnumChildWindows(pT->m_hWnd,EnumPaintGroupboxes,(LPARAM)&ecrd);

			bHandled=TRUE;
			return 1;
		}

		bHandled=FALSE;
		return 0;
	}

	static bool IsGroupBox(CWindow &win)
	{
		long Style = win.GetStyle();

		if((Style & 0xF) == BS_GROUPBOX)
		{
			TCHAR sclass[MAX_PATH];
			GetClassName(win,sclass,MAX_PATH);
			if(_tcscmp(sclass,_T("Button"))==0)
			{
				return true;
			}
		}
		return false;
	}

	static BOOL __stdcall EnumExcludeChildRegion(HWND hwnd, LPARAM lParam)
	{
		_EnumExcludeChildRegionData* pecrd = (_EnumExcludeChildRegionData*)lParam;
		CRect controlrect;
		CRgn childcontrolrgn;
		CWindow childcontrol(hwnd);

		//skip any non-visible controls - (especially the gripper which is not shown when maximized)
		if(!childcontrol.IsWindowVisible())
			return TRUE;

		if(!IsGroupBox(childcontrol))
		{
			childcontrol.GetWindowRect(controlrect);
			::MapWindowPoints(HWND_DESKTOP, pecrd->ParentWindow, (LPPOINT)(LPRECT)controlrect, (sizeof(RECT)/sizeof(POINT)));
			childcontrolrgn.CreateRectRgnIndirect(controlrect);
			pecrd->rgn.CombineRgn(childcontrolrgn,RGN_XOR);
		}

		return TRUE;
	}

	static BOOL __stdcall EnumPaintGroupboxes(HWND hwnd, LPARAM lParam)
	{
		_EnumExcludeChildRegionData* pecrd = (_EnumExcludeChildRegionData*)lParam;
		CRect controlrect;
		CWindow childcontrol(hwnd);

		if(IsGroupBox(childcontrol))
		{
			int nSaveDC = pecrd->dc.SaveDC();

			//paint groupbox manually
			childcontrol.GetWindowRect(controlrect);
			::MapWindowPoints(HWND_DESKTOP, pecrd->ParentWindow, (LPPOINT)(LPRECT)controlrect, (sizeof(RECT)/sizeof(POINT)));

			CFontHandle font = childcontrol.GetFont();

			pecrd->dc.SelectFont(font);
			pecrd->dc.SetMapMode(MM_TEXT);
			pecrd->dc.SelectBrush(GetSysColorBrush(COLOR_BTNFACE));

			TCHAR grptext[MAX_PATH];
			childcontrol.GetWindowText(grptext,MAX_PATH);

			CRect fontsizerect(0,0,0,0);
			pecrd->dc.DrawText(grptext,-1,fontsizerect,DT_SINGLELINE|DT_LEFT|DT_CALCRECT);

			CRect framerect(controlrect);
			framerect.top += (fontsizerect.Height())/2;
			long Style = childcontrol.GetStyle();

			//Draw frame
			if(pecrd->m_bIsXP)
			{
				//XP default theme style - if XP is using themes, we'll still render in standard theme style.
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
				HPEN o_pen = pecrd->dc.SelectPen(pen);
				POINT corner = { 5,5 };
				pecrd->dc.RoundRect(framerect,corner);
				pecrd->dc.SelectPen(o_pen);
			}
			else
			{
				//non XP
				if((Style & 0xF000) == BS_FLAT)
				{
					pecrd->dc.Draw3dRect(framerect,RGB(0,0,0),RGB(0,0,0));
					framerect.DeflateRect(1,1);
					pecrd->dc.Draw3dRect(framerect,RGB(255,255,255),RGB(255,255,255));
				}
				else
				{
					pecrd->dc.Draw3dRect(framerect,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHILIGHT));
					framerect.DeflateRect(1,1);
					pecrd->dc.Draw3dRect(framerect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
				}
			}

			if(_tcslen(grptext))
			{
				//Draw Caption
				pecrd->dc.SetBkMode(OPAQUE);
				pecrd->dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
				CRect fontrect(controlrect);
				fontrect.bottom = controlrect.top+fontsizerect.Height();

				if((Style & 0xF00) == BS_RIGHT)
				{
					fontrect.right -= 7;
					fontrect.left = fontrect.right - fontsizerect.Width();
				}
				else if((Style & 0xF00) == BS_CENTER)
				{
					fontrect.left += (controlrect.Width()-fontsizerect.Width())/2;
					fontrect.right = fontrect.left + fontsizerect.Width();
				}
				else //BS_LEFT or default
				{
					fontrect.left += 7;
					fontrect.right = fontrect.left + fontsizerect.Width();
				}

				pecrd->dc.DrawText(grptext,-1,fontrect,DT_SINGLELINE|DT_LEFT);
			}

			pecrd->dc.RestoreDC(nSaveDC);
		}
		return TRUE;
	}

	static BOOL __stdcall EnumSetAllGroupboxStyles(HWND hwnd, LPARAM lParam)
	{
		_EnumSetAllGroupboxStyles* pgs = (_EnumSetAllGroupboxStyles*)lParam;

		CWindow groupboxcontrol(hwnd);
		if(IsGroupBox(groupboxcontrol))
		{
			if (pgs->dwRemove || pgs->dwAdd)		groupboxcontrol.ModifyStyle(pgs->dwRemove,pgs->dwAdd);
			if (pgs->dwRemoveEx || pgs->dwAddEx)	groupboxcontrol.ModifyStyleEx(pgs->dwRemoveEx,pgs->dwAddEx);
		}
		return TRUE;
	}

private:

	struct _EnumExcludeChildRegionData
	{
		CRgn rgn;
		CWindow ParentWindow;
		CDCHandle dc;
		bool m_bIsXP;
	};

	struct _EnumSetAllGroupboxStyles
	{
		_EnumSetAllGroupboxStyles(DWORD p_dwRemove=0,DWORD p_dwAdd=0,DWORD p_dwRemoveEx=0,DWORD p_dwAddEx=0) : 
			dwRemove(p_dwRemove), dwAdd(p_dwAdd), dwRemoveEx(p_dwRemoveEx), dwAddEx(p_dwAddEx) {}

		DWORD dwRemove; 
		DWORD dwAdd; 
		DWORD dwRemoveEx; 
		DWORD dwAddEx; 
	};
};

}; //namespace WTL

#endif // __NF_RESIZE_H__