// mscom_ex.h
// 引用函数，引用基本COM的使用方法

#ifndef MSCOM_PUBLIC_DEFINE_DOOYAN
#define MSCOM_PUBLIC_DEFINE_DOOYAN

#include <atlbase.h>
#include "util/callapi.h"
#include "mscominc.h"
using namespace mscom;

//#define CMSComPtr CComPtr
#define CMSComPtr UTIL::com_ptr
 
// 快速创建本模块自己的对象
#define DllQuickCreateInstance(clsid, iid, ppv ,prot)	__MS_DllGetClassObject(clsid, iid, (void**)&ppv, prot)
MARGIN_API MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
inline HRESULT __MS_DllGetClassObject(REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv, IMSBase *prot)
{
	CMSComPtr<IClassFactory> pClassFactory;

	if(SUCCEEDED(MSDllGetClassObject(rclsid, re_uuidof(IClassFactory), (void**)&pClassFactory)) && pClassFactory)
	{
		CMSComPtr<IMSClassFactory> pMSClassFactory = pClassFactory;
		if(pMSClassFactory)
			return pMSClassFactory->CreateInstance(prot, NULL, riid, ppv);
		else
			return pClassFactory->CreateInstance(NULL, riid, ppv);
	}
	return E_FAIL;
}

// 正常的方法
class CMSComLoader : protected tImpModuleMid<CMSComLoader>  // 禁止某些基类功能
{
public:
	HRESULT (__stdcall *DllGetClassObject)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
	HRESULT (__stdcall *DllCanUnloadNow)(void);
	HRESULT (__stdcall *DllRegisterServer)(void);
	HRESULT (__stdcall *DllUnregisterServer)(void);
	
	DECLARE_FUN_BEGIN(CMSComLoader, "MSCOM")
		DECLARE_FUN(DllGetClassObject)
		DECLARE_FUN(DllCanUnloadNow)
		DECLARE_FUN(DllRegisterServer)
		DECLARE_FUN(DllUnregisterServer)
	DECLARE_FUN_END()

	virtual ~CMSComLoader(){UnloadMSCom();}

public:
	BOOL LoadMSCom(CONST TCHAR* pModPathName ,  BOOL bIgnoreRsCF = FALSE)
	{
		SetMoudlePath(pModPathName);
		CHAR szMod[MAX_PATH];
#ifdef UNICODE
			USES_CONVERSION;
	#if _MSC_VER >= 1400
			strcpy_s(szMod, MAX_PATH, W2A(pModPathName));
	#else
			strcpy(szMod, W2A(pModPathName));
	#endif
#else
	#if _MSC_VER >= 1400
			strcpy_s(szMod, MAX_PATH, pModPathName);
	#else
			strcpy(szMod, pModPathName);
	#endif
#endif	//UNICODE
		m_pModuleName = szMod;
		INT nRet = Load();
		if (nRet == -1)
		{
			return FALSE;
		}
		m_pModuleName = "";

		if(DllGetClassObject == NULL) return FALSE;


		//add by magj 为了兼容com直接返回
		return TRUE;

		/*
		DllGetClassObject(CLSID_MSClassFactory, re_uuidof(IClassFactory), (void**)&m_pClassFactory);
		if(m_pClassFactory || bIgnoreRsCF)
		{
			return TRUE;
		}
		*/



		UnloadMSCom();
		return FALSE;
	}

	VOID SetMoudlePath(CONST TCHAR* pModPathName)
	{
		_tcscpy_s(m_szMoudlePath_ , MAX_PATH , pModPathName);
	}
	LPCTSTR GetModuelPath()
	{
		return m_szMoudlePath_;
	}

	BOOL LoadMSCom(BOOL bIgnoreRsCF = FALSE)
	{
		return LoadMSCom(m_szMoudlePath_ , bIgnoreRsCF);
	}

	virtual void UnloadMSCom()
	{
		m_pClassFactory = INULL;
		UnLoad();
	}

	HRESULT CreateInstance(IN REFCLSID rclsid, IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv)
	{
		*ppv = NULL;

		RASSERT(IsLoaded(), E_FAIL);

		{
			CMSComPtr<IClassFactory> pClassFactory;

			if(SUCCEEDED(DllGetClassObject(rclsid, re_uuidof(IClassFactory), (void**)&pClassFactory)) && pClassFactory)
			{
				CMSComPtr<IMSClassFactory> pMSClassFactory = pClassFactory;
				if(pMSClassFactory)
					return pMSClassFactory->CreateInstance(prot, punkOuter, riid, ppv);
				else
					return pClassFactory->CreateInstance(punkOuter, riid, ppv);
			}
		}

		return E_FAIL;
	}

	LONG GetCount()
	{
		UTIL::com_ptr<IMSClassFactory> pMSClassFactory = m_pClassFactory;
		if(pMSClassFactory)
			return pMSClassFactory->GetCount();

		return 0;
	}

	CLSID GetAt(INT nIndex)
	{
		UTIL::com_ptr<IMSClassFactory> pMSClassFactory = m_pClassFactory;
		if(pMSClassFactory)
			return pMSClassFactory->GetAt(nIndex);

		return GUID_NULL;
	}

	LPCTSTR ProgIDFromCLSID(REFCLSID clsid)
	{
		UTIL::com_ptr<IMSClassFactory> pMSClassFactory = m_pClassFactory;
		if(pMSClassFactory)
			return pMSClassFactory->ProgIDFromCLSID(clsid);

		return _T("");
	}

	BOOL IsLoaded()
	{
		return tImpModuleMid<CMSComLoader>::IsLoaded();
	}

	LPCTSTR GetModuleName()
	{
		return m_szMoudlePath_;
	}
protected:
	typedef tImpModuleMid<CMSComLoader> TBase;

	CMSComPtr<IClassFactory> m_pClassFactory;
	TCHAR m_szMoudlePath_[MAX_PATH];


};

#endif // MSCOM_PUBLIC_DEFINE_DOOYAN