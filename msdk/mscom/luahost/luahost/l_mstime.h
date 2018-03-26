#pragma once

#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>

#include <msapi/mstime.h>

class CLuaMsTimeBand
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("mstime")
			+mluabind::Function<tstring>("GetCurrentDataTimeStr", &GetCurrentDataTimeStr)
			+mluabind::Function<tstring>("GetCurrentDateStr", &GetCurrentDateStr)
			+mluabind::Function<tstring>("GetCurrentTimeStr", &GetCurrentTimeStr)
			);

		return TRUE;
	}

	static tstring GetCurrentDataTimeStr()
	{
		TCHAR szTime[ MAX_PATH ] = { 0 };
		msapi::GetCurrentDataTimeStr( szTime, MAX_PATH);
		return szTime;
	}

	static tstring GetCurrentDateStr(bool bSplit)
	{
		TCHAR szTime[ MAX_PATH ] = { 0 };
		msapi::GetCurrentDateStr( szTime, MAX_PATH, bool2BOOL(bSplit));
		return szTime;
	}

	static tstring GetCurrentTimeStr(bool bSplit)
	{
		TCHAR szTime[ MAX_PATH ] = { 0 };
		msapi::GetCurrentTimeStr( szTime, MAX_PATH, bool2BOOL(bSplit));
		return szTime;
	}
};