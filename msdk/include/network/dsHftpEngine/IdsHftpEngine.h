/************************************************************************/
/* 
Author:

lourking. .All rights reserved.

Create Time:

	4,1th,2014

Module Name:

	IdsHftpEngine.h 

Abstract: 下载引擎接口


*/
/************************************************************************/

#ifndef __IDSHFTPENGINE_H__
#define __IDSHFTPENGINE_H__


#ifdef DSHTTPFTPENGINE_EXPORTS
#define DSHTTPFTPENGINE_API /*__declspec(dllexport)*/
#else
#define DSHTTPFTPENGINE_API /*__declspec(dllimport)*/
#endif

#include "dsHftpDef.h"

#define DS_PURE = 0

interface DSHTTPFTPENGINE_API IdsHftpEngine
{
public:
	virtual BOOL SetWebCheckUrl(LPCWSTR lpszUrl) DS_PURE;
	virtual DWORD CheckWeb(__in int nWebFlag) DS_PURE;

	virtual BOOL SetMaxRunMissionCount(int nMaxCount) DS_PURE;

	virtual int AddMission(__inout PDSHFTPMISSON pmission) DS_PURE;//, __out int *pnMissionID, __out DWORD *pdwSize) DS_PURE;
	virtual BOOL AddMissionUrl(int nMissionID, LPCWSTR lpszUrl) DS_PURE;

	virtual BOOL StartMission(int nMissionID) DS_PURE;

	virtual BOOL QueryDownloadSize(int nMissionID, int &nTotalSize, int &nDLSize, int &nRecvSize) DS_PURE;

	virtual BOOL DeleteMission(int nMissionID) DS_PURE;
	virtual BOOL DeleteMission(__in LPCTSTR lpszMissionMark) DS_PURE;

	//virtual BOOL SuspendMission(int nMissionID) DS_PURE;
	//virtual BOOL SuspendMission(__in LPCTSTR lpszMissionMark) DS_PURE;

	virtual void DeleteAllMission() DS_PURE;

	virtual void SetUserData(LPVOID lpUserData) DS_PURE;
	virtual LPVOID GetUserData() DS_PURE;
	//virtual void SuspendAllMission() DS_PURE;
	
	virtual BOOL SetNetMsgProc(DSHFTPPROC procNetMsg) DS_PURE;
};


DSHTTPFTPENGINE_API IdsHftpEngine *IdsNew_HftpEngine();






#endif /*__IDSHFTPENGINE_H__*/


