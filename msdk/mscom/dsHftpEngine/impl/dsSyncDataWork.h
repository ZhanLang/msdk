/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	4,8th,2014

Module Name:

	dsSyncDataWork.h

Abstract: 针对 对象过程和数据的 数据同步处理、事件驱动


*/
/************************************************************************/


#ifndef __DSSYNCDATAWORK_H__
#define __DSSYNCDATAWORK_H__

#include "dsLock.h"
#include "dsEvent.h"


class dsSyncDataWork
{
public:
	dsLock m_lock;
	dsEvent m_event;

public:
	dsSyncDataWork(BOOL bManualReset = FALSE){
		Init(bManualReset);
	}

public:

	inline void LockData(){
		m_lock.Lock();
	}

	inline void UnlockData(){
		m_lock.UnLock();
	}

	inline BOOL SetWork(){
		return m_event.Set();
	}

	inline BOOL ResetWork(){
		return m_event.Reset();
	}

	inline DWORD WaitWork(DWORD dwMilliseconds){
		return WaitForSingleObject(m_event, dwMilliseconds);
	}

	inline dsLock &GetLock(){
		return m_lock;
	}

	operator dsLock&(){
		return m_lock;
	}

private:	

	inline BOOL Init(BOOL bManualReset = FALSE){
		return NULL != m_event.Create(bManualReset);
	}

};

#endif /*__DSSYNCDATAWORK_H__*/