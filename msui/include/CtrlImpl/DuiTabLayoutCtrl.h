#pragma once
#include <DuiCtrl/IDuiTabLayoutCtrl.h>
#include "DuiContainerCtrl.h"
namespace DuiKit{;
class CDuiTableLayoutCtrl : 
	public IDuiTableLayoutCtrl,
	public CDuiContainerCtrl
{
public: 
	CDuiTableLayoutCtrl():m_pSelect(0),m_dwInitSelect(0){}
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiTableLayoutCtrl, CTRL_TABLELAYOUT)
		DUI_DEFINE_INTERFACE(IDuiTableLayoutCtrl, IIDuiTableLayoutCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiTableLayoutCtrl)
		DUI_SETATTRIBUTE_INT(index, m_dwInitSelect)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl)
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiContainerCtrl)
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_Initialize, OnInitialize)
	DUI_END_MSG_MAP();

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiContainerCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}


	LRESULT OnInitialize()
	{
		SelectItem(m_dwInitSelect);
		return 0;
	}

	//IDuiTableLayoutCtrl
	virtual DWORD GetCurrentSelect()
	{
		return (DWORD)m_ChildControlArray.Find(m_pSelect);
	}

	virtual VOID SelectItem(DWORD dwIndex)
	{
		if ( dwIndex < GetChildCount())
		{
			m_pSelect = GetChild(dwIndex);
			SelectItem(m_pSelect);
		}
	}

	virtual VOID SelectItem(IDuiControlCtrl* Control)
	{
		if ( Control != m_pSelect)
		{
			DuiEvent event;
			event.Ctrl = this;
			event.nEvent = DuiEvent_SelectChanged;
			Invalidate();
			DoEvent(event);
		}
	}

	//
	virtual VOID SetPos(const CDuiRect& pos)
	{
		CDuiControlCtrl::SetPos(pos);

		CDuiRect InsetRect = GetInsetSize();
		CDuiRect ThisPos = m_pos;

		ThisPos.left   +=  m_InsetRect.left;
		ThisPos.top    +=  m_InsetRect.top;
		ThisPos.right  -= m_InsetRect.right;
		ThisPos.bottom -= m_InsetRect.bottom;

		CDuiRect PaddingRect = GetPadding();
 		ThisPos.left   +=  PaddingRect.left;
 		ThisPos.top    +=  PaddingRect.top;
 		ThisPos.right  -= PaddingRect.right;
 		ThisPos.bottom -= PaddingRect.bottom;

		CDuiControlCtrl* pControl = (CDuiControlCtrl*)m_pSelect;
		RASSERTV(pControl);

		DWORD dwMaxHeight = pControl->GetMaxHeight();
		DWORD dwMinHeight = pControl->GetMinHeight();
		DWORD dwMaxWidth= pControl->GetMaxWidth();

		DWORD nCtrlHeight = dwMaxHeight ? dwMaxHeight : ThisPos.GetHeight();
		DWORD nCtrlWidth = dwMaxWidth ? dwMaxWidth : ThisPos.GetWidth();

		CDuiRect itemPos(ThisPos.left, ThisPos.top,  ThisPos.left + nCtrlWidth ,  ThisPos.top + nCtrlHeight);
		IDuiWindowCtrl* tWnd = (IDuiWindowCtrl*)pControl->QueryInterface(IIDuiWindowCtrl);

		pControl->SetPos(itemPos);
	
	}

private:
	IDuiControlCtrl* m_pSelect;
	DWORD m_dwInitSelect;
};

};