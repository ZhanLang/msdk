
#pragma once
#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"
#include "mscom/srvbase.h"
#include "SyncObject/criticalsection.h"
#include <string>

class CLuaVM : 
	public ILuaVM,
	public IRunningObjectTableEnum,
	public CMsComBase<CLuaVM>
{
public:

	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter)
	{
		if (pRot)
			m_pRot = pRot;
		else
			GrpWarning(GroupName, MsgLevel_Warning, _T("CLuaVM::init_class 未传入 ROT ，CLuaVM 将不支持ROT特性。"));
		
		return S_OK;
	}

	UNKNOWN_IMP1(ILuaVM);
	CLuaVM();
	~CLuaVM();

	STDMETHOD(OpenVM)(LuaVm_OpenMask openMask= LuaVm_All) ;
	STDMETHOD(ClosetVM)();

	STDMETHOD_(LPVOID, GetLuaState)(void) ;
	STDMETHOD_(LPVOID, GetContext)();	//这个和<luahost.h>相关

	STDMETHOD(DoString)(LPCTSTR lpszScript);
	STDMETHOD(DoFile)(LPCTSTR lpszPath);
	STDMETHOD(Register)(ILuaPlugin* pLuaPlugin);
	STDMETHOD(GetObject)(LPCWSTR lpszName, ILuaPlugin**ppObj);
	STDMETHOD(Revoke)(LPCTSTR lpszName);
	STDMETHOD(GetRunningObjectTable)(IMSBase** pRot);
	STDMETHOD_(LPCSTR , CallLuaFunction)(LPCSTR lpszFunctionName, LPCSTR lpszJsonParam);


protected:
	virtual BOOL OnRunningObjectTableEnum(REFCLSID clsid , IMSBase* pObject);
private:
	STDMETHOD(_call_main)();

private:
	STDMETHOD(BindThis)();

private:
	std::string LuaGetEnvParamString(LPCSTR lpszName);
	INT LuaGetEnvParam(LPCSTR lpszName);

	//
	std::string LuaGetSettingString(LPCSTR lpszName);
	std::string LuaGetPubSettingString(LPCSTR lpszName);

	INT	LuaGetSetting(LPCSTR lpszName);
	INT LuaGetPubSetting(LPCSTR lpszName);
	
	//设置
	bool LuaSetPubSetting(LPCSTR lpszName, LPCSTR lpszValue);
	bool LuaSetSetting(LPCSTR lpszName, LPCSTR lpszValue);
	



	LPVOID GetObjectLua(LPCSTR lpszName);

	std::string LuaCallFunction(LPCSTR lpszModuleName, LPCSTR lpszFunctionName, LPCSTR lpszParam);

	static CLuaVM* Convert(LPVOID lpConvert);
private:
	HRESULT LoadFromConfig();

	VOID SetLuaModulePath();
	VOID SetLuaModuleCPath();

	VOID SetLuaModulePath(LPCTSTR lpszPath);
	VOID SetLuaModuleCPath(LPCTSTR lpszPath);

private:
	CLuaHost* m_pLuaHost;

	DECLARE_AUTOLOCK_CS(m_pObject);
	typedef std::map<std::string , UTIL::com_ptr<ILuaPlugin>> CObjectMap;
	CObjectMap m_pObject;
};