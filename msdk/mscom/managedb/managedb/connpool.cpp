
#include "stdafx.h"
#include "connpool.h"
#include "sqlite/sqliteconn.h"
#include "ado/adoconn.h"
//#include <mslog/syslay/dbg.h>

#define DEFULAT_MIN_CONN	4
#define DEFULAT_MAX_CONN	10

CConnPool::CConnPool()
{
	m_dbType = DB_INVALID;

	m_dwMin = m_dwMax = m_dwCurrent = 0;

	m_bIsNeedReset = FALSE;
	m_dwResetCounter = 0;
}

HRESULT CConnPool::Init(IUnknown* pRot, DWORD dwMin, DWORD dwMax, MsDBType dbType, LPCTSTR szConn ,LPCTSTR szDefaultDB)
{
	m_pRot = pRot;
	if( !dwMin )
		dwMin = DEFULAT_MIN_CONN;

	if( !dwMax )
		dwMax = DEFULAT_MAX_CONN;

	m_dwMin = dwMin;
	m_dwMax = dwMax;
	m_dbType = dbType;
	m_strConn = szConn;
    m_defaultDB = szDefaultDB;

	for( DWORD i=0; i<dwMin; i++ )
	{
		CONN_IMPL* pConn = NULL;

		if( FAILED( GetRealConn(&pConn) ) )
		{
			continue;
		}

		m_Conns.push_back( pConn );
		m_freeList.push_back( pConn );
	}

	m_dwCurrent = m_Conns.size();

	m_Semaphone.init(m_dwMax);
	return S_OK;
}

VOID CConnPool::Uninit()
{
	Conn_Vec::iterator iter = m_Conns.begin();

	for( ; iter!=m_Conns.end(); ++iter )
	{
		CONN_IMPL* pConn = *iter;

		if( pConn )
		{
			pConn->Close();
			delete pConn;
		}
	}
}

CONN_IMPL* CConnPool::GetConn()
{
	// Get from free list
	if (m_Conns.size() <= 0)
	{
		if(true == m_defaultDB.empty())
		{
			return NULL;
		}

		//当连接池中为空时，尝试重新接。
		//本条件及操作
		//只针对于一开始就没有数据中心的状况。
		//不频繁尝试。30内，新的请求进不来。过后才可能重试下一次。
		static DWORD s_dwEmptyCounter = GetTickCount();
		DWORD dwNow = GetTickCount();
		if(dwNow - s_dwEmptyCounter <= 30000)
			return NULL;

		s_dwEmptyCounter = dwNow;

		//最快10秒重试一下。
		CONN_IMPL* pConn = NULL;//初始化
		if( FAILED( GetRealConn(&pConn) ) )
		{
			return NULL;
		}

		m_Conns.push_back( pConn );
		return pConn;
	}
	//检查退出事件。有信号直接退出。
	if(TRUE == m_Event.Wait(1))
		return  NULL;

	//等信号。信号不足，则等待。
	m_Semaphone.WaitSem(-1);
	boost::mutex::scoped_lock lock( m_mutex	);

	if( !m_freeList.empty() )
	{//空闲列表为不空，直接取用。
		CONN_IMPL* pFreeConn = m_freeList.front();
		m_freeList.pop_front();
		//因为异常发生时，连接指针可能己经为NULL了。
		//这里加上保护判断。
		if(NULL == pFreeConn)
		{
			m_Semaphone.ReleaseSem();
			ZM1_GrpError( MODULE_NAME, 1, _T(" CConnPool::GetConn pFreeConn is null!\n") );
			return NULL;
		}

		if(pFreeConn->IsOpen())
			return pFreeConn;	

		//若当前配置不全，则不作重连。
		if(true == m_defaultDB.empty())
		{
			m_Semaphone.ReleaseSem();
			ZM1_GrpError( MODULE_NAME, 1, _T(" CConnPool::GetConn m_defaultDB is empty\n") );
			return NULL;
		}
		
		//不频繁尝试。30秒内，新的请求进不来。过后才可能重试下一次。
		static DWORD s_dwFreeCounter = GetTickCount();
		DWORD dwNow = GetTickCount();
		if(dwNow - s_dwFreeCounter <= 30000)
		{
			m_Semaphone.ReleaseSem();
			return NULL;
		}
		s_dwFreeCounter = dwNow;

		//如果pFreeConn对象可用，并且没有Open过。则这里连接。
		HRESULT hr = pFreeConn->Connect(m_strConn.c_str());
		if(SUCCEEDED(hr))
		{
			return pFreeConn;
		}

		m_Semaphone.ReleaseSem();
		return NULL;
	}
		
	//有信号，无空闲。说明信号数大于实连接数。
	//实连接数不足，直接取实连接加入到连接池。
	CONN_IMPL* pConn = NULL;//初始化
	if( FAILED( GetRealConn(&pConn) ) )
	{
        m_Semaphone.ReleaseSem();
		return NULL;
	}
	//加入到连接池
	m_Conns.push_back( pConn );

	return pConn;
}

