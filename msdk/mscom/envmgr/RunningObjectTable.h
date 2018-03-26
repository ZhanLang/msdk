#pragma once

#include <map>
using namespace std;
#include "ObjectLoader.h"
#include "xml\rapidtree_s.h"
#include <vector>
#include "SyncObject\criticalsection.h"

struct RotInfo
{
	CLSID   clsid;
	CString strName; //标识符
	BOOL    bMustInit;
	BOOL    bMustStart;
	INT     nStart;
	UTIL::com_ptr<IMSBase> pObj;
	CString strParam;
};

typedef std::map<GUID,RotInfo,memless<GUID>> CRotInfoMap;      //主要用于查找
typedef std::vector<RotInfo>				 CRotInfoVector;   //主要用于启动排序

class CRunningObjectTable :
	public IMscomRunningObjectTable,
	public IMscomRunningObjectTableEx,
	CUnknownImp
{
public:
	UNKNOWN_IMP2_(IMscomRunningObjectTable , IMscomRunningObjectTableEx)
	HRESULT init_class(IMSBase* prot, IMSBase* punkOuter)
	{
		return S_OK;
	}

public:
	CRunningObjectTable(void);
	virtual ~CRunningObjectTable(void);

public :

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	HRESULT AddObject(LPCTSTR lpszClsid,LPCTSTR lpszName, INT nStart, BOOL bMustinit, BOOL bMuststart, LPCTSTR lpszParam, IMSBase* pObj);
	STDMETHOD(Register)(const CLSID& rpid, IMSBase *punk);
	STDMETHOD(Revoke)(const CLSID& rpid);
	STDMETHOD_(REFCLSID ,GetMainPlugin)();
	STDMETHOD(GetObject)(const CLSID& rpid, const IID& iid, void **ppunk);

	HRESULT Init(const CLSID& clsid);
	HRESULT Uninit(const CLSID& clsid);
	HRESULT InitAll();
	HRESULT UninitAll();

	HRESULT Start(const CLSID& clsid);
	HRESULT Stop(const CLSID& clsid);
	HRESULT StartAll();
	HRESULT StopAll();

protected:
	
	STDMETHOD(EnumRunningObjectTable)(IRunningObjectTableEnum* pEnum, IID iid = __uuidof(IMSBase));
	STDMETHOD(CreateInstance)(const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv);
	STDMETHOD_(BOOL ,IsRunning)(const CLSID& rpid);
	STDMETHOD(RevokeAll)();

public:
	HRESULT SetMainPlugin(REFCLSID clsid);
	
private:
	BOOL IsExclude(const CLSID& clsid);
private:
	CRotInfoMap    m_rotInfoMap;
	CRotInfoVector m_rotInfoVector;
	CLSID m_mainGuid;
	UTIL::com_ptr<IMSBase> m_pMainPlugin;
	DECLARE_AUTOLOCK_CS(rotmap);
};
