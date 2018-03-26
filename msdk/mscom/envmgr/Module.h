#pragma once
#include "interface.h"

class XModule : public IModule , public CMSComLoader,public CUnknownImp
{
private:
	GUID m_guid;
public:
	UNKNOWN_IMP1(IModule );
	void SetGuid(REFGUID guid);
	REFGUID GetGuid();

	void SetPath(LPCTSTR szPath);
	virtual LPCTSTR GetPath();

	STDMETHOD(CreateInstance)(IMSBase* prot, const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv);
public:
	XModule(void);
	~XModule(void);
};
