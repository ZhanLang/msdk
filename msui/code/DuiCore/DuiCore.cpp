// DDuiCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DuiCore.h"
#include "CtrlImpl\DuiWindowCtrl.h"

DEFINE_OBJECT(CDuiCore,OBJECT_CORE);
CDuiCore::CDuiCore():
m_hThread(0),
m_strCurrentLang(L"zh")
{

	
}
CDuiCore::~CDuiCore()
{
	{
		AUTOLOCK_CS(m_SkinMap);
		m_SkinMap.Clear();
	}
}

IDuiObject* CDuiCore::GetSignalObject(LPCWSTR lpszName)
{
	if ( lpszName && wcslen(lpszName))
	{
		AUTOLOCK_CS(m_SignalObjectMap);
		CSignalObjectMap::ValueType it = m_SignalObjectMap.Find(lpszName);
		if ( it  )
		{
			return it->Value;
		}
	}

	return NULL;
}

VOID CDuiCore::AddSignalObject(LPCWSTR lpszName, IDuiObject* pObject)
{
	if ( lpszName && wcslen(lpszName) && pObject)
	{
		AUTOLOCK_CS(m_SignalObjectMap);
		m_SignalObjectMap.Insert(lpszName, pObject);
	}
}

VOID CDuiCore::RemoveSignalObject(LPCWSTR lpszName)
{
	if ( lpszName && wcslen(lpszName) )
	{
		AUTOLOCK_CS(m_SignalObjectMap);
		m_SignalObjectMap.Remove(lpszName);
	}
}

BOOL CDuiCore::PostMessage(IDuiControlCtrl* Ctrl,INT nMsg,IDuiControlCtrl* pToCtrl,WPARAM wParam ,LPARAM lParam ,LPVOID	lpData)
{
	
	for (IDuiControlCtrl* pCtrl = pToCtrl ; pCtrl ; pCtrl = pCtrl->GetParentCtrl())
	{
		if ( pCtrl->QueryInterface(IIDuiWindowCtrl) )
		{
			return pCtrl->PostMessage(Ctrl, nMsg, wParam , lParam ,lpData);
		}
	}

	return FALSE;
}

LRESULT CDuiCore::SendMessage(IDuiControlCtrl* pCtrl,INT nMsg,IDuiControlCtrl* pToCtrl,WPARAM wParam ,LPARAM lParam ,LPVOID	lpData)
{
	if ( pToCtrl )
	{
		return pToCtrl->SendMessage(pCtrl, nMsg, wParam , lParam ,lpData);
	}

	return 0;
}

VOID CDuiCore::AddPreMessageFilter(IDuiPreMessageFilter* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiPreMessageFilterSet);
		if ( m_DuiPreMessageFilterSet.Find(pFilter) != -1)
		{
			m_DuiPreMessageFilterSet.Add(pFilter);
		}
	}
}

VOID CDuiCore::RemovePreMessageFilter(IDuiPreMessageFilter* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiPreMessageFilterSet);
		INT nIndex = m_DuiPreMessageFilterSet.Find(pFilter);
		if ( nIndex != -1)
		{
			m_DuiPreMessageFilterSet.Delete(nIndex);
		}
	}
}

VOID CDuiCore::AddProMessageFilter(IDuiProMessageFilter* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiProMessageFilterSet);
		if ( m_DuiProMessageFilterSet.Find(pFilter) != -1)
		{
			m_DuiProMessageFilterSet.Add(pFilter);
		}
	}
}

VOID CDuiCore::RemoveProMessageFilter(IDuiProMessageFilter* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiProMessageFilterSet);
		INT nIndex = m_DuiProMessageFilterSet.Find(pFilter);
		if ( nIndex != -1)
		{
			m_DuiProMessageFilterSet.Delete(nIndex);
		}
	}
}

VOID CDuiCore::AddTranslateAccelerator(IDuiTranslateAccelerator* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiTranslateAcceleratorSet);
		if ( m_DuiTranslateAcceleratorSet.Find(pFilter) != -1)
		{
			m_DuiTranslateAcceleratorSet.Add(pFilter);
		}
	}
}
VOID CDuiCore::RemoveTranslateAccelerator(IDuiTranslateAccelerator* pFilter)
{
	if ( pFilter )
	{
		AUTOLOCK_CS(m_DuiTranslateAcceleratorSet);
		INT nIndex = m_DuiTranslateAcceleratorSet.Find(pFilter);
		if ( nIndex != -1)
		{
			m_DuiTranslateAcceleratorSet.Delete(nIndex);
		}
	}
}

DWORD CDuiCore::MessageLoop()
{
	m_hThread = GetCurrentThread();
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) 
	{
		if ( !TranslateMessage(msg) )
		{
			::TranslateMessage(&msg);
		}
		
		if ( PreMessageFilter(msg) )
		{
			::DispatchMessage(&msg);
		}
		
		ProMessageFilter(msg);
	}
	return 0;
}

BOOL CDuiCore::Term(DWORD dwExitCode)
{
	if ( m_hThread )
	{
		::PostThreadMessageA(GetThreadId(m_hThread), WM_QUIT, 0, 0);
		WaitForSingleObject(m_hThread, -1);
	}
	
	return 0;
}

