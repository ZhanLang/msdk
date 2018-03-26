/************************************************************************/
/* 
Author:

lourking. .All rights reserved.

Create Time:

	3,24th,2014

Module Name:

	dsHftpWork.h 

Abstract: 下载引擎核心模块


*/
/************************************************************************/

#ifndef __DSHFTPWORK_H__
#define __DSHFTPWORK_H__

#include "dsHftpDef.h"

#include "dsLock.h"
#include "dsEvent.h"
#include "dsQueue.h"
#include "dsMap.h"

#include "dsNetMsg.h"
#include "dsMultiUrlMission.h"
#include "dsFilePiece.h"
#include "dsHftpInterface.h"
#include "dsHftpLocalFile.h"

#include "dsLocalPeroidProcess.h"






//class dsNetFilePiece
//{
//public:
//
//	dsNetFilePiece m_filepiece;
//	HANDLE m_hThreadDownload;
//	BOOL m_bFinished;
//
//	dsNetFilePiece():m_hThreadDownload(NULL),m_bFinished(FALSE)
//	{
//	}
//};


#define HEALTH_ALL		0x0000
#define HEALTH_SPEED	0x0001

typedef struct dsThreadHealthInfo
{
	HANDLE hThread;
	DWORD dwRecvSize;
	DWORD dwTipCount;
	int nFlag;

	dsThreadHealthInfo(){
		ZeroMemory(this, sizeof dsThreadHealthInfo);
	}
}DSTHI,*PDSTHI;

typedef struct dsUrlHealth
{
	int nMissionFlag;
	DWORD dwRecvSize;
	DWORD dwTipCount;
	HANDLE hThread;
	HANDLE hSuspendEvent;
	dsHftpInterface work;
	dsUrlHealth():dwRecvSize(0),dwTipCount(0),hThread(NULL),hSuspendEvent(NULL),nMissionFlag(DSHFTP_MISSION_UNSTART)
	{}
	
	BOOL UpdateInfo(dsUrlHealth *phealth, int nFlag = HEALTH_ALL){
		if(NULL == phealth)
			return FALSE;

		if(HEALTH_ALL == nFlag){
			*this = *phealth;
			return TRUE;
		}

		return FALSE;
	}

}DSURLHEALTH,*PDSURLHEALTH;
 

class dsHftpWork;


typedef LRESULT (CALLBACK *OBSERVERPROC_HFTPWORK)(dsHftpWork */*phw*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

typedef LRESULT (CALLBACK *PIECEPROC)(dsHftpWork */*phw*/, dsNetFilePiece */*pnfp*/, LPVOID /*lpParam*/);
typedef LRESULT (CALLBACK *HEALTHPROC)(dsHftpWork */*phnf*/, PDSTHI /*pthi*/, LPVOID /*lpParam*/);

class dsHftpWork
{
public:

	typedef multimap<CString, HANDLE> MAP_THREADMARK;
	typedef dsSyncMap<HANDLE, DSURLHEALTH> MAP_THREADHEALTH;
	typedef dsSyncQueue<CString> QUEUE_URL;

public:

	int m_nMissionID;
	int m_nStatus;

	dsMultiUrlMission m_mission;
	dsHftpLocalFile m_fileLocal;
	
	dsSyncMap<CString,HANDLE> m_mapThreadMark;
	dsSyncMap<HANDLE,DSURLHEALTH> m_mapThreadHealth; 

	dsSyncQueue<CString> m_queueUrl_AddThread;
	dsSyncQueue<CString> m_queueUrl_DelThread;

	OBSERVERPROC_HFTPWORK m_procObserver;//这个必须只能使用在 Thread_DownFromUrl and Thread_WriteFile内
	LPVOID m_lpParam;

	dsLock m_lockDownQuery;
	DWORD m_dwUnrealRecvBytes;
	DWORD m_dwLastRecvBytes;//上次下载（断电续传）

	//dsEvent m_suspendEvent;

	int m_nRunningThreadCount;


	dsSyncMap<DWORD,dsNetFilePiece> m_mapNfPiece;
	dsWorkQueue<dsNetFilePiece> m_queuePFW;

	dsEvent m_eventInit;

	BOOL m_bSentThrowawayMsg;

	dsLock m_lockThrowaway;

	dsHftpInterface m_dhfi;
	
public:

