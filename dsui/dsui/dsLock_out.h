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
};




#endif /*__DSLOCK_H__*/