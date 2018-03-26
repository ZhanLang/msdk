#pragma once
#include "windows.h"

namespace rwbase{;
class CriticalSection
{
public:
	VOID Lock()		  {EnterCriticalSection(&m_sec);}
	VOID UnLock()	  {LeaveCriticalSection(&m_sec);}

	CriticalSection() {InitializeCriticalSection(&m_sec);}
	~CriticalSection(){DeleteCriticalSection(&m_sec);}
private:
	CRITICAL_SECTION m_sec;
};

class CSemaphore
{
public:
	CSemaphore(long initial_count = 0){
		m_sem = CreateSemaphore(NULL,initial_count,0x7fffffff,NULL);
	}
	~CSemaphore(){
		CloseHandle(m_sem);
	}

	bool Wait(long timeout = -1){
		if (timeout < 0)
			timeout = INFINITE;

		return WaitForSingleObject(m_sem, timeout) != WAIT_TIMEOUT;
	}

	VOID Post(int count = 1){
		ReleaseSemaphore(m_sem,count,NULL);
	}

private:
	HANDLE m_sem;
};


class CCondImp
{
public:
	CCondImp(): m_gate(1),
		m_blocked(0),
		m_unblocked(0),
		m_to_unblock(0)
	{

	}
	VOID Signal( bool broadcast )
	{
		m_gate.Wait();
		m_internal.Lock();
		if (m_unblocked != 0)
		{
			m_blocked -= m_unblocked;
			m_unblocked = 0;
		}
		if (m_blocked > 0)
		{
			m_to_unblock = (broadcast) ? m_blocked : 1;
			m_internal.UnLock();
			m_queue.Post();
		}
		else
		{
			m_gate.Post();
			m_internal.UnLock();
		}
	}

	void PreWait()
	{
		m_gate.Wait();
		m_blocked++;
		m_gate.Post();
	}

	bool Wait( long timeout )
	{
		bool rc = m_queue.Wait(timeout);
		PostWait(!rc);
		return rc;
	}

private:
	VOID PostWait(bool timeout)
	{
		m_internal.Lock();
		m_unblocked++;
		if (m_to_unblock != 0)
		{
			bool last = (--m_to_unblock == 0);
			m_internal.UnLock();

			if (timeout)
				m_queue.Wait();

			if (last)
				m_gate.Post();

			else
				m_queue.Post();
		}
		else
		{
			m_internal.UnLock();
		}
	}
private:
	CSemaphore m_gate;
	CSemaphore m_queue;
	CriticalSection m_internal;
	volatile long m_blocked;
	volatile long m_unblocked;
	volatile long m_to_unblock;
};

class CCond
{
public:
	void Wait(CriticalSection& cs)
	{
		WaitInternal(cs, -1);
	}

	bool Wait( CriticalSection& cs,long timeout)
	{
		if(timeout < 0) timeout = -1;
		return WaitInternal(cs, timeout);
	}

	void Signal()
	{
		m_impl.Signal(false);
	}

	void Broadcast()
	{
		m_impl.Signal(true);
	}

private:
	bool WaitInternal(CriticalSection& cs, long timeout )
	{
		m_impl.PreWait();
		cs.UnLock();
		bool rc = m_impl.Wait(timeout);
		cs.Lock();
		return rc;
	}

	CCondImp m_impl;
};

};//namespace rwbase



//¶ÁÐ´Ëø
class CReadWriteLock
{
public:
	CReadWriteLock()
		:m_count(0),
		m_waiting_writers(0)
	{

	}
	
	VOID AcquireReadLock() const
	{
		CReadWriteLock* This = (CReadWriteLock*)this;
		This -> read_lock_internal();
	}

	VOID AcquireWriteLock() const
	{
		CReadWriteLock* This = (CReadWriteLock*)this;
		This -> write_lock_internal();
	}
	
	VOID ReleaseLock() const
	{
		CReadWriteLock* This = (CReadWriteLock*)this;
		This -> unlock_internal();
	}

private:
	void read_lock_internal()
	{
		m_cs.Lock();
		while (m_count < 0 || m_waiting_writers != 0)
			m_ok_to_read.Wait(m_cs);
		m_count++;
		m_cs.UnLock();
	}

	void write_lock_internal()
	{
		m_cs.Lock();
		while (m_count != 0)
		{
			m_waiting_writers++;
			m_ok_to_write.Wait(m_cs);
			m_waiting_writers--;
		}

		m_count = -1;
		m_cs.UnLock();
	}

	void unlock_internal()
	{
		bool ww, wr;

		m_cs.Lock();
		if (m_count < 0)
			m_count = 0;
		else
			--m_count;

		ww = (m_waiting_writers != 0 && m_count == 0);
		wr = (m_waiting_writers == 0);
		m_cs.UnLock();
	
		if (ww)
			 m_ok_to_write.Signal();
		else if (wr)
			 m_ok_to_read.Broadcast();
	}

private:
	volatile int	m_count;
	volatile unsigned int m_waiting_writers;

	rwbase::CriticalSection m_cs;
	rwbase::CCond m_ok_to_read;
	rwbase::CCond m_ok_to_write;
};

class CAutoReadLock
{
public:
	CAutoReadLock(CReadWriteLock& rwLock)
		:m_rwLock(rwLock){Lock();}
	~CAutoReadLock(){Unlock();}

private:
	VOID Lock()	 {m_rwLock.AcquireReadLock();}
	VOID Unlock(){m_rwLock.ReleaseLock();}
private:
	CReadWriteLock& m_rwLock;
};

class CAutoWriteLock
{
public:
	CAutoWriteLock(CReadWriteLock& rwLock)
		:m_rwLock(rwLock){Lock();}

	~CAutoWriteLock(){Unlock();}

private:
	VOID Lock()	 {m_rwLock.AcquireWriteLock();}
	VOID Unlock(){m_rwLock.ReleaseLock();}
private:
	CReadWriteLock& m_rwLock;
};

