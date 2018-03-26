#ifndef __SYNCOBJECTIMPL_H__
#define __SYNCOBJECTIMPL_H__


#include "ISyncObject.h"
/*namespace msdk {*/
////////////////////////////////////////////////////////////////////////////////
//临界区
////////////////////////////////////////////////////////////////////////////////
class CCriticalSection : public ISyncObject, private CUnknownImp
{
public:
	UNKNOWN_IMP1(ISyncObject);
	CCriticalSection()
	{ 
		::InitializeCriticalSection(&m_cs); 
	}
	~CCriticalSection()
	{ 
		::DeleteCriticalSection(&m_cs); 
	}
	STDMETHODIMP Lock()
	{ 
		::EnterCriticalSection(&m_cs); 
		return S_OK; 
	}
	STDMETHODIMP Unlock()
	{ 
		::LeaveCriticalSection(&m_cs); 
		return S_OK; 
	}
private:
	CRITICAL_SECTION m_cs;
};

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//同步包装者
////////////////////////////////////////////////////////////////////////////////
class CSyncWrapper
{
public:
	CSyncWrapper( ISyncObject* pSyncObject):
		m_pSyncObject(pSyncObject)
	{
		m_pSyncObject->Lock();
	};

	~CSyncWrapper()
	{
		m_pSyncObject->Unlock();
	}

private:
	ISyncObject* m_pSyncObject;
};
////////////////////////////////////////////////////////////////////////////////
//
#define DECLARE_SYNC_OBJ()\
	private:\
        CCriticalSection  m_csSyncObject

#define _SYNC_OBJ()\
    CSyncWrapper  _Sync_Wrapper(&m_csSyncObject)
////////////////////////////////////////////////////////////////////////////////
//
#define DECLARE_SYNC_OBJ_FLAG(Flag)\
	private:\
		CCriticalSection m_csSyncObject##Flag

#define _SYNC_OBJ_FLAG(Flag)\
	CSyncWrapper _Sync_Wrapper(&m_csSyncObject##Flag)

//如果需要使用BOOST同步的话，请取消注释
/*
 #include <boost/asio/detail/mutex.hpp>
 #define DECLARE_BOOST_SYNC_OBJ()\
     private:\
     boost::asio::detail::mutex m_boost_mutex
 
 #define BOOST_SYNC_OBJ()\
     boost::asio::detail::scoped_lock<boost::asio::detail::mutex> s_lock(m_boost_mutex)
*/
/*}*///namespace msdk
#endif //__SYNCOBJECTIMPL_H__