	dsHftpWork(__in PDSHFTPMISSON pmission):m_mission(pmission),m_fileLocal(pmission),
		m_nMissionID(0),m_procObserver(NULL),m_lpParam(NULL),m_nStatus(DSHFTP_MISSION_UNSTART),m_dwUnrealRecvBytes(0),m_nRunningThreadCount(0),
		m_bSentThrowawayMsg(FALSE),m_dwLastRecvBytes(0)
	{
		m_eventInit.Create(TRUE);
	}

	dsHftpWork(__in dsMultiUrlMission *pmission):m_mission(pmission),m_fileLocal(pmission),
		m_nMissionID(0),m_procObserver(NULL),m_lpParam(NULL),m_nStatus(DSHFTP_MISSION_UNSTART),m_dwUnrealRecvBytes(0),m_nRunningThreadCount(0),
		m_bSentThrowawayMsg(FALSE),m_dwLastRecvBytes(0)
	{
		m_eventInit.Create(TRUE);
	}
	
private:
	/*dsHftpWork():m_nMissionID(0),m_procObserver(NULL),m_lpParam(NULL),m_nStatus(DSHFTP_MISSION_UNSTART),m_dwUnrealRecvBytes(0),m_nRunningThreadCount(0)
	{}*/

	dsHftpWork(){}

public:
	//////////////////////////////////////////////////////////////////////////
	//data

	//BOOL HasSentThrowawayMsg(){
	//	return m_bSentThrowawayMsg;
	//}

	inline int GetStatus(){
		return m_nStatus;
	}

	DWORD GetLastMarkedPieceEnd(){

		dsLocalRefLock lock(m_mapNfPiece.GetLock());

		dsSyncMap<DWORD,dsNetFilePiece>::reverse_iterator it = m_mapNfPiece.rbegin();
		if(it != m_mapNfPiece.rend())
		{
			return it->second.m_dwEnd;
		}

		return 0;
	}

	int CreateNetFilePieceMission(__in HANDLE hThread, __out dsNetFilePiece *pnfPiece){
		
		if(NULL == pnfPiece)
			return FP_ERROR;

		DWORD dwBegin = GetLastMarkedPieceEnd();

		if(dwBegin >= m_mission.dwFileSize)
		{
			pnfPiece->m_nStatus = NFP_COMPLETE;
			//m_nStatus = DSHFTP_MISSION_FINISHED;
			return FP_DATAOK;
		}

		DWORD dwEnd = min(m_mission.dwFileSize, (dwBegin + m_mission.dwFilePieceSize));
		if(!pnfPiece->Allocate(dwBegin, dwEnd))
			return FP_ERROR;

		m_mapNfPiece.Insert(dwBegin, *pnfPiece);

		return FP_NORMAL;
	}

	BOOL UpdatePieceData(__in dsNetFilePiece *pnfPiece){

		if(NULL == pnfPiece)
			return FALSE;

		MAP_NETFILEPIECE::iterator it = m_mapNfPiece.find(pnfPiece->m_dwBegin);

		if(it != m_mapNfPiece.end())
		{

			it->second.m_nStatus = pnfPiece->m_nStatus;
			m_queuePFW.Push(*pnfPiece);

			return TRUE;
		}

		return FALSE;
	}


	void UpdateThreadHealthInfo(PDSURLHEALTH phealth, int nFlag = HEALTH_ALL){

		MAP_THREADHEALTH::iterator it = m_mapThreadHealth.find(phealth->hThread);

		if(it != m_mapThreadHealth.end()){
			it->second.UpdateInfo(phealth, nFlag);
			
			//////////////////////////////////////////////////////////////////////////
			//这里处理线程各种状况



		}else{
			m_mapThreadHealth.insert(make_pair(phealth->hThread, *phealth));
		}
	}


	void UpdateUrlThreadInfo(LPCTSTR  lpszUrl, HANDLE hThread){

		m_mapThreadMark.Insert(lpszUrl, hThread);
	}

	int ThreadCountAdd(){
		return ++m_nRunningThreadCount;
	}

	int ThreadCountMinus(){
		return --m_nRunningThreadCount;
	}

	int GetThreadCount(){
		return m_nRunningThreadCount;
	}

