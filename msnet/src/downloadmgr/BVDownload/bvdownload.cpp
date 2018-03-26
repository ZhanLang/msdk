// bvdownload.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "bvdownload.h"
#include <algorithm>
#include <map>
#include "NewTaskDialog.h"
#include <shlobj.h>
#include <commctrl.h>

enum enumListColumn
{
    LC_Stat_Index = 0,
    LC_Stat_URL,				   // URL
    LC_Stat_StartType,			   // 开始状态
    LC_Stat_FileComplete,          // 完成状态
    LC_Stat_FileSavePath,          // 下载文件保存路径
    LC_Stat_FileLength,            // 下载文件长度
    LC_Stat_CreateTime,			   // 创建时间
    LC_Stat_DownloadSize,          // 下载总字节
    LC_Stat_DownloadTimes,         // 下载时间
    LC_Stat_DownloadRate,          // 下载速度
	LC_Stat_NeedTime,              // 预计所花的时间
    LC_Stat_DownloadPercent,	   // 完成百分比
	
    LC_Null = 999,
};

struct ListColumnDef
{
    enumListColumn listColumn;
    LPCTSTR        strText;
    INT			   nWidth;
};

static ListColumnDef g_ListColumnDef[] =
{
    {LC_Stat_Index,			L"序号", 70},
    {LC_Stat_URL,			L"地址", 70},

    {LC_Stat_FileSavePath,	L"路径", 50},
    {LC_Stat_StartType,     L"开始状态", 60},
    {LC_Stat_CreateTime,	L"创建时间", 115},
    {LC_Stat_FileComplete,  L"完成状态", 60},
    {LC_Stat_FileLength,	L"文件大小", 60},
    {LC_Stat_DownloadSize,	L"下载大小", 60},
    {LC_Stat_DownloadTimes, L"耗时", 60},
    {LC_Stat_DownloadRate,	L"下载速度", 80},
	{LC_Stat_NeedTime,	    L"预计所需要", 80},
    {LC_Stat_DownloadPercent, L"完成百分比", 72},

    {LC_Null , NULL , 0},
};

enum
{
    MENU_Download_Start = 1,
    MENU_Download_Pause,
    MENU_Download_Resume,
    MENU_Download_Stop,
    MENU_Download_Delete,
	MENU_Download_Restart,
};


std::map<DWORD , CString> g_FileCompleteStateStrMap;
static struct _G_INIT
{
    _G_INIT()
    {
        g_FileCompleteStateStrMap[FCS_Fiald]       = _T("下载失败");
        g_FileCompleteStateStrMap[FCS_Success]     = _T("下载成功");
       // g_FileCompleteStateStrMap[FCS_Pause]       = _T("用户暂停");
        g_FileCompleteStateStrMap[FCS_Stop]        = _T("用户停止");
        g_FileCompleteStateStrMap[FCS_Wait]        = _T("等待下载");
        g_FileCompleteStateStrMap[FCS_Downloading] = _T("正在下载");
        g_FileCompleteStateStrMap[FCS_Unknow]      = _T("未知状态");
    }
} G_INIT;


CBVDownloadWindow::CBVDownloadWindow()
{
    m_currentSelect = -1;
    m_pDownloadMgr = NULL;
}
CBVDownloadWindow::~CBVDownloadWindow()
{
    
    m_dlHelper.UnInit();
}

LRESULT CBVDownloadWindow::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    m_SafeList.InitAlloc();
    DlgResize_Init();

    CMessageLoop *pLoop = Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);
    UIAddMenuBar(m_hWnd);
    this->CenterWindow();
    InitDLTaskMgrListCtr();

    m_dlHelper.Init(_T("downmgr.dll"));
    m_pDownloadMgr = m_dlHelper.CreateDownLoadMgr(this, _T("bvdownload"));
    if (!m_pDownloadMgr)
    {
        return 0;
    }
	DWORD dwMask = DefaultNotifyMsgMask | Msg_Downloading;
	//m_pDownloadMgr->SetOption(DLO_NotifyMsgMask,&dwMask,sizeof(dwMask));
    m_NextClipWnd = SetClipboardViewer();
    RefreshTask();
    return 0;
}


