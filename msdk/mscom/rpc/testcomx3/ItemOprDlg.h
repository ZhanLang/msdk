#pragma once

#include "commx/commx3.h"

// CItemOprDlg dialog

class CItemOprDlg : public CDialog
{
	DECLARE_DYNAMIC(CItemOprDlg)

public:
	CItemOprDlg(CLIENTINFO& ci, CWnd* pParent = NULL);   // standard constructor
	virtual ~CItemOprDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ITEM_OPR };

	CLIENTINFO& m_ci;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	int m_iMsgid;
	int m_rCount;
	CString m_strMsgInfo;
};
