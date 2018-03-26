#pragma once
#include <map>
#include "SyncObject\criticalsection.h"


struct strless_t
{
	bool operator() (const CString& left, const CString& right) const
	{
		return left.CompareNoCase(right) < 0;
	}
};

typedef std::map<CString,CMSComLoader*,strless_t> CModuleMap;
class CModuleMgr
	:public IModuleMgr
{
public:
	CModuleMgr(void);
	~CModuleMgr(void);
	HRESULT Uninit();
	HRESULT InsertModule(LPCTSTR lpszPath,CMSComLoader* pLoader = NULL);
	HRESULT CreateInstance( LPCTSTR lpszModule,REFCLSID rclsid, IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv);
	HRESULT UnloadNow(LPCTSTR lpszModule);
private:
	DECLARE_AUTOLOCK_CS(modulemgr);
	CModuleMap m_moduleMgr;
};