VOID CConnPool::FreeConn( CONN_IMPL* pConn )
{
	if( pConn->IsInTransaction() )
	{
		pConn->RollbackTrans();
	}

	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_freeList.push_back( pConn );
	
		m_Semaphone.ReleaseSem();
	}
}

HRESULT CConnPool::GetRealConn( CONN_IMPL** ppConn )
{
	INT m_size = m_defaultDB.size();
	if (!m_size)
	{
		return E_FAIL;
	}

	CONN_IMPL* pDB = new CONN_IMPL(m_pRot);
	RASSERTP( pDB, E_OUTOFMEMORY );

	if( FAILED( pDB->Connect( m_strConn.c_str() ) ) )
	{
		ZM1_GrpError( MODULE_NAME, 1, _T(" connect failed. connstr=%s\n"), m_strConn.c_str() );
		delete pDB;
		
		SetNeedReset(TRUE);
		

		return E_FAIL;
	}

	//*ppConn = pDB;
	pDB->QueryInterface( __uuidof(IGenericDB), (void**)ppConn );
	
	if(TRUE == IsNeedReset())
	{
		DWORD dwDiff = ::GetTickCount() - m_dwResetCounter;
		//一旦出现灾难性DB故障，
		//就死等两分钟，然后重启。
		//检查退出事件。有信号直接退出。
		for(INT i = 0; i < 1000 * 60 * 2 ; ++i )
		{//用等退出事件来实现wait，保证退出事件时能退出。
			if(TRUE == m_Event.Wait(1))
				return  NULL;
		}
		//::Sleep(1000 * 60 * 2);
		//if(dwDiff >=  1000 * 60 * 2)
		{//如果超过2分钟
			HRESULT hr = ResetService();
			if(FAILED(hr))
			{
				ZM1_GrpError( MODULE_NAME, 1, _T(" connect DB failed  and reset service failed!!!\n") );
				return E_FAIL;
			}

			
		}
	}


	return S_OK;
}

HRESULT CConnPool::ResetService()
{
	/*
	UTIL::com_ptr<IMscomRunningObjectTable> pRot = (IMscomRunningObjectTable*)m_pRot;
	if(NULL == pRot.m_p)
		return E_FAIL;
	//FAILEXIT_FAIL(pRot.m_p);

	ZM1_GrpError( MODULE_NAME, 1, _T(" CConnPool::ResetService is called !!!\n") );
	using namespace rsdk;
	using namespace rscom;

	UTIL::com_ptr<IExit2> pExit;
	HRESULT hr = pRot->GetObject(CLSID_RscomEnv, __uuidof(IExit2), (IUnknown**)&pExit);
	if(SUCCEEDED(hr) && pExit)
	{
		pExit->NotifyReset();
	}

	return S_OK;
	*/

	return E_NOTIMPL;

}

BOOL CConnPool::IsNeedReset()
{
	boost::mutex::scoped_lock lock( m_theLockReset );
	return m_bIsNeedReset;
}
BOOL CConnPool::SetNeedReset(BOOL bVal)
{
	boost::mutex::scoped_lock lock( m_theLockReset );
	if(0 == m_dwResetCounter)
		m_dwResetCounter = ::GetTickCount();
	return m_bIsNeedReset = bVal;
}

VOID CConnPool::GetDbgInfo( IMsBuffer* pBuffer )
{
	tostringstream oss;

	oss << _T("Connection pool info:") << std::endl
		<< _T("\tType=") << m_dbType << std::endl
		<< _T("\tConnStr=") << m_strConn << std::endl
		<< _T("Min=") << m_dwMin  << std::endl
		<< _T("Max=") << m_dwMin  << std::endl
		<< _T("Current=") << m_Conns.size()  << std::endl
		<< _T("Free=") << m_freeList.size() << std::endl;

	tstring& strInfo = oss.str();
	pBuffer->SetBuffer( (LPBYTE)strInfo.c_str(), (strInfo.length()+1)*sizeof(TCHAR) );
}

HRESULT CConnPool::ClosePoolConn(VOID)
{
	//boost::mutex::scoped_lock Connlocklock( m_MutexConn );
	m_Event.SetEvent();
	m_Semaphone.WaitAllCountFree(2000);

	UINT m_size = m_Conns.size();
	while(TRUE)
	{
		if(m_freeList.size() != m_size)
			continue;

		Conn_Vec::iterator iter = m_Conns.begin();

		for( ; iter!=m_Conns.end(); ++iter )
		{
			CONN_IMPL* pConn = *iter;

			if( pConn )
			{
				pConn->Close();
				delete pConn;
			}
		}

		break;
	}

	m_Conns.clear();
	m_freeList.clear();

   m_Event.ResetEvent();

	return S_OK;

}