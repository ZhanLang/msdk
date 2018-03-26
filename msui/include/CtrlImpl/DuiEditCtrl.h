#pragma once
#include <DuiCtrl/IDuiEditCtrl.h>
#include "DuiWindowCtrl.h"

#include <WindowsX.h>
#include <OleCtl.h>
#define CARET_INPUT_TIMER_ID		1001 //输入光标的定时器

namespace DuiKit{;
class CDuiEditCtrl : 
	public IDuiEditCtrl,
	public CDuiWindowCtrl
{
	
public:
	CDuiEditCtrl(): 
		m_uMaxChar(255), 
		m_bReadOnly(FALSE), 
		m_bPasswordMode(FALSE), 
		m_cPasswordChar(L'*'), 
		m_uButtonState(0), 
		m_dwEditbkColor(0xFFFFFFFF), 
		m_iWindowStyls(0),
		m_bEnabled(TRUE),
		m_hBkBrush(NULL),
		m_dwTextColor(0),
		m_bIsDefText(FALSE),
		m_dwDefaultTextColor(RGB(128,128,128)),
		m_dwNormalBorderColor(/*0xFDC860*/ -1),
		m_dwHotBorderColor(/*0xFDC860*/-1),
		m_dwFocusedBorderColor(/*GetSysColor(COLOR_BTNFACE)*/-1),
		m_dwDisabledBorderColor(/*GetSysColor(COLOR_BTNFACE)*/-1),
		m_hNormalBorderBrush(NULL),
		m_hHotBorderBrush(NULL),
		m_hFocusedBorderBrush(NULL),
		m_hDisabledBorderBrush(NULL)
		{
			
		}

	

		~CDuiEditCtrl()
		{
			if (m_hNormalBorderBrush)
			{
				DeleteObject(m_hNormalBorderBrush);
				m_hNormalBorderBrush = NULL;
			}

			if ( m_hHotBorderBrush )
			{
				DeleteObject(m_hHotBorderBrush);
				m_hHotBorderBrush = NULL;
			}

			if ( m_hFocusedBorderBrush )
			{
				DeleteObject(m_hFocusedBorderBrush);
				m_hFocusedBorderBrush = NULL;
			}

			if ( m_hDisabledBorderBrush )
			{
				DeleteObject(m_hDisabledBorderBrush);
				m_hDisabledBorderBrush = NULL;
			}
		}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiEditCtrl, CTRL_EDIT)
		DUI_DEFINE_INTERFACE(IDuiEditCtrl, IIDuiEditCtrl)
		DUI_DEFINE_INTERFACE(IDuiWindowCtrl, IIDuiWindowCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiEditCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(text, SetText)
		DUI_SETATTRIBUTE_INT_FUNCTION(nativebkcolor, SetNativeEditBkColor)
		DUI_SETATTRIBUTE_INT_FUNCTION(textcolor, SetTextColor)
		DUI_SETATTRIBUTE_STRING_FUNCTION(font, SetFont)
		DUI_SETATTRIBUTE_RECT_FUNCTION(textpadding, SetTextPadding)
		DUI_SETATTRIBUTE_STRING_FUNCTION(default, SetDefaultText)
		DUI_SETATTRIBUTE_INT_FUNCTION(defaultcolor, SetDefaultTextColor)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiWindowCtrl);
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiEditCtrl)
		DUI_MESSAGE_HANDLER(WM_CREATE, OnCreate)
		DUI_MESSAGE_HANDLER(OCM_CTLCOLOREDIT, OnCtlColorEdit)
		DUI_MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		DUI_MESSAGE_HANDLER(OCM_COMMAND, OnOcmCommand)
		DUI_MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		DUI_MESSAGE_HANDLER(WM_PAINT, OnPaint)
		DUI_MESSAGE_HANDLER(WM_SIZE, OnSize)
		DUI_MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		DUI_MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		DUI_MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDown, OnDuiLButtonDown)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseEnter, OnDuiMouseEnter)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseLeave, OnDuiMouseLeave)
	DUI_END_MSG_MAP();
	
	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiWindowCtrl::DoCreate(pParent, pCore,pBuilder) != S_OK)
		{
			return S_OK;
		}
		/*GetSysColor(COLOR_BTNFACE)*/
		if ( m_dwNormalBorderColor != -1)
		{
			m_hNormalBorderBrush   =::CreateSolidBrush(m_dwNormalBorderColor);
		}

		if ( m_dwHotBorderColor  != -1)
		{
			m_hHotBorderBrush      =::CreateSolidBrush(m_dwHotBorderColor);
		}

		if ( m_dwFocusedBorderColor != -1)
		{
			m_hFocusedBorderBrush  =::CreateSolidBrush(m_dwFocusedBorderColor);
		}
		
		if ( m_hDisabledBorderBrush )
		{
			m_hDisabledBorderBrush =::CreateSolidBrush(m_dwDisabledBorderColor);
		}
		
		
		
		

		IDuiFont* tFont = CDuiControlCtrl::GetFont(GetFont());
		if( tFont )
			SetWindowFont(m_hWnd, tFont->GetHandle(), TRUE);
		Edit_LimitText(GetHWND(), GetMaxChar());
		if(IsPasswordMode() ) 
			Edit_SetPasswordChar(GetHWND(), GetPasswordChar());

		Edit_SetText(GetHWND(), GetText());
		Edit_SetModify(GetHWND(), FALSE);
	//	::SendMessage(GetHWND(),EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(GetHWND(), IsEnabled() == TRUE);
		Edit_SetReadOnly(GetHWND(), IsReadOnly() == TRUE);

 		LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
 		styleValue |= m_iWindowStyls;
 		::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
 		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		if (m_sText.Length() <= 0 && !m_sDefaultText.IsEmpty())
		{
			m_bIsDefText = TRUE;
			SetPasswordChar(0);
			SetWindowText(m_hWnd,m_sDefaultText);
		}

	//	SetCenterInSingleLine();
		return S_OK;
	}

	//CDuiWindowCtrl
	virtual LPCWSTR GetSuperClassName()
	{
		return WC_EDIT;
	}

	virtual LPCWSTR GetWindowClassName()
	{
		return L"EditWnd";
	}

	virtual DWORD GetStyle()
	{
		UINT uStyle = WS_CHILD | ES_AUTOHSCROLL ;
		if(IsPasswordMode() ) uStyle |= ES_PASSWORD;
		return uStyle;
	}

	virtual DWORD GetStyleEx()
	{
		return WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;
	}
	//
	virtual VOID SetEnabled(BOOL bEnable = TRUE)
	{
		m_bEnabled = bEnable;
		if ( !m_bEnabled )
			m_uButtonState = 0;
		
	}

	virtual BOOL IsEnabled()
	{
		return m_bEnabled;
	}

	virtual VOID SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		Edit_SetText(GetHWND(), m_sText);
		Invalidate();
	}

	virtual LPCWSTR GetText()
	{
		return m_sText;
	}

	virtual VOID SetDefaultText(LPCWSTR lpszText)
	{
		if ( lpszText )
		{
			m_sDefaultText = lpszText;
		}
	}

	virtual LPCWSTR GetDefaultText()
	{
		return m_sDefaultText;
	}

	virtual VOID SetMaxChar(UINT uMax)
	{
		m_uMaxChar = uMax;
		Edit_LimitText(GetHWND(), m_uMaxChar);
	}

	virtual UINT GetMaxChar()
	{
		return m_uMaxChar;
	}

	virtual VOID SetReadOnly(BOOL bReadOnly)
	{
		if( m_bReadOnly == bReadOnly ) return;

		m_bReadOnly = bReadOnly;
		Edit_SetReadOnly(GetHWND(), m_bReadOnly);
		Invalidate();
	}

	virtual BOOL IsReadOnly() const
	{
		return m_iWindowStyls&ES_NUMBER ? true:false;
	}

	virtual VOID SetPasswordMode(BOOL bPasswordMode)
	{
		if( m_bPasswordMode == bPasswordMode ) return;
		m_bPasswordMode = bPasswordMode;
		Invalidate();
	}

	virtual BOOL IsPasswordMode() const
	{
		return m_bPasswordMode;
	}

	virtual VOID SetPasswordChar(WCHAR cPasswordChar)
	{
		if( m_cPasswordChar == cPasswordChar ) return;
		m_cPasswordChar = cPasswordChar;
		Edit_SetPasswordChar(GetHWND(), m_cPasswordChar);
		Invalidate();
	}

	virtual WCHAR GetPasswordChar() const
	{
		return m_cPasswordChar;
	}

	virtual VOID SetNumberOnly(BOOL bNumberOnly)
	{
		if( bNumberOnly )
			m_iWindowStyls |= ES_NUMBER;
		else
			m_iWindowStyls |= ~ES_NUMBER;
	}

	virtual BOOL IsNumberOnly() const
	{
		return m_iWindowStyls&ES_NUMBER ? TRUE:FALSE;
	}

	virtual LPCWSTR GetNormalImage()
	{
		return m_sNormalImage;
	}

	virtual VOID SetNormalImage(LPCWSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	virtual LPCWSTR GetHotImage()
	{
		return m_sHotImage;
	}

	virtual VOID SetHotImage(LPCWSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	virtual LPCWSTR GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	virtual VOID SetFocusedImage(LPCWSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	virtual LPCWSTR GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	virtual VOID SetDisabledImage(LPCWSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	virtual VOID SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
	}

	virtual DWORD GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	virtual DWORD GetNormalBorderColor()
	{
		return m_dwNormalBorderColor;
	}

	virtual VOID SetNormalBorderColor(DWORD dwColor)
	{
		m_dwNormalBorderColor = dwColor;
	}

	virtual DWORD GetHotBorderColor()
	{
		return m_dwHotBorderColor;
	}

	virtual VOID SetHotBorderColor(DWORD dwColor)
	{
		m_dwHotBorderColor = dwColor;
	}

	virtual DWORD GetFocusedBorderColor()
	{
		return m_dwFocusedBorderColor;
	}

	virtual VOID SetFocusedBorderColor(DWORD dwColor)
	{
		m_dwFocusedBorderColor = dwColor;
	}

	virtual DWORD GetDisabledBorderColor()
	{
		return m_dwDisabledBorderColor;
	}

	virtual VOID SetDisabledBorderColor(DWORD dwColor)
	{
		m_dwDisabledBorderColor = dwColor;
	}

	
	virtual VOID SetSel(long nStartChar, long nEndChar)
	{
		Edit_SetSel(GetHWND(), nStartChar,nEndChar);
	}

	virtual VOID SetSelAll()
	{
		SetSel(0,-1);
	}

	virtual VOID SetReplaceSel(LPCWSTR lpszReplace)
	{
		 Edit_ReplaceSel(GetHWND(), lpszReplace);
	}

	virtual DWORD GetTextColor() 
	{
		return m_dwTextColor;
	}

	virtual VOID SetTextColor(DWORD dwColor)
	{
		m_dwTextColor = dwColor;
	}

	virtual DWORD GetDefaultTextColor()
	{
		return m_dwDefaultTextColor;
	}

	virtual VOID SetDefaultTextColor(DWORD dwColor)
	{
		m_dwDefaultTextColor = dwColor;
	}

	virtual LPCWSTR GetFont()
	{
		return m_strFont;
	}

	virtual VOID SetFont(LPCWSTR lpszFont)
	{
		if ( lpszFont )
		{
			m_strFont = lpszFont;
		}
	}

	virtual CDuiRect GetTextPadding()
	{
		return m_TextPaddingRect;
	}

	virtual VOID SetTextPadding(const CDuiRect& TextPadding )
	{
		m_TextPaddingRect = TextPadding;
	}


	virtual VOID SetRectNP(const CDuiRect& rect)
	{
		::SendMessage(m_hWnd, EM_SETRECTNP, 0, (LPARAM)&rect);
	}

	virtual VOID SetMarginsEx(const CDuiRect& rect)
	{
		CDuiRect rtClient;
		GetClientRect(m_hWnd, &rtClient);

		CDuiRect rt(rtClient.left + rect.left, rtClient.top + rect.top, 
			rtClient.right - rect.right, rtClient.bottom - rect.bottom);
		SetRectNP(rt);
	}

	void DrawBorder()
	{
		
		HDC hDC=::GetWindowDC(m_hWnd);
		CDuiRect rc;
		::GetWindowRect(m_hWnd,&rc);

		rc = CDuiRect(0, 0, rc.right - rc.left, rc.bottom - rc.top);

		if (UISTATE_FOCUSED & m_uButtonState ){
			if ( m_hNormalBorderBrush && m_hFocusedBorderBrush)
			{
				::FrameRect(hDC,&rc, m_hNormalBorderBrush);
				rc.Inflate(-1,-1);
				::FrameRect(hDC,&rc,m_hFocusedBorderBrush);
			}
			
		}
		else if (UISTATE_HOT & m_uButtonState ){
			if ( m_hHotBorderBrush )
			{
				::FrameRect(hDC,&rc, m_hHotBorderBrush);
				rc.Inflate(-1,-1);
				::FrameRect(hDC,&rc,m_hHotBorderBrush);
			}
		}
		else {
			if ( m_hNormalBorderBrush )
			{
				::FrameRect(hDC,&rc, m_hNormalBorderBrush);
				rc.Inflate(-1,-1);
				::FrameRect(hDC,&rc,m_hNormalBorderBrush);
			}
		}

		::ReleaseDC(m_hWnd,hDC);
	}

protected:
	LRESULT OnCaretInputTimer()
	{
		return 0;
	}

	LRESULT OnNcCalcSize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		
		BOOL bCalcValidRects = (BOOL)wParam;
		if (bCalcValidRects)
		{
			LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
			pParam->rgrc[0].right -= m_TextPaddingRect.right;
			pParam->rgrc[0].bottom -= m_TextPaddingRect.bottom;
			pParam->rgrc[0].left += m_TextPaddingRect.left;
			pParam->rgrc[0].top += m_TextPaddingRect.top ;
			pParam->rgrc[1] = pParam->rgrc[0];
		} 
		return CallWindowProc(m_OldWndProc, m_hWnd, nMsg, wParam, lParam);
	}

	LRESULT OnCtlColorEdit(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		::SetBkMode((HDC)wParam, TRANSPARENT);
		if( m_hBkBrush == NULL ) 
		{
			DWORD clrColor = GetNativeEditBkColor();
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}

		if (m_bIsDefText)
		{
			DWORD dwTextColor = GetDefaultTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		}
		else
		{
			DWORD dwTextColor = GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		}
		
		return (LRESULT)m_hBkBrush;
	}

	LRESULT OnOcmCommand(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) 
		{
			int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
			m_sText.Empty();
			::GetWindowText(m_hWnd, m_sText.GetBufferSetLength(cchLen), cchLen);
			m_sText.ReleaseBuffer();

			DuiEvent event;
			event.Ctrl = this;
			event.nEvent = DuiEvent_TextChange;
			event.wParam = wParam;
			event.lParam = lParam;
			DoEvent(event);
		}
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) 
		{
			Invalidate();
		}
		return 0;
	}
	 
	LRESULT OnCreate(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		return CallWindowProc(m_OldWndProc, m_hWnd, nMsg, wParam, lParam);
	}

	LRESULT OnPaint(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		LRESULT lResult = CallWindowProc(m_OldWndProc, m_hWnd, nMsg, wParam, lParam);
		DrawBorder();
		return lResult;
	}

	LRESULT OnNcPaint(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
 		bHandle = FALSE;
		return 0;
	}

	LRESULT OnSize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		CDuiRect clientRect;
		GetClientRect(m_hWnd, &clientRect);
		CDuiControlCtrl::SetPos(clientRect);

		clientRect.right -= m_TextPaddingRect.right;
		clientRect.bottom -= m_TextPaddingRect.bottom;
		clientRect.left += m_TextPaddingRect.left;
		clientRect.top += m_TextPaddingRect.top ;

		LRESULT lResult =  CallWindowProc(m_OldWndProc, m_hWnd, nMsg, wParam, lParam);
		return lResult;
	}

	LRESULT OnNcHitTest(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		return HTCLIENT;
	}

	LRESULT OnSetFocus(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{

		m_uButtonState |= UISTATE_FOCUSED;
		if (m_bIsDefText)
		{
			m_bIsDefText = FALSE;
			SetPasswordChar(GetPasswordChar());
			SetWindowText(m_hWnd, _T(""));
		}
		::InvalidateRect(m_hWnd, NULL, FALSE);

		DuiEvent event = { this, DuiEvent_KillFocus, wParam, lParam};
		DoEvent(event);
		return 0;
	}

	LRESULT OnKillFocus(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		m_uButtonState &= ~UISTATE_FOCUSED;
		if (m_sText.Length() <= 0 && !m_sDefaultText.IsEmpty())
		{
			m_bIsDefText = TRUE;
			SetPasswordChar(0);
			SetWindowText(m_hWnd,m_sDefaultText);
		}
		::InvalidateRect(m_hWnd, NULL, FALSE);

		DuiEvent event = { this, DuiEvent_SetFocus, wParam, lParam};
		DoEvent(event);
		return 0;
	}

	LRESULT OnDuiLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		SetFocus(m_hWnd);
		return 0;
	}

	LRESULT OnDuiMouseEnter(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		m_uButtonState |= UISTATE_HOT;
		::InvalidateRect(m_hWnd, NULL, FALSE);
		return 0;
	}

	LRESULT OnDuiMouseLeave(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		m_uButtonState &= ~UISTATE_HOT;
		::InvalidateRect(m_hWnd, NULL, FALSE);
		return 0;
	}


private:
	UINT m_uMaxChar;
	BOOL m_bReadOnly;
	BOOL m_bPasswordMode;
	BOOL m_bEnabled;
	TCHAR m_cPasswordChar;
	UINT m_uButtonState;
	CDuiString m_sNormalImage;
	CDuiString m_sHotImage;
	CDuiString m_sFocusedImage;
	CDuiString m_sDisabledImage;

	DWORD m_dwNormalBorderColor;
	DWORD m_dwHotBorderColor;
	DWORD m_dwFocusedBorderColor;
	DWORD m_dwDisabledBorderColor;

	HBRUSH m_hNormalBorderBrush;
	HBRUSH m_hHotBorderBrush;
	HBRUSH m_hFocusedBorderBrush;
	HBRUSH m_hDisabledBorderBrush;


	DWORD m_dwEditbkColor;
	DWORD m_dwTextColor;
	DWORD m_dwDefaultTextColor;
	int m_iWindowStyls;

	CDuiString m_sText;
	CDuiString m_strFont;
	CDuiString m_sDefaultText;
	HBRUSH m_hBkBrush;

	CDuiRect m_TextPaddingRect;

	BOOL m_bIsDefText;
};

};