#include "StdAfx.h"
#include "Helper.h"

namespace Helper
{

	void GetExePath(std::wstring&  strExePath)
	{
		WCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL,path,MAX_PATH);
		WCHAR *p = wcsrchr(path,L'\\');
		if(p)
		{
			++p;
			*p = L'\0';
			strExePath = path;
		}
	}
	void GetExePath(std::string&  strExePath)
	{
		CHAR path[MAX_PATH] = {0};
		GetModuleFileNameA(NULL,path,MAX_PATH);
		CHAR *p = strrchr(path,'\\');
		if(p)
		{
			++p;
			*p = '\0';
			strExePath = path;
		}
	}

}