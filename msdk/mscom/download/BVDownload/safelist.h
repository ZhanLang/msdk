///////////////////////////////////////////////////////////////
//	文 件 名 : SafeArray.h
//	文件功能 : 一个简单的list封装类，是线程安全的
//	作    者 :
//	创建时间 :
//	项目名称 :
//	操作系统 :
//	备    注 :
//	历史记录： :
///////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <list>

namespace msdk {

template<class _Ty>
class _golbal_Alloc
{

public:
	static _golbal_Alloc<_Ty>& GetAlloc()
	{
		static _golbal_Alloc<_Ty> _golAlloc;
		return _golAlloc;
	}

	~_golbal_Alloc()
	{
		UnInit();
	}

	void Init()
	{
		if (NULL == m_hHeapAlloc)
		{
			m_hHeapAlloc = ::HeapCreate(0,4096*4,0);
		}
	}


	void UnInit()
	{
		if (m_hHeapAlloc)
		{
			::HeapDestroy(m_hHeapAlloc);
			m_hHeapAlloc =	NULL;
		}
	}

	void* AllocMem(SIZE_T MemSize)
	{
		return ::HeapAlloc(m_hHeapAlloc,HEAP_ZERO_MEMORY,MemSize);
	}

	void FreeMem(void *pMem)
	{
		//if (m_hHeapAlloc)
		{
			::HeapFree(m_hHeapAlloc,NULL,pMem);
		}

	}

private:
	_golbal_Alloc()
	{
		m_hHeapAlloc = NULL;
		Init();
	}

	HANDLE m_hHeapAlloc;

};


template <class T> class SafeListAllocator
{
public:

	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;
	typedef _golbal_Alloc<T>  g_Alloc_type;

	template <class U>
	struct rebind { typedef SafeListAllocator<U> other; };

	SafeListAllocator() {}

	SafeListAllocator(const SafeListAllocator&) {}

	template <class U>
	SafeListAllocator(const SafeListAllocator<U>&) {}

	~SafeListAllocator() {}

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return x;
	}

	pointer allocate(size_type n, const_pointer = 0)
	{
		void* p = g_Alloc_type::GetAlloc().AllocMem(n*sizeof(T));

		if (!p)
			throw std::bad_alloc();

		return static_cast<pointer>(p);

	}

	void deallocate(pointer p, size_type)
	{
		g_Alloc_type::GetAlloc().FreeMem(p);
	}

	size_type max_size() const
	{
		return static_cast<size_type>(-1) / sizeof(T);

	}

	void construct(pointer p, const value_type& x)
	{
		new(p) value_type(x);

	}

	void destroy(pointer p)
	{
		p->~value_type();
	}

private:
	void operator=(const SafeListAllocator&);

};


template<> class SafeListAllocator<void>
{
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;
	template <class U>
	struct rebind { typedef SafeListAllocator<U> other; };

};

template <class T>
inline bool operator==(const SafeListAllocator<T>&,
					   const SafeListAllocator<T>&)
{
	return true;
};

template <class T>
inline bool operator!=(const SafeListAllocator<T>&, const SafeListAllocator<T>&)
{
	return false;
};

enum MSG_RESULT
{
	RST_STOP = 0,
	RST_TIMEOUT,
	RST_FIREEVENT,
};

template <class _Ty> class CSafeList
{
public:
	typedef std::list<_Ty,SafeListAllocator<_Ty> > SAFELISTIMPL;

	CSafeList()
	{
		InitializeCriticalSection(&this->m_csLock);
		m_hEventHandle = CreateEvent(NULL,FALSE,FALSE,NULL);
		m_bIsStop = FALSE;
	}

	~CSafeList()
	{
		DeleteCriticalSection(&this->m_csLock);
		if(m_hEventHandle)
		{
			CloseHandle(m_hEventHandle);
			m_hEventHandle = NULL;
		}

		m_List.clear();

	}

	static void InitAlloc()
	{
		SafeListAllocator<_Ty>::g_Alloc_type::GetAlloc().Init();
	}

	static void UninitAlloc()
	{
		SafeListAllocator<_Ty>::g_Alloc_type::GetAlloc().UnInit();
	}

	MSG_RESULT GetAllAndDelete(SAFELISTIMPL& myVector, DWORD dwMilliseconds = INFINITE)
	{
		DWORD wait = -1;
		if (dwMilliseconds)
		{
			wait =  WaitForSingleObject(m_hEventHandle, dwMilliseconds);
		}
		
		if (m_bIsStop)
		{
			return RST_STOP;
		}

		if (WAIT_TIMEOUT == wait)
		{
			//time out
			return RST_TIMEOUT;
		}
		else
		{
			CritLock lock(&this->m_csLock);

			//O(1)级别的
			myVector.splice(myVector.end(),m_List);
		}

		return RST_FIREEVENT;

	}


	void clear()
	{
		CritLock lock(&this->m_csLock);
		m_List.clear();
	}

	inline bool empty()
	{
		CritLock lock(&this->m_csLock);
		return m_List.empty();
	}

	inline size_t size()
	{
		CritLock lock(&this->m_csLock);
		return m_List.size();
	}

	void push_back(const _Ty& myT , BOOL bNotify = TRUE)
	{
		{
			CritLock lock(&this->m_csLock);
			m_List.push_back(myT);
		}

		if (bNotify)
		{
			SetEvent(m_hEventHandle);
		}
	}

	void Stop()
	{
		m_bIsStop = true;
		SetEvent(m_hEventHandle);
	}

	bool IsStop()
	{
		return m_bIsStop;
	}

protected:

	class CritLock
	{
	public:
		CritLock(LPCRITICAL_SECTION lpCriticalSection)
			:m_prit(lpCriticalSection)
		{
			EnterCriticalSection(lpCriticalSection);
		}

		~CritLock()
		{
			LeaveCriticalSection(m_prit);
		}

	private:
		LPCRITICAL_SECTION m_prit;
		CritLock& operator=(const CritLock& rhs);
		CritLock();
	};

	CRITICAL_SECTION	m_csLock;
	SAFELISTIMPL		m_List;

	volatile bool m_bIsStop;
	HANDLE m_hEventHandle;

};


template<typename T>
class ThreadMsgQueueImp
{
private:

	typedef CSafeList<T> MSG_QUEUE;

	MSG_QUEUE m_MsgQueue;

public:
	typedef typename MSG_QUEUE::SAFELISTIMPL SAFELISTIMPL;

	void PostThreadMsg(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(!m_MsgQueue.IsStop())
		{
			MSG_INFO info;
			info.msg = msg;
			info.wParam = wParam;
			info.lParam = lParam;
			m_MsgQueue.push_back(info);
		}
	}

	void StopMsgQueue()
	{
		m_MsgQueue.Stop();
	}

	bool MsgQueueIsStop()
	{
		return m_MsgQueue.IsStop();
	}

	void ResetMsgQueue()
	{
		return m_MsgQueue.clear();
	}


	MSG_RESULT GetMsg(SAFELISTIMPL& myVector, DWORD dwMilliseconds = INFINITE)
	{
		return m_MsgQueue.GetAllAndDelete(myVector, dwMilliseconds);
	}

	void InitAlloc(){MSG_QUEUE::InitAlloc();}

	void UninitAlloc(){MSG_QUEUE::UninitAlloc();}
};

typedef struct tag_msg_info
{
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
	struct tag_msg_info()
	{
		msg = -1;
		wParam = lParam = 0;
	}
}MSG_INFO, *PMSG_INFO;

typedef ThreadMsgQueueImp<MSG_INFO> ThreadMsgQueue;

} // namespace msdk