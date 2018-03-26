
#pragma once
#include "ILuaHost.h"
#include <string>
#include "json/json.h"
#define BEGIN_DEFLUAFUNCTION_MAP(_M,_Base) STDMETHOD_(LPCSTR , GetObjectName)(){return _M;} STDMETHOD_(LPVOID,GetObjectAddr)(){return (VOID*)this;} STDMETHOD(LuaCallFuncton)(LPCSTR lpszFunctionName, LPCSTR lpParam, IStringA* pResult){ if(!(lpszFunctionName&&lpParam&&pResult))return E_FAIL; std::string result; do{
#define DEFINE_LUAFUNCTION(_N,_F) if(strcmp(_N,lpszFunctionName)==0) {result=_F(lpParam); break;}
#define DEFINE_LUAFUNCTION_NO_PARAM(_N,_F) if(strcmp(_N,lpszFunctionName)==0) {result=_F(); break;}
#define DEFINE_LUAFUNCTION_NO_INOUT_PARAM(_N,_F) if(strcmp(_N,lpszFunctionName)==0) {_F(); break;}
#define END_DEFLUAFUNCTION_MAP }while(false); pResult->Append(result.c_str()); return S_OK;}


//JSON 的处理方式
#define DEFINE_LUAFUNCTION_JSON(_N,_F)\
	if(strcmp(_N,lpszFunctionName)==0)\
	{\
		Json::Reader reader;\
		Json::Value inValue;\
		Json::Value outValue;\
		if (reader.parse(lpParam, inValue))\
		{\
			_F(inValue, outValue);\
			result = outValue.toStyledString();\
		}\
		break;\
	}\

//没有入参的JSON处理方式
#define DEFINE_LUAFUNCTION_JSON_NO_IN_PARAM(_N,_F)\
	if(strcmp(_N,lpszFunctionName)==0)\
	{\
		Json::Value outValue;\
		_F(outValue);\
		result = outValue.toStyledString();\
		break;\
	}\

//没有出参的JSON处理方式
#define DEFINE_LUAFUNCTION_JSON_NO_OUT_PARAM(_N,_F)\
	if(strcmp(_N,lpszFunctionName)==0)\
	{\
		Json::Reader reader;\
		Json::Value inValue;\
		if (reader.parse(lpParam, inValue))\
		{\
			_F(inValue);\
		}\
		break;\
	}\


namespace msdk{;
namespace mscom{;

class CLuaPluginImpl
	:public ILuaPlugin
{
protected:
	
	/*
		DEFINE_LUAPLUGIN("TestLuaPlugin")
		BEGIN_DEFLUAFUNCTION_MAP
			DEFINE_LUAFUNCTION("add", Add)
		END_DEFLUAFUNCTION_MAP

	*/

	virtual ILuaVM* GetLuaVM() = 0;

	//由派生者实现
	STDMETHOD(BindToLua)(ILuaVM* pLuaVm)
	{
		return S_FALSE;
	}


	STDMETHOD(LuaCallFuncton)(LPCSTR lpszFunctionName, LPCSTR lpParam, IStringA* pResult)
	{
		return E_NOTIMPL;
	}

	STDMETHOD_(LPCSTR, CallLuaFunction)(LPCSTR lpszFunctionName)
	{
		ILuaVM* pLuaVM = GetLuaVM();
		if (pLuaVM)
		{
			return pLuaVM->CallLuaFunction(lpszFunctionName, "");
		}

		return NULL;
	}

	STDMETHOD_(LPCSTR, CallLuaFunction)(LPCSTR lpszFunctionName, LPCSTR lpszJsonParam)
	{
		ILuaVM* pLuaVM = GetLuaVM();
		if (pLuaVM)
		{
			return pLuaVM->CallLuaFunction(lpszFunctionName, lpszJsonParam);
		}

		return NULL;
	}

	STDMETHOD(CallLuaJsonFunction)(LPCSTR lpszFunctionName, Json::Value inParam, Json::Value& outParam)
	{
		ILuaVM* pLuaVM = GetLuaVM();
		RASSERT(pLuaVM, E_FAIL);

		std::string strInParam = inParam.toStyledString();
		LPCSTR lpszOutParam = pLuaVM->CallLuaFunction(lpszFunctionName, strInParam.c_str());
		if (lpszOutParam)
		{
			Json::Reader read;
			return  read.parse(lpszOutParam, outParam) ? S_OK : E_FAIL;
		}

		return S_OK;
	}

	STDMETHOD(CallLuaJsonFunction_1)(LPCSTR lpszFunctionName, Json::Value inParam)
	{
		ILuaVM* pLuaVM = GetLuaVM();
		RASSERT(pLuaVM, E_FAIL);
		std::string strInParam = inParam.toStyledString();
		LPCSTR lpszOutParam = pLuaVM->CallLuaFunction(lpszFunctionName, strInParam.c_str());
		return S_OK;
	}

	STDMETHOD(CallLuaJsonFunction_2)(LPCSTR lpszFunctionName, Json::Value& outParam)
	{
		ILuaVM* pLuaVM = GetLuaVM();
		RASSERT(pLuaVM, E_FAIL);
		LPCSTR lpszOutParam = pLuaVM->CallLuaFunction(lpszFunctionName, "");
		if (lpszOutParam)
		{
			Json::Reader read;
			return  read.parse(lpszOutParam, outParam) ? S_OK : E_FAIL;
		}
		return S_OK;
	}
};


};};