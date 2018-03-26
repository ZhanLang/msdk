#include "StdAfx.h"
#include "LuaLog.h"
#include "mslog\logtool\logtool.h"

bool log(std::string sLog)
{
	GrpLogA(GroupNameA, MsgLevel_Log, sLog.c_str());
	return true;
}

bool error(std::string sErr)
{
	GrpErrorA(GroupNameA, MsgLevel_Error, sErr.c_str());
	return true;
}

bool warning(std::string sWarn)
{
	GrpWarningA(GroupNameA, MsgLevel_Warning, sWarn.c_str());
	return true;
}

bool dbg(std::string sDbg)
{
	USES_CONVERSION;
	ZM1_GrpDbgOutput(GRP_NAME, A2W(sDbg.c_str()));
	return true;
}

bool print(std::string s)
{
	OutputDebugStringA(s.c_str());
	OutputDebugStringA("\r\n");
	return true;
}
