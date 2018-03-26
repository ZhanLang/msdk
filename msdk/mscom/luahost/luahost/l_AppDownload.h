#pragma once
#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>

#include <AppDownload.h>
class CLuaAppDownload
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("msapp")
			+mluabind::Class<CAppDownload>("CAppDownload")
			.Constructor<LPCSTR,LPCSTR, LPCSTR , DWORD>()
			.Method<BOOL,BOOL>("Do", &CAppDownload::Do)
			.Method<>("Exit",&CAppDownload::Exit)
				);

		return TRUE;
	}
};