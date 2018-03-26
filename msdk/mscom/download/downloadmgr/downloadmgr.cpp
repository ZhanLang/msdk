// downloadmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DownloadMgr.h"
#include "Crc32.h"
#include <shlobj.h>
#include "IDownloadTaskEx.h"
#include <vector>

#define TABLE_NAME _T("_download_table_1")

#define NOTIFY_THREAD_MESSAGE WM_USER + 10001
CDownloadMgr::CDownloadMgr()
{

}

CDownloadMgr::~CDownloadMgr()
{

}


STDMETHODIMP CDownloadMgr::OpenDownloadMgr( IDownloadNotify* pNotify, LPCWSTR lpszNameSpace )
{
	RASSERT(pNotify, E_FAIL);
	
	m_dlNotify = pNotify;
	m_strNameSpace = lpszNameSpace ? lpszNameSpace : L"";

	RASSERT(CSubThread::StartThread(), E_FAIL);
	

	return S_OK;
}

STDMETHODIMP CDownloadMgr::CloseDownloadMgr()
{
	{
		AUTOLOCK_CS(m_taskMap);
		for (CTaskMap::iterator it = m_taskMap.begin() ; it != m_taskMap.end() ; it++)
		{
			UTIL::com_ptr<IDownloadTask> pTask = it->second;
			if (pTask)
			{
				pTask->DestroyTask();
				pTask = INULL;
			}
		}

		m_taskMap.clear();
	}
	

	CSubThread::StopThread(TRUE);
	return S_OK;
}

STDMETHODIMP_(DWORD) CDownloadMgr::CreateDownloadTask( LPCWSTR lpszUrl, LPCWSTR lpszSavePath , BOOL bForce/* = FALSE*/, DWORD dwEng /*= DL_EngineHttp*/)
{
	RASSERT(lpszUrl && lpszSavePath && wcslen(lpszUrl) && wcslen(lpszSavePath), -1);

	DWORD dwTaskId  = GetUrlHash(lpszUrl, bForce);
	HRESULT hr = CreateTask(lpszUrl, lpszSavePath, dwTaskId, dwEng);

	if (SUCCEEDED(hr))
	{
		return dwTaskId;
	}
	
	return -1;
}

STDMETHODIMP_(DWORD) CDownloadMgr::GetUrlHash( LPCWSTR lpszUrl , BOOL bRandom /*= FALSE*/)
{
	RASSERT(lpszUrl && wcslen(lpszUrl), 0);

	CStringW strUrl = lpszUrl;
	if (bRandom)
	{
		srand((int)GetTickCount());
		strUrl.AppendFormat(L"%d/%d/%d", rand(),rand(),rand());
	}

	strUrl.MakeLower();
	return Calc_CRC32((const UCHAR*)strUrl.GetBuffer(), strUrl.GetLength() * sizeof(WCHAR));
}

STDMETHODIMP_(DWORD) CDownloadMgr::QueryTask( IMSBase** pTask )
{
	UTIL::com_ptr<IProperty2> pProp;
	DllQuickCreateInstance(CLSID_CProperty2, __uuidof(IProperty2), pProp.m_p, NULL);
	RASSERT(pProp, 0);
	CPropSet propSet(pProp);

	AUTOLOCK_CS(m_taskMap);
	DWORD nLoop = 0;
	for (CTaskMap::iterator it = m_taskMap.begin() ; it != m_taskMap.end() ; it++, nLoop++)
	{
		propSet[nLoop] = it->first;
	}
	pProp->QueryInterface(__uuidof(IMSBase), (void**)pTask);
	return m_taskMap.size();
}

STDMETHODIMP_(IDownloadTask*) CDownloadMgr::GetDownloadTask( DWORD dwTaskId )
{
	AUTOLOCK_CS(m_taskMap);
	CTaskMap::iterator it = m_taskMap.find(dwTaskId);
	RASSERT(it != m_taskMap.end() , 0);

	return (IDownloadTask*)it->second.m_p;
}

STDMETHODIMP CDownloadMgr::StartTask( DWORD dwTaskId )
{
	if (dwTaskId == -1) //全部启动
	{
		AUTOLOCK_CS(m_taskMap);
		for (CTaskMap::iterator it = m_taskMap.begin() ; it != m_taskMap.end() ; it++)
		{
			UTIL::com_ptr<IDownloadTask> pTask = it->second;
			RASSERT2(pTask, continue);
			pTask->Start();
		}
	}
	else
	{
		UTIL::com_ptr<IDownloadTask> pTask = GetDownloadTask(dwTaskId);
		RASSERT(pTask, S_FALSE);
		return pTask->Start();
	}

	return S_OK;
}

STDMETHODIMP CDownloadMgr::StopTask( DWORD dwTaskId )
{
	if (dwTaskId == -1) //全部启动
	{
		AUTOLOCK_CS(m_taskMap);
		for (CTaskMap::iterator it = m_taskMap.begin() ; it != m_taskMap.end() ; it++)
		{
			UTIL::com_ptr<IDownloadTask> pTask = it->second;
			RASSERT2(pTask, continue);
			pTask->Stop();
		}
	}
	else
	{
		UTIL::com_ptr<IDownloadTask> pTask = GetDownloadTask(dwTaskId);
		RASSERT(pTask, S_FALSE);
		return pTask->Stop();
	}

	return S_OK;
}

