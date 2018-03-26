#pragma once
#include "combase/IMsVector.h"

template<typename IFace = IMsByteVector , 
		 typename LockType = CNullCriticalSection , 
		 typename Type = typename IFace::ItemType>
class CMsVector : 
	public IFace,
	private CUnknownImp
{
public:
	UNKNOWN_IMP1(IFace);
	typedef CStackLockWrapper<LockType> SRVATUOLOCK;
	LockType m_lock;
	static const DWORD m_dwItemSize = sizeof(Type);

	DWORD m_dwStep;  //内存申请步长

	CMsVector():m_pEntry(NULL)
	{
		m_dwStep = 1024;
		m_dwCount	 = 0;
		m_dwCapacity = m_dwStep;
		m_pEntry = (Type*)malloc(m_dwCapacity * m_dwItemSize);
	}

	~CMsVector()
	{
		free(m_pEntry);
	}
	
	STDMETHOD(SetStep)(DWORD dwStep)
	{
		RASSERT(dwStep, E_INVALIDARG);
		m_dwStep = dwStep;
		return S_OK;
	}

	STDMETHOD_(DWORD, GetCount)() 
	{
		return m_dwCount;
	}

	STDMETHOD_(Type, GetAt)(DWORD dwAt)
	{
		typename SRVATUOLOCK lock(m_lock);

		return m_pEntry[dwAt];
	}

	STDMETHOD(Add)(Type item)
	{
		typename SRVATUOLOCK lock(m_lock);
		//没有剩余空间
		if (m_dwCount == m_dwCapacity)
		{
			m_dwCapacity = m_dwCount + m_dwStep;
			m_pEntry = (Type*) realloc(m_pEntry, m_dwCapacity * sizeof(m_dwCapacity));
			RASSERT(m_pEntry, E_OUTOFMEMORY);
		}

		m_pEntry[m_dwCount++] = item;
		return S_OK;
	}

	STDMETHOD(AddAt)(DWORD dwAt, Type item)
	{
		typename SRVATUOLOCK lock(m_lock);
		if (m_dwCount == m_dwCapacity)
		{
			m_dwCapacity = m_dwCount + m_dwStep;
			m_pEntry = (Type*) realloc(m_pEntry, m_dwCapacity * sizeof(m_dwCapacity));
			RASSERT(m_pEntry, E_OUTOFMEMORY);
		}

		MoveItems(dwAt + 1, dwAt);

		m_pEntry[dwAt] = item;
		m_dwCount++;
		return S_OK;
	}

	STDMETHOD(Clear)()
	{
		typename SRVATUOLOCK lock(m_lock);

		m_dwCapacity = m_dwStep;
		m_dwCount	 = 0;
		free(m_pEntry);
		m_pEntry = (Type*)malloc(m_dwCapacity * m_dwItemSize);
		
		return m_pEntry ? S_OK : E_OUTOFMEMORY;
	}


	STDMETHOD(Lock)(DWORD dwTickCount = -1)
	{
		m_lock.Lock(dwTickCount);
		return S_OK;
	}

	STDMETHOD(UnLock)()
	{
		m_lock.Unlock();
		return S_OK;
	}

private:
	void MoveItems(int destIndex, int srcIndex)
	{
		memmove(((unsigned char *)m_pEntry) + destIndex * m_dwItemSize,
			((unsigned char  *)m_pEntry) + srcIndex * m_dwItemSize,
			m_dwItemSize * (m_dwCount  - srcIndex));
	}
private:
	Type* m_pEntry;
	DWORD m_dwCount;
	DWORD m_dwCapacity;
	
};

typedef CMsVector<IMsDwordVector,CNullCriticalSection> CMsDwordVector;
typedef CMsVector<IMsDwordVector,CAutoCriticalSection> CMsSafeDwordVector;