LRESULT CBVDownloadWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    CMessageLoop *pLoop = Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveIdleHandler(this);
    pLoop->RemoveMessageFilter(this);
	
	if (m_pDownloadMgr)
	{
		m_dlHelper.DistoryDownLoadMgr(&m_pDownloadMgr);
		m_pDownloadMgr = NULL;
	}

	m_SafeList.Stop();
    m_SafeList.UninitAlloc();

    if (m_NextClipWnd)
    {
        ::ChangeClipboardChain(m_hWnd , m_NextClipWnd);
    }

    return 0;
}

LRESULT CBVDownloadWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    m_pDownloadMgr->Stop(AnyUrlHash);
    m_dlHelper.DistoryDownLoadMgr(&m_pDownloadMgr);

    DestroyWindow();
    ::PostQuitMessage(IDCANCEL);
    return 0;
}

BOOL CBVDownloadWindow::PreTranslateMessage(MSG *pMsg)
{
    return CWindow::IsDialogMessage(pMsg);;
}


BOOL CBVDownloadWindow::OnIdle()
{
    UIUpdateMenuBar(FALSE, TRUE);
    return FALSE;
}

LRESULT CBVDownloadWindow::OnChangeCBChain(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if ((HWND)wParam == m_NextClipWnd)
    {
        m_NextClipWnd = (HWND)lParam;
    }
    else if (m_NextClipWnd != NULL)
    {
        SendMessage(m_NextClipWnd , uMsg , wParam , lParam);
    }
    return 0;
}


LRESULT CBVDownloadWindow::OnDrawClipBoard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ATL::CString strData;

    WCHAR *lpStr = NULL;
    if(::OpenClipboard(m_hWnd))
    {
        HGLOBAL hMem = NULL;
        BOOL bWChar = FALSE;
        if(IsClipboardFormatAvailable(CF_TEXT))
        {
            bWChar = FALSE;
            hMem = GetClipboardData(CF_TEXT);
        }
        else if(IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            bWChar = TRUE;
            hMem = GetClipboardData(CF_UNICODETEXT);
        }

        if(NULL != hMem)
        {
            if (bWChar)
            {
                lpStr =  (WCHAR *)::GlobalLock(hMem);
            }
            else
            {
                char *lpStrA = (char *)::GlobalLock(hMem);
                if (lpStrA)
                {
                    USES_CONVERSION;
                    lpStr = A2W(lpStrA);
                }
            }

            ::GlobalUnlock(hMem);

        }
        ::CloseClipboard();
    }

    if (lpStr)
    {
        strData = lpStr;
        strData = strData.MakeLower();
        if (strData.Find(L"http://") == 0 ||
                strData.Find(L"https://") == 0/*||*/
                //strData.Find(L"ftp://") == 0  ||
                //strData.Find(L"file://") == 0
           )
        {
            PostMessage(WM_COMMAND, MAKELONG(IDM_NewTask, BN_CLICKED), 0);
        }
    }
    if (::IsWindow(m_NextClipWnd))
    {
        SendMessage(m_NextClipWnd , uMsg , wParam , lParam);
    }

    return 0;
}

LRESULT CBVDownloadWindow::OnNMRclickListLeak(int , LPNMHDR pNMHDR, BOOL & )
{
    LPNMLISTVIEW pListView = (LPNMLISTVIEW)(pNMHDR);
    m_currentSelect = pListView->iItem;

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return 0;


    do
    {
        if (m_currentSelect == -1)
        {
            InsertMenu(hMenu, -1, MF_BYPOSITION, IDM_NewTask , _T("创建"));
            break;
        }


        sha1_hash hash = m_downloadTasks[m_currentSelect].hash;
        const DownLoadTaskInfo *pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
        if (pTaskInfo->Fsc == FCS_Downloading)
        {
            InsertMenu(hMenu, -1, MF_BYPOSITION, MENU_Download_Stop, _T("停止"));
        }
        else if(pTaskInfo->Fsc == FCS_Success)
        {
            InsertMenu(hMenu,-1,MF_BYPOSITION,MENU_Download_Restart,_T("重新开始"));
        }
		else
		{
			InsertMenu(hMenu, -1, MF_BYPOSITION, MENU_Download_Start, _T("开始"));
		}
        InsertMenu(hMenu, -1, MF_BYPOSITION, MENU_Download_Delete, _T("删除"));
    }
    while (FALSE);

    if(GetMenuItemCount(hMenu) > 0)
    {
        POINT po;
        GetCursorPos(&po);
        ::TrackPopupMenu(hMenu, TPM_LEFTALIGN, po.x, po.y, 0, m_hWnd, NULL);
    }

    ::DestroyMenu(hMenu);

    // 	m_nMenuItem = pListView->iItem;
    // 	if(m_nMenuItem < 0)
    // 		return 0;

    return 0;
}

