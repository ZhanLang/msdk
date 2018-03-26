// bvdownload.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "bvdownload.h"
#include <algorithm>
#include <map>
#include "NewTaskDialog.h"
#include <shlobj.h>
#include <commctrl.h>
#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"
enum enumListColumn
{
    LC_Stat_Index = 0,
    LC_Stat_URL,				   // URL
    LC_Stat_TaskType,			   // 开始状态
    LC_Stat_TaskError,          // 完成状态
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
    {LC_Stat_TaskType,     L"任务状态", 60},
    {LC_Stat_CreateTime,	L"创建时间", 115},
    {LC_Stat_TaskError,		L"错误信息", 60},
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
std::map<DL_Error, CString>	g_ErrorMap;
static struct _G_INIT
{
    _G_INIT()
    {
        g_FileCompleteStateStrMap[DL_TaskState_UnKnown]       = _T("未知");
        g_FileCompleteStateStrMap[DL_TaskState_Connecting]     = _T("正在连接");
        g_FileCompleteStateStrMap[DL_TaskState_Downloading]        = _T("正在下载");
        g_FileCompleteStateStrMap[DL_TaskState_Stoped]        = _T("停止");
        g_FileCompleteStateStrMap[DL_TaskState_Error] =			_T("下载错误");
        g_FileCompleteStateStrMap[DL_TaskState_Done]      = _T("下载完成");



		//初始化错误信息
		g_ErrorMap[DLE_SUCCESS] = _T("SUCCESS");				
		g_ErrorMap[DLE_S_FALSE] = _T("");			
		g_ErrorMap[DLE_ALREADYCONNECTED] = _T("S_FALSE");
		g_ErrorMap[DLE_WININETUNKERROR] = _T("WININETUNKERROR");
		g_ErrorMap[DLE_NOTINITIALIZED] = _T("NOTINITIALIZED");
		g_ErrorMap[DLE_LOGINFAILURE] = _T("LOGINFAILURE");
		g_ErrorMap[DLE_FILENOTOPENED] = _T("FILENOTOPENED");
		g_ErrorMap[DLE_BADURL] = _T("BADURL");		
		g_ErrorMap[DLE_INVALIDPARAM] = _T("INVALIDPARAM");
		g_ErrorMap[DLE_ERROR] = _T("ERROR");
		g_ErrorMap[DLE_TIMEOUT] = _T("TIMEOUT");	
		g_ErrorMap[DLE_CANTCONNECT] = _T("CANTCONNECT");	
		g_ErrorMap[DLE_FILENOTFOUND] = _T("FILENOTFOUND");
		g_ErrorMap[DLE_LOSTCONNECTION] = _T("LOSTCONNECTION");
		g_ErrorMap[DLE_NAMENOTRESOLVED] = _T("NAMENOTRESOLVED");
		g_ErrorMap[DLE_RANGESNOTAVAIL] = _T("RANGESNOTAVAIL");
		g_ErrorMap[DLE_PROXYAUTHREQ] = _T("PROXYAUTHREQ");
		g_ErrorMap[DLE_WINERROR] = _T("WINERROR");
		g_ErrorMap[DLE_NEEDREDIRECT] = _T("NEEDREDIRECT");
		g_ErrorMap[DLE_EXTERROR] = _T("EXTERROR");
		g_ErrorMap[DLE_SERVERBADREQUEST] = _T("SERVERBADREQUEST");
		g_ErrorMap[DLE_SERVERUNKERROR] = _T("SERVERUNKERROR");
		g_ErrorMap[DLE_CONNECTIONABORTED] = _T("CONNECTIONABORTED");
		g_ErrorMap[DLE_OUTOFMEMORY] = _T("OUTOFMEMORY");
		g_ErrorMap[DLE_S_REDIRECTED] = _T("S_REDIRECTED");
		g_ErrorMap[DLE_INVALIDPASSWORD] = _T("INVALIDPASSWORD");
		g_ErrorMap[DLE_INVALIDUSERNAME] = _T("INVALIDUSERNAME");
		g_ErrorMap[DLE_NODIRECTACCESS] = _T("NODIRECTACCESS");
		g_ErrorMap[DLE_NOINTERNETCONNECTION] = _T("NOINTERNETCONNECTION");
		g_ErrorMap[DLE_HTTPVERNOTSUP] = _T("HTTPVERNOTSUP");
		g_ErrorMap[DLE_BADFILESIZE] = _T("BADFILESIZE");
		g_ErrorMap[DLE_DOUBTFUL_RANGESRESPONSE] = _T("DOUBTFUL_RANGESRESPONSE");
		g_ErrorMap[DLE_E_NOTIMPL] = _T("E_NOTIMPL");
		g_ErrorMap[DLE_E_WININET_UNSUPP_RESOURCE] = _T("WININET_UNSUPP_RESOURCE");
    }
} G_INIT;




CBVDownloadWindow::CBVDownloadWindow()
{
    m_currentSelect = -1;
    //m_pDownloadMgr = NULL;
}
CBVDownloadWindow::~CBVDownloadWindow()
{
	m_downloadTasks.clear();
	//m_dlHelper.UnloadMSCom();
    //m_dlHelper.UnInit();
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

    m_NextClipWnd = SetClipboardViewer();
    RefreshTask();

	SetTimer(1000, 1000);
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
		m_pDownloadMgr->CloseDownloadMgr();
		m_pDownloadMgr = INULL;
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
	/*
    m_pDownloadMgr->Stop(AnyUrlHash);
    //m_dlHelper.DistoryDownLoadMgr(&m_pDownloadMgr);
	*/
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

        DWORD dwTaskId = m_downloadTasks[m_currentSelect];
        UTIL::com_ptr<IDownloadTask> pTask = m_pDownloadMgr->GetDownloadTask(dwTaskId);
        if (pTask->GetTaskState() == DL_TaskState_Downloading)
        {
            InsertMenu(hMenu, -1, MF_BYPOSITION, MENU_Download_Stop, _T("停止"));
        }
        else if(pTask->GetTaskState() == DL_TaskState_Done)
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

    DWORD dwTaskID = -1;

	UTIL::com_ptr<IDownloadTask>	pTask;
	if (m_currentSelect != -1)
	{
		dwTaskID = m_downloadTasks[m_currentSelect];
		pTask = m_pDownloadMgr->GetDownloadTask(dwTaskID);
	}

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
			 m_pDownloadMgr->StartTask(dwTaskID);
		}
       
