#include "StdAfx.h"
#include "XThread.h"
#include <process.h>

/////////////////////////////////////////////////////////////////////////////
XThread::XThread()
	:m_hThread(NULL),
	 m_hExitEvent(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////
XThread::~XThread()
{
	StopThread();
}

/////////////////////////////////////////////////////////////////////////////
BOOL XThread::StartThread()
{
	ASSERT(NULL == m_hThread);

	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hExitEvent)
	{
		return FALSE;
	}

	UINT dwThreadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, &dwThreadID);
	if (NULL == m_hThread)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		return FALSE;
	}

	ResumeThread(m_hThread);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID XThread::StopThread()
{
	if (NULL != m_hThread)
	{
		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (NULL != m_hExitEvent)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID XThread::ThreadProcImpl()
{
	SVC();
}

/////////////////////////////////////////////////////////////////////////////
UINT XThread::ThreadProc(LPVOID lpParameter)
{
	ASSERT(NULL != lpParameter);
	XThread* pThis = (XThread*)lpParameter;

	pThis->ThreadProcImpl();
	return 0;
}
