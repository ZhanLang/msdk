#pragma once
#include "resource.h"
#include <softeng/softengdef.h>

struct ColumnItem
{
	int			nCol;
	CString		strName;
	int			nFormat;
	int			nWidth;
};

enum EditAction
{
	EDIT_ADD = 1,
	EDIT_DEL,
	EDIT_MOD,
};

enum
{
	VIEW_SOFTRULE = 1,
	VIEW_ATTRIBUTE,
	VIEW_FILERULE,
	VIEW_PATHRULE,
};

class CEditDlg : public CDialogImpl<CEditDlg>,
	public CDialogResize<CEditDlg>
{
public:
	enum { IDD = IDD_EDIT };

	CEditDlg();

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		//NOTIFY_HANDLER(IDC_LIST_SOFTEDIT, NM_CUSTOMDRAW, OnNMCustomdrawMessage)
		NOTIFY_HANDLER(IDC_TAB_VIEW, TCN_SELCHANGE, OnTcnSelchangeTabView)
		NOTIFY_HANDLER(IDC_LIST_SOFTEDIT, LVN_ITEMCHANGED, OnLvnItemchangedListSoftedit)
		COMMAND_HANDLER(IDC_BUTTON_ADD, BN_CLICKED, OnBnClickedButtonAdd)
		COMMAND_HANDLER(IDC_BUTTON_DEL, BN_CLICKED, OnBnClickedButtonDel)
		COMMAND_HANDLER(IDC_BUTTON_MOD, BN_CLICKED, OnBnClickedButtonMod)
		COMMAND_HANDLER(IDC_BUTTON_SAVE, BN_CLICKED, OnBnClickedButtonSave)
		COMMAND_HANDLER(IDC_EDIT_SOFTID, EN_CHANGE, OnEnChangeEditSoftid)
		COMMAND_HANDLER(IDC_BUTTON_DELSOFT, BN_CLICKED, OnBnClickedButtonDelsoft)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_LIST_SOFTEDIT, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_BUTTON_ADD, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_DEL, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_MOD, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_SAVE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC1, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC2, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC3, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC4, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT1, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT2, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT3, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT4, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNMCustomdrawMessage(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnTcnSelchangeTabView(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnLvnItemchangedListSoftedit(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

	LRESULT OnBnClickedButtonAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonMod(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDelsoft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditSoftid(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	HRESULT	Init(HWND hParentHwnd, ISoftScan* pSoft);
	BOOL	CanEdit();
	HRESULT Uninit();
	HRESULT ShowEdit(SOFTID softid);
	BOOL	IsShowed(){return m_bShow;}

private:
	CTabCtrl		m_viewTab;
	CListViewCtrl	m_listEdit;
	CBrush			m_bkBrush;
	HWND			m_hParentHwnd;
	ISoftEdit*	m_pSoftEdit;
	int		m_nowSoftid;
	int		m_nowView;
	BOOL	m_bNeedSave;
	BOOL	m_bShow;

	HRESULT	ResetListColum(ColumnItem* pColum);
	HRESULT RefushView(int nView);

	HRESULT ShowSoftRule();
	HRESULT SaveSoftRule();
	HRESULT ShowSoftAttribute();
	HRESULT SaveSoftAttribute();
	HRESULT ShowSoftFileRule();
	HRESULT SaveSoftFileRule();
	HRESULT ShowSoftPathRule();
	HRESULT SaveSoftPathRule();
	HRESULT EditOneItem(EditAction editAction);
	HRESULT SaveSoftInfo();
	HRESULT DeleteSoft(SOFTID softid);
};
