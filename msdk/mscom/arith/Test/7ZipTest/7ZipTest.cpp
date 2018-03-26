// 7ZipTest.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "7ZipTest.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,  
					   HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	GrpMsg(GroupName, MsgLevel_Msg, _T("main"));
	CMSComLoader ZipModule;
	ZipModule.SetMoudlePath(_T("7Zip.dll"));
	BOOL bRet = ZipModule.LoadMSCom();
	if (!bRet)
	{
		GrpError(GroupName, MsgLevel_Error, _T("load 7Zip.dll faild"));
		return -1;
	}

	return 0;
}