	//BOOL SetStatus(LPCTSTR lpszUrl, HANDLE hThread, int nStatus){
	//	dsLocalRefLock lockLocal(m_lock);
	//	m_nStatus = nStatus;

	//	//////////////////////////////////////////////////////////////////////////
	//	//这里操作

	//	return TRUE;
	//}


	BOOL SetStatus(int nStatus){

		if(DSHFTP_MISSION_INVALID == nStatus)
			return FALSE;

		if(m_nStatus == nStatus)
			return TRUE;


		if(DSHFTP_MISSION_DELETED == m_nStatus || DSHFTP_MISSION_ERROR == m_nStatus || DSHFTP_MISSION_FINISHED == m_nStatus || DSHFTP_MISSION_FINISHED_BEFORE == m_nStatus)
			return FALSE;

		m_nStatus = nStatus;

		//////////////////////////////////////////////////////////////////////////
		//这里操作

		switch(m_nStatus)
		{

		case DSHFTP_MISSION_DELETED:


			Term();
			break;

		case DSHFTP_MISSION_FINISHED:

			Term();

			break;

		case DSHFTP_MISSION_ERROR:

			Term();

			break;

		default:

			break;
		}
			
		return TRUE;
	}


	CString GetMark(){

		if(0 != m_mission.szMissionMark[0]){
			return m_mission.szMissionMark;
		}
		else if(0 != m_mission.szUrl[0]){
			return m_mission.szUrl;
		}
		else
			return L"";
	}

	static CString GetMark(__in PDSHFTPMISSON pmission){


		if(NULL == pmission)
			return L"";

		if(0 != pmission->szMissionMark[0]){
			return pmission->szMissionMark;
		}
		else if(0 != pmission->szUrl[0]){
			return pmission->szUrl;
		}
		else
			return L"";
	}


	BOOL IsValidate(){
		
		

		return TRUE;
	}

	void FixMarksChain(){

		MAP_NETFILEPIECE::iterator it = m_mapNfPiece.begin();
		MAP_NETFILEPIECE::iterator itNext = it;

		while(it != m_mapNfPiece.end()){
			itNext ++;
			if(itNext != m_mapNfPiece.end()){

				if(it->second.m_dwEnd > itNext->second.m_dwBegin){
					dsNetFilePiece nfp = it->second;

					m_mapNfPiece.clear();
					m_mapNfPiece.insert(make_pair(nfp.m_dwBegin, nfp));

					break;
				}
				else if(it->second.m_dwEnd == itNext->second.m_dwBegin){
					if(FP_COMPLETE == it->second.m_nStatus || FP_COMPLETE == itNext->second.m_nStatus){

						it->second.SetMark(it->second.m_dwBegin, itNext->second.m_dwEnd);
						m_mapNfPiece.erase(itNext);
					}
				}
				else{

					dsNetFilePiece nfp;
					nfp.SetMark(it->second.m_dwEnd, itNext->second.m_dwBegin);
					MAP_NETFILEPIECE::_Pairib pib = m_mapNfPiece.insert(make_pair(nfp.m_dwBegin, nfp));
					_ASSERT(pib.second);
					it = pib.first;
				}

			}

			it ++;
			itNext = it;
		}
	}

	void InjectObserver(OBSERVERPROC_HFTPWORK procObserver, LPVOID lpParam){
		m_procObserver = procObserver;
		m_lpParam = lpParam;
	}

	int Init(){

		if(LOCALFILE_REALFILE == m_fileLocal.CheckFileRes()){
			if(DSHFTP_FILE_IFEXIT_OK == m_mission.nLocalFileWriteFlag ){
				return DSHFTP_MISSION_FINISHED_BEFORE;
			}
		}

		if(!m_fileLocal.InitLocalFile()){
			return DSHFTP_MISSION_ERROR;
		}


		//ATLTRACE("mission: %d,  init 2\n",m_nMissionID);
		//m_mission.dwFileSize
		//	= m_fileLocal.m_dwFileSize
		//	= GetSizeFromWeb();

		if(!m_fileLocal.ReadMark()){
			//return DSHFTP_MISSION_ERROR;这里不能return
		}
;
		m_mission.dwFileRecvedSize
			= m_dwUnrealRecvBytes
			= m_fileLocal.GetDownloadedSize();

		m_dwLastRecvBytes = m_dwUnrealRecvBytes;

		m_fileLocal.OutputNetFilePiece(m_mapNfPiece);


		if(!m_dhfi.Init(m_mission.szUrl, &m_mission.dwFileSize, m_fileLocal.GetDownloadedSize()))
			return DSHFTP_MISSION_ERROR;

		m_fileLocal.m_dwFileSize = m_mission.dwFileSize;


		if(m_fileLocal.CheckMissionComplete()){
			m_fileLocal.CompleteFile();
			return DSHFTP_MISSION_FINISHED;
		}

		return DSHFTP_MISSION_RUNNING;
	}

