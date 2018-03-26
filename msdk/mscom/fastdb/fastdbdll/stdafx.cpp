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

VOID RSLOG(const wchar_t* module_name,int level,const wchar_t* debug_string)  
{
	int nLen = wcslen(debug_string);
	std::wstring strTemp;
	strTemp += module_name;
	strTemp += L":";
	if(nLen < MAX_PATH)
	{
		strTemp += debug_string;
		OutputDebugStringW(strTemp.c_str());
		return;
	}
	WCHAR tempBuffer[MAX_PATH];
	memset(tempBuffer,0,MAX_PATH * 2);
	memcpy(tempBuffer,debug_string,MAX_PATH * 2 - 10);
	strTemp += tempBuffer;
	OutputDebugStringW(strTemp.c_str());
}
