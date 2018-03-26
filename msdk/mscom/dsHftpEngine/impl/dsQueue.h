/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	4,22th,2014

Module Name:

	dsQueue.h 

Abstract: 针对 对象过程和数据的 数据同步处理、事件驱动


*/
/************************************************************************/

#ifndef __DSQUEUE_H__
#define __DSQUEUE_H__

#include <list>
using namespace std;

#include "dsSyncDataWork.h"


template<class T>
class dsSyncQueue:
	public list<T>,
	public dsExtLock
{
public:

	void Push(T &_data){

		dsLocalRefLock lock(GetLock());
		push_back(_data);
	}

	BOOL Empty(){
		dsLocalRefLock lock(GetLock());
		return empty();
	}

	BOOL Pop(__out T &_data){ //内部已经判断empty

		dsLocalRefLock lock(GetLock());

		if(empty())
			return FALSE;

		_data = front();

		pop_front();

		return TRUE;
	}

	void Clear(){
		dsLocalRefLock lock(GetLock());		
		clear();
	}


	int Size(){
		return size();
	}
};


template<class T>
class dsWorkQueue:
	public list<T>,
	public dsSyncDataWork
{
public:
	dsWorkQueue(BOOL bManualReset = FALSE):dsSyncDataWork(bManualReset)
	{
	}
public:

	void Push(T &_data){

		dsLocalRefLock lock(GetLock());
		push_back(_data);
		SetWork();
	}

	BOOL Empty(){
		dsLocalRefLock lock(GetLock());
		return empty();
	}

	BOOL Pop(__out T &_data){ //内部已经判断empty

		dsLocalRefLock lock(GetLock());

		if(empty())
			return FALSE;

		_data = front();

		pop_front();

		return TRUE;
	}

	void Clear(){
		dsLocalRefLock lock(GetLock());		
		clear();
	}

	int Size(){
		return size();
	}
};

#endif /*__DSQUEUE_H__*/