STDMETHODIMP CDownloadMgr::RemoveTask( DWORD dwTaskId )
{
	if (dwTaskId == -1)
	{
		AUTOLOCK_CS(m_taskMap);
		for (CTaskMap::iterator it = m_taskMap.begin() ; it != m_taskMap.end() ; it++)
		{
			UTIL::com_ptr<IDownloadTask> pTask = it->second;
			UTIL::com_ptr<IDownloadTaskEx> pTaskEx = pTask;
			RASSERT2(pTask, continue);
			pTask->DestroyTask();
		}

		m_taskMap.clear();
		return S_OK;
	}

	UTIL::com_ptr<IDownloadTaskEx> pTaskEx = GetDownloadTask(dwTaskId);
	UTIL::com_ptr<IDownloadTask>   pTask = pTaskEx;
	RASSERT(pTaskEx, E_INVALIDARG);

	{
		AUTOLOCK_CS(m_taskMap);
		m_taskMap.erase(dwTaskId);
	}

	pTask->DestroyTask();

	return S_OK;
}

STDMETHODIMP_(DL_TaskType) CDownloadMgr::GetDownloadType( LPCWSTR lspzUrl )
{
	RASSERT(lspzUrl && wcslen(lspzUrl), DL_TaskType_UnKnown);
	CStringW strUrl = lspzUrl;
	strUrl.MakeLower();

	if (strUrl.Find(_T("http://")) == 0)
	{
		return DL_TaskType_Http;
	}

	if (strUrl.Find(_T("https://")) == 0)
	{
		return DL_TaskType_Https;
	}

	if (strUrl.Find(_T("ftp://")) == 0)
	{
		return DL_TaskType_Ftp;
	}

	return DL_TaskType_UnKnown;
}

STDMETHODIMP CDownloadMgr::OnDownloadNotify( DL_NotifyEvnet notifyType, DWORD dwTaskID )
{
	return PostMsg(NOTIFY_THREAD_MESSAGE, notifyType, dwTaskID);	
}





HRESULT CDownloadMgr::CreateTask( LPCWSTR lpszUrl, LPCWSTR lpszPath, DWORD dwTaskId, DWORD dwDlEng)
{
	{
		AUTOLOCK_CS(m_taskMap);
		CTaskMap::iterator it = m_taskMap.find(dwTaskId);
		RTEST(it != m_taskMap.end(), S_FALSE );
	}

	UTIL::com_ptr<IDownloadTask> pTask;
	switch(GetDownloadType(lpszUrl))
	{
	case DL_TaskType_Http:
	case DL_TaskType_Https:
		{
			switch(dwDlEng)
			{
			case DL_EngineHttp:
				DllQuickCreateInstance(CLSID_HttpDownload, __uuidof(IDownloadTask), pTask, m_pRot);
				break;
			case DL_EngineXL:
				DllQuickCreateInstance(CLSID_XlDownload, __uuidof(IDownloadTask), pTask, m_pRot);
			}
			
			break;
		}
	}

	RASSERT(pTask, -1);
	HRESULT hr = pTask->CreateTask(lpszUrl, lpszPath, this,dwTaskId);
	if (FAILED(hr))
	{
		pTask->DestroyTask();
		return E_FAIL;
	}

	{
		AUTOLOCK_CS(m_taskMap);
		m_taskMap[dwTaskId] = pTask;
	}

	return S_OK;
}


// LPTSTR strTotalSize = propSet["total_size"];
// LPTSTR strCurSize = propSet["cur_size"];

HRESULT CDownloadMgr::Run()
{
	BOOL bExit = FALSE;
	HANDLE hWaitS[] = {m_hExit};

	while(TRUE)
	{ 
		MSG msg = {0};
		DWORD dwWait = MsgWaitForMultipleObjects(
			_countof(hWaitS), hWaitS, FALSE,
			-1, QS_ALLPOSTMESSAGE);

		if(dwWait == WAIT_OBJECT_0 + _countof(hWaitS))
		{
			//消息来了
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == NOTIFY_THREAD_MESSAGE)
				{
					DL_NotifyEvnet notifyType = (DL_NotifyEvnet)msg.wParam;
					DWORD		   dwTaskID	=(DWORD)msg.lParam;

					if (m_dlNotify)
					{
						m_dlNotify->OnDownloadNotify(notifyType, dwTaskID);
					}

					switch(notifyType)
					{
					case DL_NotifyEvnet_Stop:
					case DL_NotifyEvnet_Complete:
					case DL_NotifyEvent_Error:
					case DL_NotifyEvnet_DownloadStart:
						{
							break;
						}
					}
				}
				

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//信号来了
			switch(dwWait)
			{
			case WAIT_OBJECT_0:	//要退出的信息
				bExit = TRUE;
				break;
			}
		}

		if(bExit)
			break;
	}

	return S_OK;
}

STDMETHODIMP_(DWORD) CDownloadMgr::GetSpeed()
{
	DWORD dwSpeed = m_speedMeter.GetSpeed();
	m_speedMeter.Reset();

	return dwSpeed;
}

STDMETHODIMP_(BOOL) CDownloadMgr::IsTaskExist(DWORD dwTaskID)
{
	AUTOLOCK_CS(m_taskMap);
	return m_taskMap.find(dwTaskID) != m_taskMap.end();
}


