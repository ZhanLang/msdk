#include "StdAfx.h"
#include ".\tasksvrwrap.h"
#include "GTasks.h"

CTaskSvrWrap::CTaskSvrWrap(void)
{
}

CTaskSvrWrap::~CTaskSvrWrap(void)
{
}

HRESULT CTaskSvrWrap::Init(LPVOID lpVoid)
{
	m_bExitting = FALSE;
    m_hEvent = NULL;
	m_RsTaskSrv = (IUnknown*)NULL;
	if( ! InitEvent())
        return E_FAIL;

	return S_OK;
}

//反初始化接口
HRESULT CTaskSvrWrap::Uninit()
{
	if(m_RsTaskSrv)
	{
		m_RsTaskSrv =  (IUnknown*)NULL;
	}
	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	return S_OK;
}


HRESULT CTaskSvrWrap::Start()
{
	if (!StartThread())
		return E_FAIL;

	CGTasks* pTaskSrv = new CGTasks;
	if( pTaskSrv  == NULL)
		return -1;

	RFAILED(pTaskSrv->QueryInterface(__uuidof(IRSTaskSrv), (void**)&m_RsTaskSrv));
	RASSERTP(m_RsTaskSrv,-1);
//	m_RsTaskSrv->OnMessage(TASK_MSG_SETUP,(LPVOID)m_bSetupStart,0);
	return  m_RsTaskSrv->Run();

}

HRESULT CTaskSvrWrap::Stop()
{
	StopThread();
	if(m_RsTaskSrv)
	{
		m_RsTaskSrv->Stop();
	}
	return S_OK;
}

BOOL CTaskSvrWrap::InitEvent()
{
	if( NULL == (m_hEvent = CreateCfgEvent()))
        return FALSE;
	return TRUE;
}

VOID CTaskSvrWrap::SVC()
{
	HANDLE hWaitHandles[2];
	hWaitHandles[0] = m_hExitEvent;
	hWaitHandles[1] = m_hEvent;
    
	//接收配置改变的消息
	while (TRUE)
	{
		DWORD dwResult = WaitForMultipleObjectsEx(2, hWaitHandles, FALSE, INFINITE, TRUE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			m_bExitting = TRUE;
			break;
		case WAIT_OBJECT_0 + 1:
			{
				if(m_RsTaskSrv)
					m_RsTaskSrv->CfgModify();
				break;
			}
		default:
			_ASSERT(FALSE);
		}
		if (m_bExitting)
		{
			break;
		}
	}
	SleepEx(0, TRUE);
}