// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"

int S2I(LPCTSTR lpbuf)
{
	RASSERT(lpbuf, 0);

	long l = 0;
	if(0 == _tcsnicmp(lpbuf, _T("0x"), 2))
		l = _tcstoul(lpbuf, NULL, 16);
	else
		l = _tcstoul(lpbuf, NULL, 10);
	return l;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	

	// center the dialog on the screen
	DlgResize_Init(FALSE);
	MoveWindow(0, 0, 640, 480, TRUE);
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	m_bkBrush.CreateSolidBrush(RGB(255, 134, 255));

	UIAddChildWindowContainer(m_hWnd);

	//CheckDlgButton(IDC_CHECK_ALLDB, 1);
	CheckDlgButton(IDC_CHECK_DEFDB, 1);
	CheckDlgButton(IDC_CHECK_USERDB, 1);
	CheckDlgButton(IDC_CHECK_ATTRIBUTE, 1);
	CheckDlgButton(IDC_CHECK_HIDEWHITE, 1);
	m_listSoft.Attach(GetDlgItem(IDC_LIST_SOFT));
	//设置风格
	m_listSoft.ModifyStyle(LVS_TYPEMASK,
		LVS_REPORT |
		LVS_SINGLESEL   //单行选中
		);

	m_listSoft.SetExtendedListViewStyle(
		LVS_EX_GRIDLINES |
		LVS_EX_FULLROWSELECT |
		LVS_EX_FLATSB |
		LVS_EX_TWOCLICKACTIVATE |
		LVS_EX_ONECLICKACTIVATE |
		LVS_EX_TRACKSELECT |
		LVS_EX_GRIDLINES);

	DWORD dwStyle = m_listSoft.GetExtendedListViewStyle();

	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;    //网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件
	m_listSoft.SetExtendedListViewStyle(dwStyle); //设置扩展风格

	RefushListColunm(0);
	RefushSoftList();

	

	if(g_pEditMgr)
	{
		g_pEditMgr->AddDlgEdit(IDC_EDIT_CONDITION, GetDlgItem(IDC_EDIT_CONDITION));
	}

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(g_pEditMgr)
	{
		g_pEditMgr->DelDlgEdit(IDC_EDIT_CONDITION);
	}

	m_infoDlg.Uninit();
	m_editDlg.Uninit();
	m_useSoftScan.Uninit();

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(wParam != SIZE_MINIMIZED)
	{
		DlgResize_UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return 0;
}

LRESULT CMainDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	if(lpMMI)
	{
		lpMMI->ptMinTrackSize.x = 640;
		lpMMI->ptMinTrackSize.y = 480;
	}

	return 0;
}

LRESULT CMainDlg::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HDC hdc		= (HDC)wParam;
	HWND hWnd	= (HWND)lParam;

	::SetTextColor(hdc, RGB(255, 0, 0));
	::SetBkMode(hdc, TRANSPARENT);

	HBRUSH hbr = m_bkBrush;
	return (LRESULT)hbr;
}

LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CloseDialog(IDCANCEL);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

