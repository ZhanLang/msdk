#include "StdAfx.h"
#include "ModuleMgr.h"

CModuleMgr::CModuleMgr(void)
{
}

CModuleMgr::~CModuleMgr(void)
{

}

HRESULT CModuleMgr::Uninit()
{
	AUTOLOCK_CS(modulemgr);
	CModuleMap::iterator it = m_moduleMgr.begin();
	for (;it != m_moduleMgr.end() ; it++ )
	{
		LPCTSTR lpszModule = it->first;
		HRESULT hRet = UnloadNow(lpszModule);
		if (hRet != S_OK)
		{
			GrpError(GroupName, MsgLevel_Error, _T("模块[%s]卸载失败."),lpszModule);
		}
	}

	m_moduleMgr.clear();
	return S_OK;
}

HRESULT CModuleMgr::CreateInstance(LPCTSTR lpszModule,REFCLSID rclsid, IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv)
{
	RASSERT(lpszModule,E_INVALIDARG);
	AUTOLOCK_CS(modulemgr);
	CModuleMap::iterator it = m_moduleMgr.find(lpszModule);
	if (it == m_moduleMgr.end())
	{
		GrpError(GroupName, MsgLevel_Error, _T("模块不存在，该模块尚未注册到配置文件中:[%s]"),lpszModule);
		return E_NOTIMPL;
	}
	
	CMSComLoader* pLoader = it->second;
	if (!pLoader)
	{
		pLoader = new CMSComLoader();
		pLoader->SetMoudlePath(lpszModule);
		BOOL bRet = pLoader->LoadMSCom();
		if (!bRet)
		{
			GrpError(GroupName, MsgLevel_Error, _T("MSCOM 加载失败 DLL 文件或许不存在，请检查:[%s]"),lpszModule);
			SAFE_DELETE(pLoader);
			return E_FAIL;
		}
		
		it->second = pLoader;
	}

	return pLoader->CreateInstance(rclsid,prot,punkOuter,riid,ppv);
}

HRESULT CModuleMgr::InsertModule(LPCTSTR lpszPath,CMSComLoader* pLoader /* = NULL */)
{
	RASSERT(lpszPath,E_INVALIDARG);
	AUTOLOCK_CS(modulemgr);
	RASSERT(m_moduleMgr.find(lpszPath) == m_moduleMgr.end(),S_OK);

	m_moduleMgr.insert(CModuleMap::value_type(CString(lpszPath),pLoader));
	return S_OK;
}

HRESULT CModuleMgr::UnloadNow(LPCTSTR lpszModule)
{
	AUTOLOCK_CS(modulemgr);
	CModuleMap::iterator it = m_moduleMgr.find(lpszModule);
	RASSERT(it != m_moduleMgr.end(),E_FAIL);
	CMSComLoader* pLoader = it->second;
	RASSERT(pLoader, S_OK);
	pLoader->UnloadMSCom();
	if (pLoader->IsLoaded())
	{
		GrpError(GroupName, MsgLevel_Error,_T("模块[%s]卸载失败"),lpszModule);
		return E_FAIL;
	}

	SAFE_DELETE(pLoader);
	it->second = NULL;
	return S_OK;
}