#include "StdAfx.h"
#include "DownloadCentre.h"
#include "envinfo_i.h"

#define EnvInfo_DownloadMgrNamespace _T("dlmgr")
CDownloadCentre::CDownloadCentre(void)
{
	m_pdlMgr = NULL;
}

CDownloadCentre::~CDownloadCentre(void)
{
	
}

STDMETHODIMP CDownloadCentre::Init(void*)
{
	UTIL::com_ptr<IQueryEnvInfo> pEnvInfo;
	RFAILED(m_pRot->GetObject(CLSID_EnvInfo,re_uuidof(IQueryEnvInfo),(void**)&pEnvInfo));
	RASSERT(pEnvInfo,E_FAIL);

	UTIL::com_ptr<IStringW> pDlNameSpace;
	pEnvInfo->QueryEnvInfo(EnvInfo_DownloadMgrNamespace,&pDlNameSpace.m_p);
	m_pdlMgr = CreateDownLoadMgr(this,pDlNameSpace->GetBuffer(),FALSE);
	RASSERT(m_pdlMgr,E_FAIL);
	
	AddConnectionPoint(re_uuidof(IDownLoadNotifyConnectPoint),m_NotifyPointContainer);
	return S_OK;
}

STDMETHODIMP CDownloadCentre::Uninit()
{
	DelConnectionPoint(re_uuidof(IDownLoadNotifyConnectPoint),m_NotifyPointContainer);
	m_NotifyPointContainer.EmptyConnection();
	if (m_pdlMgr)
	{
		DistoryDownLoadMgr(&m_pdlMgr);
		m_pdlMgr = NULL;
	}

	SAFE_RELEASE(m_pRot);

	return S_OK;
}


//IDownLoadNotify
VOID CDownloadCentre::OnDownLaodNotify(const sha1_hash& hash , NotifyMessageID msgid ,WPARAM wp, LPARAM lp)
{
	CallConnectPointFunc(m_NotifyPointContainer,IDownLoadNotifyConnectPoint,OnDownLaodNotify(hash,msgid,wp,lp));
	//枚举连接点
}

//IDownLoadCentre
HRESULT CDownloadCentre::EnumDownloadTask(IEnumDownLoadTask* pCallBack)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->EnumDownloadTask(pCallBack);
}

const DownLoadTaskInfo* CDownloadCentre::QueryTaskInfo(const sha1_hash& hash)
{
	RASSERT(m_pdlMgr,NULL);
	return m_pdlMgr->QueryTaskInfo(hash);
}

HRESULT CDownloadCentre::CreateDownloadTask(LPCWSTR lpUrl,LPCWSTR lpSavePath, OUT sha1_hash& hash)
{
	RASSERT(m_pdlMgr,E_FAIL);
	HRESULT hr = m_pdlMgr->CreateDownloadTask(lpUrl,lpSavePath,hash);
	if (hr == S_OK)
	{
		CallConnectPointFunc(m_NotifyPointContainer,IDownLoadNotifyConnectPoint,
			OnDownLaodNotifyEx(hash,Msg_CreateTask,NULL,NULL));
	}

	return hr;
}

HRESULT CDownloadCentre::Start(const sha1_hash& urlhash)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->Start(urlhash);
}


HRESULT CDownloadCentre::Stop(const sha1_hash& urlhash, BOOL remove_files /*= FALSE*/)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->Stop(urlhash,remove_files);
}

HRESULT CDownloadCentre::Delete(const sha1_hash& urlhash,BOOL remove_files/*=FALSE*/)
{
	RASSERT(m_pdlMgr,E_FAIL);

	//在删除之前通知一下
	CallConnectPointFunc(m_NotifyPointContainer,IDownLoadNotifyConnectPoint,
		OnDownLaodNotifyEx(urlhash,Msg_RemoveTask,NULL,NULL));
	return m_pdlMgr->Delete(urlhash,remove_files);
}

HRESULT CDownloadCentre::SetOption(/*DownLaodOption*/DWORD option,LPVOID lpdata,DWORD dwLen)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->SetOption(option,lpdata,dwLen);
}

HRESULT CDownloadCentre::GetOption(/*DownLaodOption*/DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->GetOption(option,lpData,dwLen);
}

HRESULT CDownloadCentre::SetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,LPVOID lpData,DWORD dwLen)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->SetTaskOption(hash,option,lpData,dwLen);
}

HRESULT CDownloadCentre::GetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,OUT LPVOID* lpData,OUT DWORD&dwLen)
{
	RASSERT(m_pdlMgr,E_FAIL);
	return m_pdlMgr->GetTaskOption(hash,option,lpData,dwLen);
}