HRESULT CMainDlg::RefushSoftList()
{
	m_listSoft.DeleteAllItems();

	CString sSysDrive;
	GetDlgItemText( IDC_EDIT_SYS, sSysDrive);
	if(FAILED(m_useSoftScan.Init( sSysDrive.GetAt(0))))
	{
		MessageBox(_T("加载扫描引擎失败，不能运行\r\n(没有softeng.dll吧？)"), _T("环境错误"), MB_OK|MB_ICONERROR);
		CloseDialog(IDCANCEL);
		return FALSE;
	}

	m_editDlg.Init(m_hWnd, m_useSoftScan.SoftPtr());
	m_infoDlg.Init(m_hWnd, m_useSoftScan.SoftPtr());

	BOOL bOnlyExist = !IsDlgButtonChecked(IDC_CHECK_ALLDB);
	DWORD softLib = IsDlgButtonChecked(IDC_CHECK_ENUMDB)?SOFTLIB_ENUM|SOFTLIB_MERGE:0;
	if(IsDlgButtonChecked(IDC_CHECK_DEFDB))
		softLib |= SOFTLIB_DEFAULE;
	if(IsDlgButtonChecked(IDC_CHECK_USERDB))
		softLib |= SOFTLIB_USER;

	CString strCondition;

	CString strText;
	GetDlgItemText(IDC_EDIT_CONDITION, strText);
	strCondition = strText;
	if(!strText.IsEmpty() && IsDlgButtonChecked(IDC_CHECK_ATTRIBUTE))
	{
		//key=3 and value like '%瑞星%'
		strCondition.Format(_T("softid in (select distinct softid from attribute where (%s))"), strText);
	}

	SOFTID softid = strCondition.IsEmpty()?m_useSoftScan.FindSoft(bOnlyExist, softLib):m_useSoftScan.FindSoftEx(bOnlyExist, softLib, strCondition);
	while(INVALID_SOFTID != softid)
	{
		LPCTSTR lpszHide = m_useSoftScan.GetSoftInfo( softid, SOFT_WHITE);

		if ( !(IsDlgButtonChecked(IDC_CHECK_HIDEWHITE) && _ttoi(lpszHide)) )
		{
			InsertSoft(softid);
		}
		
		softid = m_useSoftScan.FindNextSoft();
	}

	return S_OK;
}

LRESULT CMainDlg::OnBnClickedButtonScan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	RefushSoftList();
	return 0;
}

VOID CMainDlg::InsertSoft(SOFTID softid)
{
	DWORD dwCount = m_listSoft.GetItemCount();

	CString strBuf;
	strBuf.Format(_T("%d"), dwCount+1);
	int nIndex = m_listSoft.InsertItem(dwCount, strBuf);
	if(nIndex < 0)
	{
		return;
	}

	//m_listSoft.SetItemData(nIndex, softid);
	m_listSoft.SetItemData(nIndex, nIndex);

	if(softid&SOFTID_ENUM_MASK)
		strBuf.Format(_T("0x%X"), softid);
	else
		strBuf.Format(_T("%d"), softid);

	m_listSoft.SetItemText(nIndex, COLUM_SOFIID, strBuf);

	LPCWSTR lpInfo = NULL;

	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_APPID);
	if(lpInfo) m_listSoft.SetItemText(nIndex, COLUM_APPID, lpInfo);

// 	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_CLASS);
// 	if(lpInfo) m_listSoft.SetItemText(nIndex, COLUM_CLASS, m_useSoftScan.GetSoftClass(_wtoi(lpInfo)));
	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_TYPE);
	if(lpInfo) m_listSoft.SetItemText(nIndex, COLUM_CLASS, lpInfo);

	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_NAME);
	if(lpInfo) m_listSoft.SetItemText(nIndex, COLUM_NAME, lpInfo);

	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_PATH);
	if(lpInfo && _tcslen(lpInfo) > 0) 
		m_listSoft.SetItemText(nIndex, COLUM_PATH, lpInfo);
	else if(!m_useSoftScan.IsExist(softid))
		m_listSoft.SetItemText(nIndex, COLUM_PATH, _T("[未安装]"));

	lpInfo = m_useSoftScan.GetSoftInfo(softid, SOFT_COMPANY);
	if(lpInfo) m_listSoft.SetItemText(nIndex, COLUM_COMPANY, lpInfo);

}

