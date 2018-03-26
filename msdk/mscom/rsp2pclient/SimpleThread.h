#ifndef __SimpleThread_H__
#define __SimpleThread_H__


#include <deque>
#include <stdio.h>
#include <assert.h>
#include <process.h>
#include "P2PClientTool.h"

class CSimpleThreadCallback
{
public:
	virtual int Task() = 0;
};


#ifdef _DEBUG
	#define Assert assert
#else
	#define Assert
#endif


enum enThreadStatus
{
	State_Thread_No = 0, 
	State_Thread_Free, 
	State_Thread_Run, 
};

typedef struct ThreadInfo_t
{
	void*         host;
	HANDLE        h;
	unsigned int  id;
	volatile long s;   // Î´Æð×÷ÓÃ. 
}ThreadInfo_t;


#define DEFAULT_MAX_THREAD_NUM	10


class CTasksEvent
{
public:
	CTasksEvent()
	{
		CreateTasksEvent();
	}

	~CTasksEvent()
	{
		DestroyTasksEvent();
	}

	int WaitTasksEvent()
	{
		int nRet = WAIT_FAILED;
		
		if(m_hSignal) 
		{
			nRet = ::WaitForSingleObject(m_hSignal, INFINITE);
		}

		if(nRet == WAIT_OBJECT_0) return 0;
		else                      return -1;
	}

	int ReleaseTasksEvent(int nTasks = 1)
	{
		int nRet = 0;

		if(m_hSignal)
		{
			nRet = ::ReleaseSemaphore(m_hSignal, nTasks, 0);
		}

		return nRet != 0 ? 0 : -1;
	}


private:
	int CreateTasksEvent()
	{
		m_hSignal = ::CreateSemaphore(0, 0, 0x7FFFFFFF, 0);
		Assert(m_hSignal);

		return 0;
	}

	int DestroyTasksEvent()
	{
		if(m_hSignal)
		{
			::CloseHandle(m_hSignal);
			m_hSignal = 0;
		}

		return 0;
	}


private:
	HANDLE m_hSignal;
};


class CSimpleThreadPool
{
public:
	CSimpleThreadPool()
		: m_exit(0)
		, m_count(0)
		, m_tasks(0)
		, m_signal()
	{
		InitThreadPool();
	}

	~CSimpleThreadPool()
	{
		SafeStop();
	}

	int Start(CSimpleThreadCallback * call)
	{
		CBaseLockHandler l(m_lock);
		m_calls.push_back(call);

		m_signal.ReleaseTasksEvent();
		AddTasksCount();
		return 0;
	}

	int Stop()
	{
		m_exit = 1;
		return 0;
	}

	int SafeStop()
	{
		if(m_exit == 2) return 0;
		m_exit = 2;

		m_signal.ReleaseTasksEvent(m_count);

		for(int i = 0; i < m_count; i++)
		{
			DestoryThread(i);
		}

		m_count = 0;
		return 0;
	}


private:
	// thread impl. 
	static unsigned int __stdcall ThreadCallback(void* param)
	{
		Assert(param);
		int nRet = static_cast<CSimpleThreadPool*>(param)->ThreadCallbackImpl();

		Sleep(100);		// prevent for thread exited too quickly
		return nRet;
	}

	unsigned int ThreadCallbackImpl()
	{
		int nRet = 0;
		CSimpleThreadCallback * stc = 0;

		while(!m_exit)
		{
			// check two for effective. 
			if(!m_signal.WaitTasksEvent() && 
				!m_calls.empty())
			{
				CBaseLockHandler l(m_lock);

				if(!m_calls.empty())
				{
					void * call = m_calls.front();
					m_calls.pop_front();
					stc = static_cast<CSimpleThreadCallback*>(call);
				}
			}

			if(!stc) 
			{
				Sleep(1000); 
				continue;
			}

			stc->Task();
			stc = 0;

			CBaseLockHandler l(m_lock);
			nRet = SubTasksCount();
		}

		return nRet;
	}

	int CreateThread(ThreadInfo_t & ti)
	{
		/* 
		 * uintptr_t _beginthreadex( 
		 *		void     *security,
		 *		unsigned stack_size,
		 *		unsigned ( *start_address )( void * ),
		 *		void     *arglist,
		 *		unsigned initflag,
		 *		unsigned *thrdaddr 
		 *		);
		 */
		Assert(ti.s == State_Thread_No);

		unsigned int id; 
		HANDLE h = (HANDLE)_beginthreadex(
			NULL, 0, ThreadCallback, this, 0, &id);

		if(h && id)
		{
			ti.h  = h;
			ti.id = id;
			ti.s  = State_Thread_Free;
			return 0;
		}

		return -1;
	}


private:
	// thread. tasks.
	int InitThreadPool()
	{
		memset(&m_pool, 0, sizeof(m_pool));
		m_size  = sizeof(m_pool)/sizeof(m_pool[0]);
		m_count = 0;

		return 0;
	}

	int CreateThread(int idx)
	{
		Assert(!m_pool[idx].h && 
			m_pool[idx].s == State_Thread_No);

		int nRet = CreateThread(m_pool[idx]);
		Assert(!nRet);

		return 0;
	}

	int DestoryThread(int idx)
	{
		if(m_pool[idx].h)
		{
			::WaitForSingleObject(m_pool[idx].h, INFINITE);
			::CloseHandle(m_pool[idx].h);

			m_pool[idx].h = 0;
			m_pool[idx].s = State_Thread_No;
		}

		return 0;
	}

	int AddTasksCount()
	{
		m_tasks++;

		// add threads. omit failed. 
		if(m_count < m_size && 
			m_count < m_tasks)
		{
			// maybe need to destroy
			// not permit it conflict with 'safestop'. notice. 
			if(m_count == 0)
			{
				for(int i = 0; i < m_size; i++)
				{
					DestoryThread(i);
				}
			}

			m_count++;
			CreateThread(m_count);
			m_exit = 0;
		}

		return 0;
	}

	int SubTasksCount()
	{
		m_tasks--;

		// sub threads. 
		// maybe conflict with 'safestop'. omit it. 
		if(m_tasks == 0)
		{
			m_exit  = 1;
			m_signal.ReleaseTasksEvent(m_count);
			m_count = 0;
		}

		return 0;
	}


private:
	volatile char          m_exit;
	std::deque<void*>      m_calls;
	CCriticalSetionObject  m_lock;

	ThreadInfo_t           m_pool[DEFAULT_MAX_THREAD_NUM];
	int                    m_count;
	int                    m_size;
	int                    m_tasks;
	CTasksEvent            m_signal;
};


class CSimpleThread
{
public:
	CSimpleThread(CSimpleThreadCallback * c)
		: m_call(c)
	{}


	~CSimpleThread()
	{}


	static CSimpleThreadPool* Pool()
	{
		return &m_pool;
	}

	int Start()
	{
		Assert(m_call);
		return m_pool.Start(m_call);
	}

	int Stop()
	{
		return 0;
	}

	bool Running()
	{
		return false;
	}


private:
	CSimpleThreadCallback *  m_call;

	static CSimpleThreadPool m_pool;
};


#endif
