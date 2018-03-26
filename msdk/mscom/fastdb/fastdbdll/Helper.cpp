#include "StdAfx.h"
#include "Helper.h"

namespace Helper
{
	void GetDllPath(std::wstring& strDllName,std::wstring&  strDllPath)
	{
		WCHAR path[MAX_PATH] = {0};
		HMODULE hDll = GetModuleHandle(strDllName.c_str());
		if(hDll)
		{
			GetModuleFileName(hDll,path,MAX_PATH);
			WCHAR *p = wcsrchr(path,L'\\');
			if(p)
			{
				++p;
				*p = L'\0';
				strDllPath = path;
			}
		}

	}

	void GetDllPath(std::string& strDllName,std::string&  strDllPath)
	{
		CHAR path[MAX_PATH] = {0};
		HMODULE hDll = GetModuleHandleA(strDllName.c_str());
		if(hDll)
		{
			GetModuleFileNameA(hDll,path,MAX_PATH);
			CHAR *p = strrchr(path,'\\');
			if(p)
			{
				++p;
				*p = '\0';
				strDllPath = path;
			}
		}

	}


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