#pragma once

#include "resource.h"
class CNewTaskDialog:
	public CAxDialogImpl<CNewTaskDialog>
{
public:

	enum{IDD = IDD_NEWTASK_DLG};
	CNewTaskDialog(void);
	~CNewTaskDialog(void);
//
	BEGIN_MSG_MAP(CBVDownloadWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog);
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		COMMAND_HANDLER(IDC_BTN_OK, BN_CLICKED, OnBnClickedBtnOk)
		COMMAND_HANDLER(IDC_BTN_CANCEL,BN_CLICKED,OnBnClickedBtnCancel)
	END_MSG_MAP();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	//
	LRESULT OnBnClickedBtnOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LPCTSTR GetUrl();
	LPCTSTR GetSavePath();
	BOOL    IsAutoStart();
	BOOL CNewTaskDialog::SetTextFormClipboard();

	CString GetFileNameByUrl(LPCTSTR lpUrl);
private:
	CString m_strUrl;
	CString m_strPath;
	BOOL    m_bAutoStart;
};
