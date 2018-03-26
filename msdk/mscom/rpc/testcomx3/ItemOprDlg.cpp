// ItemOprDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcomx3.h"
#include ".\itemoprdlg.h"


// CItemOprDlg dialog

IMPLEMENT_DYNAMIC(CItemOprDlg, CDialog)
CItemOprDlg::CItemOprDlg(CLIENTINFO& ci, CWnd* pParent /*=NULL*/)
	: CDialog(CItemOprDlg::IDD, pParent)
	, m_ci(ci)
	, m_iMsgid(0)
	, m_rCount(1)
	, m_strMsgInfo(_T(""))
{
}

CItemOprDlg::~CItemOprDlg()
{
}

void CItemOprDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MSGID, m_iMsgid);
	DDX_Text(pDX, IDC_EDIT_RCOUNT, m_rCount);
	DDX_Text(pDX, IDC_EDIT_MSGINFO, m_strMsgInfo);
}


BEGIN_MESSAGE_MAP(CItemOprDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CItemOprDlg message handlers

void CItemOprDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	OnOK();
}
