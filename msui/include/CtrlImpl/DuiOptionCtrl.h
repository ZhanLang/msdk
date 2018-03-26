#pragma once

#include <DuiCtrl/IDuiOptionCtrl.h>
#include "DuiButtonCtrl.h"
namespace DuiKit{;

class CDuiOptionCtrl : 
	public IDuiOptionCtrl,
	public CDuiButtonCtrl
{
public:
	CDuiOptionCtrl():
	  m_bSelect(FALSE)
	  {

	  }

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiOptionCtrl, CTRL_OPTION)
		DUI_DEFINE_INTERFACE(IDuiOptionCtrl, IIDuiOptionCtrl)
		DUI_DEFINE_INTERFACE(IDuiButtonCtrl, IIDuiButtonCtrl)
		DUI_DEFINE_INTERFACE(IDuiLabelCtrl, IIDuiLabelCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiOptionCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(selectedimage,   SetSelectedImage)
		DUI_SETATTRIBUTE_STRING_FUNCTION(selectedhotimage,SetSelectedHotImage)
		DUI_SETATTRIBUTE_STRING_FUNCTION(group,	SetGroup)
		DUI_SETATTRIBUTE_BOOL(selected, m_bSelect)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiButtonCtrl);
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiButtonCtrl)
		DUI_MESSAGE_HANDLER_HAVE_HANDELER(DuiMsg_LButtonDown, OnLButtonDown)
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_OptionSelectChanged, OnOptionSelectChanged)
	DUI_END_MSG_MAP();

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiButtonCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

public:
	virtual VOID SetSelectedImage(LPCWSTR lpszImage)
	{
		if ( lpszImage && wcslen(lpszImage))
		{
			m_strSelectedImage = lpszImage;
		}
	}

	virtual LPCWSTR GetSelectedImage()
	{
		return m_strSelectedImage;
	}

	virtual VOID SetSelectedHotImage(LPCWSTR lpszImage)
	{
		if ( lpszImage && wcslen(lpszImage))
		{
			m_strSelectedHotImage = lpszImage;
		}
	}

	virtual LPCWSTR GetSelectedHotImage()
	{
		return m_strSelectedHotImage;
	}

	virtual BOOL GetSelected()
	{
		return m_bSelect;
	}

	virtual VOID SetSelected(BOOL bSelected)
	{
		RASSERTV( bSelected != m_bSelect);
		m_bSelect = bSelected;
		DuiEvent event;
		event.Ctrl = this;
		event.nEvent = DuiEvent_SelectChanged;
		Invalidate();

		if ( !m_strGroup.IsEmpty() && m_pParentControl)
		{
			IDuiControlCtrl* tParent = m_pParentControl;
			for ( ; tParent ; tParent = tParent->GetParentCtrl())
			{
				IDuiContainerCtrl* tContainerCtrl = (IDuiContainerCtrl*)tParent->QueryInterface(IIDuiContainerCtrl);
				if ( tContainerCtrl && m_strGroup.CompareNoCase(tContainerCtrl->GetGroup()) == 0)
				{
					tContainerCtrl->BroadCastMessage(this, DuiMsg_OptionSelectChanged, IIDuiOptionCtrl);
					break;
				}
			}
		}

		DoEvent(event);
	}

	virtual LPCWSTR GetGroup()
	{
		return m_strGroup;
	}

	virtual VOID SetGroup(LPCWSTR lpszGroupName = NULL)
	{
		if ( lpszGroupName && wcslen(lpszGroupName))
		{
			m_strGroup = lpszGroupName;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	LRESULT OnLButtonDown(BOOL& bHandle)
	{
		bHandle = FALSE;
		if ( m_strGroup.IsEmpty() )
			SetSelected(!m_bSelect);
		else if ( !GetSelected())
			SetSelected(TRUE);
		return 0;
	}

	LRESULT OnOptionSelectChanged(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if ( tToCtrl != this->QueryInterface(IIDuiControlCtrl))
		{
			IDuiOptionCtrl* tFromCtrl = (IDuiOptionCtrl*)tToCtrl->QueryInterface(IIDuiOptionCtrl);
			if ( m_strGroup.CompareNoCase(tFromCtrl->GetGroup()) == 0)
			{
				if ( tFromCtrl->GetSelected() )
				{
					this->SetSelected(FALSE);
				}
			}
		}
		return 0;
	}

	LRESULT OnInitialize()
	{
		SetSelected(m_bSelect);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	void PaintStatusImage(HDC hDC)
	{
		CDuiString strImage;
		if ( m_bSelect ) //»æÖÆÑ¡Ôñ×´Ì¬
		{
			strImage = m_strSelectedImage;
			if ( m_dwButtonState == UISTATE_HOT)
			{
				strImage = m_strSelectedHotImage.IsEmpty() ? m_strSelectedImage : m_strSelectedHotImage;
			}
			PrintImage(hDC, strImage, m_pos, m_rcPaint);
		}
		else
			CDuiButtonCtrl::PaintStatusImage(hDC);
	}

private:
	CDuiString m_strSelectedImage;
	CDuiString m_strSelectedHotImage;
	CDuiString m_strGroup;
	BOOL	m_bSelect;
};


};