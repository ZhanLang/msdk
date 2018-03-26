#include "stdafx.h"
#include "LuaVM.h"
#include <vector>
#include <stack>
#include <queue>
#include <list>

#include <msapi/msapp.h>
extern "C"
{
#include "..\lua-5.1.4\pthread.h"
	#include "../luasocket/luasocket.h"
};

#include "arith/encrypt/WinCrypt.h"
#include "WinService.h"
#include "WinRegKey.h"
#include "WinFileSystems.h"
#include "WinQuickLink.h"
#include "WinIni.h"
#include "xml/rapidxml/rapidxml.h"
#include "WinPath.h"
#include "arith/7Zip/7ZipLua.h"
#include "xml/ixmltree3.h"
#include "http.h"
#include "LuaLog.h"
#include "OsInof.h"
#include "WinApi.h"
#include "ParseCommandToLua.h"
#include "StringConvert.h"
#include "vercmp.h"
#include "LuaBase64.h"
#include "DesEncrypt.h"
#include "BindHttp.h"
#include "Rc4LuaEx.h"
#include "l_msapp.h"
#include "l_mstime.h"
#include "l_reg.h"
#include "WinGloble.h"
#include "leant.h"
#include "l_AppDownload.h"
#include "WinDefine_l.h"

#pragma comment(lib, "Wininet.lib")
using namespace winfunc;
STDMETHODIMP CLuaVM::OpenVM(LuaVm_OpenMask openMask)
{
	if (!m_pLuaHost)
	{
		m_pLuaHost = new CLuaHost(lua_open());
		luaL_openlibs(m_pLuaHost->GetLua());
		BindStdLibrary(m_pLuaHost);
		BindThis();
		typedef std::basic_string< TCHAR >	TString;	//std::string

		m_pLuaHost->Insert(mluabind::BindVector<std::vector<int> >("vector_int"));
		m_pLuaHost->Insert(mluabind::BindVector<std::vector<TString> >("vector_string"));
		m_pLuaHost->Insert(mluabind::BindStack<std::stack<TString> >("stack_string"));
		m_pLuaHost->Insert(mluabind::BindQueue<std::queue<TString> >("queue_string"));
		m_pLuaHost->Insert(mluabind::BindList<std::list<TString> >("list_string"));

		
		{
			m_pLuaHost->Insert(mluabind::Declare("")
				+mluabind::Function<bool>("log", &log)
				+mluabind::Function<bool>("error", &error)
				+mluabind::Function("warning", &warning)
				+mluabind::Function("dbg", &dbg)
				+mluabind::Function("print",&print)
				);
		}

		
		if (openMask & LuaVm_Crypt)
		{
			/*
			m_pLuaHost->Insert(mluabind::Declare("winfunc")
				+mluabind::Class<winfunc::WinCrypt>("WinCrypt")
				.Constructor()
				.Method("RC4_Init",						&winfunc::WinCrypt::RC4_Init)
				.Method("RC4_Decrypt",					&winfunc::WinCrypt::RC4_Decrypt)
				.Method("RC4_Encrypt",					&winfunc::WinCrypt::RC4_Encrypt)
				.Method("Base64_Encrypt",				&winfunc::WinCrypt::Base64_Encrypt)
				.Method("RC4_Decrypt_File",				&winfunc::WinCrypt::RC4_Decrypt_File)
				.Method("RC4_Encrypt_File",				&winfunc::WinCrypt::RC4_Encrypt_File)
				.Method("RC4_Encrypt_UTF8_Base64_Code",	&winfunc::WinCrypt::RC4_Encrypt_UTF8_Base64_Code)
				);
				*/
		}
		
		if (openMask & LuaVm_Service)
		{
			CWinService::BindToLua(this);
		}
		
		if (openMask & LuaVm_RegKey)
		{
			CWinRegKey::BindToLua(this);
		}
		
		if (openMask & LuaVm_FileFind)
		{
			CWinFileFind::BindToLua(this);
		}
	//
		if (openMask & LuaVm_FileSystems)
		{
			CWinFileSystems::BindToLua(this);
		}



		if (openMask & LuaVm_QuickLink)
		{
			CWinQuickLink::BindToLua(this);
		}
		
		CStringConvert::BindToLua(this);

		if (openMask & LuaVm_Ini)
		{
			CWinIni::BindToLua(this);
			
		}
		
// 		if (openMask & LuaVm_RapidXml)
// 		{
// 			luaopen_rapidxml(m_pLuaHost->GetLua());
// 		}
		
		if (openMask & LuaVm_Socket)
		{
			//luaopen_socket_core(m_pLuaHost->GetLua());
		}
		CWinComDll::BindToLua(this);
		CWinDir::BindToLua(this);
				
		/*
		DIR_WINDOWS,				// Windows directory, usually "C:\Windows"
		DIR_SYSTEM,					// Usually "C:\Windows\system32"
		DIR_PROGRAM_FILES,			// Usually "C:\Program Files"
		DIR_PROGRAM_FILESX86,		// Usually "C:\Program Files" or "C:\Programs Files (x86)"

		DIR_IE_INTERNET_CACHE,		// Temporary Internet Files directory.
		DIR_COMMON_START_MENU,		// Usually "C:\Documents and Settings\All Users\Start Menu\Programs"
		DIR_START_MENU,				// Usually "C:\Documents and Settings\<user>\Start Menu\Programs"
		DIR_APP_DATA,				// Application Data directory under the user profile.
		DIR_PROFILE,				// Usually "C:\Documents and settings\<user>.
		DIR_LOCAL_APP_DATA_LOW,		// Local AppData directory for low integrity level.
		DIR_LOCAL_APP_DATA,			// "Local Settings\Application Data" directory under the user profile.
		DIR_COMMON_APP_DATA,		// W2K, XP, W2K3: "C:\Documents and settings\All Users\Application Data"
		// Vista, W2K8 and above: "C:\ProgramData".
		*/
		
		
		//Bind7ZipLuaLibrary(m_pLuaHost->GetLua());
		COsInof::Bind(m_pLuaHost);


		CWinApi::BindToLua(this);
		CParseCommandToLua::BindToLua(this);
		CVerCmpLua::BindToLua(this);
		//CLuaBase64::BindToLua(this);
		//CDesEncrypt::BindToLua(this);
		CBindHttp::BindToLua(this);
		CLuaMsAppBand::BindToLua(this);
		CLuaMsTimeBand::BindToLua(this);
		CLuaMsRegBand::BindToLua(this);
		CBindWinGlobel::BindToLua(this);
		CLuaEnumAntivirus::BindToLua(this);
		CLuaAppDownload::BindToLua(this);
		CLuaWindDef::BindToLua(this);
		//CRc4LuaEx::BindToLua(this);
		LoadFromConfig();
		
		
		if (m_pRot)
		{
			UTIL::com_ptr<IMscomRunningObjectTableEx> pRotEx = m_pRot;
			pRotEx->EnumRunningObjectTable(this, __uuidof(ILuaPlugin));
		}
	}

	SetLuaModulePath();
	SetLuaModuleCPath();
	return m_pLuaHost ? S_OK : E_FAIL;
}

