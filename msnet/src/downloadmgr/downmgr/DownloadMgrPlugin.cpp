#include "StdAfx.h"
#include "DownloadMgrPlugin.h"
#include <envinfo_i.h>

#define EnvInfo_DownloadMgrNamespace _T("dlmgr")
CDownloadMgrPlugin::CDownloadMgrPlugin(void)
{
}

CDownloadMgrPlugin::~CDownloadMgrPlugin(void)
{
	
}

STDMETHODIMP CDownloadMgrPlugin::Init(void*)
{
	UTIL::com_ptr<IQueryEnvInfo> pEnvInfo;
	RFAILED(m_pRot->GetObject(CLSID_EnvInfo,re_uuidof(IQueryEnvInfo),(void**)&pEnvInfo));
	RASSERT(pEnvInfo,E_FAIL);
	
	UTIL::com_ptr<IStringW> pDlNameSpace;
	pEnvInfo->QueryEnvInfo(EnvInfo_DownloadMgrNamespace,&pDlNameSpace.m_p);

	m_pdlMgr = CreateDownLoadMgr(NULL,pDlNameSpace->GetBuffer(),FALSE);

	RASSERT(m_pdlMgr,E_FAIL);
	return S_OK;
}

STDMETHODIMP CDownloadMgrPlugin::Uninit()
{
	if (m_pdlMgr)
	{
		DistoryDownLoadMgr(&m_pdlMgr);
		m_pdlMgr = NULL;
	}

	m_pRot = INULL;
	return S_OK;
}

STDMETHODIMP CDownloadMgrPlugin::Start()
{
	return S_OK;
}

STDMETHODIMP CDownloadMgrPlugin::Stop()
{
	if (m_pdlMgr)
	{
		m_pdlMgr->Stop(AnyUrlHash);
	}

	return S_OK;
}

IDownLoadMgr* CDownloadMgrPlugin::GetDownLoadMgr()
{
	return m_pdlMgr;
}