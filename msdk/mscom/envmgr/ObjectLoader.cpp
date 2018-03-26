#include "StdAfx.h"
#include "ObjectLoader.h"
#include "mscom\mscom_ex.h"
#include "xml/rapidtree_s.h"
#include <algorithm>
CObjectLoader::CObjectLoader(void)
{
	AddRef();
}

CObjectLoader::~CObjectLoader(void)
{
	Uninit();
}

STDMETHODIMP CObjectLoader::Init(void*)
{
	return S_OK;
}

STDMETHODIMP CObjectLoader::Uninit()
{
	AUTOLOCK_CS(classmap);
	m_classMap.clear();
	m_ModuleMgr.Uninit();
	return S_OK;
}


STDMETHODIMP CObjectLoader::CreateInstance(IMSBase* prot, const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv)
{
	AUTOLOCK_CS(classmap);
	ClassMap::iterator it = m_classMap.find(rclsid);
	RASSERT(it != m_classMap.end(),E_NOTIMPL);
	LPCTSTR lpszModuleName = it->second.strModuleName;
	return m_ModuleMgr.CreateInstance(lpszModuleName,rclsid,prot,punkOuter,riid,ppv);
}


STDMETHODIMP CObjectLoader::Register(const CLSID& rclsid, LPCWSTR filename)
{
	USES_GUIDCONVERSION;
	return AddClsid(filename,GUID2S(rclsid),NULL,NULL);
}


STDMETHODIMP CObjectLoader::Revoke(const CLSID& rclsid)
{
	AUTOLOCK_CS(classmap);
	ClassMap::iterator it = m_classMap.find(rclsid);
	RASSERT(it != m_classMap.end() , E_NOTIMPL);
	m_classMap.erase(it);

	return S_OK;
}

HRESULT CObjectLoader::AddClsid(LPCTSTR lpszModule,LPCTSTR lpszClsid,LPCTSTR lpszProgid /* = NULL */,LPCTSTR lpszName /* = NULL */)
{
	RASSERT(lpszModule && lpszClsid , E_INVALIDARG);
	CString strClsid = lpszClsid;
	strClsid.Replace(_T("\""),_T(""));

	CLSID clsid = S2GUID(strClsid.GetBuffer(),TRUE);
	AUTOLOCK_CS(classmap);
	if(m_classMap.find(clsid) != m_classMap.end(),S_OK)
	{
		GrpError(GroupName, MsgLevel_Error, _T("类对象[%s][%s]以存在"),lpszClsid,lpszModule);
		return S_OK;
	}

	RFAILED(m_ModuleMgr.InsertModule(lpszModule,NULL));
	ClassInfo info={lpszModule,lpszProgid?lpszProgid:_T(""),lpszName?lpszName:_T("")};
	m_classMap.insert(ClassMap::value_type(clsid,info));

	return S_OK;
}