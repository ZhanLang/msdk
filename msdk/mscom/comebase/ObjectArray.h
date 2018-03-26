#pragma once
#include <combase/IObjectArray.h>

#include <util/msvector.h>
template<typename LockType = CNullCriticalSection>
class CObjectArray : public IObjectArray,private CUnknownImp
{
public:

	typedef CStackLockWrapper<LockType> SRVATUOLOCK;
	UNKNOWN_IMP1(IObjectArray);

	CObjectArray(void)
	{

	}
	~CObjectArray(void)
	{

	}

	virtual VOID Push( IMSBase* pItem )
	{
		SRVATUOLOCK lock(m_lock);
		m_Items.Add(pItem);
	}

	virtual DWORD GetSize()
	{
		SRVATUOLOCK lock(m_lock);
		return m_Items.Size();
	}

	virtual BOOL GetAt( DWORD dwAt, IMSBase** pItem )
	{
		SRVATUOLOCK lock(m_lock);
		if ( dwAt < m_Items.Size() && m_Items[dwAt])
		{
			m_Items[dwAt]->QueryInterface(__uuidof(IMSBase), (void**)pItem);
			return S_OK;
		}

		return E_FAIL;
	}

	virtual BOOL RemoveAt( DWORD dwAt )
	{
		SRVATUOLOCK lock(m_lock);
		if( dwAt < m_Items.Size() )
		{
			m_Items.DeleteFrom(dwAt);
			return S_OK;
		}

		return FALSE;
	}

	virtual VOID Clear()
	{
		SRVATUOLOCK lock(m_lock);
		m_Items.Clear();
	}


private:
	LockType m_lock;
	CApiVector<UTIL::com_ptr<IMSBase>> m_Items;
};

