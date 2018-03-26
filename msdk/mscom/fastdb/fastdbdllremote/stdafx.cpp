#include "stdafx.h"


VOID MyLogOut(LPCSTR dataMsg, ...)
{
	CHAR lpszMessage[1024] = {0};
	va_list arglist; 	
	va_start(arglist, dataMsg);
	_vsnprintf_s(lpszMessage, 1024 - 1, dataMsg, arglist);
	//替换成rsdblib中导出的log函数
	va_end(arglist);	

	OutputDebugStringA(lpszMessage);

};
