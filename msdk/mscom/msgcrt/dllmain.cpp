// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "msgcrt.h"
USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_PROGID(CLSID_MsgCenter , CMsgCenter , _T("MsgCenter.1"))
	CLIDMAPENTRY_END
END_CLIDMAP
DEFINE_ALL_EXPORTFUN
