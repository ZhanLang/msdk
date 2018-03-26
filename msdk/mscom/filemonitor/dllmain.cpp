// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "FileMonitor.h"
#include "FileMonitorSrv.h"
#include "DiskMonitor.h"
USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_NOROT(CLSID_FileMonitor,CFileMonitor)
		CLIDMAPENTRY(CLSID_FileMonitorSrv,CFileMonitorSrv)
		CLIDMAPENTRY(CLSID_DiskMonitor,CDiskMonitor)
		
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN;