#pragma once
#include <DuiCtrl/IDuiRichEditCtrl.h>
#include "DuiControlCtrl.h"
#include <atlcomcli.h>
#include <Richedit.h>
#include <textserv.h>
#include <Imm.h>
#pragma comment(lib,"imm32.lib")

namespace DuiKit{;


const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

class CDuiRichEditCtrl : 
	public IDuiRichEditCtrl,
	public CDuiControlCtrl
{
public:
	CDuiRichEditCtrl(){
		m_pTextHost = NULL;
	}

	~CDuiRichEditCtrl(){}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiControlCtrl, CTRL_RichEdit)
		DUI_DEFINE_INTERFACE(IDuiRichEditCtrl, IIDuiRichEditCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiRichEditCtrl)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiRichEditCtrl)
	DUI_END_SETATTRIBUTE;


	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiControlCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}


private:
	ITextServices* m_pTextHost;


	bool m_bVScrollBarFixing;
	bool m_bWantTab;
	bool m_bWantReturn;
	bool m_bWantCtrlReturn;
	bool m_bRich;
	bool m_bReadOnly;
	bool m_bWordWrap;
	DWORD m_dwTextColor;
	int m_iFont;
	int m_iLimitText;
	LONG m_lTwhStyle;
	bool m_bDrawCaret;
	bool m_bInited;
	bool  m_fAccumulateDBC ; // TRUE - need to cumulate ytes from 2 WM_CHAR msgs
	// we are in this mode when we receive VK_PROCESSKEY
	UINT m_chLeadByte; // use when we are in _fAccumulateDBC mode

	UINT m_uButtonState;
	CDuiString m_sNormalImage;
	CDuiString m_sHotImage;
	CDuiString m_sFocusedImage;
	CDuiString m_sDisabledImage;
	RECT m_rcTextPadding;
	CDuiString m_sTipValue;
	DWORD m_dwTipValueColor;
	UINT m_uTipValueAlign;

};

};