LRESULT CBVDownloadWindow::OnCommand(UINT , WPARAM wParam, LPARAM , BOOL &bHandled)
{
    WORD crtid = LOWORD(wParam);
    WORD code  = HIWORD(wParam);


    sha1_hash hash = NullUrlHash;
    
    switch(crtid)
    {
    case IDM_NewTask:
    {
        CNewTaskDialog newTaskDlg;
        if (newTaskDlg.DoModal(m_hWnd) == IDOK)
        {
            CreateDownloadTask(newTaskDlg.GetUrl(), newTaskDlg.GetSavePath(), newTaskDlg.IsAutoStart());
        }
        break;
    }

    case MENU_Download_Start:
    {
		if (m_currentSelect != -1)
		{
			hash = m_downloadTasks[m_currentSelect].hash;
		}
        m_pDownloadMgr->Start(hash);
        break;
    }
    case MENU_Download_Stop:
    {
		if (m_currentSelect != -1)
		{
			hash = m_downloadTasks[m_currentSelect].hash;
		}
        m_pDownloadMgr->Stop(hash);
        break;
    }
    case MENU_Download_Delete:
    {
		if (m_currentSelect != -1)
		{
			hash = m_downloadTasks[m_currentSelect].hash;
		}
        m_pDownloadMgr->Delete(hash, TRUE);
        RefreshTask();
        break;
    }
    case IDC_StartAll_Btn:
    {
        if (m_pDownloadMgr)
        {
            m_pDownloadMgr->Start(AnyUrlHash);
        }
        break;
    }
    case IDC_StopAll_Btn:
    {
        if (m_pDownloadMgr)
        {
            m_pDownloadMgr->Stop(AnyUrlHash);
        }
        break;
    }
	case IDC_DelAll_Btn:
		if(m_pDownloadMgr)
		{
			m_pDownloadMgr->Delete(AnyUrlHash,TRUE);
			RefreshTask();
		}
		break;
	case MENU_Download_Restart:
		{
			if (m_currentSelect != -1)
			{
				hash = m_downloadTasks[m_currentSelect].hash;
			}

			m_pDownloadMgr->Stop(hash,TRUE);
			const DownLoadTaskInfo* pTask = m_pDownloadMgr->QueryTaskInfo(hash);
			DeleteFile(pTask->lpSavePath);
			m_pDownloadMgr->Start(hash);
		}
    }
    return 0;
}

HRESULT CBVDownloadWindow::InitDLTaskMgrListCtr()
{
    m_dlTaskMgrListControl = (GetDlgItem(IDC_DL_TASK_MGR_LIST));

    m_dlTaskMgrListControl.ModifyStyle(LVS_TYPEMASK,
                                       LVS_REPORT |
                                       LVS_SINGLESEL   //单行选中
                                      );

    m_dlTaskMgrListControl.SetExtendedListViewStyle(
        LVS_EX_GRIDLINES |
        LVS_EX_FULLROWSELECT |
        LVS_EX_FLATSB |
        LVS_EX_TWOCLICKACTIVATE |
        LVS_EX_ONECLICKACTIVATE |
        LVS_EX_TRACKSELECT |
        LVS_EX_GRIDLINES);

    DWORD dwStyle = m_dlTaskMgrListControl.GetExtendedListViewStyle();

    dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES;    //网格线（只适用与report风格的listctrl）
    //dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件

    m_dlTaskMgrListControl.SetExtendedListViewStyle(dwStyle); //设置扩展风格
    for (int nIndex = 0 ; g_ListColumnDef[nIndex].strText != NULL ; nIndex++)
    {

		LV_COLUMN lvCol = {0};

		lvCol.mask = g_ListColumnDef[nIndex].mask;
		lvCol.fmt = g_ListColumnDef[nIndex].Fmt;
		lvCol.pszText = (LPWSTR)g_ListColumnDef[nIndex].strText;
		lvCol.cx = g_ListColumnDef[nIndex].nWidth;
		//lvCol.iSubItem =1;


		ListView_InsertColumn(m_pResultList->m_hWnd,g_ListColumnDef[nIndex].listColumn,&lvCol);

        m_dlTaskMgrListControl.InsertColumn(g_ListColumnDef[nIndex].listColumn, g_ListColumnDef[nIndex].strText, LVCFMT_LEFT | HDF_SORTDOWN, g_ListColumnDef[nIndex].nWidth);
    }

    return S_OK;
}

