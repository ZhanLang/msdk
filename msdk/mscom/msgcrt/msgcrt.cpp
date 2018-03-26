// msgcrt.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "msgcrt.h"

CMsgCenter::CMsgCenter()
{

}

CMsgCenter::~CMsgCenter()
{
	Uninit();
}

STDMETHODIMP CMsgCenter::Init(void*)
{
	RFAILED(GetMscNameSpace());
	RFAILED(CreateMsc());
	RFAILED(m_pMsc->SetPipeName(m_strNameSpace , m_strNameSpace.GetLength()));
	RFAILED(m_pMsc->Init(NULL));

	return S_OK;
}

STDMETHODIMP CMsgCenter::Uninit()
{
	if (m_pMsc)
	{
		m_pMsc->Uninit();
	}

	SAFE_RELEASE(m_pRot);
	SAFE_RELEASE(m_pMsc);

	return S_OK;
}

STDMETHODIMP CMsgCenter::Start()
{
	UTIL::com_ptr<IMsPluginRun> pRun = m_pMsc;
	RASSERT(pRun , E_FAIL);
	RFAILED(pRun->Start());

	return S_OK;
}

STDMETHODIMP CMsgCenter::Stop()
{
	UTIL::com_ptr<IMsPluginRun> pRun = m_pMsc;
	RASSERT(pRun , E_FAIL);
	pRun->Stop();
	return S_OK;
}

HRESULT CMsgCenter::CreateMsc()
{
	RASSERT(m_pRot , E_FAIL);
	RFAILED(m_pRot->CreateInstance(CLSID_MsgBus , NULL , re_uuidof(IMsgBus) , (void**)&m_pMsc));
	RASSERT(m_pMsc , E_FAIL);
	return S_OK;
}

HRESULT CMsgCenter::GetMscNameSpace()
{
	RASSERT(m_pRot , E_FAIL);

	UTIL::com_ptr<IMsEnv> pIMsEnv;
	RFAILED(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv) , (void**)&pIMsEnv));
	RASSERT(pIMsEnv , E_FAIL);

	UTIL::com_ptr<IPropertyStr> pPropStr;
	pIMsEnv->GetEnvParam((IMSBase**)&pPropStr.m_p);
	RASSERT(pPropStr, E_FAIL);

	CPropStrSet propSet(pPropStr);
	m_strNameSpace = (LPWSTR)propSet["msgcrt"];
	RASSERT(m_strNameSpace.GetLength(),E_FAIL);

	return S_OK;
}

STDMETHODIMP CMsgCenter::SetNameSpace(LPCWSTR lpstrName)
{
	RASSERT(lpstrName, E_INVALIDARG);
	m_strNameSpace = lpstrName;

	return S_OK;
}
