#include <stdafx.h>
#include "downmgr.h"
#include <list>
#include <shlobj.h>
#include "util\utility.h"

#define RefreshMsg WM_USER + 100

CDownloadMgr::CDownloadMgr()
{
    m_bStopAllTask = FALSE;
    m_pDlNotify = NULL;
}

CDownloadMgr::~CDownloadMgr()
{

}

HRESULT CDownloadMgr::InitDownloadMgr(IDownLoadNotify *pNotify, LPCWSTR lpNameSpace/* = NULL*/,BOOL bCache /*=TRUE*/)
{
    RASSERT(pNotify , FALSE);
    m_pDlNotify = pNotify;
	m_bCache = bCache;

	SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,SHGFP_TYPE_CURRENT,m_strDbPath.GetBufferSetLength(MAX_PATH));
    m_strDbPath.ReleaseBuffer();
	m_strDbPath.Append(_T("\\downmgr\\"));
	mspath::CPath::CreateDirectoryEx(m_strDbPath);
    m_strDbPath.Append(lpNameSpace);
    m_strDbPath.Append(_T("_dl.data"));
    m_strNameSpace = lpNameSpace;

	m_SafeList.InitAlloc();
	CSubThread::StartThread();
	InitDownloadTask();

	return TRUE;
}

LPCTSTR CDownloadMgr::GetNameSpace()
{
    return m_strNameSpace;
}

HRESULT CDownloadMgr::UninitDownloadMgr()
{
    Stop(AnyUrlHash);
    SyncConfigFile();

    //释放内存
	RemoveTask(AnyUrlHash);
	
	CSubThread::PostMsg(WM_QUIT,NULL,NULL);
	CSubThread::StopThread(TRUE);

	m_SafeList.Stop();
	m_SafeList.UninitAlloc();
    return TRUE;
}

BOOL CDownloadMgr::InitDownloadTask()
{
	RASSERT(m_bCache , TRUE);
	UTIL::sentry<HANDLE,UTIL::handle_sentry> hFile = CreateFile(m_strDbPath,
		GENERIC_WRITE | GENERIC_READ,
		NULL,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL) ;

	RASSERT(hFile != NULL && hFile != INVALID_HANDLE_VALUE,FALSE);
	
	
    DWORD dwTaskCount = 0;
    DWORD dwRead = 0;

    //刚创建的时候可能获取失败
    ReadFile(hFile, &dwTaskCount, sizeof(dwTaskCount), &dwRead, NULL);

    std::list<DownloadTaskProp> propList;
    for (DWORD dwIndex = 0 ; dwIndex < dwTaskCount ; dwIndex++)
    {
        dwRead = 0 ;
        DownloadTaskProp prop;
        ReadFile(hFile, &prop, sizeof(prop), &dwRead, NULL);

        //如果这里都获取失败了那就有问题了
        if (dwRead == 0)
        {
            break;
        }
        else if (dwRead != sizeof(prop))
        {
            return FALSE;
        }

        prop.lpSavePath = prop.Path;
        prop.lpUrl = prop.Url;

        propList.insert(propList.end(), prop);
    }


    for (std::list<DownloadTaskProp>::iterator it = propList.begin();
            it != propList.end(); it++)
    {
        CreateTask(&(*it));
    }

    return TRUE;
}

HRESULT CDownloadMgr::EnumDownloadTask(IEnumDownLoadTask *pCallBack)
{
    RASSERT(pCallBack, E_FAIL);

    AUTOLOCK_CS(taskmap);
    CTaskList::iterator it = m_downloadTaskList.begin();
    for (; it != m_downloadTaskList.end(); it++)
    {
        pCallBack->OnEnumDownLoadTask(it->hash);
    }

    return S_OK;
}

const DownLoadTaskInfo *CDownloadMgr::QueryTaskInfo(const sha1_hash &hash)
{
    CDownloadTask *pTask = GetDownloadTask(hash);
    RASSERT(pTask, NULL);
    return pTask->QueryTaskInfo();
}

