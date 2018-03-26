/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

3,24th,2014

Module Name:

dsHftpEngine.h 

Abstract: 


*/
/************************************************************************/


#ifndef __DSHFTPMISSIONMGR_H__
#define __DSHFTPMISSIONMGR_H__

#include "dsHftpDef.h"
#include "IdsHftpEngine.h"

#include "dsHftpWork.h"
#include "dsNetMsg.h"

#include "dsQueue.h"
#include "dsMap.h"




class dsHftpEngine:public IdsHftpEngine
{
public:

	int m_nCreateMissionKey;

	dsSyncMap<CString,int> m_mapMissionKey;
	dsSyncMap<int,dsHftpWork*> m_mapHftpWork;
	dsWorkQueue<int> m_queueMission;

	dsSyncMap<int,dsHftpWork*> m_mapHftpWorkForDel;
	dsWorkQueue<int> m_queueMissionForDel;
	
	//dsMissionQueue m_queueGetMissionSize;
	dsWorkQueue<dsNetMsg> m_queueNetMsg;

	int m_nRunningCount;
	int m_nMaxRunningCount;
	CString m_strWebCheckUrl;

	DSHFTPPROC m_procNetMsg;

	int m_nWebStatus;
public:

	dsHftpEngine():m_nRunningCount(0),m_nCreateMissionKey(0),m_procNetMsg(NULL),m_nMaxRunningCount(6)
	{
		m_lpUserData = 0;
		Init();
	}
	~dsHftpEngine(){}


private:

	void Init(){
		
		//::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_GetMissionSize, (LPVOID)this, 0, NULL);
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_StartMission, (LPVOID)this, 0, NULL);
		//::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_WritePiece, (LPVOID)this, 0, NULL);
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_OutputMsg, (LPVOID)this, 0, NULL);
		
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_DeleteMission, (LPVOID)this, 0, NULL);
		
	}


