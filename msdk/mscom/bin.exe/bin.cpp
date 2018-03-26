// bin.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "mscom/mscominc.h"
#include "mslog/logtool/logtool.h"
#include "exceptionreport/ExceptionReport.h"
/*#include "util/mspath.h"*/

#define GRP_NAME  _T("BIN.EXE")
using namespace mscom;
using namespace msdk;
CExceptionReport ExceptionReport;

/*
LPCTSTR lpszRotXml = 
L"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	L"<mscomcfg>"
		L"<mainplugin>"
			L"<class clsid=\"570DCE20-EEF2-4327-A8C0-890081499806\" name=\"CLSID_NetUi\"/>"
		L"</mainplugin>"
	L"</mscomcfg>";

LPCTSTR lpszMsComXml =
L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	L"<mscomcfg>"
		L"<moudle guid=\"BFE29572-9848-479a-9A25-54DE01FCAEC0\" path=\".\\netui.dll\">"
			L"<class clsid=\"570DCE20-EEF2-4327-A8C0-890081499806\" name=\"CLSID_NetUi\"/>"
		L"</moudle>"
	L"</mscomcfg>";
*/


#include <msapi/mswinapi.h>


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

#ifdef DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//_CrtSetBreakAlloc(9079);
	{//需要强制设置一下当前工作目录
		TCHAR lpszCurrpath[MAX_PATH] = {0};
		SetCurrentDirectory(mspath::CPath::GetCurrentPath(lpszCurrpath , MAX_PATH));
	}
	
	CMSComLoader mscomLoader;
	
	if (!mscomLoader.LoadMSCom(_T(".\\Common File\\mscom\\envmgr.dll")))
	{
		ZM1_GrpDbgOutput(GRP_NAME , _T("load envmgr.dll faild."));
		return 0;
	}

	UTIL::com_ptr<IMsEnvMgr> pEnv = INULL;

	if (FAILED(mscomLoader.CreateInstance(CLSID_MsEnv , NULL , NULL , re_uuidof(IMsEnvMgr) , (void**)&pEnv.m_p)))
	{
		ZM1_GrpDbgOutput(GRP_NAME , _T("create env object faild."));
		return 0;
	}


	CString strProcName;
	GetModuleFileName(NULL, strProcName.GetBufferSetLength(MAX_PATH) ,MAX_PATH);
	strProcName.ReleaseBuffer();
	strProcName = strProcName.Mid(strProcName.ReverseFind('\\')+1);
	strProcName = strProcName.Left(strProcName.ReverseFind('.'));
	strProcName.Append(_T(".xml"));


	if (FAILED(pEnv->Init(_T("mscomcfg.xml") , /*lpszMsComXml*/ NULL , strProcName ,/*lpszRotXml*/ NULL , hInstance)))
	{
		//ZM1_GrpDbgOutput(GRP_NAME , _T("init env object faild.\r\n"));
		HRESULT hr =  pEnv->Uninit();

		pEnv = INULL;
		return hr;
	}
	else
	{
		if (FAILED( pEnv->Start()))
		{
			//ZM1_GrpDbgOutput(GRP_NAME,_T("start plugin faild.\r\n"));
		}

		if (FAILED(pEnv->Stop()))
		{
			//ZM1_GrpDbgOutput(GRP_NAME,_T("stop plugin faild.\r\n"));
		}
	}

	HRESULT hr = pEnv->Uninit();
	SAFE_RELEASE(pEnv)
	return hr;
}
