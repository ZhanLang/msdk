#pragma once
#include "resource.h"
#include <softeng/softengdef.h>

class CSoftinfoDlg : public CDialogImpl<CSoftinfoDlg>,
	public CDialogResize<CSoftinfoDlg>
{
public:
	enum { IDD = IDD_SOFTINFO };
	CSoftinfoDlg();

	CString GetNewText();

	BEGIN_MSG_MAP(CSoftinfoDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_EDIT_PATH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_SOFTFILE, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC_PATHINFO, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_EDIT_SOFTPATH, DLSZ_MOVE_Y|DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	HRESULT	Init(HWND hParentHwnd, ISoftScan* pSoftScan);
	HRESULT Uninit();
	HRESULT ShowInfo(SOFTID softid);
	BOOL	IsShowed(){return m_bShow;}

private:
	BOOL	m_bShow;
	HWND	m_hParentHwnd;
	ISoftScan*	m_pSoftScan;

	HRESULT ShowSoftInfo(SOFTID softid);
};