public:

	virtual BOOL SetMaxRunMissionCount(int nMaxCount){

		if(nMaxCount <= 2)
			return FALSE;

		dsLocalRefLock lock(m_queueMission.GetLock());

		m_nMaxRunningCount = nMaxCount;

		m_queueMission.SetWork();

		return TRUE;
	}




	virtual int AddMission(__in PDSHFTPMISSON pmission)//, __out int *pId = NULL, __out DWORD *pdwSize = NULL)
	{

		if(NULL == pmission)
			return DSHFTP_MISSION_ERROR;

		CString strMissionMark = dsHftpWork::GetMark(pmission);

		if(!m_mapMissionKey.Find(strMissionMark,pmission->nMissionID))
			pmission->nMissionID = CreateNewMissionID();
		else
			return DSHFTP_MISSION_FILE_EXIST;

		dsHftpWork *phw = new dsHftpWork(pmission);

		if(NULL == pmission)
			return DSHFTP_MISSION_ERROR;
		
		phw->SetMissionID(pmission->nMissionID);
		phw->InjectObserver(Proc_HftpWork, this);

		if(!m_mapMissionKey.Insert(strMissionMark, pmission->nMissionID))
		{
			//phw->Term();
			delete phw;
			return DSHFTP_MISSION_ERROR;
		}

		if(!m_mapHftpWork.Insert(pmission->nMissionID, phw))
		{
			//phw->Term();
			delete phw;
			return DSHFTP_MISSION_ERROR;
		}

		return DSHFTP_MISSION_UNSTART;

	}

	virtual BOOL AddMissionUrl(int nMissionID, LPCWSTR lpszUrl){


		return TRUE;
	}

	virtual BOOL StartMission(__in LPCTSTR lpszUrl){

		int nMissionID;

		if(m_mapMissionKey.Find(lpszUrl, nMissionID))
			return StartMission(nMissionID);

		return FALSE;
	}

	virtual BOOL StartMission(int nMissionID){
		
		if(INVALID_MISSIONID == nMissionID)
			return FALSE;
		
		dsHftpWork *phw;
		if(m_mapHftpWork.Find(nMissionID, phw))
		{
			//m_queueGetMissionSize.Push(nMissionID);
			m_queueMission.Push(nMissionID);
			OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONWAITING, 0, 0);

			return TRUE;
		}

		return FALSE;
	}
	
	virtual BOOL QueryDownloadSize(int nMissionID, int &nTotalSize, int &nDLSize, int &nRecvSize){

		dsLocalRefLock lock(m_mapHftpWork.GetLock());

		dsHftpWork *phw = NULL;
		m_mapHftpWork.Find(nMissionID, phw);

		if(NULL == phw)
			return 0;

		nTotalSize = phw->GetMissionFileSize();
		nDLSize = phw->GetDownloadedSize();
		nRecvSize = phw->GetRecvSize();
		return TRUE;
	}

	//DWORD GetMissionFileSize(__in int nMissionID){

	//	dsLocalRefLock lock(m_mapHftpWork.GetLock());

	//	dsHftpWork *phw = NULL;
	//	m_mapHftpWork.Find(nMissionID, phw);

	//	if(NULL == phw)
	//		return 0;

	//	return phw->GetSizeFromWeb();
	//}

	BOOL SetMission(const LPCTSTR lpszMark, int nStatus){


		dsLocalRefLock lock(m_mapHftpWork.GetLock());

		int nMissionID;
		if(m_mapMissionKey.Find(lpszMark, nMissionID))
			return SetMissionStatus(nMissionID, nStatus);

		return FALSE;
	}

	BOOL SetMissionStatus(int nMissionID, int nStatus){


		dsHftpWork *phw = NULL;

		m_mapHftpWork.Find(nMissionID,phw);
		
		if(NULL == phw)
			return FALSE;

		dsLocalRefLock lock(m_mapHftpWork.GetLock());


		phw->SetStatus(nStatus);

		return FALSE;
	}

	void MarkAllSuspend(){

	}


	virtual BOOL DeleteMission(LPCTSTR lpszMark){

		int nMissionID = INVALID_MISSIONID;
		m_mapMissionKey.Find(lpszMark, nMissionID);

		if(nMissionID >= INVALID_MISSIONID )
			return DeleteMission(nMissionID);

		return FALSE;
	}

	virtual BOOL  DeleteMission(int nMission){
		return EraseMission(nMission, DSHFTP_MISSION_DELETED);
	}

	virtual void DeleteAllMission(){
		return EraseAllMission(DSHFTP_MISSION_DELETED);
	}

	BOOL EraseMission(LPCTSTR lpszMark, int nStatus = DSHFTP_MISSION_INVALID){

		int nMissionID = INVALID_MISSIONID;
		m_mapMissionKey.Find(lpszMark, nMissionID);

		if(nMissionID >= INVALID_MISSIONID )
			return EraseMission(nMissionID, nStatus);

		return FALSE;
	}

	BOOL EraseMission( int nMissionID, int nStatus = DSHFTP_MISSION_INVALID){

		dsHftpWork *phw = NULL;
		m_mapHftpWork.Find(nMissionID, phw);

		if(NULL == phw)
			return FALSE;

		dsLocalRefLock lockHftpWork(m_mapHftpWork.GetLock());

		
		phw->SetStatus(nStatus);

		m_mapMissionKey.Delete(phw->GetKeyString());
		m_mapHftpWork.Delete(nMissionID);

		m_mapHftpWorkForDel.Insert(nMissionID, phw);
		m_queueMissionForDel.Push(nMissionID);

		return TRUE;
	}


	void EraseAllMission( int nStatus = DSHFTP_MISSION_INVALID){

		
		dsLocalRefLock lock(m_mapHftpWork.GetLock());

		dsSyncMap<int,dsHftpWork*>::iterator it = m_mapHftpWork.begin();

		while(it != m_mapHftpWork.end())
		{
	
			m_mapMissionKey.Delete(it->second->GetKeyString());

			dsHftpWork *phw = it->second;
			int nMissionID = phw->GetMissionID();

			if(DSHFTP_MISSION_DELETED == nStatus){
				ATLTRACE("will delete mission step1: %d\n", nMissionID);
			}

			phw->SetStatus(nStatus);

			m_mapHftpWorkForDel.Insert(nMissionID, phw);
			m_queueMissionForDel.Push(nMissionID);

			it++;
		}
	
		m_mapHftpWork.Clear();
	}


	virtual BOOL SetWebCheckUrl(LPCWSTR lpszUrl){
		if(!::PathIsURL(lpszUrl))
			return FALSE;

		m_strWebCheckUrl = lpszUrl;

		return TRUE;
	}

	virtual DWORD CheckWeb(int nWebFlag = DSHFTP_NETCONN_OK){

		DWORD dwFlag = DSHFTP_NETCONN_NULL;

		DWORD dwTemp = 0;

		if(::InternetGetConnectedState(&dwTemp, 0)){

			dwFlag |= DSHFTP_NETCONN_LOCAL;

			if( (DSHFTP_NETCONN_WEB & nWebFlag) && ::InternetCheckConnection(m_strWebCheckUrl,  FLAG_ICC_FORCE_CONNECTION, 0)){

				dwFlag |= DSHFTP_NETCONN_WEB;
			}
		}

		return dwFlag;
	}


	virtual BOOL SetNetMsgProc(DSHFTPPROC procNetMsg){

		if(NULL == procNetMsg)
			return FALSE;

		m_procNetMsg = procNetMsg;

		return TRUE;
	}

	void OutputNetMsg(DSNETMSG &msg){
		m_queueNetMsg.Push(msg);
	}

	inline void OutputNetMsg(int nMissionID, UINT uNetMsg, WPARAM wParam, LPARAM lParam){
		DSNETMSG msg = {(IdsHftpEngine*)this, nMissionID, uNetMsg, wParam, lParam};
		OutputNetMsg(msg);
	}

	void RunningMissionMinus(){
		m_queueMission.LockData();
		m_nRunningCount --;
		m_queueMission.UnlockData();
		m_queueMission.SetWork();
	}

	void RunningMissionAdd(){

		m_queueMission.LockData();
		m_nRunningCount ++;
		m_queueMission.UnlockData();
		m_queueMission.SetWork();
	}

