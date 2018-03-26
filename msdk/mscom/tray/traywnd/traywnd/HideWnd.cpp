#include "StdAfx.h"
#include "HideWnd.h"

CHideWnd::CHideWnd(void)
:m_hWnd(NULL), m_pWndProc(NULL)
{
}

CHideWnd::~CHideWnd(void)
{
}

HWND CHideWnd::Create(HWND hOwnerWnd, IWinProc* pWndProc/* = NULL*/)
{
	//if ( !::IsWindow(hOwnerWnd) && NULL == pWndProc )
	//	return NULL;

	Destroy();

	// 创建一个隐藏窗体用于接收Tray的消息
	//*
	WNDCLASSEX	wce;
	ZeroMemory(&wce,sizeof(wce));
	wce.lpszClassName	=HIDEWNDCLASSNAME;
	wce.cbSize			=sizeof(wce);
	wce.lpfnWndProc		=WinProc;
	wce.style			=CS_CLASSDC;
	wce.hInstance		=GetModuleHandle(NULL);
	wce.cbWndExtra		=DLGWINDOWEXTRA;
	RegisterClassEx(&wce);
	::SetLastError(0);
	
	
	m_hWnd = CreateWindowEx(0,HIDEWNDCLASSNAME,HIDEWNDNAME,0,0,0,1,1,NULL,NULL,NULL,NULL);
	/* */

	/*m_hHideWnd = ::CreateDialogParam(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDD_DIALOG_HIDDENWINDOW),
		NULL, reinterpret_cast<DLGPROC>(&WinProc), 0);
	 */
	DWORD dwLastErr = ::GetLastError();
	
	if(IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd,SW_HIDE);
		::SetWindowLongPtr(m_hWnd, DWLP_USER, (LONG_PTR)this);

		m_hOwnerWnd					= hOwnerWnd;
		m_pWndProc	= pWndProc;
	}

	return m_hWnd;
}

VOID CHideWnd::Destroy()
{
	// 销毁隐藏窗体和窗体类
	if ( NULL != m_hWnd && ::IsWindow(m_hWnd) )
	{
		::DestroyWindow(m_hWnd);
	}

	UnregisterClass(HIDEWNDCLASSNAME, GetModuleHandle(NULL));

	m_hWnd =NULL;
}

LRESULT CALLBACK CHideWnd::WinProc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 得到当前对象自身的this指针
	CHideWnd	*pThis =(CHideWnd*)::GetWindowLongPtr(hWnd, DWLP_USER);

	if ( NULL != pThis )
	{
		if ( NULL == pThis->m_pWndProc ) 
		{
			// 类用户没有实现IWinProc接口,直接将消息发给Owner窗体
			if (::IsWindow(pThis->m_hOwnerWnd))
				return ::SendMessage(pThis->m_hOwnerWnd, msg, wParam, lParam);
		}
		else if (::IsWindow(hWnd))
		{
			if (WM_CLOSE == msg )
			{
				// 不允许别人Close
				return 0;
			}
			else if ( WM_SHOWWINDOW == msg )
			{
				if ( wParam != FALSE )
					::ShowWindow(pThis->m_hWnd, SW_HIDE);
				return 0;
			}
			else
			{
				BOOL bHandle = FALSE;
				LRESULT l = pThis->m_pWndProc->OnWinMessage(msg, wParam, lParam, bHandle);
				if(bHandle)
					return l;
			}
		}

	}//end of NULL!=pThis

	return ::DefWindowProc(hWnd, msg, wParam, lParam);

}
