/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	3,5th,2014

Module Name:

	dsLock.h 

Abstract:


*/
/************************************************************************/


#ifndef __DSLOCK_H__
#define __DSLOCK_H__

class dsLock
{
public:
	CRITICAL_SECTION m_cs;
public:
	dsLock(void)
	{
		InitializeCriticalSection(&m_cs);
	}

	~dsLock(void)
	{
		::DeleteCriticalSection(&m_cs);
	}

	void Lock()		
	{
		::EnterCriticalSection(&m_cs);
	}

	void UnLock()	
	{
		::LeaveCriticalSection(&m_cs);
	}

	void Delete()
	{
		::DeleteCriticalSection(&m_cs);
	}
};

class dsExtLock
{
public:
	dsLock m_lock;

public:
	inline void Lock(){
		m_lock.Lock();
	}

	inline void Unlock(){
		m_lock.UnLock();
	}

	inline dsLock &GetLock(){
		return m_lock;
	}
};

class dsLocalRefLock
{
public:
	dsLock &m_lockRef;

public:
	dsLocalRefLock(dsLock &lock):m_lockRef(lock)
	{
		m_lockRef.Lock();
	}

	~dsLocalRefLock(){
		m_lockRef.UnLock();
	}

	
};


#endif /*__DSLOCK_H__*/