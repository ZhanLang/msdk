#pragma once
#include <DuiCtrl/IDuiTextCtrl.h>
#include "DuiLabelCtrl.h"
namespace DuiKit{;
class CDuiTextCtrl:
	public IDuiTextCtrl,
	public CDuiLabelCtrl
{
public:
	CDuiTextCtrl():m_bAutoLine(FALSE){}
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiTextCtrl, CTRL_TEXT)
		DUI_DEFINE_INTERFACE(IDuiTextCtrl, IIDuiTextCtrl)
		DUI_DEFINE_INTERFACE(IDuiLabelCtrl, IIDuiLabelCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiLabelCtrl)
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

public:
	virtual VOID SetAutoLine(BOOL bAutoLine)
	{
		m_bAutoLine = bAutoLine;
		Invalidate();
	}

	virtual BOOL GetAutoLine()
	{
		return m_bAutoLine;
	}

private:
	BOOL m_bAutoLine;
};
};