LRESULT CBVDownloadWindow::OnDownLoadMsg(UINT msg, WPARAM wp, LPARAM lp, BOOL &bHandled)
{
    msdk::CSafeList<DownLoadMsgInfo>::SAFELISTIMPL values;
    m_SafeList.GetAllAndDelete(values, 0);

    msdk::CSafeList<DownLoadMsgInfo>::SAFELISTIMPL::iterator it = values.begin();
    for (; it != values.end(); it++)
    {
        const DownLoadMsgInfo &msgInfo = *it;
        sha1_hash hash = msgInfo.hash;
        NotifyMessageID msgid = msgInfo.msgid;
        WPARAM wp = msgInfo.wp;
        LPARAM lp = msgInfo.lp;



        DWORD dwIndex = GetTaskIndex(hash);
        switch(msgid)
        {
        case Msg_QueryInfo:
        {
            if (dwIndex != -1)
            {
                m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_FileComplete, L"正在请求...");
            }
            break;
        }
        case  Msg_DownloadSpeed:
        {
            FLOAT dwSpeed = (FLOAT)wp;
            dwSpeed = dwSpeed / FLOAT(1024);

            CString strSpeed;
            if (dwSpeed / 1024 > 1)
            {
                dwSpeed = dwSpeed / FLOAT(1024);
                strSpeed.Format(_T("%4.2f(M/S)"), dwSpeed);
            }
            else
            {
                strSpeed.Format(_T("%4.2f(K/S)"), dwSpeed);
            }

            if (dwIndex == -1) //全局速度
            {
                CString strGlobalSpeed;
                strGlobalSpeed.Format(_T("全局速度:%s"), strSpeed);
                SetDlgItemText(IDC_GLOBAL_STATIC, strGlobalSpeed);
            }
            else
            {
                const DownLoadTaskInfo *pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
                m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_DownloadRate, strSpeed);
                UpdataTaskItem(dwIndex, pTaskInfo);
            }

            break;
        }
        case Msg_FileComplete:
        {
            if (dwIndex != -1)
            {
                const DownLoadTaskInfo *pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
               
                UpdataTaskItem(dwIndex, pTaskInfo);
            }

            break;
        }
        case Msg_StartDownload:
        {
            if (dwIndex != -1)
            {
                const DownLoadTaskInfo *pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
                CString strStartType;
                StartDownloadState startType = (StartDownloadState)wp;
                strStartType = (startType == SDS_Start ? L"开始下载" : L"断点续下");
                m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_StartType, strStartType);
                m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_FileLength, FormatUINT64(pTaskInfo->DLSize.totalSize));
            }
            break;
        }
		case Msg_Downloading:
			{
				if (dwIndex != -1)
				{
					const DownLoadTaskInfo *pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);

					UpdataTaskItem(dwIndex, pTaskInfo,TRUE);
				}
				break;
			}
        }
    }

    return S_OK;
    //
}

VOID CBVDownloadWindow::OnDownLaodNotify(const sha1_hash &hash , NotifyMessageID msgid , WPARAM wp, LPARAM lp)
{
    DownLoadMsgInfo msgInfo;
    msgInfo.hash = hash;
    msgInfo.msgid = msgid;
    msgInfo.wp = wp;
    msgInfo.lp = lp;

    m_SafeList.push_back(msgInfo);

    PostMessage(RefreshMsg, NULL, NULL);

}

