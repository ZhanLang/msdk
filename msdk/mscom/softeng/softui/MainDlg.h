// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "UseSoftScan.h"
#include <atlctrls.h>
#include <atlframe.h>
#include "editdlg.h"
#include "SoftinfoDlg.h"
enum
{
	COLUM_INDEX = 0,
	COLUM_SOFIID,
	COLUM_APPID,
	COLUM_NAME,
	COLUM_COMPANY,
	COLUM_CLASS,
	COLUM_PATH,
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
	public CMessageFilter, public CIdleHandler,
	public CDialogResize<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_HANDLER(IDC_BUTTON_SCAN, BN_CLICKED, OnBnClickedButtonScan)
		NOTIFY_HANDLER(IDC_LIST_SOFT, NM_CUSTOMDRAW, OnNMCustomdrawMessage)
		NOTIFY_HANDLER(IDC_LIST_SOFT, LVN_GETINFOTIP, OnLvnGetInfoTipMessage)
		NOTIFY_HANDLER(IDC_LIST_SOFT, NM_DBLCLK, OnNMDblclkListSoft)
		NOTIFY_HANDLER(IDC_LIST_SOFT, NM_RDBLCLK, OnNMRDblclkListSoft)
		NOTIFY_HANDLER(IDC_LIST_SOFT, LVN_ITEMCHANGED, OnLvnItemchangedListSoft)
		NOTIFY_HANDLER(IDC_LIST_SOFT, LVN_COLUMNCLICK, OnLvnColumnclickListSoft)
		COMMAND_HANDLER(IDC_BUTTON_CLEAN, BN_CLICKED, OnBnClickedButtonClean)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_LIST_SOFT, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNMCustomdrawMessage(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnLvnGetInfoTipMessage(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNMDblclkListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNMRDblclkListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnLvnItemchangedListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnLvnColumnclickListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
	LRESULT OnBnClickedButtonScan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	HRESULT	RefushListColunm(int nType);
	HRESULT RefushSoftList();

	VOID InsertSoft(SOFTID softid);

	int		m_nSortColumn;
	BOOL	m_sortNum;
	BOOL	m_bAsc;
	int	 SortListItem(int nItem1, int nItem2);
private:
	CListViewCtrl	m_listSoft;
	CUseSoftScan	m_useSoftScan;
	CBrush			m_bkBrush;
	CEditDlg		m_editDlg;
	CSoftinfoDlg	m_infoDlg;
public:
	LRESULT OnBnClickedButtonClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