BOOL CDuiCore::InitDuiCore()
{
	return TRUE;
}

BOOL CDuiCore::UnInitDuiCore()
{
	/*
	

	if ( m_pDuiFontSet)
	{
		m_pDuiFontSet->DeleteThis();
		m_pDuiFontSet = NULL;
	}
	*/
	{
		AUTOLOCK_CS(m_DuiPreMessageFilterSet);
		m_DuiPreMessageFilterSet.Clear();
	}

	{
		AUTOLOCK_CS(m_DuiProMessageFilterSet);
		m_DuiProMessageFilterSet.Clear();
	}
	return TRUE;
}

VOID CDuiCore::SetModuleInstance(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

HINSTANCE CDuiCore::GetModuleInstace()
{
	return m_hInstance;
}

IDuiSkin* CDuiCore::GetSkin(LPCWSTR lpszSkinName)
{
	AUTOLOCK_CS(m_SkinMap);
	CDuiSkinMap::ValueType v = m_SkinMap.Find(lpszSkinName);
	if ( v )
	{
		return v->Value;
	}
	return NULL;
}

VOID CDuiCore::AddSkin(LPCWSTR lpszSkinName, IDuiSkin* pTheme)
{
	if ( lpszSkinName && wcslen(lpszSkinName) && pTheme)
	{
		AUTOLOCK_CS(m_SkinMap);
		m_SkinMap.Insert(lpszSkinName, pTheme);
	}
}

VOID CDuiCore::RemoveSkin(LPCWSTR lpszSkinName)
{
	if ( lpszSkinName && wcslen(lpszSkinName))
	{
		AUTOLOCK_CS(m_SkinMap);
		m_SkinMap.Remove(lpszSkinName);
	}
}





BOOL CDuiCore::TranslateMessage(const MSG &msg)
{
	
	AUTOLOCK_CS(m_DuiTranslateAcceleratorSet);
	for (INT nLoop = 0 ; nLoop < m_DuiTranslateAcceleratorSet.Size() ; nLoop++)
	{
		IDuiTranslateAccelerator* tTranslate = m_DuiTranslateAcceleratorSet[nLoop];
		if ( tTranslate )
		{
			BOOL bHandle = FALSE;
			return tTranslate->OnTranslateAccelerator(msg,bHandle);
		}
	}

	return FALSE;
}

BOOL CDuiCore::PreMessageFilter(const MSG &msg )
{
	CDuiWindowCtrl* pWnd = reinterpret_cast<CDuiWindowCtrl*>(::GetProp(msg.hwnd, _T("WndX_WndProc")));
	if ( !pWnd )
		return FALSE;


	DuiMsg duiMsg = { NULL , pWnd, msg.message, msg.wParam, msg.lParam, NULL};
	BOOL bFilter = FALSE;

	do 
	{
		//break;
		if ( (GetWindowStyle(msg.hwnd) & WS_CHILD) == 0)
			break;

		HWND hParentWnd = msg.hwnd;

		for ( HWND hTemp = GetParent(hParentWnd) ; hTemp ; hTemp = GetParent(hTemp))
		{
			hParentWnd = hTemp;
		}
		
		CDuiPoint point;
		GetCursorPos(&point);
		ScreenToClient(hParentWnd, &point);

		switch(msg.message)
		{
		case WM_MOUSELEAVE:
			bFilter = TRUE;
			break;

		case WM_MOUSEMOVE :
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			bFilter = TRUE;
			::SendMessage(hParentWnd, msg.message , msg.wParam ,MAKELPARAM(point.x, point.y));
			break;
		}
		

	} while (FALSE);


	
	if ( bFilter )
		return FALSE;

	AUTOLOCK_CS(m_DuiPreMessageFilterSet);
	for(INT dwLoop = 0 ; dwLoop < m_DuiPreMessageFilterSet.Size() ; dwLoop++)
	{
		BOOL bHandle = TRUE;
		LRESULT lResult = m_DuiPreMessageFilterSet[dwLoop]->OnPreMessageFilter(duiMsg,bHandle);
		if ( !bHandle )
		{
			return FALSE;
		}
	}
	

	return TRUE;
}

BOOL CDuiCore::ProMessageFilter(const MSG &msg)
{
	CDuiWindowCtrl* pWnd = reinterpret_cast<CDuiWindowCtrl*>(::GetProp(msg.hwnd, _T("WndX_WndProc")));
	DuiMsg duiMsg = { NULL , pWnd, msg.message, msg.wParam, msg.lParam, NULL};

	if ( pWnd )
	{
		AUTOLOCK_CS(m_DuiProMessageFilterSet);
		for(INT dwLoop = 0 ; dwLoop < m_DuiProMessageFilterSet.Size() ; dwLoop++)
		{
			BOOL bHandle = FALSE;
			if (m_DuiProMessageFilterSet[dwLoop]->OnProMessageFilter(duiMsg,bHandle))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

VOID CDuiCore::SetCurrentLang(LPCWSTR lpszLangName)
{
	if ( lpszLangName && wcslen(lpszLangName))
	{
		m_strCurrentLang = lpszLangName;
	}
}

LPCWSTR CDuiCore::GetCurrentLang()
{
	return m_strCurrentLang;
}
