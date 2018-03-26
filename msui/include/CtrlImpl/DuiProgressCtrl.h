#pragma once
#include <DuiCtrl/IDuiProgressCtrl.h>
#include <DuiImpl/DuiRenderEngine.h>

#include "DuiLabelCtrl.h"
namespace DuiKit{;
class CDuiProgressCtrl :
	public IDuiProgressCtrl,
	public CDuiLabelCtrl
{
public:
	CDuiProgressCtrl(): 
		m_bHorizontal(TRUE), 
		m_nMinValue(0), 
		m_nMaxValue(100), 
		m_nValue(0), 
		m_bStretchfore(TRUE)
	  {

	  }

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiProgressCtrl, CTRL_PROGRESS)
		DUI_DEFINE_INTERFACE(IDuiProgressCtrl, IIDuiProgressCtrl)
		DUI_DEFINE_INTERFACE(IDuiLabelCtrl, IIDuiLabelCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiProgressCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(foreimage, SetForegroundImage)
		DUI_SETATTRIBUTE_BOOL_FUNCTION(hor, SetHorizontal)
		DUI_SETATTRIBUTE_INT_FUNCTION(min, SetMinValue)
		DUI_SETATTRIBUTE_INT_FUNCTION(max, SetMaxValue)
		DUI_SETATTRIBUTE_INT_FUNCTION(value, SetValue)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiLabelCtrl);
	DUI_END_SETATTRIBUTE;

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiLabelCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	//
	VOID PaintStatusImage(HDC hDC)
	{
		if( m_nMaxValue <= m_nMinValue ) m_nMaxValue = m_nMinValue + 1;
		if( m_nValue > m_nMaxValue ) m_nValue = m_nMaxValue;
		if( m_nValue < m_nMinValue ) m_nValue = m_nMinValue;

		RECT rc = m_pos;

		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMinValue) * (m_pos.right - m_pos.left) / (m_nMaxValue - m_nMinValue);
		}
		else {
			rc.top = (m_pos.bottom - m_pos.top) * (m_nMaxValue - m_nValue) / (m_nMaxValue - m_nMinValue);
			rc.right = m_pos.right - m_pos.left;
			rc.bottom = m_pos.bottom - m_pos.top;
		}


		IDuiImage* tImage = GetImage(m_strForeImage);
		if ( !tImage )
			return ;
		
		if ( !tImage->GetHandle())
		{
			tImage->Create();
		}

		CDuiRenderEngine::DrawImage(hDC, tImage,rc, m_rcPaint);
	}

public:
	virtual VOID SetForegroundImage(LPCWSTR lpszImage)
	{
		if ( lpszImage && wcslen(lpszImage))
		{
			m_strForeImage = lpszImage;
		}
	}

	virtual LPCWSTR GetForegroundImage()
	{
		return m_strForeImage;
	}

	virtual VOID SetHorizontal(BOOL bHorizontal)
	{
		m_bHorizontal = bHorizontal;
	}

	virtual BOOL GetHorizontal()
	{
		return m_bHorizontal;
	}

	virtual INT GetMinValue() const 
	{
		return m_nMinValue;
	}

	virtual VOID SetMinValue(INT nMin)
	{
		m_nMinValue = nMin;
	}

	virtual INT GetMaxValue() const
	{
		return m_nMaxValue;
	}

	virtual VOID SetMaxValue(INT nMax)
	{
		m_nMaxValue = nMax;
	}

	virtual INT GetValue() const
	{
		return m_nValue;
	}

	virtual VOID SetValue(INT nValue)
	{
		 m_nValue = nValue;
	}

private:
	INT m_nMaxValue;
	INT m_nMinValue;
	INT m_nValue;
	CDuiString m_strForeImage;
	BOOL m_bStretchfore;
	BOOL m_bHorizontal;
};

};