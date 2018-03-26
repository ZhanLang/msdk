#pragma once


class _declspec(novtable)  CSubThread
{
public:
	CSubThread():m_hExit(NULL), m_hThread(NULL), m_dwThreadId(0)
	{
	}

	virtual ~CSubThread()
	{
		WaitForSubThreadExit();
		if(m_hExit)
		{
			CloseHandle(m_hExit);
			m_hExit = NULL;
		}
	}

	BOOL StartThread()
	{
		if(NULL == m_hExit)
			m_hExit	= CreateEvent(NULL, TRUE, FALSE, NULL);

		if (!m_hExit)
			return FALSE;

		if ( m_hThread )
			return FALSE;
		
		

		m_dwThreadId = 0;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, CREATE_SUSPENDED, &m_dwThreadId);
		if (NULL == m_hThread)
		{
			return FALSE;
		}

		ResumeThread(m_hThread);
		ResetEvent(m_hExit);
		return TRUE;
	}

	void WaitForSubThreadExit(DWORD dwMilliseconds = INFINITE)
	{
		//为了防止，在同一个线程上下文调用等待函数导致死锁问题
		if(m_hThread && m_dwThreadId != GetCurrentThreadId())
		{
			DWORD dwWait = WaitForSingleObject(m_hThread, dwMilliseconds);
			if(WAIT_OBJECT_0 != dwWait)
			{
				TerminateThread(m_hThread, -1);
			}

			CloseHandle(m_hThread);
			m_hThread = NULL;
			m_dwThreadId = 0;
		}
	}

	virtual void StopThread(BOOL bWaitStoped = FALSE)
	{
		if(m_hExit)
		{
			SetEvent(m_hExit);	//通知退出
		}

		if(bWaitStoped)
		{
			WaitForSubThreadExit();
		}
	}

	virtual BOOL IsNeedSubThreadExit()
	{
		if (!m_hExit)
		{
			return TRUE;
		}

		return (WAIT_OBJECT_0 == WaitForSingleObject(m_hExit, 0));
	}

	BOOL PostMsg(UINT msgId, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		if ( !m_dwThreadId )
		{
			return FALSE;
		}
		return ::PostThreadMessage(m_dwThreadId, msgId, wParam, lParam);
	}

protected:
	virtual HRESULT	Run() = 0;

	static UINT WINAPI ThreadFunc(LPVOID lp)
	{
		CSubThread* pThis = static_cast<CSubThread*>(lp);
		if ( !pThis)
		{
			return FALSE;
		}
	

		return pThis->ThreadProcImpl();
	}

	DWORD ThreadProcImpl()
	{
		UINT uRet = (UINT)Run();
		if (m_hThread)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		m_dwThreadId = 0;
		return uRet;
	}

protected:
	HANDLE		m_hThread;
	UINT		m_dwThreadId;
	HANDLE		m_hExit;		 //退出事件
};