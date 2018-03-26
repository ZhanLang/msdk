// testLog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
//#include "mslog/stdlog_s.h"
#include "mslog/stdlog_dll.h"

void func()
{
	PROFILE_SCOPE(func);

	Msg(_T("in func...\n"));
	AssertMsg(FALSE, _T("OYE"));

	Msg(_T("out func...\n"));
}

//性能测试
void func2()
{
	PROFILE_SCOPE(func2);
	Msg(_T("test func2...\n"));
}

//因为有可能全局类的构造和析构函数都中会打印日志，
//因此，我们尽早初始化，并且把反初始化尽量也放到最后去做


BOOL SetConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bResult = SetConsoleTextAttribute(hConsole, wAttributes);

	//CloseHandle(hConsole);
	return bResult;
}

//#include "locale.h"
int _tmain(int argc, _TCHAR* argv[])
{
	//CDumpLib dumpLib;
	//	setlocale( LC_ALL, "chs" );

	//命令行: -dbgstr -cons -group app2 2 onlyu 3 -pdb -mt -file

	SetConsoleColor(  FOREGROUND_GREEN    | FOREGROUND_BLUE          | FOREGROUND_INTENSITY);
	std::cout << "控制台下默认文字背景颜色:\n";
	  printf("控制台下默认文字背景颜色:\n");
	CPUInformation ci=Sys_CPUInfo();
	MsgA("CPU ID=%s, speed=%I64d(%dM)HZ,over!\n",ci.m_szProcessorID, ci.m_Speed, (ci.m_Speed/100000000L)*100);

	Msg(_T("app2=2, onlyu=3\n"));
	
	//Releaes版不会运行的！
	AssertFunc(FALSE, func());

	for(int i=0;i<10;i++)
		func2();

	GrpMsg(_T("app2"), 1, _T("test group msg app2 at level: 1\n"));
	GrpMsg(_T("app2"), 2, _T("test group msg app2 at level: 2\n"));
	GrpMsg(_T("app2"), 3, _T("test group msg app2 at level: 3\n"));
	GrpMsg(_T("app2"), 4, _T("test group msg app2 at level: 4\n"));
	GrpMsg(_T("onlyu"), 1, _T("test group msg onlyu at level: 1\n"));
	GrpMsg(_T("onlyu"), 2, _T("test group msg onlyu at level: 2\n"));
	GrpMsg(_T("onlyu"), 3, _T("test group msg onlyu at level: 3\n"));
	GrpMsg(_T("onlyu"), 4, _T("test group msg onlyu at level: 4\n"));

	//程序中动态的改变日志输出级别
	DbgActivate(_T("app2"), 3);
	DbgActivate(_T("onlyu"), 4);

	Msg(_T("app2=3, onlyu=4,...\n"));

	GrpMsg(_T("app2"), 1, _T("test group msg app2 at level: 1\n"));
	GrpMsg(_T("app2"), 2, _T("test group msg app2 at level: 2\n"));
	GrpMsg(_T("app2"), 3, _T("test group msg app2 at level: 3\n"));
	GrpMsg(_T("app2"), 4, _T("test group msg app2 at level: 4\n"));
	GrpMsg(_T("onlyu"), 1, _T("test group msg onlyu at level: 1\n"));
	GrpMsg(_T("onlyu"), 2, _T("test group msg onlyu at level: 2\n"));
	GrpMsg(_T("onlyu"), 3, _T("test group msg onlyu at level: 3\n"));
	GrpMsg(_T("onlyu"), 4, _T("test group msg onlyu at level: 4\n"));

	return 0;
}