#pragma once

#include<string>
#include <vector>
#include <memory>
#include <map>
#include "SyncObject/criticalsection.h"
#include "ModuleMgr.h"

struct ClassInfo
{
	CString strModuleName;
	CString strProgid;
	CString strName;
};
typedef std::map<CLSID , ClassInfo ,memless<CLSID>> ClassMap;

class CObjectLoader:
	public IMsObjectLoader , 
	public IMsOLManager , 
	CUnknownImp
{
public:
	UNKNOWN_IMP2_(IMsObjectLoader , IMsOLManager)
	HRESULT init_class(IMSBase* prot, IMSBase* punkOuter)
	{
		return S_OK;
	}

public:
	CObjectLoader(void);
	~CObjectLoader(void);
	
 	STDMETHOD(Init)(void*);
 	STDMETHOD(Uninit)();

protected:
	STDMETHOD(CreateInstance)(IMSBase* prot, const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv);
	STDMETHOD(Register)(const CLSID& rclsid, LPCWSTR filename);
	STDMETHOD(Revoke)(const CLSID& rclsid);
	
public:
	 HRESULT AddClsid(LPCTSTR lpszModule,LPCTSTR lpszClsid,LPCTSTR lpszProgid = NULL,LPCTSTR lpszName = NULL);
private:
	ClassMap    m_classMap;
	CModuleMgr  m_ModuleMgr;
private:
	DECLARE_AUTOLOCK_CS(classmap);
};
