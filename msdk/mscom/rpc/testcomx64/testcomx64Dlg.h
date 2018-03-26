
// testcomx64Dlg.h : header file
//

#pragma once


#define MAX_BUFFER_LEN		(MAX_PATH*2)

#define WM_MY_ONMESSAGE		(WM_USER + 2008)


// Ctestcomx64Dlg dialog
class Ctestcomx64Dlg : public CDialog
	, public IMessageCallBack
	, public CUnknownImp
{
// Construction
public:
	UNKNOWN_IMP1(IMessageCallBack)

	Ctestcomx64Dlg(CWnd* pParent = NULL);	// standard constructor

	TCHAR m_szAppDatapPath[MAX_BUFFER_LEN];

	CProcCom3Help	m_hComHelp;

	UTIL::com_ptr<ICCenter> m_pCCenter;
	UTIL::com_ptr<IClientObject> m_pClient;

	virtual HRESULT Dispatch(IMessage* pMsg);

	void ShowUI();

	INT m_iRightSelObject;

	typedef CArray<CLIENTINFO> CClientList;
	CClientList m_arList;
// Dialog Data
	enum { IDD = IDD_TESTCOMX3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
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
