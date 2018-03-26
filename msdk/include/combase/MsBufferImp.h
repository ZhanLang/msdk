#pragma once
#include <combase\IMsBuffer.h>
#include "SyncObject\criticalsection.h"

#define DEFAULT_BUFFER_SIZE			MAX_PATH
#define DEFAULT_BLOCK_SIZE			1024	   //每次分配的内存块大小


template<typename T>
struct sysbufmgr
{
	inline T* Alloc(DWORD _size)
	{
		if (!_size)
		{
			_size = 1;
		}

		T* p = (T*)malloc(_size *sizeof(T));
		if (p)
		{
			memset(p,0,_size);
		}
		return p;
	}

	inline void  Free(T * _chuck , DWORD _size)
	{
		if (_chuck)
		{
			free(_chuck );
			_chuck = NULL;
		}
	}

	inline T* ReAlloc(T* _chuck, DWORD _size)
	{
		if (_chuck)
		{
			T* p =  (T*)realloc(_chuck, _size * sizeof(T));
			//ZeroMemory(p,_size * sizeof(T));
			return p;
		}

		return NULL;
	}
};

template<typename LockType = CNullCriticalSection ,typename T2 = sysbufmgr<BYTE>>
class CMsBufferImp:
	public IMsBufferEx,private CUnknownImp
{
public:

	UNKNOWN_IMP2(IMsBuffer,IMsBufferEx);

	typedef CStackLockWrapper<LockType> SRVATUOLOCK;

	CMsBufferImp()
	{
		m_dwBufSize = 0;
		m_dwDataSize = 0;
		m_lpByteBuf = NULL;
		Reserve();
	}

	virtual ~CMsBufferImp(void)
	{
		if(m_lpByteBuf)
		{
			m_bufMgr.Free(m_lpByteBuf , m_dwBufSize);
			m_lpByteBuf = NULL;
		}
	}



	STDMETHOD(GetRawBuffer)(LPBYTE* lppoutData,DWORD* dwBufSize)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(lppoutData != NULL &&  dwBufSize != NULL,E_INVALIDARG);

		*lppoutData = m_lpByteBuf;
		*dwBufSize = m_dwDataSize;
		return S_OK;
	}

	STDMETHOD(SetBuffer)(LPBYTE lpBuf,DWORD dwSize,DWORD dwStartIndex = 0)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(m_lpByteBuf,E_FAIL);
		RASSERT(lpBuf != NULL,E_INVALIDARG);
		RASSERT(dwSize>0,S_OK);
		RASSERT(dwStartIndex <= m_dwDataSize,E_INVALIDARG);


		if (dwSize + dwStartIndex <=m_dwBufSize)
		{	
			CopyMemory(m_lpByteBuf+dwStartIndex,lpBuf,dwSize);
		}
		else
		{
			
			DWORD dwTempBufLen = dwSize +dwStartIndex + DEFAULT_BLOCK_SIZE;
			m_lpByteBuf = m_bufMgr.ReAlloc(m_lpByteBuf, dwTempBufLen);
			
			LPBYTE point = m_lpByteBuf+dwStartIndex;
			memcpy(point, lpBuf, dwSize);
			//CopyMemory(point,lpBuf,dwSize);
			m_dwBufSize = dwTempBufLen;

			/*
			DWORD dwTempBufLen = dwSize +dwStartIndex;
			BYTE* lpTempBuf = m_bufMgr.Alloc(dwTempBufLen);
			if (lpTempBuf)
			{
				CopyMemory(lpTempBuf,m_lpByteBuf,m_dwDataSize);
				m_bufMgr.Free(m_lpByteBuf , m_dwBufSize);
				m_lpByteBuf = lpTempBuf;
				CopyMemory(m_lpByteBuf+dwStartIndex,lpBuf,dwSize);
				m_dwBufSize = dwTempBufLen;
			}
			*/
		}

		m_dwDataSize = (dwSize+dwStartIndex);

		return S_OK;
	}

	STDMETHOD(GetBuffer)(LPBYTE lpoutData, DWORD dwSize,DWORD* pDwReadCount,DWORD dwStartIndex = 0)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(m_lpByteBuf,E_FAIL);
		RASSERT(lpoutData != NULL &&  pDwReadCount != NULL,E_INVALIDARG);
		RASSERT(dwStartIndex <= m_dwDataSize,E_INVALIDARG);
		DWORD dwTempSize = min(dwSize,m_dwDataSize-dwStartIndex);
		CopyMemory(lpoutData,m_lpByteBuf+dwStartIndex,dwTempSize);
		*pDwReadCount = dwTempSize;
		return S_OK;
	}

	STDMETHOD(Reserve)(DWORD dwDesiredSize = 0)
	{
		dwDesiredSize = dwDesiredSize ? dwDesiredSize : DEFAULT_BUFFER_SIZE;

		SRVATUOLOCK lock(m_lock);
		if (dwDesiredSize <= m_dwBufSize )
		{
			return S_OK;
		}

		if (m_lpByteBuf == NULL)
		{
			m_lpByteBuf = m_bufMgr.Alloc(dwDesiredSize);
			RASSERT(m_lpByteBuf,E_FAIL);
		}
		else 
		{
			m_lpByteBuf = m_bufMgr.ReAlloc(m_lpByteBuf, dwDesiredSize);
			/*
			if (!dwDesiredSize)
			{
				dwDesiredSize = DEFAULT_BUFFER_SIZE;
			}

			BYTE* lpTempBuf = m_bufMgr.Alloc(dwDesiredSize);
			if (lpTempBuf)
			{
				if (m_dwDataSize)
				{
					CopyMemory(lpTempBuf,m_lpByteBuf,m_dwDataSize);
				}

				m_bufMgr.Free(m_lpByteBuf , m_dwBufSize);
				m_lpByteBuf = lpTempBuf;

			}
			*/
		}

		m_dwBufSize = dwDesiredSize;

		return S_OK;
	}

	STDMETHOD_(DWORD,GetReservedSize)()
	{
		SRVATUOLOCK lock(m_lock);
		return m_dwBufSize;
	}

	STDMETHOD_(DWORD,GetBufferSize)()
	{
		SRVATUOLOCK lock(m_lock);
		return m_dwDataSize;
	}

	STDMETHOD(Clone)(IMsBuffer** lpIBuufer) 
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(lpIBuufer != NULL,E_INVALIDARG);

		*lpIBuufer = NULL;
		UTIL::sentry<CMsBufferImp*> p(new CMsBufferImp);
		RASSERT(p, E_UNEXPECTED);
		p->Reserve(m_dwBufSize);
		LPBYTE lppoutData = NULL;
		DWORD  dwDataSize = 0;
		RFAILED(GetRawBuffer(&lppoutData,&dwDataSize));
		p->SetBuffer(lppoutData,dwDataSize,0);
		((IUnknown*)p)->AddRef(); // nondelegation, protect reference count
		HRESULT hr = ((IUnknown*)p)->QueryInterface(re_uuidof(IMsBuffer), (void **)lpIBuufer);
		((IUnknown*)p.detach())->Release(); // nondelegation, balance reference count or destroy.
		return hr;
	}

	STDMETHOD(AddTail)(LPBYTE lpBuf,DWORD dwSize)
	{
		return SetBuffer(lpBuf,dwSize,m_dwDataSize);
	}

	STDMETHOD(AddHead)(LPBYTE lpBuf,DWORD dwSize)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(m_lpByteBuf,E_FAIL);
		RASSERT(lpBuf != NULL,E_INVALIDARG);

		if (dwSize + m_dwDataSize <=m_dwBufSize)
		{	
			CopyMemory(m_lpByteBuf+dwSize,m_lpByteBuf,m_dwDataSize);
			CopyMemory(m_lpByteBuf,lpBuf,dwSize);	
		}
		else
		{
			DWORD dwTempBufLen = dwSize + m_dwDataSize + DEFAULT_BLOCK_SIZE;
			m_lpByteBuf = m_bufMgr.ReAlloc(m_lpByteBuf, dwTempBufLen);
			CopyMemory(m_lpByteBuf+dwSize,m_lpByteBuf,m_dwDataSize);
			CopyMemory(m_lpByteBuf,lpBuf,dwSize);

			m_dwBufSize = dwTempBufLen;

			/*
			DWORD dwTempBufLen = dwSize + m_dwDataSize;

			BYTE* lpTempBuf = m_bufMgr.Alloc(dwTempBufLen);
			if (lpTempBuf)
			{	
				CopyMemory(lpTempBuf+dwSize,m_lpByteBuf,m_dwDataSize);
				CopyMemory(lpTempBuf,lpBuf,dwSize);
				m_bufMgr.Free(m_lpByteBuf , m_dwBufSize);
				m_lpByteBuf = lpTempBuf;

				m_dwBufSize = dwTempBufLen;
			}
			*/
		}

		m_dwDataSize += dwSize;

		return S_OK;
	}
	STDMETHOD_(const LPBYTE, GetData)()
	{
		return m_lpByteBuf;
	}

	STDMETHOD(SetDataSize)(DWORD dwDataSize)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(dwDataSize<=m_dwBufSize,E_FAIL);
		m_dwDataSize = dwDataSize;
		return S_OK;
	}

	STDMETHOD(Clear)()
	{
		m_dwDataSize = 0;
		return S_OK;
	}

	STDMETHOD(ClearAndFree)()
	{
		m_dwDataSize = 0;
		m_bufMgr.Free(m_lpByteBuf , m_dwBufSize);
		m_lpByteBuf = NULL;

		return Reserve();
	}

private:
	LPBYTE m_lpByteBuf;		//内存体
	DWORD  m_dwBufSize;		//内存的大小
	DWORD  m_dwDataSize;	//数据的大小
	T2 m_bufMgr;
	LockType m_lock;
};
