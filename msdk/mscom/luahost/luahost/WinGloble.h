#pragma once
#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>


class CBindWinGlobel
{
public:

	enum
	{
		HLM = (DWORD)HKEY_LOCAL_MACHINE,
	};
	static BOOL BindToLua( ILuaVM* pLuaVM )
	{
		return TRUE;
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("win")
			+mluabind::Enum("HLM", (DWORD)HKEY_LOCAL_MACHINE)
			+mluabind::Enum("HCU", (DWORD)HKEY_CURRENT_USER)
			+mluabind::Enum("HCU", (DWORD)HKEY_CLASSES_ROOT)
			);

		return TRUE;
	}
};