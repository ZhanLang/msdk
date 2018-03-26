#pragma once
#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>
#include <util/registry.h>
class CLuaMsRegBand
{
public:

	enum KEY
	{
		HLM,
		HCU,
		HCR,
	};

	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("msapi")
			+mluabind::Enum("HLM", HLM)
			+mluabind::Enum("HCU", HCU)
			+mluabind::Enum("HCR", HCR)
			+mluabind::Function<>("RegGerString", &GetRegString)
			+mluabind::Function("RegSetString", &RegSetString)
			+mluabind::Function("RegGetDowrd", &RegGetDowrd)
			+mluabind::Function("RegSetDword", &RegSetDword)
			+mluabind::Function("RegDeleteKey", &RegDeleteKey)
			+mluabind::Function("RegDeleteValue", &RegDeleteValue)
			);


		return TRUE;
	}

	static ULONG RegDeleteValue( KEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueNale, BOOL b64)
	{
		return CRegistry::DeleteValue(GetKey(hRoot), lpszSubKey, lpszValueNale, b64);
	}

	static ULONG RegDeleteKey( KEY hRoot, LPCTSTR lpszSubKey )
	{
		return CRegistry::DeleteKey(GetKey(hRoot), lpszSubKey);
	}

	static ULONG RegSetDword(KEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwValue)
	{
		return CRegistry::SetDword( GetKey(hRoot), lpszSubKey, lpszValueName, dwValue);
	}

	static DWORD RegGetDowrd(KEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName)
	{
		return CRegistry::GetDowrd(GetKey(hRoot), lpszSubKey, lpszValueName, NULL);
	}

	static LONG RegSetString(KEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPCTSTR lpszValue)
	{
		return CRegistry::SetString(GetKey(hKey), lpszSubKey, lpszValueName, lpszValue);
	}

	static fstring GetRegString(KEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, BOOL b64)
	{
		return CRegistry::GetString(GetKey(hKey), lpszSubKey, lpszValueName,b64);
	}

	static HKEY GetKey( KEY hKey)
	{
		switch( hKey )
		{
		case HLM:
			return HKEY_LOCAL_MACHINE;
		case HCU:
			return HKEY_CURRENT_USER;
		case HCR:
			return HKEY_CLASSES_ROOT;
		}

		return NULL;
	}
};