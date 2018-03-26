#include "StdAfx.h"
#include "StringConvert.h"
#include "luahost/luahost.h"

CStringConvert::CStringConvert(void)
{
}


CStringConvert::~CStringConvert(void)
{
}

BOOL CStringConvert::BindToLua( ILuaVM* pLuaVM )
{
	GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("convert")
		
		+mluabind::Function("wstring_to_sting", &CStringConvert::wstring_to_sting)
		+mluabind::Function("string_to_wstring", &CStringConvert::string_to_wstring)
		+mluabind::Function("wstring_to_string_utf8", &CStringConvert::wstring_to_string_utf8)
		+mluabind::Function("utf8_string_to_wstring", &CStringConvert::utf8_string_to_wstring)
		);

	return TRUE;
}

std::wstring CStringConvert::string_to_wstring( std::string s )
{
	USES_CONVERSION;
	return A2W(s.c_str());
}

std::string CStringConvert::wstring_to_sting( std::wstring s )
{
	USES_CONVERSION;
	return W2A(s.c_str());
}

std::string CStringConvert::wstring_to_string_utf8(const WCHAR* SourceStr)
{
	std::string str ="";
	int  nSize = WideCharToMultiByte(CP_UTF8, 0,SourceStr, -1, NULL, 0,NULL,NULL);
	CHAR* buff = new CHAR[nSize+1];
	if(NULL == buff) return str;
	WideCharToMultiByte(CP_UTF8, 0,SourceStr, -1, buff, nSize+1,NULL,NULL);
	str = buff;
	delete[] buff;
	return str;
}

std::wstring CStringConvert::utf8_string_to_wstring(const char* SourceStr)
{
	std::wstring str =_T("");
	int  nSize = MultiByteToWideChar(CP_UTF8, 0,SourceStr, -1, NULL, 0);
	WCHAR* buff = new WCHAR[nSize+1];
	if(NULL == buff) return str;
	MultiByteToWideChar(CP_UTF8, 0,SourceStr, -1, buff, nSize+1);
	str = buff;
	delete[] buff;
	return str;
}
