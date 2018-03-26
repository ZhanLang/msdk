#pragma once
#include "util/vercmp.h"
class CVerCmpLua
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("VerCmp")
				+mluabind::Class<CVerCmp>("CVerCmp")
					.Constructor<LPCSTR>()
					.Constructor<LPCWSTR>()
					.Method<ULONG>("GetMinVersion", &CVerCmp::GetMinVersion)
					.Method<ULONG>("GetMaxVersion", &CVerCmp::GetMaxVersion)
					.Method<ULONGLONG>("GetVersion", &CVerCmp::GetVersion)

				+mluabind::Function("Compare", &CVerCmp::Compare)
				+mluabind::Function<INT,LPCSTR,LPCSTR>("CompareStr", &CVerCmp::CompareStr)
				+mluabind::Function("CompareMax", &CVerCmp::CompareMax)
				+mluabind::Function("CompareStrMax", &CVerCmp::CompareStrMax)
				+mluabind::Function("CompareMin", &CVerCmp::CompareMin)
				+mluabind::Function("CompareStrMin", &CVerCmp::CompareStrMin)
			);

		return TRUE;
	}
};