HRESULT CDownloadMgr::CreateDownloadTask(LPCWSTR lpUrl, LPCWSTR lpSavePath, OUT sha1_hash &hash)
{
    RASSERT(lpUrl , E_INVALIDARG);
    RASSERT(lpSavePath, E_INVALIDARG);

	CString strSavePath = lpSavePath;
	strSavePath = strSavePath.Trim();

    RASSERT(_tcslen(lpUrl) && _tcslen(lpSavePath), E_INVALIDARG);
    hash = calc_hash(lpUrl);

    RASSERT(!GetDownloadTask(hash), S_FALSE);
	
    DownloadTaskProp TaskProperty;
    _tcscpy_s(TaskProperty.Url,_countof(TaskProperty.Url), lpUrl);
    _tcscpy_s(TaskProperty.Path,_countof(TaskProperty.Path),  strSavePath);
    TaskProperty.lpUrl = TaskProperty.Url;
    TaskProperty.lpSavePath = TaskProperty.Path;
    TaskProperty.task_hash = hash;

    TaskProperty.DLTime.createTime = GetCurrentFileTime();
    return CreateTask(&TaskProperty);
}

HRESULT CDownloadMgr::CreateTask(DownloadTaskProp *pProp)
{
    RASSERT(pProp , E_INVALIDARG);
    CDownloadTask *pTask = new CDownloadTask();
    RASSERT(pTask, E_FAIL);
	pTask->InitDownloadTask(this,&m_dlSetting);
    if (!pTask->SetProp(pProp))
    {
        SAFE_DELETE(pTask);
        return E_FAIL;
    }

    AddTask(pProp->task_hash, pTask);
    return S_OK;
}

HRESULT CDownloadMgr::Start(const sha1_hash &urlhash)
{
    m_bStopAllTask = FALSE;
    return TaskControl(urlhash, Start_Control) ? S_OK : E_FAIL;
}

HRESULT CDownloadMgr::Stop(const sha1_hash &urlhash, BOOL remove_files/* = FALSE*/)
{
    if (urlhash == AnyUrlHash)
    {
        m_bStopAllTask = TRUE;
    }
	
    return TaskControl(urlhash, Stop_Control, remove_files) ? S_OK : E_FAIL;
}

HRESULT CDownloadMgr::Delete(const sha1_hash &urlhash, BOOL remove_files/*=FALSE*/)
{
	RASSERT(urlhash != NullUrlHash,E_INVALIDARG);

    if (urlhash == AnyUrlHash)
    {
        m_bStopAllTask = TRUE;
    }

	HRESULT hRet = TaskControl(urlhash, Delete_Control, remove_files) ? S_OK : E_FAIL;
	RemoveTask(urlhash);
    return hRet;
}

HRESULT CDownloadMgr::SetOption(DWORD option, LPVOID lpdata, DWORD dwLen)
{
    return m_dlSetting.SetOption(option, lpdata, dwLen);
}

HRESULT CDownloadMgr::GetOption(DWORD option, OUT LPVOID *lpData, OUT DWORD &dwLen)
{
    return m_dlSetting.GetOption(option, lpData, dwLen);
}

HRESULT CDownloadMgr::SetTaskOption(const sha1_hash& hash,DWORD option,LPVOID lpData,DWORD dwLen) 
{
	CDownloadTask* pTask = GetDownloadTask(hash);
	RASSERT(pTask,E_FAIL);

	return pTask->SetTaskOption(option,lpData,dwLen);
}	

HRESULT CDownloadMgr::GetTaskOption(const sha1_hash& hash,DWORD option,OUT LPVOID* lpData,OUT DWORD&dwLen)
{
	CDownloadTask* pTask = GetDownloadTask(hash);
	RASSERT(pTask,E_FAIL);
	
	return pTask->GetTaskOption(option,lpData,dwLen);
}

BOOL CDownloadMgr::TaskControl(const sha1_hash &hash, TaskControlType control , BOOL remove_files/* =FALSE */)
{
    RASSERT(hash != NullUrlHash , E_INVALIDARG);
    if (hash == AnyUrlHash)
    {
		AUTOLOCK_CS(taskmap);
        for (CTaskList::iterator it = m_downloadTaskList.begin(); it != m_downloadTaskList.end(); it++)
        {
            TaskControl(it->hash, control, remove_files);
        }
    }
    else
    {
        CDownloadTask *task = GetDownloadTask(hash);
        RASSERT(task , FALSE);
        switch(control)
        {
        case Start_Control:
            NewTaskStart(hash);
            break;
        
        case Stop_Control:
            task->Stop(remove_files);
            break;
        
        case Delete_Control:
            task->Stop(remove_files);
            break;
        }
    }
    return S_OK;
}


