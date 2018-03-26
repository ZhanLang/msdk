#include "StdAfx.h"
#include "EditTextDlg.h"

CEditTextDlg::CEditTextDlg(LPCTSTR lpText)
{
	if(lpText)
		m_strText = lpText;
}

CString CEditTextDlg::GetNewText()
{
	return m_strText;
}

LRESULT CEditTextDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(FALSE);

	SetDlgItemText(IDC_EDIT_EDIT, m_strText);

	return 0;
}

LRESULT CEditTextDlg::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(wParam != SIZE_MINIMIZED)
	{
		DlgResize_UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return 0;
}

LRESULT CEditTextDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CEditTextDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItemText(IDC_EDIT_EDIT, m_strText);
	EndDialog(IDOK);
	return 0;
}

LRESULT CEditTextDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}
