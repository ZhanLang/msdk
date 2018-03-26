#ifndef _CRITICALSECTION_INCLUDE_H_
#define _CRITICALSECTION_INCLUDE_H_

#ifdef _WIN32
#pragma once
#endif

#include <windows.h>

namespace msdk {

class CNullCriticalSection
{
public:
	void Lock(DWORD dwTimeout = INFINITE) {}
	void Unlock() {}
	CNullCriticalSection() {}
	~CNullCriticalSection() {}
};

class CAutoCriticalSection
{
public:
	void Lock(DWORD dwTimeout = INFINITE) {EnterCriticalSection(&m_sec);}
	void Unlock() {LeaveCriticalSection(&m_sec);}
	CAutoCriticalSection() {InitializeCriticalSection(&m_sec);}
	~CAutoCriticalSection() {DeleteCriticalSection(&m_sec);}

	CRITICAL_SECTION m_sec;
};

class CSingleAutoCriticalSection
{
protected:
	inline LONG Increment(LPLONG p) {return ::InterlockedIncrement(p);}
	inline LONG Decrement(LPLONG p) {return ::InterlockedDecrement(p);}
public:
	CSingleAutoCriticalSection() 
	{
		m_dwRef = 1L;
		InitializeCriticalSection(&m_sec);
	}
	~CSingleAutoCriticalSection() 
	{
		DeleteCriticalSection(&m_sec);
	}	
	
	void Lock(DWORD dwTimeout = INFINITE) {EnterCriticalSection(&m_sec);}
	void Unlock() {LeaveCriticalSection(&m_sec);}
	
	LONG AddRef() {return Increment(&m_dwRef);}
	LONG ReleaseRef()
	{
		LONG l = Decrement(&m_dwRef);		
		return l;
	}

	CRITICAL_SECTION m_sec;
	LONG m_dwRef;
};

template<class _TInterface>
class CSingleInstance : public _TInterface
{
protected:
	LONG m_dwRef;

	inline ULONG Increment(LPLONG p) {return ::InterlockedIncrement(p);}
	inline ULONG Decrement(LPLONG p) {return ::InterlockedDecrement(p);}
public:
	CSingleInstance() 
	{
		m_dwRef = 1L;
	}
	virtual ~CSingleInstance() 
	{
		Decrement(&m_dwRef);
	}

	virtual long AddRef() {return Increment(&m_dwRef);}
	virtual long ReleaseRef()
	{
		LONG l = Decrement(&m_dwRef);
		if (l == 0)
		{
			delete this;
		}
		return l;
	}
};


template <class T,DWORD Timeout=INFINITE>
class CStackLockWrapper
{
	public:
		CStackLockWrapper(T& _t) : t(_t)
		{
			t.Lock(Timeout);
		}
		~CStackLockWrapper()
		{
			t.Unlock();
		}
		T &t;
};


template <class T,DWORD Timeout=INFINITE>
class CStackLockWrapper2
{
public:
	CStackLockWrapper2(T* _t) : t(_t)
	{
		t->Lock(Timeout);
	}
	~CStackLockWrapper2()
	{
		t->Unlock();
	}
	T *t;
};

typedef CStackLockWrapper<CAutoCriticalSection> CStackAutoCSLock;


//help macro:
#define	DECLARE_AUTOLOCK_CS(name)				private : CAutoCriticalSection	m_cs ## name
#define AUTOLOCK_CS(name)						CStackAutoCSLock lock(m_cs ## name)
#define AUTOLOCK_CS_LOCK(name)					m_cs ## name.Lock()		
#define AUTOLOCK_CS_UNLOCK(name)				m_cs ## name.Unlock()

} //namespace msdk
#endif // _CRITICALSECTION_INCLUDE_H_