        break;
    }
    case MENU_Download_Stop:
    {
		
        m_pDownloadMgr->StopTask(dwTaskID);
        break;
    }
    case MENU_Download_Delete:
    {
		if (m_currentSelect != -1)
		{
			m_pDownloadMgr->RemoveTask(dwTaskID);
		}
        
        RefreshTask();
        break;
    }
    case IDC_StartAll_Btn:
    {
        if (m_pDownloadMgr)
        {
            m_pDownloadMgr->StartTask(-1);
        }
        break;
    }
    case IDC_StopAll_Btn:
    {
        if (m_pDownloadMgr)
        {
            m_pDownloadMgr->StopTask(-1);
        }
        break;
    }
	case IDC_DelAll_Btn:
		if(m_pDownloadMgr)
		{
			m_pDownloadMgr->RemoveTask(-1);
			RefreshTask();
		}
		break;
	case MENU_Download_Restart:
		{
			if (m_currentSelect != -1)
			{
				m_pDownloadMgr->StopTask(dwTaskID);
				CString strUrl = pTask->GetUrl();
				CString strPath = pTask->GetSavePath();
				m_pDownloadMgr->RemoveTask(dwTaskID);
				CreateDownloadTask(strUrl, strPath, TRUE);
			}
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
		lvCol.pszText = (LPWSTR)g_ListColumnDef[nIndex].strText;
		lvCol.cx = g_ListColumnDef[nIndex].nWidth;
        m_dlTaskMgrListControl.InsertColumn(g_ListColumnDef[nIndex].listColumn, g_ListColumnDef[nIndex].strText, LVCFMT_LEFT , g_ListColumnDef[nIndex].nWidth);
    }

    return S_OK;
}