STDMETHODIMP CLuaVM::ClosetVM()
{
	RASSERT(m_pLuaHost, S_FALSE);
	lua_close(m_pLuaHost->GetLua());
	SAFE_DELETE(m_pLuaHost);

	AUTOLOCK_CS(m_pObject);
	for (CObjectMap::iterator it = m_pObject.begin(); it != m_pObject.end(); it++)
	{
		it->second = INULL;
	}
	m_pObject.clear();
	return S_OK;
}

STDMETHODIMP_(LPVOID) CLuaVM::GetLuaState( void )
{
	if (m_pLuaHost)
	{
		return m_pLuaHost->GetLua();
	}
	return NULL;
}

STDMETHODIMP_(LPVOID) CLuaVM::GetContext()
{
	return m_pLuaHost;
}

STDMETHODIMP CLuaVM::DoString( LPCTSTR lpszScript )
{
	RASSERT(lpszScript && wcslen(lpszScript), E_INVALIDARG);
	RASSERT(m_pLuaHost, E_FAIL);

	USES_CONVERSION;
	if (m_pLuaHost->DoString(W2A(lpszScript)))
	{
		_call_main();
		return S_OK;
	}
	return E_FAIL;
}

STDMETHODIMP CLuaVM::DoFile( LPCTSTR lpszPath )
{
	RASSERT(lpszPath && wcslen(lpszPath), E_INVALIDARG);
	RASSERT(m_pLuaHost, E_FAIL);
	
	USES_CONVERSION;
	if (m_pLuaHost->DoFile(W2A(lpszPath)))
	{
		_call_main();
		return S_OK;
	}
	
	
	return E_FAIL;
}

CLuaVM::CLuaVM()
{
	m_pLuaHost = NULL;
}

CLuaVM::~CLuaVM()
{
	ClosetVM();
}

