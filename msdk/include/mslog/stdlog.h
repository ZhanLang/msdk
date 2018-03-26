
#pragma once

#define STDLOG_LINK_STATIC
#define SYSLAY_LINK_STATIC

#include "stdlog/stdlog.h"
#include "syslay/dbg.h"
#include "syslay/platform.h"


#ifdef _MT
#	ifdef _DEBUG
#		pragma comment(lib, "stdlog_s_d_mt.lib")
#		pragma comment(lib, "syslay_s_d_mt.lib")
#	else
#		pragma comment(lib, "stdlog_s_r_mt.lib")
#		pragma comment(lib, "syslay_s_r_mt.lib")
#	endif
#else
#		pragma comment(lib, "stdlog.lib")
#		pragma comment(lib, "syslay.lib")
#endif


#ifndef S_STD_LOG_INIT
#define S_STD_LOG_INIT
static class CDumpLib
{
public:
	CDumpLib()
	{
		//通用的日志输出管理
		InitDumpLib(0);
	}
	~CDumpLib()
	{
		TermDumpLib();
	}
}s_log_init;
#endif