VOID CDownloadMgr::OnDownLaodNotify(const sha1_hash &hash , NotifyMessageID msgid , WPARAM wp, LPARAM lp)
{
	DownLoadMsgInfo msgInfo;
	msgInfo.hash = hash;
	msgInfo.msgid = msgid;
	msgInfo.wp = wp;
	msgInfo.lp = lp;

	m_SafeList.push_back(msgInfo,FALSE);
	CSubThread::PostMsg(RefreshMsg, NULL, NULL);
}


//做一次完整的消息转发
HRESULT CDownloadMgr::Run()
{
	BOOL bRet = FALSE;
	MSG msg = {0};
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (m_SafeList.IsStop()) break;
		if(msg.message != RefreshMsg) continue;
		
		msdk::CSafeList<DownLoadMsgInfo>::SAFELISTIMPL values;
		m_SafeList.GetAllAndDelete(values, 0);
		if(!values.size()) continue;

		msdk::CSafeList<DownLoadMsgInfo>::SAFELISTIMPL::iterator it = values.begin();
		
		for (; it != values.end(); it++)
		{
			const DownLoadMsgInfo &msgInfo = *it;
			sha1_hash hash = msgInfo.hash;
			NotifyMessageID msgid = msgInfo.msgid;
			WPARAM wp = msgInfo.wp;
			LPARAM lp = msgInfo.lp;

			if (msgid == Msg_FileComplete)
			{
				FileCompleteState state = (FileCompleteState)wp;
				if (state == FCS_Fiald   ||
					state == FCS_Success ||
					state == FCS_Stop)
				{
					if (!m_bStopAllTask)
					{
						NewTaskStart();
					}
				}
			}

			if (m_pDlNotify)
			{
				m_pDlNotify->OnDownLaodNotify(hash,msgid,wp,lp);
			}
		}
	}

	return S_OK;
}


VOID CDownloadMgr::GetSpeed()
{
    if (!m_pDlNotify) return ;

    AUTOLOCK_CS(taskmap);

    DWORD dwTotal = 0;
    CTaskMap::iterator it = m_downloadTaskMap.begin();
    for (; it != m_downloadTaskMap.end(); it++)
    {
        CDownloadTask *pTask = it->second;
        if (pTask && pTask->GetProp()->Fsc == FCS_Downloading)
        {
            DWORD taskSpeed = pTask->GetSpeed();
            dwTotal += taskSpeed;
			
			OnDownLaodNotify(it->first, Msg_DownloadSpeed, (WPARAM)taskSpeed, NULL);
        }
    }
	
    OnDownLaodNotify(AnyUrlHash, Msg_DownloadSpeed, (WPARAM)dwTotal, NULL);
}

DWORD CDownloadMgr::GetDownloadingTaskCount()
{
    DWORD dwCount = 0;

    AUTOLOCK_CS(taskmap);
    for (CTaskList::iterator it = m_downloadTaskList.begin(); it != m_downloadTaskList.end(); it++)
    {
        CDownloadTask *pTask = GetDownloadTask(it->hash);
        if (pTask && pTask->GetProp()->Fsc == FCS_Downloading)
        {
            dwCount ++;
        }
    }

    return dwCount;
}
CDownloadTask *CDownloadMgr::GetDownloadTask(const sha1_hash &hash)
{
	RASSERT(hash != NullUrlHash && hash != AnyUrlHash,NULL);

    AUTOLOCK_CS(taskmap);
    CTaskMap::iterator it = m_downloadTaskMap.find(hash);
    RASSERT(it != m_downloadTaskMap.end(), NULL);
    return it->second;
}