	BOOL RunMotor(){

		if(m_nRunningThreadCount > 0)
			return FALSE;

		if(m_mission.m_setUrl.size() <= 0)
			return FALSE;
		

		SET_URL::const_iterator it = m_mission.m_setUrl.begin();

		while(it != m_mission.m_setUrl.end()){
			CString s = *it;
			m_queueUrl_AddThread.Push(s);
			it ++;
		}
		int nThreadCount = m_queueUrl_AddThread.size();

		m_nRunningThreadCount = 2 + nThreadCount;
	
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_Init, (LPVOID)this, 0, NULL);

		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_WriteFile, (LPVOID)this, 0, NULL);

		for (int i=0; i< nThreadCount; i++)
		{

			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_DownFromUrl, (LPVOID)this, 0, NULL);
		}

		return TRUE;
	}


	inline BOOL CanBeDeleted(){

		return GetThreadCount() <= 0;
	
	}

	BOOL CheckOK(){

		return TRUE;
	}


	CString GetKeyString(){
		return m_mission.GetMark();
	}
//////////////////////////////////////////////////////////////////////////
//data set
	DWORD GetSizeFromWeb(){

		dsHftpInterface dhfi;

		if(dhfi.Init(m_mission.szUrl, &m_mission.dwFileSize, m_fileLocal.GetDownloadedSize()))
		{
			dhfi.Close();

		}
		else
			return INVALID_FILE_SIZE;

		return m_mission.dwFileSize;
	}

	inline DWORD GetMissionFileSize(){
		return m_mission.dwFileSize;
	}


	DWORD GetDownloadedSize(){
		dsLocalRefLock lock(m_lockDownQuery);

		//ATLTRACE("mission: %d, size :%d - %d = %d\n", m_nMissionID, m_dwUnrealRecvBytes, m_dwLastRecvBytes, m_dwUnrealRecvBytes - m_dwLastRecvBytes);

		return m_dwUnrealRecvBytes - m_dwLastRecvBytes;
	}

	DWORD GetRecvSize(){
		dsLocalRefLock lock(m_lockDownQuery);

		//ATLTRACE("mission: %d, size :%d - %d = %d\n", m_nMissionID, m_dwUnrealRecvBytes, m_dwLastRecvBytes, m_dwUnrealRecvBytes - m_dwLastRecvBytes);

		return m_dwUnrealRecvBytes;
	}

	BOOL AddUrl(LPCTSTR lpszUrl){
		if(m_mission.AddUrl(lpszUrl))
		{
			return NewUrlDownload(lpszUrl);
		}
		return FALSE;
	}	

	BOOL NewUrlDownload(LPCTSTR lpszUrl){

		m_queueUrl_AddThread.Push(CString(lpszUrl));
		int nThreadCount = m_queueUrl_AddThread.Size();

		return NULL != ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_DownFromUrl, (LPVOID)this, 0, NULL);
	}

	void SetMissionID(int nMissionID){
		m_fileLocal.m_nMissionID = m_mission.nMissionID = m_nMissionID = nMissionID;
		

	}

	inline int GetMissionID(){
		return m_nMissionID;
	}


	static BOOL DeleteHftpWork(dsHftpWork *phw){

		//

		return TRUE;
	}

	inline DWORD GetRecvBytes(){
		return m_dwUnrealRecvBytes;
	}


	inline BOOL CheckKeepOnNetWork(){

		return (m_nStatus != DSHFTP_MISSION_ERROR && 
			m_nStatus != DSHFTP_MISSION_WEBFINISHED && 
			m_nStatus != DSHFTP_MISSION_FINISHED_BEFORE &&
			m_nStatus != DSHFTP_MISSION_DELETED);
	}

	inline BOOL CheckKeepOnWork(){

		return (m_nStatus != DSHFTP_MISSION_ERROR && 
			m_nStatus != DSHFTP_MISSION_FINISHED && 
			m_nStatus != DSHFTP_MISSION_FINISHED_BEFORE &&
			m_nStatus != DSHFTP_MISSION_DELETED);
	}

	void Term(){
		////m_nStatus = DSHFTP_MISSION_DELETED;

		//SetStatus(DSHFTP_MISSION_DELETED);
		m_fileLocal.Term();
		//Term_HealthInfo();
		//Term_DeleteUnwriteData();
	}


	BOOL CanSendThrowaway_AutoSet(){

		dsLocalRefLock lock(m_lockThrowaway);

		if(m_bSentThrowawayMsg)
			return FALSE;	

		
		BOOL bRet = m_bSentThrowawayMsg;
		m_bSentThrowawayMsg = TRUE;
		return !bRet;
	}

