#pragma once
//
#include "mscom/msplugin.h"
namespace msdk{
namespace mscom{

//这里面主要实现插件的初始化部分，其实就是为懒人准备的，还有就是将代码架构化
class CPluginBaseImp : public IMsPlugin , public IMsPluginRun , public CUnknownImp
{
public: 
	UNKNOWN_IMP2_(IMsPlugin , IMsPluginRun);
public:
	CMSComPtr<IMscomRunningObjectTable> m_pRot;
	CMSComPtr<IMsBufferMgr> m_pBufMgr;
	CMSComPtr<IMsObjectLoader> m_pLoader;
public:
	virtual HRESULT OnAfterInit() {return S_OK;}
	virtual HRESULT OnAfterUninit(){return S_OK;}
	virtual HRESULT OnAfterStart(){return S_OK;}
	virtual HRESULT OnAfterStop(){return S_OK;}
public:
	
	HRESULT init_class(IUnknown* prot, IUnknown* punkOuter) {
		RASSERTP(prot , E_INVALIDARG);
		m_pRot = prot;
		RASSERTP(m_pRot , E_INVALIDARG);
		return S_OK;
	}
	
	IMsObjectLoader* GetObjectLoader(	) {
		return m_pLoader;
	}

	IMscomRunningObjectTable* GetRot(	) {
		return m_pRot;
	}


	IMsBuffer* CreateBuffer(DWORD dwBufSize) {
		if (m_pBufMgr) {
			CMSComPtr<IMsBuffer> pBuf = INULL;
			m_pBufMgr->CreateMsBuf(&pBuf.m_p , dwBufSize);
			return pBuf.detach();
		}
		return NULL;
	}

	//IMsPlugin
	STDMETHOD(Init)(void*	) {
		m_pRot->GetObject(CLSID_ObjectLoader , re_uuidof(IMsObjectLoader) , (void**)&m_pLoader);
		HRESULT hr = m_pRot->GetObject(CLSID_MsBufferMgr , re_uuidof(IMsBufferMgr) , (void **)&m_pBufMgr.m_p);

		if (FAILED(hr) && m_pLoader) {
			hr = m_pLoader->CreateInstance(m_pRot , CLSID_MsBufferMgr , NULL , re_uuidof(IMsBufferMgr) , (void**)&m_pBufMgr.m_p);
			if (SUCCEEDED(hr)) {
				m_pRot->Register(CLSID_MsBufferMgr , m_pBufMgr);
			}
		}
		return OnAfterInit();
	}

	STDMETHOD(Uninit)(		) {
		m_pBufMgr = INULL;
		m_pRot    = INULL;
		m_pLoader = INULL;
		return OnAfterUninit();
	}

	//IMsPluginRun
	STDMETHOD(Start)(		)  {
		return OnAfterStart();
	}

	STDMETHOD(Stop)(		) {
		return OnAfterStop();
	}

};

}
}