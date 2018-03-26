#include "StdAfx.h"
#include "MsMsc.h"
#include "MSC.h"
#include "XMSCProxy.h"
#include "MSCManager.h"
#include "MsXMSCProxy.h"

CMsMsc::CMsMsc(void)
{
	m_pRsBufferManager =(IUnknown*)0;
	m_pRuningTable = (IUnknown*)0;
    m_pPipeName = NULL;//= defaultPipeName;
}

CMsMsc::~CMsMsc(void)
{
	//Uninit();
    FreeName();
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CMsMsc::init_class(IUnknown* prot, IUnknown* punkOuter)
{
	RASSERT(prot, E_INVALIDARG);
	//取出根对象
	//
	m_pRuningTable = prot;
	RASSERT(m_pRuningTable,E_UNEXPECTED);
	RFAILED(m_pRuningTable->GetObject(CLSID_MsBufferMgr,__uuidof(IMsBufferMgr), (void**)&m_pRsBufferManager));
	
	return S_OK;
}

HRESULT CMsMsc::SetPipeName(const TCHAR *szName,DWORD dwMax)
{
    FreeName();

	if (szName && dwMax)
	{
		DWORD dwLen = dwMax+1;
		m_pPipeName = new TCHAR[dwLen];
		ZeroMemory(m_pPipeName , dwLen*sizeof(TCHAR));
		_tcscpy_s(m_pPipeName,dwLen ,szName);
	}
  
    return S_OK;
}

HRESULT CMsMsc::Init(void* lpVoid)
{
	CMSCManager* lpMSCManager = CMSCManager::GetInstance();
	if (NULL == lpMSCManager)
	{
		return E_FAIL;
	}
	if(lpMSCManager->Initialize(m_pPipeName) == TRUE)
		return S_OK;
	return E_FAIL;

}

/////////////////////////////////////////////////////////////////////////////
HRESULT CMsMsc::Uninit()
{
	CMSCManager::GetInstance()->Uninitialize();
	CMSCManager::DestroyInstance();
    FreeName();

	SAFE_RELEASE(m_pRuningTable);
	SAFE_RELEASE(m_pRsBufferManager);
	SAFE_RELEASE(m_pObjLoader);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CMsMsc::Start()
{
	if(TRUE== CMSCManager::GetInstance()->Start())
		return S_OK;
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CMsMsc::Stop()
{
	CMSCManager::GetInstance()->Stop();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsMsc::CreateProxy(IXMsgBusProxy** lppXMSCProxy)
{
	if (NULL == lppXMSCProxy)
	{
		return FALSE;
	}

	CMsXMSCProxy* prot = new CMsXMSCProxy(m_pRsBufferManager);
	if (NULL == *lppXMSCProxy)
	{
		return FALSE;
	}
	
	this->QueryInterface(re_uuidof(IXMsgBusProxy) , (void**)lppXMSCProxy);
	//(*lppXMSCProxy)->AddRef();
	return TRUE;
}

void CMsMsc::FreeName()
{
    if(NULL  != m_pPipeName)
    {
       SAFE_DELETE_BUFFER(m_pPipeName);
    }
}