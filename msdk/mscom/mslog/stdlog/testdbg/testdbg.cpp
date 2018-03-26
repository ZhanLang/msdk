// stdlog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mslog/stdlog_dll.h"

void testMBS()
{
	unsigned char pBuffer[]={"123ÈðÐÇ456"};

	MsgA("test MBS,...\n");
	MsgA("test msg\n");
	WarningA("test warning\n");	
	ReportA(pBuffer, sizeof(pBuffer));

	DbgActivateA("app2", 2);
	DbgActivateA("onlyu", 3);

	MsgA("app2=2, onlyu=3\n");

	GrpMsgA("app2", 1, "test group msg app2 at level: 1\n");
	GrpMsgA("app2", 2, "test group msg app2 at level: 2\n");
	GrpMsgA("app2", 3, "test group msg app2 at level: 3\n");
	GrpMsgA("app2", 4, "test group msg app2 at level: 4\n");
	GrpMsgA("onlyu", 1, "test group msg onlyu at level: 1\n");
	GrpMsgA("onlyu", 2, "test group msg onlyu at level: 2\n");
	GrpMsgA("onlyu", 3, "test group msg onlyu at level: 3\n");
	GrpMsgA("onlyu", 4, "test group msg onlyu at level: 4\n");

	DbgActivateA("app2", 0);
	DbgActivateA("onlyu", 1);

	MsgA("app2=0, onlyu=1,...\n");

	GrpMsgA("app2", 1, "test group msg app2 at level: 1\n");
	GrpMsgA("app2", 2, "test group msg app2 at level: 2\n");
	GrpMsgA("app2", 3, "test group msg app2 at level: 3\n");
	GrpMsgA("app2", 4, "test group msg app2 at level: 4\n");
	GrpMsgA("onlyu", 1, "test group msg onlyu at level: 1\n");
	GrpMsgA("onlyu", 2, "test group msg onlyu at level: 2\n");
	GrpMsgA("onlyu", 3, "test group msg onlyu at level: 3\n");
	GrpMsgA("onlyu", 4, "test group msg onlyu at level: 4\n");
}

//#include "locale.h"
int _tmain(int argc, _TCHAR* argv[])
{
//	setlocale( LC_ALL, "chs" );

	wchar_t pBuffer[]={L"123ÈðÐÇ456"};

	Msg(_T("123ÈðÐÇ456\n"));

	Msg(_T("test msg\n"));
	Warning(_T("test warning\n"));
	Report((unsigned char *)pBuffer, sizeof(pBuffer));

	//DbgActivate(_T("app2"), 2);
	//DbgActivate(_T("onlyu"), 3);
	
	Msg(_T("app2=2, onlyu=3\n"));

	GrpMsg(_T("app2"), 1, _T("test group msg app2 at level: 1\n"));
	GrpMsg(_T("app2"), 2, _T("test group msg app2 at level: 2\n"));
	GrpMsg(_T("app2"), 3, _T("test group msg app2 at level: 3\n"));
	GrpMsg(_T("app2"), 4, _T("test group msg app2 at level: 4\n"));
	GrpMsg(_T("onlyu"), 1, _T("test group msg onlyu at level: 1\n"));
	GrpMsg(_T("onlyu"), 2, _T("test group msg onlyu at level: 2\n"));
	GrpMsg(_T("onlyu"), 3, _T("test group msg onlyu at level: 3\n"));
	GrpMsg(_T("onlyu"), 4, _T("test group msg onlyu at level: 4\n"));

	DbgActivate(_T("app2"), 0);
	DbgActivate(_T("onlyu"), 1);

	Msg(_T("app2=0, onlyu=1,...\n"));

	GrpMsg(_T("app2"), 1, _T("test group msg app2 at level: 1\n"));
	GrpMsg(_T("app2"), 2, _T("test group msg app2 at level: 2\n"));
	GrpMsg(_T("app2"), 3, _T("test group msg app2 at level: 3\n"));
	GrpMsg(_T("app2"), 4, _T("test group msg app2 at level: 4\n"));
	GrpMsg(_T("onlyu"), 1, _T("test group msg onlyu at level: 1\n"));
	GrpMsg(_T("onlyu"), 2, _T("test group msg onlyu at level: 2\n"));
	GrpMsg(_T("onlyu"), 3, _T("test group msg onlyu at level: 3\n"));
	GrpMsg(_T("onlyu"), 4, _T("test group msg onlyu at level: 4\n"));

	testMBS();

	Error(_T("Press any key to exit"));
	return 0;
}

