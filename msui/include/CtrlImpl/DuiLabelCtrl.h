#pragma once
#include <DuiCtrl/IDuiLableCtrl.h>
#include "DuiControlCtrl.h"
namespace DuiKit{;

class CDuiLabelCtrl : 
	public IDuiLabelCtrl,
	public CDuiControlCtrl
{
	CDuiString m_strText;

public:
	CDuiLabelCtrl():
	  m_EnableEffect(FALSE),
	  m_dwTextColor(0),
	  m_TextHalign(TextHalign_Left),
	  m_TextValign(TextValign_Center)
	{

	}
public:
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiLabelCtrl, CTRL_LABEL)
		DUI_DEFINE_INTERFACE(IDuiLabelCtrl, IIDuiLabelCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiLabelCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(text, SetText)
		DUI_SETATTRIBUTE_STRING_FUNCTION(font, SetFont)
		DUI_SETATTRIBUTE_BOOL_FUNCTION(enabledeffect, SetEnabledEffect) //是否使用字体特效
		DUI_SETATTRIBUTE_RECT_FUNCTION(textpadding, SetTextPadding)		//文字显示的边距
		DUI_SETATTRIBUTE_INT_FUNCTION(textcolor, SetTextColor)
		DUI_BENGIN_SUB_SETATTRIBUTE(valign)
			DUI_SUB_SETATTRIBUTE_FUNCTION(top, SetTextValign, TextValign_Top)
			DUI_SUB_SETATTRIBUTE_FUNCTION(Center, SetTextValign, TextValign_Center)
			DUI_SUB_SETATTRIBUTE_FUNCTION(Bottom, SetTextValign, TextValign_Bottom)
		DUI_END_SUB_SETATTRIBUTE;

		DUI_BENGIN_SUB_SETATTRIBUTE(halign)
			DUI_SUB_SETATTRIBUTE_FUNCTION(Left, SetTextHalign, TextHalign_Left)
			DUI_SUB_SETATTRIBUTE_FUNCTION(Center, SetTextHalign, TextHalign_Center)
			DUI_SUB_SETATTRIBUTE_FUNCTION(Right, SetTextHalign, TextHalign_Right)
		DUI_END_SUB_SETATTRIBUTE;

		DUI_SETATTRIBUTE_SUB_CLASS(CDuiControlCtrl);
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

	//IDuiLabelCtrl
	virtual VOID SetText( LPCWSTR lpszText )
	{
		if ( lpszText && wcslen(lpszText))
		{
			m_strText = lpszText;
		}
	}


	virtual LPCWSTR GetText()
	{
		return m_strText;
	}

	//
	virtual VOID PaintText(HDC hDC)
	{
		if ( m_strText.IsEmpty() )
			return ;
		
		RECT rc = m_pos;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		IDuiFont* tFont = CDuiControlCtrl::GetFont(m_strFontName);
		
		ASSERT( tFont );

		if(/*!GetEnabledEffect()*/ TRUE ) /*特效以后再做*/
		{
			UINT uTextStyle = DT_SINGLELINE ;
			switch( m_TextValign )
			{
			case TextValign_Bottom:
					uTextStyle |= DT_BOTTOM;
					break;
			case TextValign_Center:
					uTextStyle |= DT_VCENTER;
					break;
			case TextValign_Top:
					uTextStyle |= DT_TOP;
					break;
			};

			switch( m_TextHalign )
			{
			case TextHalign_Center:
					uTextStyle |= DT_CENTER;
					break;
			case TextHalign_Left:
					uTextStyle |= DT_LEFT;
					break;
			case TextHalign_Right:
					uTextStyle |= DT_RIGHT;
					break;
			};

			if ( m_EnableEffect )
				 uTextStyle |= DT_END_ELLIPSIS;
			
			CDuiRenderEngine::DrawText(hDC, rc, GetLangText(m_strText),m_dwTextColor , tFont , uTextStyle);
		}
	}

	//attr
	virtual VOID SetFont(LPCWSTR lpszFontName)
	{
		m_strFontName = lpszFontName;
	}

	virtual LPCWSTR GetFont()
	{
		return m_strFontName;
	}

	virtual VOID SetEnabledEffect(BOOL bEnabledEffect)
	{
		m_EnableEffect = bEnabledEffect;
	}

	virtual BOOL GetEnabledEffect()
	{
		return m_EnableEffect;
	}

	virtual VOID SetTextPadding(const CDuiRect& padding)
	{
		m_rcTextPadding = padding;
	}

	virtual CDuiRect GetTextPadding()
	{
		return m_rcTextPadding;
	}

	virtual VOID SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
	}

	virtual DWORD GetTextColor()
	{
		return m_dwTextColor;
	}

	
	virtual VOID SetTextValign(TextValign valign)
	{
		m_TextValign = valign;
	}

	virtual TextValign GetTextValign()
	{
		return m_TextValign;
	}

	
	virtual VOID SetTextHalign(TextHalign halign)
	{
		m_TextHalign = halign;
	}

	virtual TextHalign GetTextHalign()
	{
		return m_TextHalign;
	}


	
private:
	CDuiString m_strFontName;
	BOOL	   m_EnableEffect;
	CDuiRect   m_rcTextPadding;
	DWORD	   m_dwTextColor;
	TextHalign m_TextHalign;
	TextValign m_TextValign;
};
};