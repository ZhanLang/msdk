#include "StdAfx.h"
#include "ParseCommandToLua.h"


// CParseCommandToLua::CParseCommandToLua(void)
// {
// }
// 
// 
// CParseCommandToLua::~CParseCommandToLua(void)
// {
// }

BOOL CParseCommandToLua::BindToLua(ILuaVM* pLuaVm)
{
	RASSERT(pLuaVm, FALSE);

	GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("winfunc")
		+mluabind::Class<CParseCommandToLua>("CParseCommand")
		.Constructor()
		.Constructor<bool>()

		.Method<bool, LPCSTR>("ParseCmd", &CParseCommandToLua::ParseCmd)
		.Method<bool>("IsExist", &CParseCommandToLua::IsExist)
		.Method("GetArgCount", &CParseCommandToLua::GetArgCount)


		.Method<LPCSTR, LPCSTR>("GetArg", &CParseCommandToLua::GetArg)
		.Method<LPCSTR, LPCSTR, LPCSTR>("GetArgDef", &CParseCommandToLua::GetArgDef)

		.Method<INT, LPCSTR>("GetArgInt", &CParseCommandToLua::GetArgInt)
		.Method<INT, LPCSTR, INT>("GetArgIntDef", &CParseCommandToLua::GetArgIntDef)

		.Method("GetExeFilePath", &CParseCommandToLua::GetExeFilePath)
		
		
		.Method<VOID,LPCSTR, LPCSTR>("SetArg", &CParseCommandToLua::SetArg)
		.Method<VOID, LPCSTR, INT>("SetIntArg", &CParseCommandToLua::SetIntArg)

		.Method("GetCmdLine", &CParseCommandToLua::GetCmdLine)

		.Method("Clear", &CParseCommandToLua::Clear)

		);


	return TRUE;
}