HRESULT CDownloadMgr::NewTaskStart(const sha1_hash &hash)
{
	//这里其实已经指定了某个任务是否启动
    if (hash != NullUrlHash && hash != AnyUrlHash)
    {
        CDownloadTask *pTask = GetDownloadTask(hash);

		//正在下载，直接跳过吧。已启动的任务不必再次去检查。
        RASSERT(pTask && pTask->GetProp()->Fsc != FCS_Downloading, E_FAIL);

        if (GetDownloadingTaskCount() >= m_dlSetting.GetMaxTaskNum())
        {
			UINT64 dwSaveSize = mspath::CPath::GetFileSize(pTask->GetProp()->lpSavePath);
			UINT64 dwSvrSize = pTask->GetProp()->DLSize.totalSize;

			//文件存在，并且大小相同,我们认为下载成功
			if (PathFileExists(pTask->GetProp()->lpSavePath) && dwSaveSize == dwSvrSize)
			{
				 OnDownLaodNotify(hash,Msg_FileComplete,FCS_Success,NULL);
				 return S_OK;
			}
			
			//发现该任务可以被调度，但是已经是可下载任务的上限，只能等待。
            pTask->SetFileCompleteState(FCS_Wait);
			pTask->CheckInit();
            OnDownLaodNotify(hash, Msg_FileComplete, FCS_Wait, NULL);
        }
        else
        {
            pTask->Start();
        }
        return S_OK;
    }

    //////////////////////////////////////////////////////////////////////////
    //自动添加任务

    if (GetDownloadingTaskCount() >= m_dlSetting.GetMaxTaskNum() || !m_downloadTaskList.size())
    {
        return S_OK;
    }

	AUTOLOCK_CS(taskmap);

	//因为m_downloadTaskList是按时间排序的，所以找到的第一个任务就可以了。
    CTaskList::iterator it = m_downloadTaskList.begin();
    for (; it != m_downloadTaskList.end() ; it++)
    {
        CDownloadTask *pTask = GetDownloadTask(it->hash);
        if (pTask && pTask->GetProp()->Fsc == FCS_Wait)
        {
            pTask->Start();
            break;
        }
    }

    return S_OK;
}

HRESULT CDownloadMgr::AddTask(const sha1_hash &hash, CDownloadTask *pMgr)
{
    AUTOLOCK_CS(taskmap);
    RASSERT(!GetDownloadTask(hash), S_FALSE);

    m_downloadTaskMap.insert(m_downloadTaskMap.end(), CTaskMap::value_type(hash, pMgr));

    DownloadTaskItemInfo taskInfo;
    taskInfo.hash = hash;
    taskInfo.pTask = pMgr;
    m_downloadTaskList.push_back(taskInfo);
    std::sort(m_downloadTaskList.begin(), m_downloadTaskList.end(), lessTaskItem());
    return S_OK;
}

HRESULT CDownloadMgr::RemoveTask(const sha1_hash &hash)
{
	RASSERT(hash != NullUrlHash,E_INVALIDARG);
    AUTOLOCK_CS(taskmap);

	//全部删除
	if (hash == AnyUrlHash)
	{
		for (CTaskMap::iterator it = m_downloadTaskMap.begin();it != m_downloadTaskMap.end() ; it++)
		{
			CDownloadTask* pTask = it->second;
			if (pTask)
			{
				delete pTask;
			}
		}

		m_downloadTaskMap.clear();
		m_downloadTaskList.clear();
		return S_OK;
	}

	CTaskMap::iterator it = m_downloadTaskMap.find(hash);
	RASSERT(it != m_downloadTaskMap.end(),S_OK);

	if (it != m_downloadTaskMap.end())
	{
		CDownloadTask* pTask = it->second;
		if (pTask)
		{
			delete pTask;
		}
	}

    m_downloadTaskMap.erase(hash);
    for (CTaskList::iterator itList = m_downloadTaskList.begin() ; itList != m_downloadTaskList.end() ;)
    {
        if (itList->hash == hash)
        {
            itList = m_downloadTaskList.erase(itList);
        }
        else
        {
            itList ++;
        }
    }

    return S_OK;
}

HRESULT CDownloadMgr::SyncConfigFile()
{

	RASSERT(m_bCache , TRUE);
    UTIL::sentry<HANDLE,UTIL::handle_sentry> hFile = CreateFile(m_strDbPath,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL,
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);


	RASSERT(hFile != NULL && hFile != INVALID_HANDLE_VALUE,FALSE);

    DWORD dwWrite = 0;
    AUTOLOCK_CS(taskmap);
    DWORD dwTaskCount = m_downloadTaskMap.size();
    BOOL bRet = WriteFile(hFile, &dwTaskCount, sizeof(dwTaskCount), &dwWrite, NULL);
    for (CTaskMap::iterator it = m_downloadTaskMap.begin(); it != m_downloadTaskMap.end(); it++)
    {
        const DownloadTaskProp *taskProp = it->second->GetProp();
        dwWrite = bRet = 0;
        bRet = WriteFile(hFile, taskProp, sizeof(DownloadTaskProp), &dwWrite, NULL);
    }

 
    hFile = NULL;
    return S_OK;
}