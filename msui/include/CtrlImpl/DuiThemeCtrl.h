#pragma once
#include "DuiVerticalLayoutCtrl.h"

#include <shlobj.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <gdiplus.h>

#include <atlbase.h>
#include <atlapp.h>
#include<atlgdi.h>
using namespace Gdiplus;
namespace DuiKit{;

class CDuiThemeCtrl : public IDuiPreMessageFilter, public CDuiVerticalLayoutCtrl
{
public:
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiVerticalLayoutCtrl, CTRL_VERTICALLAYOUT)
		DUI_DEFINE_INTERFACE(IDuiVerticalLayoutCtrl, IIDuiVerticalLayoutCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
		DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiThemeCtrl)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiVerticalLayoutCtrl);
	DUI_END_SETATTRIBUTE;

	virtual LRESULT OnPreMessageFilter(const DuiMsg& duiMsg, BOOL& bHandle)
	{
		if ( duiMsg.nMsg == WM_THEMECHANGED )
		{
			if ( m_hTheme ){
				CloseThemeData(m_hTheme);
				m_hTheme = NULL;
			}

			m_hTheme = OpenThemeData(GetRootCtrl()->GetHWND(), L"REBAR");
		}
		return 0;
	}

	CDuiThemeCtrl()
	{
		m_hTheme = NULL;
	}

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( !CDuiVerticalLayoutCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return E_FAIL;
		}


		IDuiControlCtrl* pCtrl = (IDuiControlCtrl*)GetRootCtrl()->QueryInterface(IIDuiControlCtrl);
		pCtrl->AddPreMessageFilter(this);
		m_hTheme = OpenThemeData(GetRootCtrl()->GetHWND(), L"REBAR");

		return S_OK;
	}

	virtual VOID DoFinalMessage()
	{
		if ( m_hTheme ){
			CloseThemeData(m_hTheme);
			m_hTheme = NULL;
		}
		
		IDuiControlCtrl* pCtrl = (IDuiControlCtrl*)GetRootCtrl()->QueryInterface(IIDuiControlCtrl);
		pCtrl->RemovePreMessageFilter(this);
		CDuiVerticalLayoutCtrl::DoFinalMessage();
	}

	virtual VOID DoPaint(HDC hDC, const RECT& rcPaint )
	{
		CDCHandle dc( hDC );
		CDC dcRebar(::GetDC(::GetParent(GetRootCtrl()->GetHWND())));
		CDuiRect rcClient,rcSnap;
		rcClient = GetPos();
		rcSnap = rcClient;

		if ( !m_hTheme )
			m_hTheme = OpenThemeData(GetRootCtrl()->GetHWND(), L"REBAR");
		

		if(IsThemeActive())
		{
			int nAlpha = 0;
			COLORREF cr = {0};

			::GetThemeInt(m_hTheme, RP_BAND, 0, TMT_ALPHATHRESHOLD, &nAlpha);
			::GetThemeInt(m_hTheme, RP_BACKGROUND, 0, TMT_ALPHATHRESHOLD, &nAlpha);
			::GetThemeColor(m_hTheme, RP_BAND, 0, TMT_BLENDCOLOR, &cr);
			SolidBrush sbr(Color(nAlpha, GetRValue(cr), GetRValue(cr), GetRValue(cr))); //Color(/*50*/2, 0, 0, 0)

			Graphics gp(dc);
			gp.FillRectangle(&sbr, 0, 0, rcClient.GetWidth(), rcClient.GetHeight());
			::MapWindowPoints(GetRootCtrl()->GetHWND(), ::GetParent(GetRootCtrl()->GetHWND()), (LPPOINT)&rcSnap, 2);
			dc.StretchBlt(0, 0, rcClient.GetWidth(), rcClient.GetHeight(), dcRebar, rcSnap.left - 1,  rcSnap.top, 1, rcSnap.GetHeight(), SRCCOPY);
		}

		CDuiVerticalLayoutCtrl::DoPaint(hDC, rcPaint);
	}

private:
	HTHEME m_hTheme;
};

};