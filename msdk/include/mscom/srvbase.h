#pragma once
#include <string>
#include <msapi/mspath.h>
#include "prophelpers.h"


namespace msdk{;

//注意释放 m_pRot
template<typename _Base>
class CMsComBase:
	public virtual CUnknownImp
{
public:
	CMsComBase()
	{
		ZeroMemory(m_strCurrentModulePath , sizeof(m_strCurrentModulePath));
		ZeroMemory(m_strCurrentModuleName , sizeof(m_strCurrentModuleName));
		ZeroMemory(m_strCurrentPath , sizeof(m_strCurrentPath));
		m_hPorcInstance = NULL;
	}

	virtual ~CMsComBase(){SAFE_RELEASE(m_pRot);}

	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter)
	{
		RASSERT(pRot , E_INVALIDARG);
		m_pRot = pRot;
		return S_OK;
	}

	IMscomRunningObjectTable* GetRot()
	{
		return m_pRot;
	}


	LPCWSTR GetEnvParamString(LPCSTR lpszName) const
	{
		RASSERT(m_pRot, L"");
		UTIL::com_ptr<IMsEnv> pMsEnv;
		RFAILEDP(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv) , (void**)&pMsEnv), L"");
		RASSERT(pMsEnv ,  L"");


		UTIL::com_ptr<IPropertyStr> pProxyStr;
		pMsEnv->GetEnvParam((IMSBase**)&pProxyStr.m_p);
		RASSERT(pProxyStr, L"");


		CPropStrSet propSet(pProxyStr);
		LPCWSTR lpszNameSpace = propSet[lpszName];
		return lpszNameSpace ? lpszNameSpace : L"";
	}

	INT GetEnvParam(LPCSTR lpszName)
	{
		RASSERT(m_pRot, 0);
		UTIL::com_ptr<IMsEnv> pMsEnv;
		RFAILEDP(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv) , (void**)&pMsEnv), 0);
		RASSERT(pMsEnv ,  0);


		UTIL::com_ptr<IPropertyStr> pProxyStr;
		pMsEnv->GetEnvParam((IMSBase**)&pProxyStr.m_p);
		RASSERT(pProxyStr, 0);


		CPropStrSet propSet(pProxyStr);
		switch( propSet[lpszName].Val().vt )
		{
		case VT_LPSTR:
			return atoi(propSet[lpszName]);
		case VT_LPWSTR:
			return _wtoi( propSet[lpszName] );
		}
		return propSet[lpszName];
	}

	LPCTSTR GetCurrentModuleName()
	{
		if (lstrlen(m_strCurrentModulePath) == 0)
		{
			GetModuleFileName(g_hinstance , m_strCurrentModuleName , SIZEOF_BUFFER(m_strCurrentModuleName));
		}
		
		return m_strCurrentModuleName;
	}

	

	LPCTSTR GetCurrentModulePath()
	{
		if (lstrlen(m_strCurrentModulePath) == 0)
		{
			RASSERT(g_hinstance , m_strCurrentModulePath);
			msapi::GetModulePath(g_hinstance , m_strCurrentModulePath , SIZEOF_BUFFER(m_strCurrentModulePath));
		}
		
		return m_strCurrentModulePath;
	}

	LPCTSTR GetCurrentPath()
	{
		RASSERT(m_pRot , NULL);
		UTIL::com_ptr<IMsEnv> pEnv;
		RFAILEDP(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv),(void**)&pEnv) , NULL);
		RASSERT(pEnv , NULL);

		_tcscpy_s(m_strCurrentPath, MAX_PATH, pEnv->GetWorkPath());
		
		if (!m_strCurrentPath[0])
		{
			msapi::GetModulePath(GetPorcessInstance() , m_strCurrentPath , SIZEOF_BUFFER(m_strCurrentPath));
		}

		return m_strCurrentPath;
	}

	//获取当前进程句柄
	HINSTANCE GetPorcessInstance()
	{
		RASSERT(!m_hPorcInstance , m_hPorcInstance);
		
		RASSERT(m_pRot , NULL);
		UTIL::com_ptr<IMsEnv> pEnv;
		RFAILEDP(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv),(void**)&pEnv) , NULL);
		RASSERT(pEnv , NULL);

		m_hPorcInstance = pEnv->GetInstance();
		return m_hPorcInstance;
	}

	//获取当前模块句柄
	HINSTANCE GetModuleInstance()
	{
		return g_hinstance;
	}

	HRESULT CreateInstance(const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv)
	{
		RASSERT(m_pRot , E_FAIL);
		return m_pRot->CreateInstance(rclsid , punkOuter , riid , ppv);
	}

	HRESULT CreateBuffer(IMsBuffer** pBuf , DWORD dwSize = 0 , BOOL bSafe = FALSE)
	{
		RASSERT(m_pRot , E_FAIL);
		UTIL::com_ptr<IMsBufferMgr> pBufMgr;
		RFAILED(m_pRot->GetObject(CLSID_MsBufferMgr , re_uuidof(IMsBufferMgr) , (void**)&pBufMgr));
		RASSERT(pBufMgr , E_FAIL);

		return bSafe ? pBufMgr->CreateSafeBuf(pBuf , dwSize) : pBufMgr->CreateMsBuf(pBuf , dwSize);
	}

	HRESULT CreateBuffer(IMsBuffer** pBuf, LPCWSTR lpszBuf)
	{
		RASSERT(lpszBuf && wcslen(lpszBuf), E_FAIL);
		UTIL::com_ptr<IMsBufferMgr> pBufMgr;
		RFAILED(m_pRot->GetObject(CLSID_MsBufferMgr , re_uuidof(IMsBufferMgr) , (void**)&pBufMgr));
		RASSERT(pBufMgr , E_FAIL);

		DWORD dwSize = (wcslen(lpszBuf) + 1) * sizeof(wchar_t);
		HRESULT hRet = bSafe ? pBufMgr->CreateSafeBuf(pBuf , dwSize) : pBufMgr->CreateMsBuf(pBuf , dwSize);
		RFAILED(hRet);

		return (*pBuf)->SetBuffer((LPBYTE)lpszBuf, dwSize)
	}

	HRESULT CreateBuffer(IMsBuffer** pBuf, LPCSTR lpszBuf)
	{
		RASSERT(lpszBuf && wcslen(lpszBuf), E_FAIL);
		UTIL::com_ptr<IMsBufferMgr> pBufMgr;
		RFAILED(m_pRot->GetObject(CLSID_MsBufferMgr , re_uuidof(IMsBufferMgr) , (void**)&pBufMgr));
		RASSERT(pBufMgr , E_FAIL);

		DWORD dwSize = (wcslen(lpszBuf) + 1);
		HRESULT hRet = bSafe ? pBufMgr->CreateSafeBuf(pBuf , dwSize) : pBufMgr->CreateMsBuf(pBuf , dwSize);
		RFAILED(hRet);

		return (*pBuf)->SetBuffer((LPBYTE)lpszBuf, dwSize)
	}

	HRESULT CreateStringW(IStringW** pStrW , BOOL bSafe = FALSE)
	{
		RASSERT(m_pRot , E_FAIL);
		UTIL::com_ptr<IStringMgr> pStrMgr;
		RFAILED(m_pRot->GetObject(CLSID_StringMgr , re_uuidof(IStringMgr) , (void**)&pStrMgr));
		RASSERT(pStrMgr , E_FAIL);

		return bSafe ? pStrMgr->CreateStringW_Safe(pStrW) : pStrMgr->CreateStringW(pStrW);
	}

	HRESULT CreateStringA(IStringA** pStrA , BOOL bSafe = FALSE)
	{
		RASSERT(m_pRot , E_FAIL);
		UTIL::com_ptr<IStringMgr> pStrMgr;
		RFAILED(m_pRot->GetObject(CLSID_StringMgr , re_uuidof(IStringMgr) , (void**)&pStrMgr));
		RASSERT(pStrMgr , E_FAIL);

		return bSafe ? pStrMgr->CreateStringA_Safe(pStrA) : pStrMgr->CreateStringA(pStrA)
	}


protected:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
	HINSTANCE m_hPorcInstance;
	TCHAR m_strCurrentPath[MAX_PATH];
	TCHAR m_strCurrentModulePath[MAX_PATH];
	TCHAR m_strCurrentModuleName[MAX_PATH];
};

}//namespace msdk
