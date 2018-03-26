#pragma once
#include "mstask\imstask.h"
#include <mscom/msplugin.h>
#include "xThread.h"

class CTaskSvrWrap: public ITaskSvrWrap,
					public IMsPlugin,
					public IMsPluginRun,
					public XThread,
					private  CUnknownImp

{
public:
	CTaskSvrWrap(void);
	~CTaskSvrWrap(void);

	UNKNOWN_IMP3_(ITaskSvrWrap,IMsPlugin,IMsPluginRun);

	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter)
	{return S_OK;}

	STDMETHOD(Init)(LPVOID lpVoid =NULL);
	STDMETHOD(Uninit)();

	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	virtual VOID SVC() ;

private:
	BOOL InitEvent();
	UTIL::com_ptr<IRSTaskSrv> m_RsTaskSrv;
	HANDLE     m_hEvent;
	BOOL m_bExitting;

};
