// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "downmgr.h"
#include "HttpDownload.h"
#include "combase/filestream.h"
#include "httpdownloadtask.h"
DOWNLOAD_API IDownLoadMgr*  CreateDownLoadMgr(IDownLoadNotify* pNotify,LPCWSTR lpNameSpace /*= NULL*/,BOOL bCache /*=TRUE*/)
{
	return NULL;
}

DOWNLOAD_API VOID DistoryDownLoadMgr(IDownLoadMgr** ppMgr)
{
	
}


USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_NOROT(CLSID_HttpDownlod,CHttpDownload)
		CLIDMAPENTRY_NOROT(CLSID_MsFileStream,CFileStream<CNullCriticalSection>)
		CLIDMAPENTRY_NOROT(CLSID_HttpDwonloadTask,CHttpDwonloadTask)
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN;
