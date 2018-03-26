// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DownloadMgr.h"
#include "HttpDownloadTask.h"
#include "DownloadTaskXL.h"
#include "HttpAsyncRequest.h"
#include "combase\MsBufferImp.h"
#include "HttpLuaEx.h"
#include <mscom\tproperty.h>
USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_NOROT(CLSID_MsBuffer, CMsBufferImp<>)
		CLIDMAPENTRY_NOROT(CLSID_HttpDownload, CHttpDownloadTask)
		CLIDMAPENTRY_NOROT(CLSID_XlDownload, CDownloadTaskXL)
		CLIDMAPENTRY_NOROT(CLSID_DownloadMgr,CDownloadMgr)
		CLIDMAPENTRY_PROGID(CLSID_CProperty2, CProperty2, _T("Property2.1"))
		//CLIDMAPENTRY(CLSID_HttpAsyncRequst, CHttpAsyncRequest)
		//CLIDMAPENTRY_NOROT(CLSID_HttpSyncRequest, CHttpSyncRequest)
		//CLIDMAPENTRY_NOROT(CLSID_Http_LuaEx, CHttpLuaEx)
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN;