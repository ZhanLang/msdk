// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "fsHttpConnection.h"
#include "fsInternetSession.h"
#include "fsHttpFile.h"
#include "fsFtpFile_i.h"
#include "fsFtpFile.h"
#include "fsFtpConnection.h"
#include "fsLocalFile_i.h"
#include "fsLocalFile.h"
#include "fsHttpFiles.h"
#include "fsInternetFiles_i.h"
#include "fsFtpFiles.h"
#include "INetBaseMgr.h"
#include "fsinet_i.h"


USE_DEFAULT_DLL_MAIN

BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_NOROT_PROGID(CLISD_fsInternetSession,fsInternetSession , _T("Porg.fsInternetSession.1"))

		CLIDMAPENTRY_NOROT_PROGID(CLSID_fsHttpConnection,fsHttpConnection , _T("Porg.fsHttpConnection.1"))
		CLIDMAPENTRY_NOROT_PROGID(CLSID_fsHttpFile,fsHttpFile , _T("Porg.fsHttpFile.1"))

		CLIDMAPENTRY_NOROT_PROGID(CLISD_fsFtpConnection,fsFtpConnection , _T("Porg.fsFtpConnection.1"))
		CLIDMAPENTRY_NOROT_PROGID(CLSID_fsFtpFile,fsFtpFile , _T("Porg.fsFtpFile.1"))
		
		CLIDMAPENTRY_NOROT_PROGID(CLSID_fsLocalFile , fsLocalFile , _T("Porg.fsLocalFile.1"))

		CLIDMAPENTRY_NOROT_PROGID(CLISD_fsHttpFiles , fsHttpFiles , _T("Porg.fsHttpFiles.1"))
		CLIDMAPENTRY_NOROT_PROGID(CLSID_fsFtpFiles , fsFtpFiles , _T("Porg.fsFtpFiles.1"))

		CLIDMAPENTRY_NOROT_PROGID(CLSID_INetBaseMgr , CINetBaseMgr , _T("Porg.CINetBaseMgr.1"))
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN


EXTERN_C __declspec(dllexport) HRESULT CreateINetBaseMgr(msinet::IINetBaseMgr** lpMgr)
{
	return __MS_DllGetClassObject(CLSID_INetBaseMgr , re_uuidof(IINetBaseMgr) , (LPVOID*)lpMgr , NULL);
}