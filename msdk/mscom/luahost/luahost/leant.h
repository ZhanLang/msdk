#pragma once
//√∂æŸ…±»Ì

#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>

#include <msapi/EnumAntivirus.h>
class CLuaEnumAntivirus
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("ant")
			+mluabind::Enum("Antivirus_Rav", msapi::Antivirus_Rav)
			+mluabind::Enum("Antivirus_King", msapi::Antivirus_King)
			+mluabind::Enum("Antivirus_360Safe", msapi::Antivirus_360Safe)
			+mluabind::Enum("Antivirus_360Sd", msapi::Antivirus_360Sd)
			+mluabind::Enum("Antivirus_Biadu", msapi::Antivirus_Biadu)
			+mluabind::Enum("Antivirus_BaiduSd", msapi::Antivirus_BaiduSd)
			+mluabind::Enum("Antivirus_QQMgr", msapi::Antivirus_QQMgr)
			+mluabind::Enum("Antivirus_HipsDaemon", msapi::Antivirus_HipsDaemon)
			+mluabind::Enum("Antivirus_2345Safe", msapi::Antivirus_2345Safe)

			+mluabind::Function("Enum", Enum)
			+mluabind::Function("Has", Has)
			);
		return TRUE;
	}

	static std::wstring Enum()
	{
		return msapi::CEnumAntivirus().Enum();
	}

	static bool Has( INT antivirus) 
	{
		return msapi::CEnumAntivirus().Has(antivirus) == TRUE;
	}
};