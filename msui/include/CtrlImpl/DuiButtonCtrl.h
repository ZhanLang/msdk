#pragma once
#include <DuiCtrl/IDuiButtonCtrl.h>
#include "DuiLabelCtrl.h"
namespace DuiKit{;


class CDuiButtonCtrl:
	public IDuiButtonCtrl,
	public CDuiLabelCtrl
{
public:
	CDuiButtonCtrl():
		m_bEnable(TRUE),
		m_dwButtonState(UISTATE_NORMAL)
	  {

	  }
	//在子类中会用到
	DWORD		m_dwButtonState;
public:
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiButtonCtrl, CTRL_BUTTON)
		DUI_DEFINE_INTERFACE(IDuiButtonCtrl, IIDuiButtonCtrl)
		DUI_DEFINE_INTERFACE(IDuiLabelCtrl, IIDuiLabelCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiButtonCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(normalimage,   SetNormalImage)
		DUI_SETATTRIBUTE_STRING_FUNCTION(hotimage,		SetHotImage)
		DUI_SETATTRIBUTE_STRING_FUNCTION(pushedimage,	SetPushedImage)
		DUI_SETATTRIBUTE_STRING_FUNCTION(disabledimage,	SetDisabledImage)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiLabelCtrl);
	DUI_END_SETATTRIBUTE;

public:
	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiControlCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	DUI_BEGIN_MSG_MAP(CDuiButtonCtrl)
		//只接受鼠标的几个鼠标键
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_MouseEnter, OnMouseEnter)
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_MouseLeave, OnMouseLeave)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDown, OnLButtonDown)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonUp, OnLButtonUp)
		DUI_MESSAGE_HANDLER_HAVE_HANDELER(DuiMsg_SetCursor, OnDuiSetCurrsor)
	DUI_END_MSG_MAP();

public:
	LRESULT OnMouseEnter()
	{
		if( GetEnabled() ) 
		{	
			m_dwButtonState |= UISTATE_HOT;
			Invalidate();	
		}

		return 0;
	}

	LRESULT OnMouseLeave()
	{
		if( GetEnabled() ) 
		{
			m_dwButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		
		return 0;
	}

	LRESULT OnLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( ::PtInRect(&m_pos, CDuiPoint(lParam)) && GetEnabled() ) {
			m_dwButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			Invalidate();
		}


		return 0;
	}

	LRESULT OnLButtonUp(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( (m_dwButtonState & UISTATE_CAPTURED) != 0 ) 
		{
			if( ::PtInRect(&m_pos, CDuiPoint(lParam)) ) 
			{
				DuiEvent event = { this, DuiEvent_Click};
				DoEvent(event);
			}

			m_dwButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			Invalidate();
		}

		return 0;
	}

	LRESULT OnDuiSetCurrsor(BOOL& bHandle)
	{
		bHandle = FALSE;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return 0;
	}


	//CDuiLabelCtrl
	void PaintStatusImage(HDC hDC)
	{

// 		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
// 		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !GetEnabled() ) 
			m_dwButtonState |= UISTATE_DISABLED;
		else 
			m_dwButtonState &= ~ UISTATE_DISABLED;


		if( (m_dwButtonState & UISTATE_DISABLED) != 0 ) 
		{
			if( !m_strDisabledImage.IsEmpty() )
			{
				if( !PrintImage(hDC, m_strDisabledImage, m_pos, m_rcPaint) )
				{
					m_strDisabledImage.Empty();
					goto Label_ForeImage;
				}
				return;
			}
		}
		if( (m_dwButtonState & UISTATE_PUSHED) != 0 ) 
		{
			if( !m_strPushedImage.IsEmpty() ) 
			{
				if( !PrintImage(hDC, m_strPushedImage, m_pos, m_rcPaint) )
				{
					m_strPushedImage.Empty();
					goto Label_ForeImage;
				}
				return;
				//else goto Label_ForeImage;
			}
		}
		if( (m_dwButtonState & UISTATE_HOT) ) 
		{
			if( !m_strHotImage.IsEmpty() ) 
			{
				if( !PrintImage(hDC, m_strHotImage, m_pos, m_rcPaint) )
				{
					m_strHotImage.Empty();
					goto Label_ForeImage;
				}
				return;
			}
		}

		if( !m_strNormalImage.IsEmpty() ) 
		{
			if( !PrintImage(hDC, m_strNormalImage, m_pos, m_rcPaint) ) 
				m_strNormalImage.Empty();
		}
		return;

Label_ForeImage:
		if( !m_strNormalImage.IsEmpty() ) 
		{
			if( !PrintImage(hDC, m_strNormalImage, m_pos, m_rcPaint) ) 
				m_strNormalImage.Empty();
		}
	}

	virtual DWORD GetControlFlags()
	{
		return UIFLAG_SETCURSOR;
	}

	//IDuiButtonCtrl
	virtual VOID SetNormalImage(LPCWSTR lpszNormalImage)
	{
		if ( lpszNormalImage && wcslen(lpszNormalImage))
		{
			m_strNormalImage = lpszNormalImage;
		}
	}

	
	virtual LPCWSTR GetNormalImage()
	{
		return m_strNormalImage;
	}

	
	virtual VOID SetPushedImage(LPCWSTR lpszPushedImage)
	{
		if ( lpszPushedImage && wcslen(lpszPushedImage))
		{
			m_strPushedImage = lpszPushedImage;
		}
	}

	
	virtual LPCWSTR GetPushedImage()
	{
		return m_strPushedImage;
	}

	
	virtual VOID SetDisabledImage(LPCWSTR lpszDisabledImage)
	{
		if ( lpszDisabledImage && wcslen(lpszDisabledImage))
		{
			m_strDisabledImage = lpszDisabledImage;
		}
	}

	virtual LPCWSTR GetDisableImage()
	{
		return m_strDisabledImage;
	}

	
	virtual VOID SetHotImage(LPCWSTR lpszHotImage)
	{
		if ( lpszHotImage && wcslen(lpszHotImage))
		{
			m_strHotImage = lpszHotImage;
		}
	}

	virtual LPCWSTR GetHotImage()
	{
		return m_strHotImage;
	}

	virtual VOID SetEnabled(BOOL bEnabled)
	{
		m_bEnable = bEnabled;
	}

	virtual BOOL GetEnabled()
	{
		return m_bEnable;
	}

	virtual DWORD GetState()
	{
		return m_dwButtonState;
	}
public:
	CDuiString m_strNormalImage;
	CDuiString m_strPushedImage;
	CDuiString m_strDisabledImage;
	CDuiString m_strHotImage;

	BOOL		m_bEnable;

	
};
};