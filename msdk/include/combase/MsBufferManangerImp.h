#pragma once
#include <combase\IMsBuffer.h>
#include "MsBufferImp.h"


class CMsBufferManangerImp :
	public IMsBufferMgr,
	private CUnknownImp
{
public:
	UNKNOWN_IMP1(IMsBufferMgr);

	STDMETHOD(init_class)(IUnknown *prot, IUnknown *punkOuter)
	{
		return S_OK;
	}

	
	STDMETHOD(CreateMsBuf)(IMsBuffer** lpbuf,DWORD dwDesiredSize)
	{
		return CreateBuf<CNullCriticalSection>(lpbuf , dwDesiredSize);
	}

	STDMETHOD(CreateSafeBuf)(IMsBuffer** lpbuf,DWORD dwDesiredSize)
	{
		return CreateBuf<CAutoCriticalSection>(lpbuf , dwDesiredSize);
	}

private:
	template<typename T1>
	HRESULT CreateBuf(IMsBuffer** lpbuf,DWORD dwDesiredSize)
	{
		RASSERT(lpbuf != NULL,E_INVALIDARG);
		DWORD dwTempSize = dwDesiredSize;
		if (dwDesiredSize <= DEFAULT_BUFFER_SIZE)
		{
			dwTempSize = DEFAULT_BUFFER_SIZE;
		}

		//为了保证传引用计数器的一致
		if ( *lpbuf )
		{
			(*lpbuf)->Release();
		}

		*lpbuf = NULL;
 		UTIL::com_ptr<IMsBuffer> pBuf(new CMsBufferImp<T1>());
		RASSERTP(pBuf , E_FAIL);
		HRESULT hr = pBuf->Reserve(dwTempSize);
		if (SUCCEEDED(hr))
		{
			hr = pBuf->QueryInterface(re_uuidof(IMsBuffer) , (void**)lpbuf);
		}

		return hr;
	}
};
