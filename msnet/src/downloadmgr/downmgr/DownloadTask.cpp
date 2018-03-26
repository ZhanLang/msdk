#include "StdAfx.h"
#include "DownloadTask.h"
//#include "FtpDownload.h"

CDownloadTask::CDownloadTask()
{	
	m_pNotify		= NULL;
	m_pDownLoad		= NULL;
	m_bRemoveFile	= FALSE;
	m_pDownSetting	= NULL;
	m_dwNotifyIdMask = DefaultNotifyMsgMask;
}

CDownloadTask::~CDownloadTask(void)
{
	UninitDLTask();
	m_dlTaskOptionMap.clear();
}

BOOL CDownloadTask::SetProp(const DownloadTaskProp *prop)
{
	m_prop = prop;

    //为了以防万一
    mspath::CPath::CreateDirectoryEx(prop->Path);
    m_strBackFile = prop->Path;
    m_strBackFile.Append(_T("_back"));

	//这里要做一下初始化
	m_prop.DLTime.needTime = -1;
	m_prop.Speed = 0;

	this->SetTaskOption(DLTO_NotifyMsgMask,&m_dwNotifyIdMask,sizeof(m_dwNotifyIdMask));
	CheckInit();

    return TRUE;
}

const DownloadTaskProp *CDownloadTask::GetProp()
{
    return &m_prop;
}

HRESULT CDownloadTask::CheckInit()
{
	//成功状态就检查了
	RASSERT(m_prop.Fsc != FCS_Success ,S_OK);
	m_prop.DLSize.currentSize = PathFileExists(m_strBackFile)? mspath::CPath::GetFileSize(m_strBackFile):0;

	return S_OK;
}
	
HRESULT CDownloadTask::Start()
{
	RTEST(m_prop.Fsc == FCS_Downloading, S_OK);

	CheckInit();

	//判断下载文件
	if (PathFileExists(m_prop.Path))
	{
		if (mspath::CPath::GetFileSize(m_prop.Path) == m_prop.DLSize.totalSize)
		{
			m_prop.Fsc = FCS_Success;
			m_prop.DLSize.currentSize = m_prop.DLSize.totalSize;
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
			return S_OK;
		}
		else
		{
			DeleteFile(m_prop.Path);
		}
	}

	m_prop.Fsc = FCS_Downloading;
	return CSubThread::StartThread() ? S_OK : E_FAIL;
}



HRESULT CDownloadTask::Stop(BOOL remove_files /*= FALSE*/)
{
    m_bRemoveFile = remove_files;
    if (m_pDownLoad)
    {
        m_pDownLoad->Abort();
    }

	m_prop.DLTime.needTime = -1;
	m_prop.Speed = 0;

	//WaitForSingleObject(m_hExit,-1);
    //需要等待线程退出
    StopThread(TRUE); //这里有点小问题，在同一个线程回调中等待线程退出会出现死锁的现象
    if(remove_files && PathFileExists(m_strBackFile))
    {
        DeleteFile(m_strBackFile);
    }

	if(m_prop.Fsc == FCS_Wait)
	{
		m_prop.Fsc = FCS_Stop;
		m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
    }

    return S_OK;
}

DownLoadTaskInfo *CDownloadTask::QueryTaskInfo()
{
	UINT64 uSize = m_prop.DLSize.totalSize - m_prop.DLSize.currentSize;
	m_prop.DLTime.needTime = uSize ? (m_prop.Speed? uSize / m_prop.Speed:-1) :0;
	return &m_prop;
}

DWORD CDownloadTask::GetSpeed()
{
	DWORD dwSpeed = m_speedMeter.GetSpeed();
	m_prop.Speed = dwSpeed;

	UINT64 uSize = m_prop.DLSize.totalSize - m_prop.DLSize.currentSize;
	m_prop.DLTime.needTime = uSize ? (m_prop.Speed? uSize / m_prop.Speed:-1) :0;

	m_prop.DLTime.userTime += 1000;   //暂时这么处理吧
	m_speedMeter.Reset();
	return dwSpeed;
}

HRESULT CDownloadTask::InitDownloadTask(IDownLoadNotify* pNotift,IDwonloadSetting* pSetting)
{
	RASSERT(pNotift && pSetting ,E_FAIL);
	m_pNotify = pNotift;
	m_pDownSetting = pSetting;
	return S_OK;
}