VOID CBVDownloadWindow::OnEnumDownLoadTask(const sha1_hash &hash)
{
    const DownLoadTaskInfo	*pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
    if (pTaskInfo)
    {
        DownloadTaskItemInfo taskItem;
        taskItem.hash = hash;
        taskItem.createTime = pTaskInfo->DLTime.createTime;
        m_downloadTasks.insert(m_downloadTasks.end(), taskItem);
    }


}

HRESULT CBVDownloadWindow::RefreshTask()
{
    m_downloadTasks.clear();
    HRESULT hr = m_pDownloadMgr->EnumDownloadTask(this);
    std::sort(m_downloadTasks.begin(), m_downloadTasks.end(), lessTaskItem());


    m_dlTaskMgrListControl.DeleteAllItems();
    DWORD dwCount = m_downloadTasks.size();
    for (DWORD dwIndex = 0 ; dwIndex < dwCount ; dwIndex++)
    {
        InertTaskItem(m_downloadTasks[dwIndex].hash);
    }

    return hr;
}

HRESULT CBVDownloadWindow::InertTaskItem(const sha1_hash &hash)
{
    const DownLoadTaskInfo	*pTaskInfo = m_pDownloadMgr->QueryTaskInfo(hash);
    DWORD dwCount = m_dlTaskMgrListControl.GetItemCount();
    CString strIndex;
    strIndex.Format(_T("%d"), dwCount + 1);
    int nIndex = m_dlTaskMgrListControl.InsertItem(dwCount, strIndex);
    if(nIndex < 0)
    {
        return E_FAIL;
    }


    CString strDownloadPercent;
    if (pTaskInfo->DLSize.currentSize)
    {
        FLOAT flPercent = FLOAT(pTaskInfo->DLSize.currentSize) / FLOAT(pTaskInfo->DLSize.totalSize);
        flPercent = flPercent * FLOAT(100);;
        strDownloadPercent.Format(_T("%9.2f%%"), flPercent);
    }
    else
    {
        strDownloadPercent.Format(_T("%d%%"), 0);
    }

    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_StartType, _T("未开始"));
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_URL, pTaskInfo->lpUrl);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileComplete, g_FileCompleteStateStrMap[pTaskInfo->Fsc]);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileSavePath, pTaskInfo->lpSavePath);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileLength, FormatUINT64(pTaskInfo->DLSize.totalSize));
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_CreateTime, FormatFileTimeToStr(pTaskInfo->DLTime.createTime));
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadSize, FormatUINT64(pTaskInfo->DLSize.currentSize));

    CString strUserTime;
    FLOAT  dwUserTime = FLOAT(pTaskInfo->DLTime.userTime);
    dwUserTime = dwUserTime / FLOAT(1000);
    strUserTime.Format(_T("%4.2f(秒)"), dwUserTime);

    if (dwUserTime / 60 > 1)
    {
        dwUserTime = dwUserTime / FLOAT(60);
        strUserTime.Format(_T("%4.2f(分)"), dwUserTime);
    }
    if (dwUserTime / 60 > 1)
    {
        dwUserTime = dwUserTime / FLOAT(60);
        strUserTime.Format(_T("%4.2f(时)"), dwUserTime);
    }

    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadTimes, strUserTime);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadPercent, strDownloadPercent);

    return S_OK;
}

DWORD CBVDownloadWindow::GetTaskIndex(const sha1_hash &hash)
{
    DWORD dwCount = m_downloadTasks.size();
    for (DWORD dwIndex = 0 ; dwIndex < dwCount ; dwIndex++)
    {
        if (m_downloadTasks[dwIndex].hash == hash)
        {
            return dwIndex;
        }
    }

    return -1;
}

