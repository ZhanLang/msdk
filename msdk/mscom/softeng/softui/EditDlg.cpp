#include "StdAfx.h"
#include "EditDlg.h"

CString I2S(int nValue, BOOL bZeroSetEmpty = TRUE, int n = 10)
{
	CString str;
	if(0 == nValue && bZeroSetEmpty)
		return str;

	if(n == 10)
		str.Format(_T("%d"), nValue);
	else if(n == 16)
		str.Format(_T("0x%X"), nValue);

	return str;
}

CEditDlg::CEditDlg():m_bNeedSave(FALSE), m_nowSoftid(0),m_nowView(0),m_bShow(FALSE)
{

}

HRESULT	CEditDlg::Init(HWND hParentHwnd, ISoftScan* pSoft)
{
	m_hParentHwnd = hParentHwnd;
	m_pSoftEdit = (ISoftEdit*)pSoft;
	return S_OK;
}

BOOL CEditDlg::CanEdit()
{
	if(m_pSoftEdit)
		return TRUE;

	return FALSE;
}

HRESULT CEditDlg::Uninit()
{
	if(m_pSoftEdit)
		m_pSoftEdit = NULL;

	if(m_hWnd)
	{
		DestroyWindow();
	}

	return S_OK;
}

HRESULT CEditDlg::ShowEdit(SOFTID softid)
{
	if(softid&SOFTID_ENUM_MASK)
	{
		m_bShow = FALSE;
		if(m_hWnd)
			ShowWindow(SW_HIDE);
		return S_FALSE;
	}

	RASSERT(m_pSoftEdit, E_FAIL);

	if(!m_hWnd)
		Create(m_hParentHwnd);
	RASSERT(m_hWnd, E_FAIL);

	m_bShow = TRUE;
	ShowWindow(SW_SHOW);
	
	SOFTID defSoftid = 0, userSoftid = 0;
	m_pSoftEdit->GetCurMaxSoftid(&defSoftid, &userSoftid);

	CString str;
	str.Format(_T("当前库已有最大软件ID(官方库:%d 用户库:%d)"), defSoftid, userSoftid);
	SetDlgItemText(IDC_STATIC_MAXSOFTID, str);

	if(0 == softid)
		softid = defSoftid + 1;

//	int nOldSoftid = GetDlgItemInt(IDC_EDIT_SOFTID);
//	if(nOldSoftid != softid)
		SetDlgItemInt(IDC_EDIT_SOFTID, softid);

	return S_OK;
}

LRESULT CEditDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(FALSE);
	m_bkBrush.CreateSolidBrush(RGB(0xcc, 0xff, 0xff));

	//::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(m_hWnd, NULL, 245, LWA_ALPHA);

	m_viewTab.Attach(GetDlgItem(IDC_TAB_VIEW));
	m_viewTab.InsertItem(0, _T("扫描规则"));
	m_viewTab.InsertItem(1, _T("软件属性"));
	m_viewTab.InsertItem(3, _T("文件规则"));
	m_viewTab.InsertItem(4, _T("路径规则"));

	m_listEdit.Attach(GetDlgItem(IDC_LIST_SOFTEDIT));
	m_listEdit.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT/* | LVS_EX_CHECKBOXES*/);

	if(g_pEditMgr)
	{
		g_pEditMgr->AddDlgEdit(IDC_EDIT1, GetDlgItem(IDC_EDIT1));
		g_pEditMgr->AddDlgEdit(IDC_EDIT2, GetDlgItem(IDC_EDIT2));
		g_pEditMgr->AddDlgEdit(IDC_EDIT3, GetDlgItem(IDC_EDIT3));
		g_pEditMgr->AddDlgEdit(IDC_EDIT4, GetDlgItem(IDC_EDIT4));
	}

	return 0;
}

LRESULT CEditDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(g_pEditMgr)
	{
		g_pEditMgr->DelDlgEdit(IDC_EDIT1);
		g_pEditMgr->DelDlgEdit(IDC_EDIT2);
		g_pEditMgr->DelDlgEdit(IDC_EDIT3);
		g_pEditMgr->DelDlgEdit(IDC_EDIT4);
	}

	return 0;
}

LRESULT CEditDlg::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(wParam != SIZE_MINIMIZED)
	{
		DlgResize_UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return 0;
}

LRESULT CEditDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	if(lpMMI)
	{
		lpMMI->ptMinTrackSize.x = 420;
		lpMMI->ptMinTrackSize.y = 420;
	}

	return 0;
}

LRESULT CEditDlg::OnTcnSelchangeTabView(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	int nSel = m_viewTab.GetCurSel();
	
	BOOL bHave = m_pSoftEdit->IsHave(m_nowSoftid);
	if(!bHave && nSel != 0)
	{
		m_viewTab.SetCurSel(0);
		return 0;
	}

	if(FAILED(RefushView(bHave?nSel+1:VIEW_SOFTRULE)))
	{
		//todo
	}

	return 0;
}

LRESULT CEditDlg::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HDC hdc		= (HDC)wParam;
	HWND hWnd	= (HWND)lParam;

	::SetTextColor(hdc, RGB(255, 0, 0));
	::SetBkMode(hdc, TRANSPARENT);

	HBRUSH hbr = m_bkBrush;
	return (LRESULT)hbr;
}

LRESULT CEditDlg::OnNMCustomdrawMessage(int /*idCtrl*/, LPNMHDR pNMHdr, BOOL& /*bHandled*/)
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

LRESULT CEditDlg::OnEnChangeEditSoftid(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_nowSoftid = GetDlgItemInt(IDC_EDIT_SOFTID);
	if(0 ==m_nowSoftid || m_nowSoftid&SOFTID_ENUM_MASK)
	{
		MessageBox(_T("软件ID应该在 0至0x80000000(2147483648) 之间"), _T("参数错误"), MB_OK|MB_ICONSTOP);
		ShowEdit(0);
	}

	BOOL bHave = m_pSoftEdit->IsHave(m_nowSoftid);
	GetDlgItem(IDC_STATIC_).ShowWindow(bHave?SW_HIDE:SW_SHOW);
	GetDlgItem(IDC_BUTTON_DELSOFT).ShowWindow(bHave?SW_SHOW:SW_HIDE);

	int nView = VIEW_SOFTRULE;
	if(bHave)
		nView = m_nowView?m_nowView:VIEW_SOFTRULE;

	RefushView(nView);
	return 0;
}

LRESULT CEditDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_bNeedSave)
	{
		if(IDYES != ::MessageBox(NULL, _T("当前内容需要保存，是否放弃？"), _T("提示"), MB_YESNO))
			return 0;
	}
	m_bShow = FALSE;
	m_bNeedSave = FALSE;	//强行取消之前的修改
	GetDlgItem(IDC_BUTTON_SAVE).EnableWindow(m_bNeedSave);

	ShowWindow(SW_HIDE);
	return 0;
}

HRESULT CEditDlg::RefushView(int nView)
{
	if(m_bNeedSave)
	{
		RFAILED(SaveSoftInfo());
	}

	switch(nView)
	{
	case VIEW_SOFTRULE:
		ShowSoftRule();
		break;
	case VIEW_ATTRIBUTE:
		ShowSoftAttribute();
		break;
	case VIEW_FILERULE:
		ShowSoftFileRule();
		break;
	case VIEW_PATHRULE:
		ShowSoftPathRule();
		break;
	default:
		return E_INVALIDARG;
	}

	m_nowView = nView;
	return S_OK;
}

