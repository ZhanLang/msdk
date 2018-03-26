/************************************************************************/
/* 
Author:

lourking .All rights reserved.

Create Time:

	3,24th,2014

Module Name:

	dsMultiUrlMission.h 

Abstract: 内部网络任务


*/
/************************************************************************/


#ifndef __DSINNERHFTPMISSION_H__
#define __DSINNERHFTPMISSION_H__

#include <set>
using namespace std;

#define IHM_ALLINFO			0xffff
#define IHM_LOCALFILENAME	0x0001
#define IHM_SIZE			0x0002
#define IHM_URL				0x0004
#define IHM_URLSERVER		0x0008
#define IHM_SAVEPATH		0x0010


#define INVALID_MISSIONID	-1
#define MISSION_EXIST -2

typedef set<CString> SET_URL;

class dsMultiUrlMission:public DSHFTPMISSON
{
public:
	SET_URL m_setUrl;
	int m_nStatus;

public:
	dsMultiUrlMission(const DSHFTPMISSON &misson){
		memcpy_s(this, sizeof DSHFTPMISSON, &misson, sizeof DSHFTPMISSON);
		AddUrl(misson.szUrl);
	}

	dsMultiUrlMission(const PDSHFTPMISSON pmission){
		memcpy_s(this, sizeof DSHFTPMISSON, pmission, sizeof DSHFTPMISSON);
		AddUrl(pmission->szUrl);
	}

	dsMultiUrlMission(){
		memset(this, 0, sizeof DSHFTPMISSON);
	}

	BOOL AddUrl(LPCWSTR lpszUrl){
		//?//if is url 
		if(!::PathIsURL(lpszUrl))
			return FALSE;
		return (m_setUrl.insert(lpszUrl)).second;
	}

	BOOL DeleteUrl(LPCWSTR lpszUrl){
		SET_URL::iterator it = m_setUrl.find(lpszUrl);

		if(it == m_setUrl.end())
			return FALSE;

		m_setUrl.erase(it);
		return TRUE;
	}

	CString GetMark(){
		if(0 != szMissionMark[0]){
			return szMissionMark;
		}
		else if(0 != szUrl[0]){
			return szUrl;
		}
		else
			return L"";
	}

	BOOL CheckOK(int nFlag = IHM_ALLINFO){
		
		

		return TRUE;
	}

	static BOOL CheckOK(PDSHFTPMISSON pmission, int nFlag = IHM_ALLINFO){
		if(NULL == pmission)
			return FALSE;

		return TRUE;
	}

};


#endif /*__DSINNERHFTPMISSION_H__*/