LRESULT CBVDownloadWindow::OnDownLoadMsg(UINT msg, WPARAM wp, LPARAM lp, BOOL &bHandled)
{
	RASSERT(m_pDownloadMgr, E_FAIL);


	DWORD dwTaskID = (DWORD)lp;
	DL_NotifyEvnet dlEvent = (DL_NotifyEvnet)wp;
	UTIL::com_ptr<IDownloadTask> pTask = m_pDownloadMgr->GetDownloadTask(dwTaskID);

	DWORD dwIndex = GetTaskIndex(dwTaskID);
	if (dwIndex == -1)
	{
		return 0;
	}
	switch(dlEvent)
	{
	
	case DL_NotifyEvnet_Connecting:		//链接通知
		{
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskError, g_ErrorMap[pTask->GetTaskError()]);
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType, g_FileCompleteStateStrMap[DL_TaskState_Connecting]);
			break;
		}
	case DL_NotifyEvnet_Downloading:		//正在下载通知
		{
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_DownloadSize, FormatUINT64(pTask->GetCurrentFileSize()));
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType, g_FileCompleteStateStrMap[DL_TaskState_Downloading]);

			CString strDownloadPercent;
			FLOAT flPercent = FLOAT(pTask->GetCurrentFileSize()) / FLOAT(pTask->GetTotalFileSize());
			flPercent = flPercent * FLOAT(100);;
			strDownloadPercent.Format(_T("%9.2f%%"), flPercent);
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_DownloadPercent, strDownloadPercent);
			break;
		}
	case DL_NotifyEvnet_DownloadStart:	//开始下载
		{
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType, _T("开始下载"));
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_FileLength, FormatUINT64(pTask->GetTotalFileSize()));
			break;
		}
	case DL_NotifyEvnet_Complete:		//下载完成
		{
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType,  g_FileCompleteStateStrMap[DL_TaskState_Done]);
			break;
		}
	case DL_NotifyEvnet_Stop:
		{
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType,  g_FileCompleteStateStrMap[DL_TaskState_Stoped]);
			break;
		}
	case DL_NotifyEvent_Error:			//下载失败
		{

			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskType, g_FileCompleteStateStrMap[DL_TaskState_Error]);
			m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_TaskError, g_ErrorMap[pTask->GetTaskError()]);
			break;
		}
	}
	
	return 0;
}


HRESULT CBVDownloadWindow::RefreshTask()
{
	RASSERT(m_pDownloadMgr, E_FAIL);

	m_downloadTasks.clear();
	m_dlTaskMgrListControl.DeleteAllItems();
	UTIL::com_ptr<IProperty2> pTaskProp;
	DWORD dwCount = m_pDownloadMgr->QueryTask((IMSBase**)&pTaskProp.m_p);

	CPropSet propSet(pTaskProp);
	for (int dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
	{
		DWORD dwTaskId = propSet[dwLoop];
		InertTaskItem(dwTaskId);
	}
	
    return S_OK;
}


HRESULT CBVDownloadWindow::InertTaskItem(DWORD dwTaskID)
{
    UTIL::com_ptr<IDownloadTask> pTask = m_pDownloadMgr->GetDownloadTask(dwTaskID);
    DWORD dwCount = m_dlTaskMgrListControl.GetItemCount();
    CString strIndex;
    strIndex.Format(_T("%d"), dwCount + 1);
    int nIndex = m_dlTaskMgrListControl.InsertItem(dwCount, strIndex);
    if(nIndex < 0)
    {
        return E_FAIL;
    }


    CString strDownloadPercent;
    if (pTask->GetCurrentFileSize())
    {
        FLOAT flPercent = FLOAT(pTask->GetCurrentFileSize()) / FLOAT(pTask->GetTotalFileSize());
        flPercent = flPercent * FLOAT(100);;
        strDownloadPercent.Format(_T("%9.2f%%"), flPercent);
    }
    else
    {
        strDownloadPercent.Format(_T("%d%%"), 0);
    }

  
	 m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_TaskError, g_ErrorMap[pTask->GetTaskError()]);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_URL, pTask->GetUrl());
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_TaskType, g_FileCompleteStateStrMap[pTask->GetTaskState()]);
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileSavePath, pTask->GetSavePath());
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_FileLength, FormatUINT64(pTask->GetTotalFileSize()));
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_CreateTime, FormatFileTimeToStr(FileTimeToUINT64(pTask->GetCreateTime())));
    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadSize, FormatUINT64(pTask->GetCurrentFileSize()));


    m_dlTaskMgrListControl.SetItemText(nIndex, LC_Stat_DownloadPercent, strDownloadPercent);
	
	m_downloadTasks[nIndex] = dwTaskID;
    return S_OK;
}


