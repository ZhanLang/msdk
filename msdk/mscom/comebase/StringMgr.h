
#pragma once
#include "MStringA.h"
#include "MStringW.h"

class MStringMgr : public IStringMgr , CUnknownImp
{
public:
	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter)
	{
		return S_OK;
	}
	UNKNOWN_IMP1(IStringMgr);

	STDMETHOD(CreateStringA)(IStringA** pString) 
	{
		return __MS_DllGetClassObject(CLSID_StringA , re_uuidof(IStringA) , (void**)pString,NULL);
	}

	STDMETHOD(CreateStringA_Safe)(IStringA** pString)
	{
		return __MS_DllGetClassObject(CLSID_StringA_Safe , re_uuidof(IStringA) , (void**)pString,NULL);
	}

	STDMETHOD(CreateStringW)(IStringW** pString)
	{
		return __MS_DllGetClassObject(CLSID_StringW , re_uuidof(IStringW) , (void**)pString,NULL);;
	}

	STDMETHOD(CreateStringW_Safe)(IStringW** pString)
	{
		return __MS_DllGetClassObject(CLSID_StringW_Safe , re_uuidof(IStringW) , (void**)pString,NULL);
	}
};