public:

	//////////////////////////////////////////////////////////////////////////
	//start mission thread
	static DWORD CALLBACK Thread_StartMission(LPVOID lpParameter){
		dsHftpEngine *pthis = (dsHftpEngine*)lpParameter;

		do 
		{
			if(WAIT_FAILED == pthis->m_queueMission.WaitWork(INFINITE)){
				break;
			}

			int nMissionID = INVALID_MISSIONID;

			while(pthis->m_nRunningCount < pthis->m_nMaxRunningCount && pthis->m_queueMission.Pop(nMissionID))
			{

				//////////////////////////////////////////////////////////////////////////
				//这里run net thread

				dsHftpWork *phw = NULL;

				pthis->m_mapHftpWork.Lock();

				pthis->m_mapHftpWork.Find(nMissionID, phw);

				if(NULL != phw){
					if(phw->RunMotor())
						pthis->m_nRunningCount ++;
					else
					{
						pthis->EraseMission(nMissionID, DSHFTP_MISSION_ERROR);
						pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONFAID, 0, 0);

					}
				}
				else
					pthis->m_mapHftpWork.Delete(nMissionID);

				pthis->m_mapHftpWork.Unlock();


			}

		} while (TRUE);

		return 0;
	}


	static DWORD CALLBACK Thread_OutputMsg(LPVOID lpParameter){
		dsHftpEngine *pthis = (dsHftpEngine*)lpParameter;


		do{
			if(WAIT_FAILED == pthis->m_queueNetMsg.WaitWork(INFINITE))
				break;


			DSNETMSG nm;
			while (pthis->m_queueNetMsg.Pop(nm)){

				LRESULT lResult = 0;

				if(DSHFTP_MSG_MISSIONFAID == nm.uNetMsg ||
					DSHFTP_MSG_MISSIONDELETED == nm.uNetMsg ||
					DSHFTP_MSG_MISSIONSUCCESS == nm.uNetMsg){
					pthis->RunningMissionMinus();
				}

				if(DSHFTP_MSG_MISSIONFAID == nm.uNetMsg){

					DWORD dwConnFlag = pthis->CheckWeb(DSHFTP_NETCONN_LOCAL);

					if(dwConnFlag < DSHFTP_NETCONN_LOCAL){
						if(NULL != pthis->m_procNetMsg)
							lResult = pthis->m_procNetMsg(nm.pEngine, 0, DSHFTP_MSG_DISCONNECT, 0, dwConnFlag);

						//pthis->m_queueNetMsg.Clear();
					}
				}

				//BOOL bTreated = FALSE;

				//dsHftpWork *phw = NULL;
				//if(pthis->m_mapHftpWork.Find(nm.nMissionID, phw)){
				//	dsLocalRefLock lock(pthis->m_mapHftpWork.GetLock());

				//	if(NULL != phw->m_mission.procThisMission){
				//		bTreated = TRUE;
				//		lResult = phw->m_mission.procThisMission(nm.pEngine, nm.nMissionID, nm.uNetMsg, nm.wParam, nm.lParam);
				//	}
				//}

				if(NULL != pthis->m_procNetMsg)
					lResult = pthis->m_procNetMsg(nm.pEngine, nm.nMissionID, nm.uNetMsg, nm.wParam, nm.lParam);
			}
		}while(TRUE);


		return 0;
	}


	static DWORD CALLBACK Thread_DeleteMission(LPVOID lpParameter){
		dsHftpEngine *pthis = (dsHftpEngine*)lpParameter;

		do 
		{
			if(WAIT_FAILED == pthis->m_queueMissionForDel.WaitWork(INFINITE))
				break;

			int nMissionID = INVALID_MISSIONID;
			while(pthis->m_queueMissionForDel.Pop(nMissionID))
			{

				dsHftpWork *phw = NULL;
				pthis->m_mapHftpWorkForDel.Find(nMissionID, phw);


				pthis->m_mapHftpWorkForDel.Lock();

				if(NULL != phw){

					phw->Term();
					if(phw->CanBeDeleted()){

						ATLTRACE(_T("erase mission: %d - "), nMissionID);

						int nStatus = phw->GetStatus();

						delete phw;
						pthis->m_mapHftpWorkForDel.Delete(nMissionID);

						if(DSHFTP_MISSION_DELETED == nStatus)
						{
							ATLTRACE(_T("delete mission: %d\n"), nMissionID);
							pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONDELETED, 0, 0);
						}
						else
							ATLTRACE(_T("\n"));

					}

				}

				pthis->m_mapHftpWorkForDel.Unlock();

			}
		}while(TRUE);


	return 0;
	}

	static LRESULT CALLBACK Proc_HftpWork(dsHftpWork *phw, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		dsHftpEngine *pthis = (dsHftpEngine*)phw->m_lpParam;
		int nMissionID = phw->GetMissionID();

		switch(uMsg)
		{
		case DSHFTP_MSG_PIECEIN:
			{
				dsNetFilePiece *pnfp = (dsNetFilePiece*)lParam;

				if(NULL != pnfp){

					if(NFP_DATAOK == pnfp->m_nStatus ){
						//1.这里通知写文件线程有任务来了,可以用event
						//2.如果文件下载完毕，则告诉用户，下载完毕，正在写文件，或者在校验
					}
					if(FP_ERROR == pnfp->m_nStatus)
					{
						//这里出错,通知用户，或者重试下载，这里应该通过框架回调
						//pnfp->JustBurnData();
						//pthis->DeleteMission(phw->GetMissionID);
						return -1;
					}

					if(NFP_DATAOK == pnfp->m_nStatus)
						pnfp->m_nStatus = NFP_COMPLETE;

				}
			}
			
			break;

		case DSHFTP_MSG_LITTLEPIECEIN:
			{
				PDSLPI pthinfo = (PDSLPI)lParam;

				//if(-1 == pthinfo->nFlag)
				//{
				//	//这里不检测，让下载线程尽早退出
				//	//pthis->OutputNetMsg(phnf->m_nMissionID, DSHFTP_MSG_DISCONNECT, 0, 0);

				//	return -1;
				//}
				pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONRUNNING, phw->m_dwUnrealRecvBytes, pthinfo->dwDownBytesCount);
			}
			break;


		case DSHFTP_MSG_MISSIONSUCCESS:
			{
				pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONSUCCESS, 0, 0);
				pthis->EraseMission(nMissionID);
			}
			break;

		case DSHFTP_MSG_MISSIONDELETED:
			{
				//pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONDELETED, 0, 1);
				pthis->m_queueMissionForDel.Push(nMissionID);
				return 0;
			}
			break;

		case DSHFTP_MSG_MISSIONFAID:
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONFAID, 0, 1);
			pthis->EraseMission(nMissionID);
			break;

		case DSHFTP_MSG_MISSION_RECVSIZE:
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSION_RECVSIZE, wParam, lParam);
			break;
		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		/*go away
			
			switch(uMsg)
			{
			case DSHFTP_MSG_PIECEIN:
			{
			dsNetFilePiece *pnfp = (dsNetFilePiece*)lParam;

			if(NULL != pnfp){

			if(NFP_DATAOK == pnfp->m_nStatus ){
			//1.这里通知写文件线程有任务来了,可以用event
			//2.如果文件下载完毕，则告诉用户，下载完毕，正在写文件，或者在校验
			}
			if(FP_ERROR == pnfp->m_nStatus)
			{
			//这里出错,通知用户，或者重试下载，这里应该通过框架回调
			//pnfp->JustBurnData();
			//pthis->DeleteMission(phw->GetMissionID);
			return -1;
			}

			if(NFP_DATAOK == pnfp->m_nStatus)
			pnfp->m_nStatus = NFP_COMPLETE;

			}
			}

			break;

			case DSHFTP_MSG_LITTLEPIECEIN:
			{
			PDSLPI pthinfo = (PDSLPI)lParam;

			//if(-1 == pthinfo->nFlag)
			//{
			//	//这里不检测，让下载线程尽早退出
			//	//pthis->OutputNetMsg(phnf->m_nMissionID, DSHFTP_MSG_DISCONNECT, 0, 0);

			//	return -1;
			//}
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONRUNNING, phw->m_dwUnrealRecvBytes, pthinfo->dwDownBytesCount);
			}
			break;


			case DSHFTP_MSG_MISSIONSUCCESS:
			{
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONSUCCESS, 0, 0);
			}
			break;

			case DSHFTP_MSG_MISSIONDELETED:
			{
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSIONDELETED, 0, 1);
			return 0;
			}
			break;

			case DSHFTP_MSG_MISSION_RECVSIZE:
			pthis->OutputNetMsg(nMissionID, DSHFTP_MSG_MISSION_RECVSIZE, wParam, lParam);
			break;
			default:
			break;
			}
		*/
		//////////////////////////////////////////////////////////////////////////



		return 0;
	}

	static LRESULT CALLBACK Proc_LocalFile(dsHftpLocalFile *pfileLocal, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{


		return 0;
	}


	virtual void SetUserData(LPVOID lpUserData)
	{
		m_lpUserData = lpUserData;
	}

	virtual LPVOID GetUserData()
	{
		return m_lpUserData ;
	}
private:

	int CreateNewMissionID(){
		return m_nCreateMissionKey ++;
	}

	LPVOID m_lpUserData;
};



#endif /*__DSHFTPMISSIONMGR_H__*/

