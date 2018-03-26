
#pragma once

#include <vector>
#include <deque>
#include <database/genericdb.h>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <assert.h>

class CONN_IMPL;

#define  ASSERT assert
class CMyEvent
{
public: 
	CMyEvent()
	{
		m_hEvent = CreateEvent(
			0,
			TRUE,
			FALSE,
			NULL
			);
		ASSERT(m_hEvent);
	};
	~CMyEvent()
	{
		ASSERT(m_hEvent);
		BOOL bRet = CloseHandle(m_hEvent);
	};
public:
	BOOL SetEvent()
	{
		ASSERT(m_hEvent);
		return ::SetEvent(m_hEvent);

	};
	BOOL ResetEvent()
	{
		ASSERT(m_hEvent);
		return ::ResetEvent(m_hEvent);
	};

	BOOL Wait(DWORD dwTimeOutInterval)
	{
		DWORD dwRet = ::WaitForSingleObject(
			m_hEvent
			,dwTimeOutInterval
			);
		if(WAIT_TIMEOUT == dwRet)
			return FALSE;
		return TRUE;
	};

private:
	HANDLE m_hEvent;
};


//信号量.
class CMySemaphore
{
public:
	CMySemaphore()
	{
		//const INT MAX_SEM_COUNT = 500;
		//m_hSemaphore = CreateSemaphore( 
		//	NULL,           // default security attributes
		//	MAX_SEM_COUNT,  // initial count
		//	MAX_SEM_COUNT,  // maximum count
		//	NULL);          // unnamed semaphore
		//if(NULL == m_hSemaphore)
		//	return;
		//m_lMaxCount = MAX_SEM_COUNT;


	};
	CMySemaphore(INT nSemaCount)
	{
		m_hSemaphore = CreateSemaphore( 
			NULL,           // default security attributes
			nSemaCount,		// initial count
			nSemaCount,		// maximum count
			NULL);          // unnamed semaphore
		if(NULL == m_hSemaphore)
			return;
		m_lMaxCount = nSemaCount;

	};
	~CMySemaphore()
	{
		if(m_hSemaphore)
			CloseHandle(m_hSemaphore);
	};

	VOID init(INT nSemaCount)
	{
		m_hSemaphore = CreateSemaphore( 
			NULL,           // default security attributes
			nSemaCount,		// initial count
			nSemaCount,		// maximum count
			NULL);          // unnamed semaphore
		if(NULL == m_hSemaphore)
			return;
		m_lMaxCount = nSemaCount;
	}

	BOOL WaitSem(INT iTimeOutInterval)
	{
		DWORD dwWaitResult = WaitForSingleObject(m_hSemaphore,iTimeOutInterval);  
		if(WAIT_TIMEOUT == dwWaitResult)
			return FALSE;
		return TRUE;
	}
	BOOL ReleaseSem(LONG* pCount = NULL)
	{
		BOOL bRet = ReleaseSemaphore(m_hSemaphore,1,pCount);
		return bRet;
	}

	//等待10ms * iTimeOutInterval
	BOOL WaitAllCountFree(UINT uTimeOutInterval)
	{

		DWORD dwWaitTimeOut = 0;
		while(TRUE)
		{
			BOOL bRet = WaitSem(10);
			if(FALSE == bRet)
			{//超时
				continue;
			}
			LONG lCount = 0;
			bRet = ReleaseSem(&lCount);
			if(m_lMaxCount - 1 ==lCount)
				return TRUE;

			//循环等待.
			Sleep(10);
			if(++dwWaitTimeOut >= uTimeOutInterval)
				return TRUE;					
		}

		return FALSE;
	}

private:
	LONG	m_lMaxCount;
	HANDLE m_hSemaphore;
};

class CGetSem
{
public:
	CGetSem(CMySemaphore* pSem)
	{
		if(NULL == pSem)
			return ;
		m_pSem = pSem;
		BOOL bRet = m_pSem->WaitSem(-1);

	};
	~CGetSem()
	{
		if(NULL == m_pSem)
			return ;
		BOOL bRet = m_pSem->ReleaseSem();
	};
private:
	CMySemaphore* m_pSem;		

};

class CConnPool
{
public:
	CConnPool();

	HRESULT Init(IUnknown* pRot, DWORD dwMin, DWORD dwMax, MsDBType dbType, LPCTSTR szConn ,LPCTSTR szDefaultDB);
	VOID	Uninit( VOID );

	CONN_IMPL* GetConn();
	VOID FreeConn( CONN_IMPL* pConn );

	VOID	GetDbgInfo( IMsBuffer* pBuffer );
	 STDMETHOD(ClosePoolConn)(VOID);

private:
	HRESULT GetRealConn( CONN_IMPL** ppConn );

	//++++++++++++++++++++
	/*VOID ConnVecLock() {m_mutexReal.lock();}
	VOID ConnVecUnLock() {m_mutexReal.unlock();}
	VOID FreeListLock() {m_mutex.lock();}
	VOID FreeListUnLock() {m_mutex.unlock();}*/
	//+++++++++++++++++++++

	HRESULT ResetService();
	BOOL IsNeedReset();
	BOOL SetNeedReset(BOOL bVal);
	



private:
	typedef std::vector<CONN_IMPL*>		Conn_Vec;
	typedef std::deque<CONN_IMPL*>		Free_List;		// 空闲连接的索引

	Conn_Vec			m_Conns;
	boost::mutex		m_mutexReal;

	Free_List			m_freeList;		// 空闲队列
	boost::mutex		m_mutex;
	boost::condition	m_condition;
	
	MsDBType		m_dbType;
	tstring	         m_strConn;
	tstring          m_defaultDB;
	
	DWORD			m_dwMin;		// 最小连接数
	DWORD			m_dwMax;		// 最小连接数
	DWORD volatile	m_dwCurrent;	// 当前连接数

	CMySemaphore   m_Semaphone;
	CMyEvent    m_Event;

	//boost::mutex		m_MutexConn;


	boost::mutex		m_theLockReset;
	BOOL			m_bIsNeedReset;
	DWORD			m_dwResetCounter;

	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;

	

};