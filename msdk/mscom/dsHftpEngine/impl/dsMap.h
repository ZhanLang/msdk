/************************************************************************/
/* 
Author:

	lourking. All rights reserved.

Create Time:

	4,8th,2014

Module Name:

	dsMap.h 

Abstract:


*/
/************************************************************************/


#ifndef __DSMAP_H__
#define __DSMAP_H__

#include <map>
using namespace std;



#include "dsSyncDataWork.h"


template<class _Kty,
class _Ty,
class _Pr = less<_Kty>,
class _Alloc = allocator<pair<const _Kty, _Ty> >>

class dsSyncMap:
	public map<_Kty,_Ty,_Pr,_Alloc>,
	public dsExtLock
{
public:

	BOOL Insert(_Kty _key, _Ty _Val){
		dsLocalRefLock lock(GetLock());
		return insert(make_pair(_key, _Val)).second;
	}

	//need add lock
	BOOL Find(_Kty _key,__out _Ty &_Val){
		dsLocalRefLock lock(GetLock());
		
		iterator it = find(_key);

		if(it != end()){
			_Val = it->second;
			return TRUE;
		}

		return FALSE;
	}

	BOOL Delete(_Kty _key){
		dsLocalRefLock lock(GetLock());

		iterator it = find(_key);

		if(it != end()){
			erase(it);
			return TRUE;
		}
		return FALSE;
	}

	BOOL Empty(){
		dsLocalRefLock lock(GetLock());
		return empty();
	}

	void Clear(){
		dsLocalRefLock lock(GetLock());		
		clear();
	}

	int Size(){
		dsLocalRefLock lock(GetLock());
		return size();
	}

};


template<class _Kty,
class _Ty,
class _Pr = less<_Kty>,
class _Alloc = allocator<pair<const _Kty, _Ty> >>
class dsWorkMap:
	public map<_Kty,_Ty,_Pr,_Alloc>,
	public dsSyncDataWork
{
public:
	dsWorkMap(BOOL bManualReset = FALSE):dsSyncDataWork(bManualReset)
	{
	}
public:

	BOOL Insert(_Kty _key, _Ty _Val){

		dsLocalRefLock lock(GetLock());
		BOOL bRet = insert(make_pair(_key, _Val)).second;
		SetWork();
		return bRet;
	}

	//need add lock
	BOOL Find(_Kty _key,__out _Ty &_Val){
		dsLocalRefLock lock(GetLock());

		iterator it = find(_key);

		if(it != end()){
			_Val = it->second;
			return TRUE;
		}

		return FALSE;
	}

	BOOL Delete(_Kty _key){
		dsLocalRefLock lock(GetLock());

		iterator it = find(_key);

		if(it != end()){
			erase(it);
			return TRUE;
		}
		return FALSE;
	}

	BOOL Empty(){
		dsLocalRefLock lock(GetLock());
		return empty();
	}

	void Clear(){
		dsLocalRefLock lock(GetLock());		
		clear();
	}

	int Size(){
		dsLocalRefLock lock(GetLock());
		return size();
	}
};



#endif /*__DSMAP_H__*/