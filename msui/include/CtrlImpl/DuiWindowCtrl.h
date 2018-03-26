#pragma once

#include "DuiHostWindowCtrl.h"

#pragma comment(lib,"comctl32.lib")
namespace DuiKit{;

class CDuiWindowCtrl : 
	public CDuiHostWindowCtrl
{
public:
	CDuiWindowCtrl()
	  {
		
	  }

	  ~CDuiWindowCtrl()
	  {
		  m_hWnd = NULL;
	  }

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiWindowCtrl, CTRL_WINDOW)
		DUI_DEFINE_INTERFACE(IDuiWindowCtrl, IIDuiWindowCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP

	
	virtual HRESULT DoCreate(IDuiObject* Parent, IDuiCore* Core, IDuiBuilder* pBuilder)
	{
		if ( S_OK != CDuiContainerCtrl::DoCreate(Parent, Core,pBuilder))
		{
			return E_FAIL;
		}
		
		
		
		Create
		(
			m_hParentWnd, 
			m_strTitle, 
			GetWindowClassName(), 
			GetSuperClassName(), 
			CS_DBLCLKS,
			m_pos.top,
			m_pos.left,
			GetFixedWidth(),
			GetFixedHeight(),
			NULL
			);

		DoInitMessage();
		if( m_nTransparent != 255)
		{
			SetTransparent(m_nTransparent);
		}
		
		return GetHWND() ? S_OK : E_FAIL;
	}
};







};//namespace DuiKit{;