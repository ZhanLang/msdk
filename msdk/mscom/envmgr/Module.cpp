#include "StdAfx.h"
#include "Module.h"

XModule::XModule(void)
{
	m_guid = GUID_NULL;
}

XModule::~XModule(void)
{
	CMSComLoader::UnloadMSCom();
}

REFGUID XModule::GetGuid()
{
	return m_guid;
}

void XModule::SetGuid(REFGUID guid)
{
	m_guid = guid;
}

void XModule::SetPath(LPCTSTR szPath)
{
	CMSComLoader::SetMoudlePath(szPath);
}

LPCTSTR XModule::GetPath()
{
	return CMSComLoader::GetModuleName();
}

STDMETHODIMP XModule::CreateInstance(IMSBase* prot, const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv)
{
	if (!CMSComLoader::IsLoaded() && !CMSComLoader::LoadMSCom())
	{
		//GrpMsg(GroupName , DBG_ERROR , _T("load mscom [module name=%s] faild.") , CMSComLoader::GetModuleName());
		return E_FAIL;
	}

	if (CMSComLoader::IsLoaded())
	{
		return CMSComLoader::CreateInstance(rclsid , prot , punkOuter , riid , ppv);
	}
	return E_FAIL;
}
