#pragma once
#include "IHeapMem.h"
class CHeapMem
	:public IHeapMem
	,private CUnknownImp
{
public:
	UNKNOWN_IMP1(IHeapMem);
	CHeapMem()
	{
		m_hHeap = NULL;
	}

	~CHeapMem()
	{
		DestroyHeap();
	}

	STDMETHOD(CreateHeap)(DWORD dwInitialSize, DWORD dwMaximumSize = 0)
	{
		m_hHeap = HeapCreate(0,dwInitialSize,dwMaximumSize);
		RASSERT(m_hHeap,E_FAIL);
		return S_OK;
	}

	STDMETHOD(DestroyHeap)()
	{
		if (m_hHeap)
		{
			CloseHandle(m_hHeap);
			m_hHeap = NULL;
		}

		return S_OK;
	}

	STDMETHOD_(LPVOID,AllocMemory)(DWORD dwSize, BOOL bInitZero)
	{
		RASSERT(m_hHeap, NULL);
		PVOID pRet = NULL;
		pRet = HeapAlloc(m_hHeap, bInitZero ? HEAP_ZERO_MEMORY : 0,dwSize);
		return pRet;
	}

	STDMETHOD_(LPVOID,ReallocMemory)(PVOID pPoint, DWORD dwSize)
	{
		DWORD dwSrcSize= HeapSize(m_hHeap,0,pPoint);
		RASSERT(dwSrcSize < dwSize, pPoint);
		PVOID pDest= HeapReAlloc(m_hHeap,0,pPoint, dwSize);
		return pDest;
	}

	STDMETHOD(FreeMemory)(LPVOID lpPoint)
	{
		RASSERT(m_hHeap && lpPoint, E_FAIL);
		return HeapFree(m_hHeap, 0, lpPoint) ? S_OK : E_FAIL;
	}

private:
	HANDLE m_hHeap;
};