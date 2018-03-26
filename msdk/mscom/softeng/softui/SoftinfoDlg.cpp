#include "StdAfx.h"
#include "SoftinfoDlg.h"

CSoftinfoDlg::CSoftinfoDlg():m_bShow(FALSE)
{

}

LRESULT CSoftinfoDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(FALSE);
	return 0;
}

LRESULT CSoftinfoDlg::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(wParam != SIZE_MINIMIZED)
	{
		DlgResize_UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return 0;
}

LRESULT CSoftinfoDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	if(lpMMI)
	{
		lpMMI->ptMinTrackSize.x = 165;
		lpMMI->ptMinTrackSize.y = 300;
	}

	return 0;
}

LRESULT CSoftinfoDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bShow = FALSE;
	ShowWindow(SW_HIDE);
	return 0;
}

HRESULT	CSoftinfoDlg::Init(HWND hParentHwnd, ISoftScan* pSoftScan)
{
	m_hParentHwnd = hParentHwnd;
	m_pSoftScan = pSoftScan;
	RASSERT(m_pSoftScan, E_FAIL);
	return S_OK;
}

HRESULT CSoftinfoDlg::Uninit()
{
	if(m_pSoftScan)
	{
		m_pSoftScan = NULL;
	}

	if(m_hWnd)
	{
		DestroyWindow();
	}

	return S_OK;
}

HRESULT CSoftinfoDlg::ShowInfo(SOFTID softid)
{
	if(0 == softid)
	{
		m_bShow = FALSE;
		if(m_hWnd)
			ShowWindow(SW_HIDE);
		return S_FALSE;
	}

	RASSERT(m_pSoftScan, E_FAIL);
	if(!m_hWnd)
		Create(m_hParentHwnd);
	RASSERT(m_hWnd, E_FAIL);

	ShowSoftInfo(softid);
	ShowWindow(SW_SHOW);
	m_bShow = TRUE;

	return S_OK;
}

HRESULT CSoftinfoDlg::ShowSoftInfo(SOFTID softid)
{
	RASSERT(m_pSoftScan, E_FAIL);
	BOOL bInstall = m_pSoftScan->IsExist(softid);

	CString strPath = _T("未安装");
	if(bInstall)
	{
		strPath = m_pSoftScan->GetSoftInfo(softid, SOFT_PATH);
	}

	SOFTFILE_LIST fileList;
	CString strFiles = _T("未安装");
	if(bInstall && SUCCEEDED(m_pSoftScan->GetSoftFile(softid, fileList)))
	{
		strFiles.Empty();
		long lSize = fileList.GetSize();
		for(long i=0; i < lSize; i++)
		{
			if(!strFiles.IsEmpty())
				strFiles += _T("\r\n");
			strFiles += fileList[i].strFile;
		}
	}

	SOFTPATH_LIST pathList;
	CString strPaths;
	if(SUCCEEDED(m_pSoftScan->GetSoftPath(softid, pathList)))
	{
		strPaths.Empty();
		long lSize = pathList.GetSize();
		for(long i=0; i < lSize; i++)
		{
			if(!strPaths.IsEmpty())
				strPaths += _T("\r\n");

			CString strPath;
			strPath.Format(_T("%d >%s"), pathList[i].nType, pathList[i].strPath);
			strPaths += strPath;
		}
	}

	CString strSoftid;
	if(softid&SOFTID_ENUM_MASK)
		strSoftid.Format(_T("0x%X"), softid);
	else
		strSoftid.Format(_T("%d"), softid);
	SetDlgItemText(IDC_EDIT_SOFTID, strSoftid);
	SetDlgItemText(IDC_EDIT_PATH, strPath);
	SetDlgItemText(IDC_EDIT_SOFTFILE, strFiles);
	SetDlgItemText(IDC_EDIT_SOFTPATH, strPaths);

	return S_OK;
}