HRESULT	CMainDlg::RefushListColunm(int nType)
{
	while(m_listSoft.DeleteColumn(0));	//删除原来所有的列

	//m_listSoft.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP /*| LVS_EX_CHECKBOXES*/);
	m_listSoft.InsertColumn(COLUM_INDEX,  _T("序号"),		LVCFMT_RIGHT, 45);
	m_listSoft.InsertColumn(COLUM_SOFIID, _T("ID"),			LVCFMT_RIGHT, 92);
	m_listSoft.InsertColumn(COLUM_APPID,  _T("应用ID"),		LVCFMT_RIGHT, 60);
	m_listSoft.InsertColumn(COLUM_CLASS,  _T("分组"),		LVCFMT_LEFT, 220);
	m_listSoft.InsertColumn(COLUM_NAME,	  _T("名称"),		LVCFMT_LEFT, 200);
	m_listSoft.InsertColumn(COLUM_COMPANY,_T("公司"),		LVCFMT_LEFT, 45);
	m_listSoft.InsertColumn(COLUM_PATH,	  _T("安装路径"),	LVCFMT_LEFT, 500);

	return S_OK;
}

LRESULT CMainDlg::OnNMCustomdrawMessage(int /*idCtrl*/, LPNMHDR pNMHdr, BOOL& /*bHandled*/)
{
	LPNMLVCUSTOMDRAW  lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHdr); 
	RASSERT(lplvcd, CDRF_DODEFAULT);
	if(lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		// Request prepaint notifications for each item.
		return CDRF_NOTIFYITEMDRAW;
	}
	else if(lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) 	
	{             		
		// Below giving different colors to alternate items. 
		if(!(lplvcd->nmcd.dwItemSpec % 2)) 		
		{ 		
			lplvcd->clrText   = RGB(0, 0, 0);
			//lplvcd->clrTextBk = RGB(0xB6, 0xDD, 0xE8);
			lplvcd->clrTextBk = RGB(37, 172, 30);
		} 		
		else 		
		{ 			
			lplvcd->clrText   = RGB(0, 0, 0); 			
			//lplvcd->clrTextBk = RGB(0xDB, 0xEE, 0xF3); 	
			lplvcd->clrTextBk = RGB(146, 235, 141); 		
		} 

		return CDRF_DODEFAULT;            	
	}

	return CDRF_DODEFAULT;
}

LRESULT CMainDlg::OnLvnGetInfoTipMessage(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	// TODO: Add your control notification handler code here

	RASSERT(pGetInfoTip, 0);
	if (pGetInfoTip->iItem >= 0 && pGetInfoTip->pszText && pGetInfoTip->cchTextMax > 0)
	{
		CString strSoftid;
		m_listSoft.GetItemText(pGetInfoTip->iItem, COLUM_SOFIID, strSoftid);
		SOFTID softid = S2I(strSoftid);//m_listSoft.GetItemData(pGetInfoTip->iItem);

		CString strText;
		strText += _T("  软件属性  [软件:");
		if(m_useSoftScan.IsExist(softid))
			strText += _T("已安装]");
		else
			strText += _T("未安装]");

		strText += _T("\r\n──┬───────────────────");
		DWORD dwKey;
		DWORD enumPos = -1;
		LPCWSTR lpInfo = NULL;
		while(lpInfo = m_useSoftScan.EnumSoftInfo(softid, dwKey, enumPos))
		{
			CString str;
			str.Format(_T("\r\n%4d│%s"), dwKey, lpInfo);
			strText += str;
			strText += _T("\r\n──┼───────────────────");
		}

		int nFind = strText.ReverseFind(_T('┼'));
		if(nFind > 0)
			strText.SetAt(nFind, _T('┴'));

		int nLen = min(strText.GetLength(), pGetInfoTip->cchTextMax-1);
		_tcsncpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, strText.GetBuffer(0), nLen);
		pGetInfoTip->pszText[nLen] = 0x00;
	}

	return 0;
}

LRESULT CMainDlg::OnNMDblclkListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	SOFTID softid = 0;
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;
	if (lpNMItem->iItem >= 0)
	{
		CString strSoftid;
		m_listSoft.GetItemText(lpNMItem->iItem, COLUM_SOFIID, strSoftid);
		softid = S2I(strSoftid);//m_listSoft.GetItemData(lpNMItem->iItem);
	}

	m_editDlg.ShowEdit(softid);
	return 0;
}

