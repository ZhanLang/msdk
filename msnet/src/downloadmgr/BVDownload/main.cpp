
#include "stdafx.h"
#include "bvdownload.h"
#include "ExceptionReport.h"
CExceptionReport gReport;
CAppModule Module;
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{

	
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(194);
	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	CBVDownloadWindow *bvWnd = new CBVDownloadWindow();
	Module.Init(NULL , hInstance);
	CMessageLoop theLoop;
	Module.AddMessageLoop(&theLoop);

	if (bvWnd->Create(NULL))
	{
		bvWnd->ShowWindow(SW_SHOW);
		theLoop.Run();
	}
	Module.Term();
	delete bvWnd;
	return 0;
}
