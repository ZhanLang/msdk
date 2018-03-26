
#include "stdafx.h"
#include "bvdownload.h"
#include "ExceptionReport.h"
CExceptionReport gReport;
CAppModule Module;

/*
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{

	
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(194);
	

	if (bvWnd->Create(NULL))
	{
		bvWnd->ShowWindow(SW_SHOW);
		theLoop.Run();
	}
	Module.Term();
	delete bvWnd;
	return 0;
}
*/

USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY(CLSID_BvDownload, CBVDownloadWindow)
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN;