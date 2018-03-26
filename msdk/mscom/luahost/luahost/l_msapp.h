#pragma once

#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>

#include <msapi/msapp.h>





class CLuaMsApp
{
public:

	static tstring GetPublicDataPath()
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		return msdk::msapi::CApp::GetPubSettingPath(szPath, MAX_PATH);
	}

	static tstring GetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszDef)
	{
		TCHAR szData[ MAX_PATH * 4] = { 0 };
		msdk::msapi::CApp::GetPublicData(lpszNode, lpszKey,lpszDef,szData, _countof(szData) );
		return szData;
	}

	static DWORD GetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return msdk::msapi::CApp::GetPublicData(lpszNode, lpszKey,dwDef );
	}


	static BOOL SetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return msdk::msapi::CApp::SetPublicData(lpszNode, lpszKey,lpszData );
	}

	static BOOL SetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwData)
	{
		return msdk::msapi::CApp::SetPublicData(lpszNode, lpszKey,dwData );
	}


	static tstring GetPubSetting(LPCTSTR lpszKey, LPCTSTR lpszDef)
	{
		TCHAR szData[ MAX_PATH * 4] = { 0 };
		msdk::msapi::CApp::GetPubSetting(lpszKey,lpszDef,szData, _countof(szData) );
		return szData;
	}

	static DWORD GetPubSetting(LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return msdk::msapi::CApp::GetPubSetting(lpszKey,dwDef);
	}

	static BOOL SetPubSetting(LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return msdk::msapi::CApp::SetPubSetting(lpszKey,lpszData);
	}
	

	static BOOL SetPubSetting(LPCTSTR lpszKey, DWORD dwData)
	{
		return msdk::msapi::CApp::SetPubSetting(lpszKey,dwData);
	}


	static tstring GetDiskSerial( )
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		return msdk::msapi::CApp::GetDiskSerial(szPath, MAX_PATH);
	}

	static tstring GetUserGuid()
	{
		TCHAR lpszUserGuid[ MAX_PATH ] = { 0 };
		return msdk::msapi::CApp::GetUserGuid(lpszUserGuid, _countof(lpszUserGuid));
	}

	//////////////////////////////////////////////////////////////////////////
	CLuaMsApp(LPCTSTR lpszAppName):m_app(lpszAppName)
	{

	}

	
	tstring GetSetupPath()
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		m_app.GetSetupPath(szPath, MAX_PATH);
		return szPath;
	}

	BOOL SetSetupPath(LPCTSTR lpszPath)
	{
		return m_app.SetSetupPath(lpszPath);
	}

	tstring GetVersion()
	{
		TCHAR szVer[ MAX_PATH ] = { 0 };
		m_app.GetVersion(szVer, MAX_PATH);
		return szVer;
	}

	BOOL SetVersion(LPTSTR lpszVersion)
	{
		return m_app.SetVersion(lpszVersion);
	}

	tstring GetDataPath()
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		m_app.GetDataPath(szPath, MAX_PATH);
		return szPath;
	}

	tstring GetSettingFile( )
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		m_app.GetSettingFile(szPath, MAX_PATH);
		return szPath;
	}

	tstring GetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszDef)
	{
		TCHAR szData[ MAX_PATH * 4] = { 0 };
		m_app.GetPrivateData(lpszNode, lpszKey, lpszDef, szData, _countof(szData));
		return szData;
	}

	DWORD GetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return m_app.GetPrivateData( lpszNode, lpszKey, dwDef);
	}

	tstring GetSetting(LPCTSTR lpszKey, LPCTSTR lpszDef)
	{
		TCHAR szData[ MAX_PATH * 4] = { 0 };
		m_app.GetSetting( lpszKey, lpszDef, szData, _countof(szData));
		return szData;
	}

	DWORD GetSetting(LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return m_app.GetSetting( lpszKey, dwDef);
	}

	BOOL SetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return m_app.SetPrivateData( lpszNode, lpszKey, lpszData);
	}

	BOOL SetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwData)
	{
		return m_app.SetPrivateData( lpszNode, lpszKey, dwData);
	}

	BOOL SetSetting(LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return m_app.SetSetting( lpszKey, lpszData);
	}

	BOOL SetSetting(LPCTSTR lpszKey, DWORD dwData)
	{
		return m_app.SetSetting( lpszKey, dwData);
	}
public:
	msdk::msapi::CApp m_app;

};




class CLuaMsAppBand
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		 		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("msapp")
		 			+mluabind::Class<CLuaMsApp>("CApp")
		 			.Constructor<LPCTSTR>()
		 	
		 			.Method<tstring>("GetSetupPath", &CLuaMsApp::GetSetupPath)
		 			.Method<BOOL>("SetSetupPath", &CLuaMsApp::SetSetupPath)

		 			.Method<tstring>("GetVersion", &CLuaMsApp::GetVersion)
					.Method<BOOL,LPTSTR>("SetVersion", &CLuaMsApp::SetVersion)

					.Method<tstring>("GetDataPath", &CLuaMsApp::GetDataPath)
					.Method<tstring>("GetSettingFile", &CLuaMsApp::GetSettingFile)

					.Method<tstring,LPCTSTR, LPCTSTR, LPCTSTR>("GetPrivateData", &CLuaMsApp::GetPrivateData)
					.Method<DWORD,LPCTSTR, LPCTSTR, DWORD>("GetPrivateData", &CLuaMsApp::GetPrivateData)


					.Method<tstring,LPCTSTR, LPCTSTR>("GetSetting", &CLuaMsApp::GetSetting)
					.Method<DWORD,LPCTSTR, DWORD>("GetSetting", &CLuaMsApp::GetSetting)

					.Method<BOOL, LPCTSTR,LPCTSTR,LPCTSTR>("SetPrivateData", &CLuaMsApp::SetPrivateData)
					.Method<BOOL, LPCTSTR,LPCTSTR,DWORD>("SetPrivateData", &CLuaMsApp::SetPrivateData)

					.Method<BOOL,LPCTSTR,LPCTSTR>("SetSetting", &CLuaMsApp::SetSetting)
					.Method<BOOL,LPCTSTR,DWORD>("SetSetting", &CLuaMsApp::SetSetting)


					//////////////////////////////////////////////////////////////////////////
		 			+mluabind::Function("GetPublicDataPath", &CLuaMsApp::GetPublicDataPath)
		 			+mluabind::Function<tstring,LPCTSTR,LPCTSTR,LPCTSTR>("GetPublicData", &CLuaMsApp::GetPublicData)
					+mluabind::Function<DWORD,LPCTSTR,LPCTSTR,DWORD>("GetPublicData", &CLuaMsApp::GetPublicData)
					+mluabind::Function<BOOL, LPCTSTR,LPCTSTR,LPCTSTR>("SetPublicData", &CLuaMsApp::SetPublicData)
					+mluabind::Function<BOOL, LPCTSTR,LPCTSTR,DWORD>("SetPublicData", &CLuaMsApp::SetPublicData)
					+mluabind::Function<tstring,LPCTSTR,LPCTSTR>("GetPubSetting", &CLuaMsApp::GetPubSetting)
					+mluabind::Function<DWORD,LPCTSTR,DWORD>("GetPubSetting", &CLuaMsApp::GetPubSetting)
					+mluabind::Function<BOOL,LPCTSTR,LPCTSTR>("SetPubSetting", &CLuaMsApp::SetPubSetting)
					+mluabind::Function<BOOL,LPCTSTR,DWORD>("SetPubSetting", &CLuaMsApp::SetPubSetting)
					+mluabind::Function<tstring>("GetDiskSerial", &CLuaMsApp::GetDiskSerial)
					+mluabind::Function<tstring>("GetUserGuid", &CLuaMsApp::GetUserGuid)



					//////////////////////////////////////////////////////////////////////////
				


					//////////////////////////////////////////////////////////////////////////
					//

		 			);

		return TRUE;
	}




};