LRESULT CBVDownloadWindow::OnMenuCommand(WORD , WORD , HWND , BOOL & )
{
    return 0;
}

HRESULT CBVDownloadWindow::CreateDownloadTask(LPCTSTR lpUrl, LPCTSTR lpsavePath, BOOL bAutoStart/*=FALSE*/)
{
	
    if (lpUrl && lpsavePath && m_pDownloadMgr)
    {
        DWORD dwTaskID = m_pDownloadMgr->CreateDownloadTask(lpUrl, lpsavePath, FALSE, DL_EngineXL);
        if (dwTaskID == -1) return E_FAIL;
		 
	
        RefreshTask();
        if (bAutoStart)
        {
            m_pDownloadMgr->StartTask(dwTaskID);
        }
    }

    return S_OK;
}

LRESULT CBVDownloadWindow::OnStartAllTask(WORD , WORD , HWND , BOOL & )
{
    if (m_pDownloadMgr)
    {
        m_pDownloadMgr->StartTask(-1);
    }
	
    return 0;
}

LRESULT CBVDownloadWindow::OnStopAllTask(WORD , WORD , HWND , BOOL & )
{
    if (m_pDownloadMgr)
    {
        m_pDownloadMgr->StopTask(-1);
    }
	
    return 0;
}

STDMETHODIMP CBVDownloadWindow::NotifyExit( bool* bExit /*= NULL*/ )
{
	return S_OK;
}

STDMETHODIMP CBVDownloadWindow::OnDownloadNotify( DL_NotifyEvnet notifyType, DWORD dwTaskId )
{
	PostMessage(DOWNLOAD_TASK_MESSAGE, notifyType, dwTaskId);
	return S_OK;
}


TCHAR strScript[] = _T(" local url = \"www.baidu.com\";")
					_T("local strurl = http.encode_url(url);");

