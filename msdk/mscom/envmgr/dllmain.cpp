#include "stdafx.h"
#include "ObjectLoader.h"
#include "RunningObjectTable.h"
#include "MscomEnv.h"
#include "ObjectRun.h"
#include "MainRun.h"


USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY(CLSID_MsEnv , CMscomEnv)
		CLIDMAPENTRY(CLSID_ObjectLoader,CObjectLoader)
		CLIDMAPENTRY(CLSID_RunningObjectTable,CRunningObjectTable)
		CLIDMAPENTRY(CLSID_MainRun,CMainRun)
	CLIDMAPENTRY_END
END_CLIDMAP
DEFINE_ALL_EXPORTFUN