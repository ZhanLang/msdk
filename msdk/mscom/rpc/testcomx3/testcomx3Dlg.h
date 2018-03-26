// testcomx3Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"


#include "afxcmn.h"

#define MAX_BUFFER_LEN		(MAX_PATH*2)

#define WM_MY_ONMESSAGE		(WM_USER + 2008)

// Ctestcomx3Dlg 对话框
class Ctestcomx3Dlg : public CDialog
			, public IMessageCallBack
			, public CUnknownImp
{
// 构造
public:
	UNKNOWN_IMP1(IMessageCallBack)

	Ctestcomx3Dlg(CWnd* pParent = NULL);	// 标准构造函数

	TCHAR m_szAppDatapPath[MAX_BUFFER_LEN];

	CProcCom3Help	m_hComHelp;

	UTIL::com_ptr<ICCenter> m_pCCenter;
	UTIL::com_ptr<IClientObject> m_pClient;

	virtual HRESULT Dispatch(IMessage* pMsg);

	void ShowUI();

	INT m_iRightSelObject;

	typedef CArray<CLIENTINFO> CClientList;
	CClientList m_arList;
// 对话框数据
	enum { IDD = IDD_TESTCOMX3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonLogon();
	afx_msg void OnBnClickedButtonLogoff();
	afx_msg void OnBnClickedButtonRefresh();
	CString m_strSubsysName;
	CString m_strWorkPath;
	DWORD	m_defid;
	CComboBox m_cKnownSubsys;
	CString m_strOnMessages;
	//CListCtrlEx m_cList;
	CListCtrl m_cList;
	afx_msg void OnNMRclickListObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRightSendmessage();
	LRESULT OnMyMessage(WPARAM, LPARAM);
	afx_msg void OnCbnSelchangeComboKnownsys();
};
