#include "StdAfx.h"
#include "WinIni.h"
#include "luahost\luahost.h"

namespace winfunc {
	TString CWinIni::GetIniString( LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName )
	{
		TCHAR szValue[MAX_PATH];
		::RtlZeroMemory(szValue, sizeof(TCHAR) * MAX_PATH);
		::GetPrivateProfileString(lpSectionName, lpKeyName, TEXT(""), szValue, MAX_PATH, lpFileName);
		TString strValue = szValue;
		return strValue.c_str();
	}

	bool CWinIni::SetIniString( LPCTSTR lszFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpszValue )
	{
		msapi::CreateDirectoryEx(lszFileName);
		return BOOL2bool(WritePrivateProfileString(lpSectionName, lpKeyName, lpszValue, lszFileName));
	}

	

}