HRESULT CBVDownloadWindow::UpdataTaskItem(DWORD nIndex , const DownLoadTaskInfo *pTaskInfo,BOOL bDownloading/* = FALSE*/)
{
    if (!pTaskInfo)
    {
        return E_FAIL;
    }


    CString strDownloadPercent;
    if (pTaskInfo->DLSize.totalSize)
    {
        FLOAT flPercent = FLOAT(pTaskInfo->DLSize.currentSize) / FLOAT(pTaskInfo->DLSize.totalSize);
        flPercent = flPercent * FLOAT(100);;
        strDownloadPercent.Format(_T("%9.2f%%"), flPercent);
    }
    else
    {
        strDownloadPercent.Format(_T("%d%%"), 0);
    }


    CString strUserTime;
    FLOAT  dwUserTime = FLOAT(pTaskInfo->DLTime.userTime);
    dwUserTime = dwUserTime / FLOAT(1000);
    strUserTime.Format(_T("%4.2f(秒)"), dwUserTime);

    if (dwUserTime / 60 > 1)
    {
        dwUserTime = dwUserTime / FLOAT(60);
        strUserTime.Format(_T("%4.2f(分)"), dwUserTime);
    }
    if (dwUserTime / 60 > 1)
    {
        dwUserTime = dwUserTime / FLOAT(60);
        strUserTime.Format(_T("%4.2f(时)"), dwUserTime);
    }


	CString strNeedTime;

	if (pTaskInfo->DLTime.needTime != -1)
	{
		dwUserTime = FLOAT(pTaskInfo->DLTime.needTime);
		//dwUserTime = dwUserTime / FLOAT(1000);
		strNeedTime.Format(_T("%4.2f(秒)"), dwUserTime);

		if (dwUserTime/ 60 > 1)
		{
			dwUserTime = dwUserTime / FLOAT(60);
			strNeedTime.Format(_T("%4.2f(分)"), dwUserTime);
		}
		if (dwUserTime / 60 > 1)
		{
			//dwUserTime / FLOAT(60);
			strNeedTime.Format(_T("%4.2f(时)"), dwUserTime);
		}
	}
	else
	{
		strNeedTime = _T("未知");
	}
	
	FileCompleteState completeState = (FileCompleteState)pTaskInfo->Fsc;
	if (bDownloading)
	{
		m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadSize, FormatUINT64(pTaskInfo->DLSize.currentSize));
		m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadPercent, strDownloadPercent);
		m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_NeedTime, strNeedTime);

		return TRUE;
	}

	m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadSize, FormatUINT64(pTaskInfo->DLSize.currentSize));
	m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadPercent, strDownloadPercent);
	m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_NeedTime, strNeedTime);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadTimes, strUserTime);
	//m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadRate, _T(""));
	m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileComplete, g_FileCompleteStateStrMap[completeState]);
    
    return TRUE;
}

LRESULT CBVDownloadWindow::OnMenuCommand(WORD , WORD , HWND , BOOL & )
{
    return 0;
}

HRESULT CBVDownloadWindow::CreateDownloadTask(LPCTSTR lpUrl, LPCTSTR lpsavePath, BOOL bAutoStart/*=FALSE*/)
{
    if (lpUrl && lpsavePath && m_pDownloadMgr)
    {
        sha1_hash hash = NullUrlHash;
        HRESULT hr = m_pDownloadMgr->CreateDownloadTask(lpUrl, lpsavePath, hash);
        if (hr != S_OK) return hr;
		 
		DWORD lpText = 1000;
		m_pDownloadMgr->SetTaskOption(hash , 1000,(LPVOID)&lpText , sizeof(lpText));

		lpText = 0;
		DWORD *dwLen = 0;
		m_pDownloadMgr->GetTaskOption(hash,1000,(LPVOID*)&dwLen,lpText);
	
        RefreshTask();
        if (bAutoStart)
        {
            m_pDownloadMgr->Start(hash);
        }
    }

    return S_OK;
}

LRESULT CBVDownloadWindow::OnStartAllTask(WORD , WORD , HWND , BOOL & )
{
    if (m_pDownloadMgr)
    {
        m_pDownloadMgr->Start(AnyUrlHash);
    }
    return 0;
}

LRESULT CBVDownloadWindow::OnStopAllTask(WORD , WORD , HWND , BOOL & )
{
    if (m_pDownloadMgr)
    {
        m_pDownloadMgr->Stop(AnyUrlHash);
    }

    return 0;
}