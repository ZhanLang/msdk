#pragma once
#include "mscdef.h"
class CMsMsc:public IMsgBus,
			 public IMsPluginRun,
			 private CUnknownImp
{
public:
	CMsMsc(void);
	~CMsMsc(void);
	UNKNOWN_IMP3_(IMsPluginRun,IMsPlugin,IMsgBus);

	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter);
    STDMETHOD(SetPipeName)(const TCHAR *szName,DWORD dwMax);
	STDMETHOD_(BOOL,CreateProxy)(IXMsgBusProxy** lppXMSCProxy);
	//初始化
	STDMETHOD(Init)(void*lpVoid);
	//反初始化接口
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();

private:
    void FreeName();

	UTIL::com_ptr<IMsBufferMgr> m_pRsBufferManager;
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRuningTable;
    UTIL::com_ptr<IMsObjectLoader> m_pObjLoader;
    TCHAR * m_pPipeName;

};
