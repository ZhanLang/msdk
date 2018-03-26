
#pragma once
#include <luahost/ILuaHost.h>
#include <luahost/luahost.h>
#include <msapi/EnumWnd.h>
#include <msapi/mswinapi.h>
class CLuaWindDef
{
public:
	static BOOL BindToLua(ILuaVM* pLuaVM)
	{
		GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("WindDef")

			//SERVICE_CONTROL
			+mluabind::Enum("SERVICE_CONTROL_SESSIONCHANGE",SERVICE_CONTROL_SESSIONCHANGE)
			+mluabind::Enum("SERVICE_CONTROL_STOP", SERVICE_CONTROL_STOP)

			//WM_WTSSESSION_CHANGE
			+mluabind::Enum("WTS_SESSION_LOGON",WTS_SESSION_LOGON)
			+mluabind::Enum("WTS_SESSION_LOGOFF", WTS_SESSION_LOGOFF)
			

			+mluabind::Enum("SECURITY_MANDATORY_HIGH_RID", SECURITY_MANDATORY_HIGH_RID)


			//////////////////////////////////////////////////////////////////////////
			//WND

			+mluabind::Enum("SW_HIDE", SW_HIDE)
			+mluabind::Enum("SW_SHOW",SW_SHOW)
			+mluabind::Enum("SW_SHOWNA",SW_SHOWNA)
			+mluabind::Enum("SW_SHOWNORMAL",SW_SHOWNORMAL)
			+mluabind::Enum("EnumWnd_Class", msapi::EnumWnd_Class)
			+mluabind::Enum("EnumWnd_Name", msapi::EnumWnd_Name)
			+mluabind::Enum("EnumWnd_All", msapi::EnumWnd_All)


			//////////////////////////////////////////////////////////////////////////
			//

			+mluabind::Enum("MicroVersion_Unknown"	,msapi::MicroVersion_Unknown)
			+mluabind::Enum("MicroVersion_Lower"	,msapi::MicroVersion_Lower)
			+mluabind::Enum("MicroVersion_WinXP"	,msapi::MicroVersion_WinXP)
			+mluabind::Enum("MicroVersion_WinXP_SP1",msapi::MicroVersion_WinXP_SP1)
			+mluabind::Enum("MicroVersion_WinXP_SP2",msapi::MicroVersion_WinXP_SP2)
			+mluabind::Enum("MicroVersion_WinXP_SP3",msapi::MicroVersion_WinXP_SP3)
			+mluabind::Enum("MicroVersion_Vista"	,msapi::MicroVersion_Vista)
			+mluabind::Enum("MicroVersion_Vista_SP1",msapi::MicroVersion_Vista_SP1)
			+mluabind::Enum("MicroVersion_Vista_SP2",msapi::MicroVersion_Vista_SP2)
			+mluabind::Enum("MicroVersion_Win7"		,msapi::MicroVersion_Win7)
			+mluabind::Enum("MicroVersion_Win7_SP1"	,msapi::MicroVersion_Win7_SP1)
			+mluabind::Enum("MicroVersion_Win8"		,msapi::MicroVersion_Win8)
			+mluabind::Enum("MicroVersion_Win8_1"	,msapi::MicroVersion_Win8_1)
			+mluabind::Enum("MicroVersion_Win10"	,msapi::MicroVersion_Win10)
			+mluabind::Enum("MicroVersion_Advanced"	,msapi::MicroVersion_Advanced)
			);

		return TRUE;
	}
};