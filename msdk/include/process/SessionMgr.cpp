#include "StdAfx.h"
#include "SessionMgr.h"
#include <vistafunc/vistafunc.h>

CSessionMgr* CSessionMgr::g_sessionMgr(NULL);

CSessionMgr::CSessionMgr(void)
:m_pNotifyCallback(NULL),m_hExit(NULL)
{
}

CSessionMgr::~CSessionMgr(void)
{
}

BOOL CSessionMgr::Init(IUserSessionNotify* pNotify/* = NULL*/)
{
	m_hExit = CreateEvent(NULL, FALSE, FALSE, NULL);
	RASSERT(m_hExit, FALSE);

	g_sessionMgr = this;
	m_pNotifyCallback = pNotify;

	//DWORD dwCurrentSession = GetCurrentSe
	//SessionLogon()
	return TRUE;
}

void CSessionMgr::Uninit()
{
	m_pNotifyCallback = NULL;

	//等待线程结束

	{
		AUTOLOCK_CS(Thread);
		HANDLE_SET::iterator _it = m_handleSet.begin();
		for(; _it != m_handleSet.end(); _it++)
		{
			HANDLE hThread = *_it;
			if(hThread)
			{
				WaitForSingleObject(hThread, INFINITE);
				CloseHandle(hThread);
				hThread = NULL;
			}
		}

		m_handleSet.clear();
	}

	if(m_hExit)
	{
		CloseHandle(m_hExit);
		m_hExit = NULL;
	}
}

void CSessionMgr::Stop()
{
	g_sessionMgr = NULL;

	if(m_hExit)
	{
		SetEvent(m_hExit);
	}
}

BOOL CSessionMgr::SessionLogon(DWORD dwSessionId, LPVOID lpVoid/* = NULL*/)
{
	if(CVistaTools::IsVista() && 0 == dwSessionId)
		return TRUE;
	//开线程等待对应session的explorer启动
	RASSERT(m_hExit && g_sessionMgr, FALSE);
	
	LPST_THREAD_PARAM pParam = new ST_THREAD_PARAM();
	RASSERT(pParam, FALSE);

	pParam->dwSessionId = dwSessionId;
	pParam->lpParam = lpVoid;

	DWORD dwThreadID = 0;
	HANDLE hTimerThread = CreateThread(NULL, 0L, WaitLogonThread, (LPVOID)pParam, 0L, &dwThreadID );
	if (hTimerThread)
	{
		CloseHandle(hTimerThread);
		hTimerThread = NULL;
	}
	else
	{
		delete pParam;
		pParam = NULL;
	}

	return TRUE;
}

DWORD CSessionMgr::WaitLogonThread(LPVOID lpVoid)
{
	if(g_sessionMgr)
		g_sessionMgr->WaitLogon(lpVoid);

	return 0;
}

void CSessionMgr::WaitLogon(LPVOID lpVoid)
{
	LPST_THREAD_PARAM pParam = (LPST_THREAD_PARAM)lpVoid;
	if(NULL == pParam || NULL == m_hExit)
		return;

	ST_THREAD_PARAM threadParam;
	memcpy(&threadParam, pParam, sizeof(ST_THREAD_PARAM));
	delete pParam;
	pParam = NULL;

	HANDLE hThread = GetCurrentThread();

	{
		AUTOLOCK_CS(Thread);
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hExit, 0))
			return;
		m_handleSet.insert(hThread);
	}

	DWORD dwSessionId = threadParam.dwSessionId;

	HANDLE hShellReady = NULL;
	TCHAR strEventName[100];
	_stprintf_s(strEventName, _T("Session\\%d\\ShellReadyEvent"), dwSessionId);

	while (TRUE)
	{
		hShellReady = OpenEvent(SYNCHRONIZE, FALSE, strEventName);
		if (hShellReady)
			break;
		else
		{
			DWORD dwError = GetLastError();
			if ( dwError == ERROR_FILE_NOT_FOUND)
			{
				Sleep(1000);
			}
			else
			{
				return;
			}
		}
	}

	HANDLE hWaitHandle[] = {m_hExit, hShellReady};

	DWORD dwWait = WaitForMultipleObjects(2, hWaitHandle, FALSE, INFINITE);
	switch(dwWait)
	{
	case WAIT_OBJECT_0:	//退出，不需要等了
		break;
	case WAIT_OBJECT_0 + 1:	//已经起来了
		if(m_pNotifyCallback)
		{
			m_pNotifyCallback->UserSessionLogon(dwSessionId, threadParam.lpParam);
		}
		break;
	}   

	CloseHandle(hShellReady);
	hShellReady = NULL;

	AUTOLOCK_CS(Thread);
	m_handleSet.erase(hThread);
	CloseHandle(hThread);
	hThread = NULL;
}