HRESULT CLuaVM::LoadFromConfig()
{
	CString strCurPath = GetCurrentPath();
	CString strFind = strCurPath + _T("\\*.luaex");

	WIN32_FIND_DATA findData = {0};
	HANDLE hFind = FindFirstFile(strFind , &findData);
	while(hFind != INVALID_HANDLE_VALUE )
	{
		CString strPath = strCurPath + _T("\\") + findData.cFileName;

		do 
		{
			UTIL::com_ptr<IXMLTree3>	pXmlTree3;
			CreateInstance(CLSID_IXMLTreeW, NULL, re_uuidof(IXMLTree3), (void**)&pXmlTree3);
			if (!pXmlTree3)
			{
				GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::LoadFromConfig::CreateInstance::CLSID_IXMLTreeW"));
				break;
			}

			if (!pXmlTree3->Load(strPath))
			{
				GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::LoadFromConfig::IXMLTree3::Load(%s)"), strPath.GetBuffer());
				break;
			}

			HXMLTREEITEM h_luaex = pXmlTree3->LocalItem(pXmlTree3->GetRoot(), _T("luaex"));
			if (!h_luaex)
			{
				GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::LoadFromConfig::IXMLTree3::LocalItem::luaex"));
				break;
			}

			for (HXMLTREEITEM hXmlItem = pXmlTree3->GetChild(h_luaex) ; hXmlItem ; hXmlItem = pXmlTree3->GetNextItem(hXmlItem))
			{
				LPCTSTR lpszClsid = pXmlTree3->GetAttribute(hXmlItem,_T("clsid"));
				RASSERT2(lpszClsid, continue);
				LPCTSTR lpszProgid = pXmlTree3->GetAttribute(hXmlItem,_T("progid"));
				LPCTSTR lpszName = pXmlTree3->GetAttribute(hXmlItem,_T("name"));

				UTIL::com_ptr<ILuaExtend> pLuaHostEx;
				CreateInstance(S2GUID(lpszClsid), NULL, re_uuidof(ILuaExtend), (void**)&pLuaHostEx);
				if (!pLuaHostEx)
				{
					GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::LoadFromConfig::CreateInstance(clsid=%s,progid=%s,name=%s)"), lpszClsid, lpszProgid, lpszName);
					break;
					
				}

				if (FAILED(pLuaHostEx->RegisterLuaEx(this)))
				{
					GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::LoadFromConfig::ILuaHostEx::RegisterLuaEx(clsid=%s,progid=%s,name=%s)"), lpszClsid, lpszProgid, lpszName);
					break;
				}
			}
			
		} while (0);
		
		

		if (!FindNextFile(hFind , &findData))
		{
			break;
		}
	}

	if (hFind)
	{
		FindClose(hFind);
		hFind = NULL;
	}
	return S_OK;
}

STDMETHODIMP CLuaVM::_call_main()
{
	CLuaHost* pHost = static_cast<CLuaHost*>(GetContext());
	RASSERT(pHost, E_FAIL);
	return pHost->CallLuaFunctionRet<int, 0 , VOID*>("main", this);
	return S_OK;
}

STDMETHODIMP CLuaVM::BindThis()
{
	m_pLuaHost->Insert(mluabind::Declare("LuaVM")
		+mluabind::Class<CLuaVM>("CLuaVM")
			.Constructor()
			.Method<std::string,LPCSTR>("GetEnvParamString", &CLuaVM::LuaGetEnvParamString)
			.Method<INT,LPCSTR>("GetEnvParam", &CLuaVM::LuaGetEnvParam)

			.Method<std::string,LPCSTR>("GetSettingString", &CLuaVM::LuaGetSettingString)
			.Method<std::string,LPCSTR>("GetPubSettingString", &CLuaVM::LuaGetPubSettingString)

			.Method<INT,LPCSTR>("GetSetting", &CLuaVM::LuaGetSetting)
			.Method<INT,LPCSTR>("GetPubSetting", &CLuaVM::LuaGetPubSetting)

			.Method<bool,LPCSTR,LPCSTR>("SetPubSetting", &CLuaVM::LuaSetPubSetting)
			.Method<bool,LPCSTR,LPCSTR>("SetSetting", &CLuaVM::LuaSetSetting)

			.Method("GetObject", &CLuaVM::GetObjectLua)
			.Method("LuaCallFunction", &CLuaVM::LuaCallFunction)
		+mluabind::Function("ConvertToLuaVM", &CLuaVM::Convert)
		);


	/*
	std::string LuaGetPubSetting(LPCSTR lpszName);
	std::string LuaGetSettingString(LPCSTR lpszName);



	//设置
	bool LuaSetSetting(LPCSTR lpszName, LPCSTR lpszValue);
	bool LuaSetPubSetting(LPCSTR lpszName, LPCSTR lpszValue);
	*/
	return S_OK;
}

std::string CLuaVM::LuaGetEnvParamString(LPCSTR lpszName)
{
	std::string sRet;
	LPCWSTR lpszPasram = GetEnvParamString(lpszName);
	if (lpszName)
	{
		USES_CONVERSION;
		sRet = W2A(lpszPasram);
	}

	return sRet;
}

INT CLuaVM::LuaGetEnvParam(LPCSTR lpszName)
{
	INT nRet = GetEnvParam(lpszName);
	return nRet;
}

std::string CLuaVM::LuaGetSettingString(LPCSTR lpszName)
{
	TCHAR szData[ 1024 ] = { 0 };
	USES_CONVERSION;
	msapi::CApp(GetEnvParamString("productname")).GetSetting(A2T(lpszName), _T("") , szData, _countof(szData) );
	
	return T2A( szData );
}

std::string CLuaVM::LuaGetPubSettingString(LPCSTR lpszName)
{
	TCHAR szData[ 1024 ] = { 0 };
	USES_CONVERSION;
	msapi::CApp::GetPubSetting(A2T(lpszName), _T("") , szData, _countof(szData) );
	return T2A( szData );
}

INT CLuaVM::LuaGetSetting(LPCSTR lpszName)
{
	USES_CONVERSION;
	return msapi::CApp(GetEnvParamString("productname")).GetSetting(A2T(lpszName), 0 );
}

INT CLuaVM::LuaGetPubSetting(LPCSTR lpszName)
{
	USES_CONVERSION;
	return msapi::CApp::GetPubSetting(A2T(lpszName), 0 );
}

bool CLuaVM::LuaSetPubSetting(LPCSTR lpszName, LPCSTR lpszValue)
{
	USES_CONVERSION;
	return msapi::CApp::SetPubSetting(A2T(lpszName), A2T(lpszValue) )? true : false;
}

bool CLuaVM::LuaSetSetting(LPCSTR lpszName, LPCSTR lpszValue)
{
	USES_CONVERSION;
	return msapi::CApp(GetEnvParamString("productname")).SetSetting(A2T(lpszName), A2T(lpszValue) ) ? true : false;
}

STDMETHODIMP CLuaVM::Register(ILuaPlugin* pLuaPlugin)
{
	RASSERT(pLuaPlugin, E_FAIL);

	LPCSTR lpszName = pLuaPlugin->GetObjectName();
	AUTOLOCK_CS(m_pObject);
	CObjectMap::iterator it = m_pObject.find(lpszName);
	if (it != m_pObject.end())
	{
		GrpWarning(GRP_NAME, MsgLevel_Warning, _T("CLuaVM::Register %s exist."), lpszName);
	}
	m_pObject[std::string(lpszName)] = pLuaPlugin;
	return S_OK;
}

STDMETHODIMP CLuaVM::GetObject(LPCTSTR lpszName, ILuaPlugin**ppObj)
{
	RASSERT(lpszName, E_FAIL);

	USES_CONVERSION;
	AUTOLOCK_CS(m_pObject);
	
	CObjectMap::iterator it = m_pObject.find(std::string(W2A(lpszName)));
	if (it != m_pObject.end())
	{
		return it->second->QueryInterface(__uuidof(IMSBase), (void**)ppObj);
	}

	return E_FAIL;
}

STDMETHODIMP CLuaVM::Revoke(LPCTSTR lpszName)
{
	RASSERT(lpszName, E_FAIL);
	AUTOLOCK_CS(m_pObject);

	USES_CONVERSION;
	m_pObject.erase(std::string(W2A(lpszName)));
	return S_OK;
}

LPVOID CLuaVM::GetObjectLua( LPCSTR lpszName )
{
	RASSERT(lpszName, NULL);

	AUTOLOCK_CS(m_pObject);
	CObjectMap::iterator it = m_pObject.find(std::string(lpszName));
	if (it != m_pObject.end())
	{
		UTIL::com_ptr<ILuaPlugin> pLuaPlugin = it->second;
		if (pLuaPlugin)
		{
			return pLuaPlugin->GetObjectAddr();
		}
	}
	return NULL;
}

CLuaVM* CLuaVM::Convert( LPVOID lpConvert )
{
	return static_cast<CLuaVM*>(lpConvert);
}

