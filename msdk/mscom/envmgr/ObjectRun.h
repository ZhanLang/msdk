#pragma once
#include "mscom/pluginbase.h"

#include "RunningObjectTable.h"
#include "ObjectLoader.h"

class CObjectRun : public IMsPlugin , public IMsPluginRun , public CUnknownImp
{

public:
	UNKNOWN_IMP2_(IMsPlugin , IMsPluginRun);
	HRESULT init_class(IMSBase* prot, IMSBase* punkOuter);
public:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	HRESULT AddMsComFile(LPCTSTR lpFileName);
	HRESULT AddMsComXml(LPCTSTR  lpXml);

	HRESULT AddMsRotFile(LPCTSTR lpFileName);
	HRESULT AddMsRotXml(LPCTSTR  lpXml);

private:
	HRESULT initPlugin(REFCLSID clsid);
	HRESULT startPlugin(REFCLSID clsid);

	HRESULT StartPlugins();
	HRESULT StopPlugins();
	HRESULT InitPlugins();
	HRESULT UnInitPlugins();

	BOOL    IsExcludeClsid(REFCLSID clsid);

protected:
	HRESULT OnRegiestRot();
private:
		BOOL CreateMscCenter();
public:
	CObjectRun(void);
	~CObjectRun(void);
	
	
	CRunningObjectTable m_Rot;
	CObjectLoader       m_Loader;


	//msc 消息中心
	CString m_strMsgCrtName;
	BOOL    m_bCreateCenter;
};