LRESULT CMainDlg::OnNMRDblclkListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	SOFTID softid = 0;
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;
	if (lpNMItem->iItem >= 0)
	{
		CString strSoftid;
		m_listSoft.GetItemText(lpNMItem->iItem, COLUM_SOFIID, strSoftid);
		softid = S2I(strSoftid);//m_listSoft.GetItemData(lpNMItem->iItem);
	}
	m_infoDlg.ShowInfo(softid);
	return 0;
}

LRESULT CMainDlg::OnLvnItemchangedListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	RASSERT(pNMLV->uNewState&LVIS_SELECTED, 0);	
	
	int nItem = m_listSoft.GetSelectedIndex();
	SOFTID softid = 0;
	if(nItem >= 0)
	{
		CString strSoftid;
		m_listSoft.GetItemText(nItem, COLUM_SOFIID, strSoftid);
		softid = S2I(strSoftid);//m_listSoft.GetItemData(nItem);
	}

	if(m_infoDlg.IsShowed())
		m_infoDlg.ShowInfo(softid);

	if(m_editDlg.IsShowed())
		m_editDlg.ShowEdit(softid);

	return 0;
}

int	 CMainDlg::SortListItem(int nItem1, int nItem2)
{
	int nSubItem  = m_nSortColumn;
	BOOL bSortNum = m_sortNum;

	CString strItem1, strItem2;
	m_listSoft.GetItemText(nItem1, nSubItem, strItem1);
	m_listSoft.GetItemText(nItem2, nSubItem, strItem2);
	
	int nResult = 0;
	if(bSortNum)
	{
		int n1 = S2I(strItem1);
		int n2 = S2I(strItem2);
		nResult = (n1==n2?0:(n1<n2?-1:1));
	}
	else
	{
		nResult = _stricmp(CW2A(strItem1), CW2A(strItem2));	
		//nResult = strItem1.CompareNoCase(strItem2); //unicode对中文比较有问题
	}

	if(!m_bAsc)
		nResult *= -1;

	return nResult;
}

int CALLBACK SortLVProc(LPARAM lpParam1, LPARAM lpParam2, LPARAM lpParamSort)
{
	CMainDlg* pDlg = (CMainDlg*)lpParamSort;
	RASSERT(pDlg, 0);

	return pDlg->SortListItem(lpParam1, lpParam2);
}

LRESULT CMainDlg::OnLvnColumnclickListSoft(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	RASSERT(pNMHDR && pNMLV->iSubItem >=0, 0);
	m_nSortColumn = pNMLV->iSubItem;

	static int sSortColumn = -1;
	if(sSortColumn == m_nSortColumn)
		m_bAsc = !m_bAsc;
	else
	{
		m_bAsc = TRUE;
		sSortColumn = m_nSortColumn;
	}

	//m_bAsc = FALSE;
	LVCOLUMN lvColumn;
	ZeroMemory(&lvColumn, sizeof(lvColumn));
	lvColumn.mask = LVCF_FMT;
	m_listSoft.GetColumn(m_nSortColumn, &lvColumn);
	m_sortNum = m_nSortColumn==0?TRUE:lvColumn.fmt&LVCFMT_RIGHT;

	m_listSoft.SortItemsEx(SortLVProc, (LPARAM)this);

	return 0;
}


LRESULT CMainDlg::OnBnClickedButtonClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	
	INT dwSelCount = m_listSoft.GetSelectedCount();

	for ( int nCurSel = m_listSoft.GetNextItem(-1, LVNI_ALL); nCurSel >= 0 ; nCurSel = m_listSoft.GetNextItem(nCurSel, LVNI_ALL))
	{
		BOOL bSelect = m_listSoft.GetCheckState(nCurSel);
		if ( bSelect )
		{
			CString strID;
			m_listSoft.GetItemText(nCurSel, COLUM_SOFIID, strID);
			if (strID.GetLength())
			{
				DWORD dwSoftID = S2I(strID);
				
				m_useSoftScan.Clean(dwSoftID, SOFT_CLEAN_ALL, NULL);

			}
		}

	}

	return 0;
}
