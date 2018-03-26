
//=============================================================================

#pragma once

#define SYSLAY_LINK_STATIC
#define STDLOG_LINK_STATIC

#include "syslay/dbg.h"
#include "stdlog.h"

//现在只使用静态库的版本，不再依赖stdlog.dll和syslay.dll
#ifdef	_DEBUG 
#pragma comment(lib, "syslaysD.lib")	
#pragma comment(lib, "stdlogsD.lib")	
#else
#pragma comment(lib, "syslays.lib")	
#pragma comment(lib, "stdlogs.lib")
#endif

class CDumpLib
{
public:
	CDumpLib()
	{
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		InitDumpLib(0);
	}
	~CDumpLib()
	{
		TermDumpLib();
	}
};
//__declspec(selectany) CDumpLib s_dbg;
extern CDumpLib s_dbg;


//包含此文件以后，可以在任何app.exe 的后面加参数如：
// app.exe -time -pid -tid -dbgstr -cons -group updatec 5 -mt -file
//
