#pragma once
#include <DuiCtrl/IDuiGroupCtrl.h>
#include "DuiContainerCtrl.h"

namespace DuiKit{;
class CDuiGroupCtrl : 
	public IDuiGroupCtrl,
	public CDuiContainerCtrl
{
public:
	CDuiGroupCtrl():
	  m_dwGroupStyle(UISTATE_NORMAL),
		  m_bEnable(TRUE)
	  {

	  }

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiGroupCtrl, CTRL_GROUP)
		DUI_DEFINE_INTERFACE(IDuiGroupCtrl, IIDuiGroupCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiGroupCtrl)
		DUI_SETATTRIBUTE_RECT_FUNCTION(inset, SetInsetSize);
		DUI_SETATTRIBUTE_STRING_FUNCTION(group, SetGroup)
			DUI_SETATTRIBUTE_STRING_FUNCTION(normalimage,   SetNormalImage)
			DUI_SETATTRIBUTE_STRING_FUNCTION(hotimage,		SetHotImage)
			DUI_SETATTRIBUTE_STRING_FUNCTION(pushedimage,	SetPushedImage)
			DUI_SETATTRIBUTE_STRING_FUNCTION(disabledimage,	SetDisabledImage)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl)
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiContainerCtrl)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDown, OnLButtonDown)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonUp, OnLButtonUp)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDblClk, OnLButtonDblClk)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseEnter, OnMouseEnter)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseLeave, OnMouseLeave)
	DUI_END_MSG_MAP();

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiContainerCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}



	LRESULT OnMouseLeave(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( GetEnabled() ) 
		{
			if ( m_dwGroupStyle & UISTATE_HOT)
			{
				m_dwGroupStyle &= ~UISTATE_HOT;
				CDuiControlCtrl::Invalidate();
			}
			
//			GrpError(GroupName, MsgLevel_Warning, _T("OnMouseLeave"));
		}

		return 0;
	}

	LRESULT OnLButtonUp(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if ( (m_dwGroupStyle & UISTATE_PUSHED) != 0)
		{
			m_dwGroupStyle &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
//			GrpError(GroupName, MsgLevel_Warning, _T("OnLButtonUp"));
			CDuiControlCtrl::Invalidate();
		}
		

		DuiEvent event = { 0 };
		event.Ctrl = this;
		event.nEvent = DuiEvent_Click;
		
		DoEvent(event);
		return 0;
	}

	LRESULT OnLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( ::PtInRect(&m_pos, CDuiPoint(lParam)) && GetEnabled() ) 
		{
			if ( (m_dwGroupStyle & UISTATE_PUSHED) == 0)
			{
				m_dwGroupStyle |= UISTATE_PUSHED | UISTATE_CAPTURED;
				CDuiControlCtrl::Invalidate();
			}
			

			DuiEvent event = { 0 };
			event.Ctrl = this;
			event.nEvent = DuiEvent_Click;
			
			DoEvent(event);
		}

		return 0;
	}

	LRESULT OnLButtonDblClk(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( ::PtInRect(&m_pos, CDuiPoint(lParam)) && GetEnabled() ) 
		{
			m_dwGroupStyle |= UISTATE_PUSHED | UISTATE_CAPTURED;
//			GrpError(GroupName, MsgLevel_Warning, _T("OnLButtonDblClk"));
			CDuiControlCtrl::Invalidate();

			DuiEvent event = { 0 };
			event.Ctrl = this;
			event.nEvent = DuiEvent_DbClick;
			
			DoEvent(event);
		}

		return 0;
	}

	LRESULT OnMouseEnter(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if( GetEnabled() ) 
		{
			if ( !(m_dwGroupStyle & UISTATE_HOT))
			{
				m_dwGroupStyle |= UISTATE_HOT;
				CDuiControlCtrl::Invalidate();
			}
		}

		return 0;
	}

	//virtual VOID DoPrint()
protected:
	//CDuiLabelCtrl
	void PaintStatusImage(HDC hDC)
	{
//		GrpMsg(GroupName, MsgLevel_Msg, _T("PaintStatusImage:%d,%d,%d,%d"),m_rcPaint.left, m_rcPaint.top, m_rcPaint.bottom , m_rcPaint.right);
// 		if( !GetEnabled() ) 
// 			m_dwGroupStyle |= UISTATE_DISABLED;
// 		else 
// 			m_dwGroupStyle &= ~ UISTATE_DISABLED;

		if( (m_dwGroupStyle & UISTATE_DISABLED) != 0 ) 
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
	   if( (m_dwGroupStyle & UISTATE_PUSHED) != 0 ) 
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
	   if( (m_dwGroupStyle & UISTATE_HOT) ) 
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
		return m_dwGroupStyle;
	}

private:
	DWORD m_dwGroupStyle;
	CDuiString m_strNormalImage;
	CDuiString m_strPushedImage;
	CDuiString m_strDisabledImage;
	CDuiString m_strHotImage;
	BOOL		m_bEnable;
};


};