BOOL CLuaVM::OnRunningObjectTableEnum(REFCLSID clsid , IMSBase* pObject)
{
	UTIL::com_ptr<ILuaPlugin> pLuaScript = pObject;
	if (pLuaScript)
	{
		LPCSTR lpszName = pLuaScript->GetObjectName();
		if ( !lpszName )
		{
			return FALSE;
		}


		if (FAILED(pLuaScript->BindToLua(this)))
		{
			USES_GUIDCONVERSION;
			GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaVM::OnRunningObjectTableEnum::BindToLua(%s)"), GUID2S(clsid));
		}
		Register(pLuaScript);
	}

	return TRUE;
}



STDMETHODIMP CLuaVM::GetRunningObjectTable(IMSBase** pRot)
{
	return m_pRot->QueryInterface(re_uuidof(IMSBase), (void**) pRot);
}

STDMETHODIMP_(LPCSTR) CLuaVM::CallLuaFunction(LPCSTR lpszFunctionName, LPCSTR lpszJsonParam)
{
	if (m_pLuaHost)
	{
		return m_pLuaHost->CallLuaFunction<LPCSTR>(lpszFunctionName, lpszJsonParam);
	}

	return NULL;
}

std::string CLuaVM::LuaCallFunction( LPCSTR lpszModuleName, LPCSTR lpszFunctionName, LPCSTR lpszParam )
{
	CObjectMap::iterator it = m_pObject.find(lpszModuleName);
	if (it == m_pObject.end())
	{
		GrpErrorA(GroupNameA, MsgLevel_Error, "CLuaVM::LuaCallFunction(%s) 模块名未找到!",lpszModuleName);
		return "";
	}

	UTIL::com_ptr<ILuaPlugin> pLuaPlugin = it->second;
	if (pLuaPlugin && m_pRot)
	{
		UTIL::com_ptr<IStringA> pResult;
		m_pRot->CreateInstance(CLSID_StringA, NULL, __uuidof(IStringA), (void**)&pResult.m_p);
		pLuaPlugin->LuaCallFuncton(lpszFunctionName, lpszParam, pResult.m_p);
		if (pResult)
		{
			 return pResult->GetBuffer();
		}
	}

	return "";
}

VOID CLuaVM::SetLuaModulePath()
{
	//.\?.lua;G:\svn\maguojun\dev\jwhfs\bin\lua\?.lua;G:\svn\maguojun\dev\jwhfs\bin\lua\?\init.lua;G:\svn\maguojun\dev\jwhfs\bin\?.lua;G:\svn\maguojun\dev\jwhfs\bin\?\init.lua

	CString lpszCurPath = GetCurrentPath();
	{
		CString sTemp = lpszCurPath;
		sTemp.Append(_T("\\script\\?.lua"));
		SetLuaModulePath(sTemp);
	}

	{
		CString sTemp = lpszCurPath;
		sTemp.Append(_T("\\script\\?\\init.lua"));
		SetLuaModulePath(sTemp);
	}
}

VOID CLuaVM::SetLuaModulePath(LPCTSTR lpszPath)
{
	if (m_pLuaHost && wcslen(lpszPath))
	{
		lua_State* pLuaState = m_pLuaHost->GetLua();
		lua_getglobal(pLuaState , "package" );
		lua_getfield( pLuaState, -1, "path" );
		std::string cur_path = lua_tostring( pLuaState, -1 ); 
		cur_path.append( ";" );
		USES_CONVERSION;
		cur_path.append(W2A(lpszPath));
		lua_pop( pLuaState, 1 );
		lua_pushstring( pLuaState, cur_path.c_str() );
		lua_setfield( pLuaState, -2, "path" );
		lua_pop( pLuaState, 1 );
	}
}

VOID CLuaVM::SetLuaModuleCPath()
{
	CString lpszCurPath = GetCurrentPath();
	{
		CString sTemp = lpszCurPath;
		sTemp.Append(_T("\\script\\?.dll"));
		SetLuaModulePath(sTemp);
	}
}

VOID CLuaVM::SetLuaModuleCPath(LPCTSTR lpszPath)
{
	if (m_pLuaHost && wcslen(lpszPath))
	{
		lua_State* pLuaState = m_pLuaHost->GetLua();
		lua_getglobal(pLuaState , "package" );
		lua_getfield( pLuaState, -1, "cpath" );
		std::string cur_path = lua_tostring( pLuaState, -1 ); 
		cur_path.append( ";" );
		USES_CONVERSION;
		cur_path.append(W2A(lpszPath));
		lua_pop( pLuaState, 1 );
		lua_pushstring( pLuaState, cur_path.c_str() );
		lua_setfield( pLuaState, -2, "cpath" );
		lua_pop( pLuaState, 1 );
	}
}