HRESULT CDownloadTask::OnDownLoadEventNotify(DownloaderEvent dlEvent, WPARAM wp, LPARAM lp)
{
    switch(dlEvent)
    {
		case DLEvent_Connecting:
		{
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_QueryInfo, NULL, NULL);
			break;
		}
		case DLEvent_DownloadStart:
		{
			UINT64 uPos = *((UINT64 *)wp);
			UINT64 uSize = *((UINT64 *)lp);
			m_prop.DLSize.totalSize = uSize;
			m_prop.DLSize.currentSize = uPos;

			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_StartDownload, uPos ? SDS_Continue : SDS_Start, NULL);

			m_prop.Fsc = FCS_Downloading;
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
			break;
		}
		case DLEvent_Downloading:
		{
			DWORD  dwSize = (DWORD)wp;
			m_prop.DLSize.currentSize += dwSize;
			m_speedMeter.Done(dwSize);


			DWORD *lpdwGlobalMask =NULL;
			DWORD dwLen = 0;
			m_pDownSetting->GetOption(DLO_NotifyMsgMask,(LPVOID*)&lpdwGlobalMask,dwLen);
			DWORD dwMaskId = lpdwGlobalMask ? *lpdwGlobalMask : DefaultNotifyMsgMask;
			dwMaskId |= m_dwNotifyIdMask;
			

			if (dwMaskId & Msg_Downloading)
			{
				m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_Downloading , wp,lp);
			}
			break;
		};
		case DLEvent_Done:
		{
			BOOL bSuccess = PathFileExists(m_strBackFile) && MoveFile(m_strBackFile, m_prop.Path);

			m_prop.DLTime.finishTime = GetCurrentFileTime();
			m_prop.Fsc = bSuccess ? FCS_Success : FCS_Fiald;
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
			break;
		}
		case DLEvent_Error:
		{
			m_prop.Fsc = FCS_Fiald;
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
			break;
		}
		case DLEvent_Abort:
		{
			m_prop.Fsc = FCS_Stop;
			m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
			break;
		}
    }
    return S_OK;
}

HRESULT CDownloadTask::InitDLTask()
{
    UninitDLTask();
    fsURL urlCrack;
    RASSERT(urlCrack.Crack(m_prop.Url) == IR_SUCCESS, E_FAIL);

    switch(urlCrack.GetInternetScheme())
    {
    case INTERNET_SCHEME_FTP:

       // m_pDownLoad = new CFtpDownload();
        RASSERT(m_pDownLoad, E_FAIL);
        break;

    case INTERNET_SCHEME_HTTP:
    case INTERNET_SCHEME_HTTPS:

        m_pDownLoad = new CHttpDownload();
        RASSERT(m_pDownLoad, E_FAIL);
        break;

    }

    return S_OK;
}

HRESULT CDownloadTask::UninitDLTask()
{
    if (m_pDownLoad)
    {
        if (m_bRemoveFile)
        {
            m_pDownLoad->DeleteConfigFile();
        }

        m_pDownLoad->Close();
        m_pDownLoad = NULL;
    }
	StopThread(TRUE);
    return S_OK;
}

HRESULT CDownloadTask::Run()
{
    if (InitDLTask() == S_OK && m_pDownLoad && m_pDownLoad->Open(this, m_prop.Url, m_strBackFile) == S_OK)
    {
        m_pDownLoad->Download();
    }
    else
    {
        m_prop.Fsc = FCS_Fiald;
        m_pNotify->OnDownLaodNotify(m_prop.task_hash, Msg_FileComplete, m_prop.Fsc, NULL);
    }

    UninitDLTask();
    return S_OK;
}

HRESULT CDownloadTask::SetFileCompleteState(FileCompleteState sfc)
{
    m_prop.Fsc = sfc;
    return S_OK;
}


HRESULT CDownloadTask::SetTaskOption(DWORD option, LPVOID lpData, DWORD dwLen)
{
	RASSERT(lpData && dwLen , E_INVALIDARG);

	switch(option)
	{
	case DLTO_Cookies:
		_tcscpy_s(m_prop.Cookies,_countof(m_prop.Cookies),static_cast<LPCTSTR>(lpData));
		break;
	case DLTO_Referer:
		_tcscpy_s(m_prop.Referer,_countof(m_prop.Referer),static_cast<LPCTSTR>(lpData));
		break;
	case DLTO_NotifyMsgMask:
		{
			m_dwNotifyIdMask = static_cast<DWORD>(*static_cast<LPDWORD>(lpData));
			break;
		}

	default: //不需要特殊处理的都放到这里吧
		{
			XBuffer xBuf;
			xBuf.SetBuffer(static_cast<unsigned char*>(lpData),dwLen);
			m_dlTaskOptionMap[option] = xBuf;
		}
	}

	return S_OK;
}
HRESULT CDownloadTask::GetTaskOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen)
{
	RASSERT(lpData ,E_INVALIDARG);

	switch(option)
	{
	case DLTO_Cookies:
		*lpData = static_cast<LPVOID>(m_prop.Cookies);
		dwLen = _tcslen(m_prop.Cookies);
		break;

	case DLTO_Referer:
		*lpData = static_cast<LPVOID>(m_prop.Referer);
		dwLen = _tcslen(m_prop.Referer);
		break;
	case DLTO_NotifyMsgMask:
		{
			LPDWORD* lppDw = (LPDWORD*)lpData;
			*lpData= (&m_dwNotifyIdMask);
			dwLen = sizeof(DWORD);
			break;
		}
	default:
		{
			CDLTaskOptionMap::iterator it = m_dlTaskOptionMap.find(option);
			RASSERT(it != m_dlTaskOptionMap.end(), E_NOTIMPL);
			return  it->second.GetRawBuffer((UCHAR**)(lpData),&dwLen) ? S_OK:E_FAIL;
		}
	}
	return S_OK;
}

IDwonloadSetting * CDownloadTask::GetSetting()
{
	return m_pDownSetting;
}