#include "StdAfx.h"
#include "NewTaskDialog.h"
#include "resource.h"
#include <atlctrls.h>

CNewTaskDialog::CNewTaskDialog(void)
{
	m_bAutoStart = FALSE;
}

CNewTaskDialog::~CNewTaskDialog(void)
{
}

LRESULT CNewTaskDialog::OnInitDialog(UINT , WPARAM , LPARAM , BOOL& )
{
	WTL::CButton checkBtn = GetDlgItem(IDC_AUTO_START_CHECK);
	SetTextFormClipboard();
	return 0;
}
LRESULT CNewTaskDialog::OnClose(UINT , WPARAM , LPARAM , BOOL& )
{
	return EndDialog(IDCANCEL) ? E_FAIL : S_OK;
}

LRESULT CNewTaskDialog::OnBnClickedBtnOk(WORD , WORD , HWND , BOOL& )
{
	{
		CEdit edit(GetDlgItem(IDC_Url_EDIT));
		edit.GetWindowText(m_strUrl);
	}
	{
		CEdit edit(GetDlgItem(IDC_SavePath_EDIT));
		edit.GetWindowText(m_strPath);
	}

	{
		CButton btn(GetDlgItem(IDC_AUTO_START_CHECK));
		m_bAutoStart = btn.GetCheck();
	}

	return EndDialog(IDOK) ? E_FAIL : S_OK;
}

LRESULT CNewTaskDialog::OnBnClickedBtnCancel(WORD , WORD , HWND , BOOL& )
{
	return EndDialog(IDCANCEL) ? E_FAIL : S_OK;
	
}
LPCTSTR CNewTaskDialog::GetSavePath()
{
	return m_strPath;
}

LPCTSTR CNewTaskDialog::GetUrl()
{
	return m_strUrl;
}

BOOL CNewTaskDialog::IsAutoStart()
{
	return m_bAutoStart;
}

BOOL CNewTaskDialog::SetTextFormClipboard()
{
	ATL::CString strData;
	WCHAR* lpStr = NULL;
	if(::OpenClipboard(m_hWnd))
	{
		HGLOBAL hMem = NULL;
		BOOL bWChar = FALSE;
		if(IsClipboardFormatAvailable(CF_TEXT))
		{
			bWChar = FALSE;
			hMem = GetClipboardData(CF_TEXT);
		}
		else if(IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			bWChar = TRUE;
			hMem = GetClipboardData(CF_UNICODETEXT);
		}

		if(NULL != hMem)
		{
			if (bWChar)
			{
				lpStr =  (WCHAR*)::GlobalLock(hMem);
			}
			else
			{
				char* lpStrA = (char*)::GlobalLock(hMem);
				if (lpStrA)
				{
					USES_CONVERSION;
					lpStr = A2W(lpStrA);
				}
			}

			::GlobalUnlock(hMem);

		}
		::CloseClipboard();
	}

	BOOL bFind = FALSE;
	if (lpStr)
	{
		strData = lpStr;
		//strData = strData.MakeLower();
		if (strData.Find(L"http://") == 0 ||
			strData.Find(L"https://") == 0/*||*/
// 			strData.Find(L"ftp://") == 0  ||
// 			strData.Find(L"file://") == 0
			)
		{
			bFind = TRUE;
			SetDlgItemText(IDC_Url_EDIT , strData);
		}
	}

	SetDlgItemText(IDC_SavePath_EDIT,GetFileNameByUrl(bFind?lpStr:NULL));


	return TRUE;
}

CString CNewTaskDialog::GetFileNameByUrl(LPCTSTR lpUrl)
{
	CString strUrl = _T("C:\\DWONLOAD\\");
	if (lpUrl && _tcslen(lpUrl))
	{
		CString strFileName;
		strFileName = lpUrl;
		strFileName = strFileName.Mid(strFileName.ReverseFind('/')+1);
		strUrl += strFileName;
	}
	return strUrl;
}