private: 

	void Term_DeleteUnwriteData(){
		MAP_NETFILEPIECE::iterator it = m_mapNfPiece.begin();

		while(it != m_mapNfPiece.end()){
			if(NFP_COMPLETE != it->second.m_nStatus)
			{
			
			}
				//it->second.JustBurnData();

			it ++;
		}
	}

	void Term_HealthInfo(){

		MAP_THREADHEALTH::iterator it = m_mapThreadHealth.begin();

		while(it != m_mapThreadHealth.end()){
			//::TerminateThread(it->first, 1);
			//it->second.work.Close();
			it ++;
		}


	}



	

private:

	static void CALLBACK ProcThreadCountAdd(LPVOID lpParam){
		dsHftpWork *pthis = (dsHftpWork*)lpParam;
		pthis->ThreadCountAdd();
	}

	static void CALLBACK ProcThreadCountMinus(LPVOID lpParam){
		dsHftpWork *pthis = (dsHftpWork*)lpParam;
		pthis->ThreadCountMinus();


		

		switch(pthis->m_nStatus)
		{
		case DSHFTP_MISSION_ERROR:
			if(pthis->CanSendThrowaway_AutoSet())
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONFAID, 0, 0);

			break;
			
		case DSHFTP_MISSION_DELETED:
			ATLTRACE("mission thread count: %d - ", pthis->GetThreadCount());

			ATLTRACE("will delete mission step2: %d\n", pthis->GetMissionID());

			pthis->m_queuePFW.SetWork();

			if(pthis->CanBeDeleted()){
				
				ATLTRACE("will delete mission step3: %d\n", pthis->GetMissionID());
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONDELETED, 0, 0);
			}

			break;

		case DSHFTP_MISSION_FINISHED:
		case DSHFTP_MISSION_FINISHED_BEFORE:
			if(pthis->CanSendThrowaway_AutoSet())
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONSUCCESS, 0, 0);
			break;
		
		case DSHFTP_MISSION_RUNNING:
			pthis->m_procObserver(pthis, DSHFTP_MSG_MISSION_RECVSIZE, pthis->GetMissionFileSize(), pthis->GetRecvBytes());
			break;

		
		
		default:
			break;

		}
		
	}

	static void CALLBACK ProcThreadCountMinus_Web(LPVOID lpParam){
		dsHftpWork *pthis = (dsHftpWork*)lpParam;
		pthis->ThreadCountMinus();

		switch(pthis->m_nStatus)
		{
		case DSHFTP_MISSION_ERROR:
			if(pthis->CanSendThrowaway_AutoSet())
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONFAID, 0, 0);
			break;

		case DSHFTP_MISSION_DELETED:
			ATLTRACE("mission thread count: %d - ", pthis->GetThreadCount());

			ATLTRACE("will delete mission step2: %d\n", pthis->GetMissionID());

			pthis->m_queuePFW.SetWork();

			if(pthis->CanBeDeleted()){

				ATLTRACE("will delete mission step3: %d\n", pthis->GetMissionID());
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONDELETED, 0, 0);
			}

			break;
			
		case DSHFTP_MISSION_WEBFINISHED:

			break;

		case DSHFTP_MISSION_FINISHED:
		case DSHFTP_MISSION_FINISHED_BEFORE:
			if(pthis->CanSendThrowaway_AutoSet())
				pthis->m_procObserver(pthis, DSHFTP_MSG_MISSIONSUCCESS, 0, 0);
				
			break;

		case DSHFTP_MISSION_RUNNING:
			pthis->m_procObserver(pthis, DSHFTP_MSG_MISSION_RECVSIZE, 0, pthis->GetRecvBytes());
			break;



		default:
			break;

		}

	}


	static DWORD CALLBACK Thread_Init(LPVOID lpParam){

		dsHftpWork *pthis = (dsHftpWork*)lpParam;

		dsLocalPeroidProcess lppThreadCount(lpParam, NULL, ProcThreadCountMinus);

		int nRet = pthis->Init();

		pthis->SetStatus(nRet);
		pthis->m_eventInit.Set();

		ATLTRACE("mission:%d ,init ok get size:%d \n", pthis->m_nMissionID, pthis->m_mission.dwFileSize);

		return 0;
	}
	

	//////////////////////////////////////////////////////////////////////////
	//download thread
	static DWORD CALLBACK Thread_DownFromUrl(LPVOID lpParam){

		dsHftpWork *pthis = (dsHftpWork*)lpParam;

		dsLocalPeroidProcess lppThreadCount(lpParam, NULL, ProcThreadCountMinus_Web);

		if(WAIT_FAILED == pthis->m_eventInit.Wait(INFINITE)){

			pthis->SetStatus(DSHFTP_MISSION_ERROR);
			return 1;
		}

		if(!pthis->CheckKeepOnNetWork())
			return 1;

		HANDLE hThread = GetCurrentThread();


		CString strUrl;
		if(!pthis->m_queueUrl_AddThread.Pop(strUrl))
			return 1;

		pthis->UpdateUrlThreadInfo(strUrl, hThread);
		

		if(pthis->m_mission.nMissionID == 10)
			int a= 0;
		
		//dsHftpInterface dshi;

		//PDSURLHEALTH phealth = new DSURLHEALTH;

		//phealth->hThread = hThread;
		//phealth->hSuspendEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		//phealth->work = dshi;

		DSURLHEALTH health;

		health.hThread = hThread;
		//health.hSuspendEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		//health.work = dshi;

		pthis->UpdateThreadHealthInfo(&health);

		DWORD _dwTickCount = ::GetTickCount();

		ATLTRACE("mission:%d ,Thread_DownFromUrl begin down loop get size:%d \n", pthis->m_nMissionID, pthis->m_mission.dwFileSize);

		DWORD dwRecvSize = pthis->m_fileLocal.GetDownloadedSize();
		
		//if(pthis->m_dhfi.Init(strUrl, &pthis->m_mission.dwFileSize, dwRecvSize))
		{


			UINT uTimeout = 10 * 1000;
			pthis->m_dhfi.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, &uTimeout, sizeof UINT);

			DWORD dwNoCache = INTERNET_FLAG_NO_CACHE_WRITE;
			pthis->m_dhfi.SetOption(INTERNET_OPTION_RESET_URLCACHE_SESSION , &dwNoCache, sizeof DWORD);


			BOOL bSet = FALSE;

			while( pthis->CheckKeepOnNetWork())
			{
				//WaitForSingleObject(phealth->hSuspendEvent, INFINITE);

				//ATLTRACE("mission:%d ,Thread_DownFromUrl in down loop1 get size:%d \n", pthis->m_nMissionID, pthis->m_mission.dwFileSize);
				//////////////////////////////////////////////////////////////////////////
				//这里下载数据切片
				dsNetFilePiece nfp;
				int nRet = pthis->CreateNetFilePieceMission(hThread, &nfp);//need

				if(FP_NORMAL == nRet)
				{
					DWORD dwDownBytesCount = 0;

					/*if(!bSet){
						dshi.SetFileOffset(nfp.m_dwBegin);
						bSet = TRUE;
					}*/

					int nRetryCount = 0;
					BOOL bRet = FALSE;
					DWORD dwTempCount;
					DWORD dwNeedReadCount = 0;

					int nTotalPieceSize = nfp.GetSize();

					DSLPI lpi;

					while (pthis->CheckKeepOnNetWork())
					{
						dwTempCount = 0;
						dwNeedReadCount = min(pthis->m_dhfi.m_dwDownloadSizePerRead, nTotalPieceSize - dwDownBytesCount);

						

						bRet = pthis->m_dhfi.DownloadPieceOnce(nfp.m_data + dwDownBytesCount, dwNeedReadCount, dwTempCount);

						DWORD dwTick = ::GetTickCount();

						if(dwTick - _dwTickCount > 1000){
							_dwTickCount = dwTick;

							ATLTRACE("mission: %d, downsize: %d\n", pthis->m_nMissionID, pthis->m_dwUnrealRecvBytes);
						}

						
						DWORD dwError = WSAGetLastError();

						pthis->m_lockDownQuery.Lock();
						pthis->m_dwUnrealRecvBytes += dwTempCount;
						pthis->m_lockDownQuery.UnLock();

						lpi.SetInfo(bRet, dwTempCount, dwError);
						pthis->m_procObserver(pthis, DSHFTP_MSG_LITTLEPIECEIN, 0, (LPARAM)&lpi);

						if (bRet && 0 == dwError)
						{
							if (dwTempCount > 0)
							{
								dwDownBytesCount += dwTempCount;
							}
							else if (0 == dwTempCount)
							{
								if(dwDownBytesCount != nTotalPieceSize)
								{
									bRet = FALSE;
									pthis->SetStatus(DSHFTP_MISSION_ERROR);
								}
								
								break; //下载到文件最末尾
							}

							if(dwDownBytesCount == nTotalPieceSize)
								break; //切片下载完成
						}
						else if(ERROR_INTERNET_TIMEOUT == dwError && nRetryCount++ < pthis->m_dhfi.m_uRequestTryTimes){
							if (dwTempCount > 0)
							{
								dwDownBytesCount += dwTempCount;
							}
						}
						else{

							bRet = FALSE;
							pthis->SetStatus(DSHFTP_MISSION_ERROR);
							break;
						}
					}

					if(bRet)
					{
						nfp.m_nStatus = NFP_DATAOK;
						pthis->UpdatePieceData(&nfp);
					}
					else
					{
						nfp.m_nStatus = NFP_ERROR;
						pthis->UpdatePieceData(&nfp);
						break;
					}

					//ATLTRACE("mission:%d ,Thread_DownFromUrl in down loop2 get size:%d \n", pthis->m_nMissionID, pthis->m_dwUnrealRecvBytes);

				}
				else if(FP_DATAOK == nRet)
				{
					pthis->SetStatus(DSHFTP_MISSION_WEBFINISHED);
					break;
				}
				else//error
				{
					//这里要对失败做出对策
					pthis->SetStatus(DSHFTP_MISSION_ERROR);
					break;
				}
			}
		}
		

		pthis->m_dhfi.Close();

		return 0;
	}


	//////////////////////////////////////////////////////////////////////////
	//THREAD for write file

	static DWORD CALLBACK Thread_WriteFile(LPVOID lpParam){

		dsHftpWork *pthis = (dsHftpWork*)lpParam;

		dsLocalPeroidProcess lppThreadCount(lpParam, NULL, ProcThreadCountMinus);

		if(pthis->m_mission.nMissionID == 10)
			int a= 0;

		while(pthis->CheckKeepOnWork()){

			if(WAIT_FAILED == pthis->m_queuePFW.WaitWork(INFINITE)){

				pthis->SetStatus(DSHFTP_MISSION_ERROR);
				break;
			}

			dsNetFilePiece nfp;
			while(pthis->m_queuePFW.Pop(nfp) && pthis->CheckKeepOnWork()){

				if(NFP_DATAOK != nfp.m_nStatus)
					continue;

				pthis->m_fileLocal.WriteNetPieceToFile(&nfp);
				pthis->m_fileLocal.WritePieceMarkToFile(&nfp);
				pthis->m_fileLocal.UpdateLocalPieceData(&nfp);

				nfp.JustBurnData();
				//pthis->UpdatePieceData(&nfp);
				pthis->m_procObserver(pthis, DSHFTP_MSG_PIECEWRITE, nfp.m_dwBegin, nfp.m_dwEnd);
			}

			if(pthis->m_fileLocal.CheckMissionComplete())
			{
				pthis->m_fileLocal.CompleteFile();
				pthis->SetStatus(DSHFTP_MISSION_FINISHED);
				break;
			}
		}

		return 0;
	}
};




#endif /*__DSHFTPWORK_H__*/