HRESULT	CEditDlg::ResetListColum(ColumnItem* pColum)
{
	RASSERT(pColum, E_INVALIDARG);
	m_listEdit.DeleteAllItems();

	BOOL bSel = FALSE;
	GetDlgItem(IDC_BUTTON_ADD).EnableWindow(!bSel);
	GetDlgItem(IDC_BUTTON_DEL).EnableWindow(bSel);
	GetDlgItem(IDC_BUTTON_MOD).EnableWindow(bSel);

	static ColumnItem* lastColum = NULL;
	if(lastColum == pColum)
	{
		int i = 0;
		int nEdit = IDC_EDIT1;
		while(!pColum[i].strName.IsEmpty())
		{
			GetDlgItem(nEdit+i).SetWindowText(_T(""));
			i++;
		}

		return S_OK;
	}

	lastColum = pColum;
	
	while(m_listEdit.DeleteColumn(0));

	int nStatic = IDC_STATIC1;
	int nEdit = IDC_EDIT1;
	for(; nStatic <= IDC_STATIC4; nStatic++, nEdit++)
	{
		GetDlgItem(nStatic).ShowWindow(SW_HIDE);
		GetDlgItem(nEdit).ShowWindow(SW_HIDE);
	}

	int i = 0;
	nStatic = IDC_STATIC1;
	nEdit = IDC_EDIT1;
	while(!pColum[i].strName.IsEmpty())
	{
		const ColumnItem& item = pColum[i];
		m_listEdit.InsertColumn(item.nCol, item.strName, item.nFormat, item.nWidth);

		GetDlgItem(nStatic+i).SetWindowText(item.strName);
		GetDlgItem(nStatic+i).ShowWindow(SW_SHOW);
		GetDlgItem(nEdit+i).SetWindowText(_T(""));
		GetDlgItem(nEdit+i).ShowWindow(SW_SHOW);

		i++;
	}
	m_bNeedSave = FALSE;
	GetDlgItem(IDC_BUTTON_SAVE).EnableWindow(m_bNeedSave);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
enum
{
	RULE_PATH = 0,
	RULE_REGPATH,
	RULE_APPID,
};
ColumnItem columRule[] = 
{
	{RULE_PATH,		_T("path"),		LVCFMT_LEFT, 100},
	{RULE_REGPATH,	_T("regpath"),	LVCFMT_LEFT, 300},
	{RULE_APPID,	_T("appid"),	LVCFMT_RIGHT, 60},
	{0, _T("")}
};

HRESULT CEditDlg::ShowSoftRule()
{
	RASSERT(m_pSoftEdit, E_FAIL);
	ResetListColum(columRule);
	
	SOFTRULE_LIST ruleList;
	m_pSoftEdit->GetSoftRule(m_nowSoftid, ruleList);
	RASSERT(ruleList.GetSize() > 0, S_OK);

	long lSize = ruleList.GetSize();
	long i = 0;
	for(; i < lSize; i++)
	{
		const ST_SOFTRULE& rule = ruleList[i];

		int nIndex = m_listEdit.InsertItem(m_listEdit.GetItemCount(), rule.strPath);
		RASSERT(nIndex >= 0, E_FAIL);

		m_listEdit.SetItemText(nIndex, RULE_REGPATH,rule.strRegPath);
		m_listEdit.SetItemText(nIndex, RULE_APPID,	I2S(rule.dwAppid));
	}
	return S_OK;
}

HRESULT CEditDlg::SaveSoftRule()
{
	RASSERT(VIEW_SOFTRULE == m_nowView && m_listEdit.GetHeader().GetItemCount() == 3, S_FALSE);

	SOFTRULE_LIST ruleList;

	int i = 0;
	int nCount = m_listEdit.GetItemCount();
	while(i < nCount)
	{
		ST_SOFTRULE rule;
		ZeroMemory(&rule, sizeof(rule));

		CString strText;

		m_listEdit.GetItemText(i, RULE_PATH, strText);
		_tcscpy_s(rule.strPath, sizeof(rule.strPath)/sizeof(WCHAR), strText);

		m_listEdit.GetItemText(i, RULE_REGPATH, strText);
		_tcscpy_s(rule.strRegPath, sizeof(rule.strRegPath)/sizeof(WCHAR), strText);

		m_listEdit.GetItemText(i, RULE_APPID, strText);
		rule.dwAppid = _ttoi(strText);

		ruleList.push_back(rule);
		i++;
	}
	
	return  m_pSoftEdit->SetSoftRule(m_nowSoftid, ruleList);
}

//////////////////////////////////////////////////////////////////////////
enum
{
	ATTRIBUTE_KEY = 0,
	ATTRIBUTE_VALUE,
};

ColumnItem columAttribute[] = 
{
	{ATTRIBUTE_KEY,		_T("key"),		LVCFMT_RIGHT, 60},
	{ATTRIBUTE_VALUE,	_T("value"),	LVCFMT_LEFT, 200},
	{0, _T("")}
};

CString GetAttributeKeyName(DWORD dwKey)
{
	CString strName;
	switch(dwKey)
	{
	case SOFT_CLASS:
		strName = _T("组");break;
	case SOFT_NAME:
		strName = _T("名称");break;
	case SOFT_COMPANY:
		strName = _T("公司");break;
	case SOFT_TYPE:
		strName = _T("分类");break;
	case SOFT_UNINSTALLSTR:
		strName = _T("卸载程序");break;
	case SOFT_VER:
		strName = _T("版本");break;
	case SOFT_SIZE:
		strName = _T("大小");break;
	}
	return strName;
}

HRESULT CEditDlg::ShowSoftAttribute()
{
	RASSERT(m_pSoftEdit, E_FAIL);

	ResetListColum(columAttribute);

	SOFTATTRIBUTE_LIST ruleList;
	m_pSoftEdit->GetSoftAttribute(m_nowSoftid, ruleList);
	RASSERT(ruleList.GetSize() > 0, S_OK);

	long lSize = ruleList.GetSize();
	long i = 0;
	for(; i < lSize; i++)
	{
		const ST_SOFTATTRIBUTE& rule = ruleList[i];
		
		DWORD dwKey = rule.dwKey;
		if(dwKey < SOFT_CLASS)	//之前的是动态的，所以不要显示
			continue;

		CString strKeyName = GetAttributeKeyName(dwKey);
		CString strKey;
		if(strKeyName.IsEmpty())
			strKey = I2S(dwKey, FALSE);
		else
			strKey.Format(_T("%d:%s"), dwKey, strKeyName);

		int nIndex = m_listEdit.InsertItem(m_listEdit.GetItemCount(), strKey);
		RASSERT(nIndex >= 0, E_FAIL);

		m_listEdit.SetItemText(nIndex, ATTRIBUTE_VALUE, rule.strValue);
	}
	return S_OK;
}

HRESULT CEditDlg::SaveSoftAttribute()
{
	RASSERT(VIEW_ATTRIBUTE == m_nowView && m_listEdit.GetHeader().GetItemCount() == 2, S_FALSE);

	SOFTATTRIBUTE_LIST ruleList;

	int i = 0;
	int nCount = m_listEdit.GetItemCount();
	while(i < nCount)
	{
		ST_SOFTATTRIBUTE rule;
		ZeroMemory(&rule, sizeof(rule));

		CString strText;

		m_listEdit.GetItemText(i, ATTRIBUTE_KEY, strText);
		rule.dwKey = _ttoi(strText);

		m_listEdit.GetItemText(i, ATTRIBUTE_VALUE, strText);
		_tcscpy_s(rule.strValue, sizeof(rule.strValue)/sizeof(WCHAR), strText);

		ruleList.push_back(rule);
		i++;
	}

	return m_pSoftEdit->SetSoftAttribute(m_nowSoftid, ruleList);
}

//////////////////////////////////////////////////////////////////////////
enum
{
	FILERULE_FILE = 0,
};

ColumnItem columFilerule[] = 
{
	{FILERULE_FILE,	_T("file"),	LVCFMT_LEFT, 200},
	{0, _T("")}
};

HRESULT CEditDlg::ShowSoftFileRule()
{
	RASSERT(m_pSoftEdit, E_FAIL);
	ResetListColum(columFilerule);
	
	SOFTFILE_LIST ruleList;
	m_pSoftEdit->GetSoftFileRule(m_nowSoftid, ruleList);
	RASSERT(ruleList.GetSize() > 0, S_OK);

	long lSize = ruleList.GetSize();
	long i = 0;
	for(; i < lSize; i++)
	{
		const ST_SOFTFILE& rule = ruleList[i];

		int nIndex = m_listEdit.InsertItem(m_listEdit.GetItemCount(), rule.strFile);
		RASSERT(nIndex >= 0, E_FAIL);
	}
	return S_OK;
}

HRESULT CEditDlg::SaveSoftFileRule()
{
	RASSERT(VIEW_FILERULE == m_nowView && m_listEdit.GetHeader().GetItemCount() == 1, S_FALSE);

	SOFTFILE_LIST ruleList;

	int i = 0;
	int nCount = m_listEdit.GetItemCount();
	while(i < nCount)
	{
		ST_SOFTFILE rule;
		ZeroMemory(&rule, sizeof(rule));

		CString strText;

		m_listEdit.GetItemText(i, FILERULE_FILE, strText);
		_tcscpy_s(rule.strFile, sizeof(rule.strFile)/sizeof(WCHAR), strText);

		ruleList.push_back(rule);
		i++;
	}

	return m_pSoftEdit->SetSoftFileRule(m_nowSoftid, ruleList);
}

//////////////////////////////////////////////////////////////////////////
enum
{
	PATHRULE_TYPE = 0,
	PATHRULE_PATH,
};

ColumnItem columPathrule[] = 
{
	{PATHRULE_TYPE,	_T("type"),	LVCFMT_RIGHT, 100},
	{PATHRULE_PATH,	_T("path"),	LVCFMT_LEFT, 200},
	{0, _T("")}
};

CString GetPathTypeName(int nType)
{
	CString strName;
	switch(nType)
	{
	case PATHTYPE_FILE:
		strName = _T("文件");break;
	case PATHTYPE_COM:
		strName = _T("COM组件");break;
	case PATHTYPE_REG_KEY:
		strName = _T("注册表键项");break;
	case PATHTYPE_REG_VALUE:
		strName = _T("注册表键值");break;
	}
	return strName;
}

HRESULT CEditDlg::ShowSoftPathRule()
{
	RASSERT(m_pSoftEdit, E_FAIL);
	ResetListColum(columPathrule);

	SOFTPATH_LIST ruleList;
	m_pSoftEdit->GetSoftPathRule(m_nowSoftid, ruleList);
	RASSERT(ruleList.GetSize() > 0, S_OK);

	long lSize = ruleList.GetSize();
	long i = 0;
	for(; i < lSize; i++)
	{
		const ST_SOFTPATH& rule = ruleList[i];

		CString strTypeName = GetPathTypeName(rule.nType);
		CString strType;
		if(strTypeName.IsEmpty())
			strType = I2S(rule.nType);
		else
			strType.Format(_T("%d:%s"), rule.nType, strTypeName);

		int nIndex = m_listEdit.InsertItem(m_listEdit.GetItemCount(), strType);
		RASSERT(nIndex >= 0, E_FAIL);

		m_listEdit.SetItemText(nIndex, PATHRULE_PATH, rule.strPath);
	}
	return S_OK;
}

HRESULT CEditDlg::SaveSoftPathRule()
{
	RASSERT(VIEW_PATHRULE == m_nowView && m_listEdit.GetHeader().GetItemCount() == 2, S_FALSE);

	SOFTPATH_LIST ruleList;

	int i = 0;
	int nCount = m_listEdit.GetItemCount();
	while(i < nCount)
	{
		ST_SOFTPATH rule;
		ZeroMemory(&rule, sizeof(rule));

		CString strText;

		m_listEdit.GetItemText(i, PATHRULE_TYPE, strText);
		rule.nType = _ttoi(strText);

		m_listEdit.GetItemText(i, PATHRULE_PATH, strText);
		_tcscpy_s(rule.strPath, sizeof(rule.strPath)/sizeof(WCHAR), strText);

		ruleList.push_back(rule);
		i++;
	}

	return m_pSoftEdit->SetSoftPathRule(m_nowSoftid, ruleList);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CEditDlg::EditOneItem(EditAction editAction)
{
	int nItem = m_listEdit.GetSelectedIndex();
	int nColum = m_listEdit.GetHeader().GetItemCount();
	RASSERT(nColum > 0, 0);

	if((nItem < 0 && editAction != EDIT_ADD)
		|| (nItem > 0 && editAction == EDIT_ADD))
		return E_INVALIDARG;


	if(EDIT_DEL == editAction)
	{
		m_bNeedSave = TRUE;
		GetDlgItem(IDC_BUTTON_SAVE).EnableWindow(m_bNeedSave);
		m_listEdit.DeleteItem(nItem);
		return S_OK;
	}

	int n = 0;
	for(n = 0; n < nColum; n++)
	{
		CString strText;
		GetDlgItem(IDC_EDIT1+n).GetWindowText(strText);
		if(!strText.IsEmpty())
			break;
	}

	if(n >= nColum)
	{
		//全为空
		MessageBox(_T("各项值都为空？这不对吧。"), _T("参数错误"), MB_OK|MB_ICONSTOP);
		return E_FAIL;
	}

	for(n = 0; n < nColum; n++)
	{
		CString strText;
		GetDlgItem(IDC_EDIT1+n).GetWindowText(strText);

		if(EDIT_ADD == editAction)
		{
			if(0 == n)
				m_listEdit.InsertItem(m_listEdit.GetItemCount(), strText);
			else
				m_listEdit.SetItemText(m_listEdit.GetItemCount()-1, n, strText);
		}
		else if(EDIT_MOD == editAction)
		{
			m_listEdit.SetItemText(nItem, n, strText);
		}
	}

	m_bNeedSave = TRUE;
	GetDlgItem(IDC_BUTTON_SAVE).EnableWindow(m_bNeedSave);

	return S_OK;
}

HRESULT CEditDlg::SaveSoftInfo()
{
	HRESULT hr = E_FAIL;
	switch(m_nowView)
	{
	case VIEW_SOFTRULE:
		hr = SaveSoftRule();
		break;
	case VIEW_ATTRIBUTE:
		hr = SaveSoftAttribute();
		break;
	case VIEW_FILERULE:
		hr = SaveSoftFileRule();
		break;
	case VIEW_PATHRULE:
		hr = SaveSoftPathRule();
		break;
	default:
		return E_INVALIDARG;
	}

	if(SUCCEEDED(hr))
	{
		m_bNeedSave = FALSE;
		GetDlgItem(IDC_BUTTON_SAVE).EnableWindow(m_bNeedSave);

		if(VIEW_SOFTRULE == m_nowView)
		{
			ShowEdit(m_nowSoftid);
		}
	}
	else
	{
		MessageBox(_T("保存失败了，搞什么搞！"), _T("操作失败"), MB_OK|MB_ICONERROR);
	}
	
	return S_OK;
}

HRESULT CEditDlg::DeleteSoft(SOFTID softid)
{
	return m_pSoftEdit->DelSoft(softid);
}

LRESULT CEditDlg::OnLvnItemchangedListSoftedit(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	RASSERT(pNMLV->uOldState&LVIS_SELECTED || pNMLV->uNewState&LVIS_SELECTED, 0);	

	int nItem = m_listEdit.GetSelectedIndex();
	int nColum = m_listEdit.GetHeader().GetItemCount();
	RASSERT(nColum > 0, 0);

	int n = 0;
	for(; n < nColum; n++)
	{
		CString strText;
		if(nItem >= 0)
			m_listEdit.GetItemText(nItem, n, strText);
		GetDlgItem(IDC_EDIT1+n).SetWindowText(strText);
	}

	BOOL bSel = (nItem >= 0);
	GetDlgItem(IDC_BUTTON_ADD).EnableWindow(!bSel);
	GetDlgItem(IDC_BUTTON_DEL).EnableWindow(bSel);
	GetDlgItem(IDC_BUTTON_MOD).EnableWindow(bSel);
	
	return 0;
}

LRESULT CEditDlg::OnBnClickedButtonAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EditOneItem(EDIT_ADD);
	return 0;
}

LRESULT CEditDlg::OnBnClickedButtonDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EditOneItem(EDIT_DEL);
	return 0;
}

LRESULT CEditDlg::OnBnClickedButtonMod(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EditOneItem(EDIT_MOD);
	return 0;
}

LRESULT CEditDlg::OnBnClickedButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SaveSoftInfo();
	return 0;
}

LRESULT CEditDlg::OnBnClickedButtonDelsoft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(SUCCEEDED(DeleteSoft(m_nowSoftid)))
		ShowEdit(0);
	return 0;
}
