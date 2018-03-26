// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DeviceManageFactory.h"
#include "OsInfoLuaEx.h"

USE_DEFAULT_DLL_MAIN
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_PROGID(CLSID_DeviceManageFactory, CDeviceManageFactory, _T("DeviceManageFactory.1"))
	//	CLIDMAPENTRY_PROGID(CLSID_OsInfoLuaEx, COsInfoLuaEx, _T("OsInfoLuaEx.1"))
	CLIDMAPENTRY_END
END_CLIDMAP
DEFINE_ALL_EXPORTFUN