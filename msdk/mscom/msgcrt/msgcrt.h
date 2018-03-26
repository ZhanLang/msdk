#pragma once
#include "msgcrt\IMsgCrenter.h"
#include "msc\mscdef.h"

class CMsgCenter:
	public IMsPlugin,
	public IMsPluginRun,
	public IMsgCenter,
	public CUnknownImp
{
public:
	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter)
	{
		RASSERT(pRot , E_INVALIDARG);
		m_pRot = pRot;
		return S_OK;
	}
	UNKNOWN_IMP3_(IMsPlugin,IMsPluginRun , IMsgCenter);

	CMsgCenter();
	~CMsgCenter();

public:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	STDMETHOD(Start)();
	STDMETHOD(Stop)();
	STDMETHOD(SetNameSpace)(LPCWSTR lpstrName);
private:
	HRESULT CreateMsc();
	HRESULT GetMscNameSpace();
private:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
	UTIL::com_ptr<IMsgBus> m_pMsc;
	CString m_strNameSpace;
};
