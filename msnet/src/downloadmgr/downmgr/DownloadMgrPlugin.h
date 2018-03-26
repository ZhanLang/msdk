#pragma once
#include "download\downloadmgr_i.h"

class CDownloadMgrPlugin :
	public IDownloadMgrPlugin,
	public IMsPlugin,
	public IMsPluginRun,
	public CUnknownImp
{
public:
	UNKNOWN_IMP3_(IDownloadMgrPlugin,IMsPlugin,IMsPluginRun);
	HRESULT init_class(IMSBase* pRot , IMSBase* pOuter)
	{
		RASSERT(pRot,E_INVALIDARG);
		m_pRot = pRot;
		return S_OK;
	}

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();
	IDownLoadMgr* GetDownLoadMgr();

	CDownloadMgrPlugin(void);
	~CDownloadMgrPlugin(void);
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
	IDownLoadMgr*				            m_pdlMgr;
};