STDMETHODIMP CBVDownloadWindow::Init( void* )
{

	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	//CBVDownloadWindow *bvWnd = new CBVDownloadWindow();
	Module.Init(NULL , GetModuleInstance());


	m_pRot->CreateInstance(CLSID_DownloadMgr, NULL, re_uuidof(IDownloadMgr), (void**)&m_pDownloadMgr);
	RASSERT(m_pDownloadMgr, E_FAIL);
	RFAILED(m_pDownloadMgr->OpenDownloadMgr(this, _T("bvdownload")));
	//m_pDownloadMgr->CreateDownloadTask(_T("http://www.kyan.com.cn/kyan/build/KYan_1.0.2.4_kyan.com.cn.exe"), _T("C:\\KYan_1.0.2.4_kyan.com.cn.exe"));

//	UTIL::com_ptr<ILuaVM> pLuaVM;
	//m_pRot->CreateInstance(CLSID_LuaVM, NULL, re_uuidof(ILuaVM), (VOID**)&pLuaVM);
	//pLuaVM->OpenVM();
	//pLuaVM->DoFile (L"bvdownload.lua");

	//CLuaHost* pHost = static_cast<CLuaHost*>(pLuaVM->GetContext());

	std::string sUrl;
	try
	{
		//sUrl = pHost->CallLuaFunction<std::string>("http_request", "http://www.kyan.com.cn/home/updata/check?usrid={940CEE0A-9233-28FE-95EE-0C943392FE28}&usrname=magj&prodid={7C0A7FB2-C4F7-461f-A527-CBA0D3367C1A}&prodname=kyan&prodver=1.0.2.1&oper=kyan.com.cn");
	//	bool bRet = pHost->CallLuaFunction<bool>("http_download", "http://down.360safe.com/se/360se_setup.exe", "c:\\360se_setup.exe");
	}
	catch(CLuaException e)
	{
		std::string s =e.what();
	}


	//pLuaVM->ClosetVM();
	//pLuaVM = INULL;
	UTIL::com_ptr<IHttpSyncRequest> pHttpClinet;
	m_pRot->CreateInstance(CLSID_HttpSyncRequest, NULL, re_uuidof(IHttpSyncRequest), (void**)&pHttpClinet);
	if (pHttpClinet)
	{
		UTIL::com_ptr<IMsBuffer>	pBuffer;
		//pHttpClinet->HttpRequest(L"http://www.kyan.com.cn/home/updata/check?usrid={940CEE0A-9233-28FE-95EE-0C943392FE28}&usrname=magj&prodid={7C0A7FB2-C4F7-461f-A527-CBA0D3367C1A}&prodname=kyan&prodver=1.0.2.1&oper=kyan.com.cn", (IMSBase**)&pBuffer);
	}
	return S_OK;
}

STDMETHODIMP CBVDownloadWindow::Uninit()
{

	SAFE_RELEASE(m_pRot);
	if (m_pDownloadMgr)
	{
		m_pDownloadMgr->CloseDownloadMgr();
		m_pDownloadMgr = INULL;
	}
	Module.Term();
	return S_OK;
}

STDMETHODIMP CBVDownloadWindow::Start()
{
	CMessageLoop theLoop;
	Module.AddMessageLoop(&theLoop);

	if (Create(NULL))
	{
		ShowWindow(SW_SHOW);
		theLoop.Run();
	}

	return S_OK;
}

STDMETHODIMP CBVDownloadWindow::Stop()
{
	return S_OK;
}

DWORD CBVDownloadWindow::GetTaskIndex( DWORD dwTaskID )
{
	for (CTaskMap::iterator it = m_downloadTasks.begin() ; it != m_downloadTasks.end() ; it++)
	{
		if (it->second == dwTaskID)
		{
			return it->first;
		}
	}

	return -1;
}

LRESULT CBVDownloadWindow::OnTimer( UINT , WPARAM wParam, LPARAM , BOOL& bHandled )
{
	DWORD wTimerID = wParam;
	if (wTimerID == 1000 && m_pDownloadMgr)
	{
		

		UTIL::com_ptr<IProperty2> pTaskProp;
		DWORD dwCount = m_pDownloadMgr->QueryTask((IMSBase**)&pTaskProp.m_p);

		DWORD dwTotalSpeed = 0;
		CPropSet propSet(pTaskProp);
		for (int dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
		{
			DWORD dwTaskId = propSet[dwLoop];
			DWORD dwIndex = GetTaskIndex(dwTaskId);

			UTIL::com_ptr<IDownloadTask> pTask = m_pDownloadMgr->GetDownloadTask(dwTaskId);

			if (pTask->GetTaskState() == DL_TaskState_Downloading)
			{
				FLOAT dwSpeed = (FLOAT)pTask->GetSpeed();
				dwSpeed = dwSpeed / FLOAT(1024);
				dwTotalSpeed += dwSpeed;
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

				m_dlTaskMgrListControl.SetItemText(dwIndex, LC_Stat_DownloadRate, strSpeed);
			}
		}

		
		CString strGlobalSpeed;
		strGlobalSpeed.Format(_T("全局速度:%d/秒"), dwTotalSpeed);
		SetDlgItemText(IDC_GLOBAL_STATIC, strGlobalSpeed);
		
	}
	return 0;
}
