#pragma once

#include <string>
namespace msdk{;

#ifndef fstring
#	ifdef UNICODE
#		define fstring std::wstring
#	else
#		define fstring std::string
#	endif//ifdef UNICODE
#endif//#ifndef fstring

class CRegistry
{
public:

	static fstring GetString(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwREGSAM  = KEY_QUERY_VALUE, LONG* lRet = NULL)
	{
		fstring sRet;
		DWORD dwSize = 0;
		DWORD dwType;

		LONG lTemp = GetValue(hRoot, lpszSubKey, lpszValueName, NULL, dwSize, dwType, dwREGSAM);
		if ( !(dwSize && lTemp == ERROR_SUCCESS))
		{
			if (lRet)
				*lRet = lTemp;

			return sRet;
		}


	
		LPBYTE pData = new BYTE[dwSize];
		while( (lTemp = GetValue(hRoot, lpszSubKey, lpszValueName, pData, dwSize, dwType, dwREGSAM)) == ERROR_MORE_DATA)
		{
			if (pData)
				delete[] pData;

			pData = new BYTE[dwSize];
		}

		if (lTemp == ERROR_SUCCESS)
		{
			if (dwType == REG_SZ)
				sRet = (LPCTSTR)pData;
			else if(dwType == REG_DWORD)
			{
				TCHAR temp[MAX_PATH] = { 0 };
				_stprintf_s(temp, MAX_PATH,_T("%d"), *((DWORD*)pData));
				sRet = temp;
			}
		}
		
		if (pData)
			delete[] pData;
		
		if (lRet)
			*lRet = lTemp;

		return sRet;
	}

	static LONG SetString(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPCTSTR lpszValue,DWORD dwREGSAM  = KEY_WRITE)
	{
		return SetValue(hRoot, lpszSubKey, lpszValueName, REG_SZ, (LPBYTE)lpszValue, (_tcslen(lpszValue) + 1) * sizeof(TCHAR), dwREGSAM);
	}

	static DWORD GetDowrd(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LONG* lRet)
	{
		DWORD dwRet = 0;
		DWORD dwSize = MAX_PATH;
		DWORD dwType =0;
		BYTE  pData[MAX_PATH] = { 0 };
		LONG lTemp = GetValue(hRoot, lpszSubKey, lpszValueName, pData, dwSize, dwType);

		if ( !(dwSize && lTemp == ERROR_SUCCESS))
		{
			if (lRet)
				*lRet = lTemp;

			return dwRet;
		}

		if(dwType == REG_SZ && dwSize <= sizeof(sizeof(DWORD)))
			dwRet = _ttol((LPCTSTR)pData);
		else if (dwType == REG_DWORD)
		{
			dwRet = *((DWORD*)pData);
		}
		
		if ( lRet )
			*lRet = lTemp;

		return dwRet;
	}


	static LONG SetDword(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwValue, DWORD dwREGSAM  = KEY_QUERY_VALUE)
	{
		return SetValue(hRoot, lpszSubKey, lpszValueName, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue), dwREGSAM);
	}

	static LONG GetValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPBYTE lpValue, DWORD& dwSize, DWORD& dwTpye , DWORD dwREGSAM  = KEY_QUERY_VALUE){
		HKEY hSubKey = NULL;

		LONG  dwRet = RegOpenKeyEx(hRoot, lpszSubKey, 0, dwREGSAM,  &hSubKey);
		if ( dwRet != ERROR_SUCCESS)
			return dwRet;

		dwRet = RegQueryValueEx(hSubKey, lpszValueName, 0, &dwTpye, lpValue, &dwSize);
		RegCloseKey(hSubKey);
		return dwRet;
	}


	static LONG SetValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwValueType, LPBYTE lpValue, DWORD dwSize, DWORD dwREGSAM  = KEY_WRITE)
	{
		DWORD dwRet = 0;
		HKEY hKey = NULL;

		dwRet = RegOpenKeyEx(hRoot, lpszSubKey, 0, dwREGSAM,  &hKey);
		if (dwRet != ERROR_SUCCESS)
		{
			dwRet = RegCreateKeyEx(hRoot, lpszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, dwREGSAM, NULL, &hKey, NULL);
			if (dwRet != ERROR_SUCCESS)
			{
				return dwRet;
			}
		}

		dwRet = RegSetValueEx(hKey, lpszValueName, 0,  dwValueType ,  lpValue, dwSize);
		RegCloseKey(hKey);
		return dwRet;
	}

	static LONG DeleteKey(HKEY hRoot, LPCTSTR lpszSubKey, DWORD dwREGSAM  = KEY_WRITE)
	{
		if ( !(lpszSubKey && _tcslen(lpszSubKey)))
		{
			return ERROR_SUCCESS;
		}
		
		HKEY hRet = 0;
		LONG lRet = RegOpenKeyEx(hRoot, lpszSubKey,0,dwREGSAM, &hRet);
		if ( lRet == ERROR_SUCCESS && hRet)
		{
			int index = 0;
			while(TRUE)
			{
				TCHAR szName[MAX_PATH] = {0};
				lRet = RegEnumKey(hRet, index, szName, MAX_PATH );
				if( !(ERROR_SUCCESS == lRet && _tcslen(szName)))
				{
					break;
				}

				DeleteKey(hRet, szName);
				index++;
			}
		}
		lRet = RegDeleteKey(hRoot, lpszSubKey);
		return lRet;
	}

	static LONG DeleteValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueNale, BOOL b64 = FALSE)
	{
		DWORD dwRet = 0;
		HKEY hKey = NULL;
		dwRet = RegOpenKeyEx(hRoot, lpszSubKey, 0, b64 ?  KEY_ALL_ACCESS | KEY_WOW64_64KEY : KEY_ALL_ACCESS  ,  &hKey);

		if ( dwRet != ERROR_SUCCESS)
		{
			return dwRet;
		}


		dwRet = RegDeleteValue(hKey, lpszValueNale);
		RegCloseKey(hKey);

		return dwRet;
	}

	static HKEY GetRoot( LPCTSTR lpszRoot)
	{
		if ( _tcsnicmp(lpszRoot, _T("HKEY_CLASSES_ROOT"), 17) == 0 )
			return HKEY_CLASSES_ROOT;
		
		if ( _tcsnicmp(lpszRoot, _T("HKEY_CURRENT_USER"), 17) == 0 )
			return HKEY_CURRENT_USER;

		if ( _tcsnicmp(lpszRoot, _T("HKEY_LOCAL_MACHINE"), 18) == 0 )
			return HKEY_LOCAL_MACHINE;

		if ( _tcsnicmp(lpszRoot, _T("HKEY_USERS"), 10) == 0 )
			return HKEY_USERS;

		if ( _tcsnicmp(lpszRoot, _T("HKEY_CURRENT_CONFIG"), 19) == 0 )
			return HKEY_CURRENT_CONFIG;


		return NULL;
	}

	static HKEY GetKey( LPCTSTR lpszKey, LPCTSTR* lpszRetKey)
	{
		HKEY hRoot = GetRoot( lpszKey);
		if ( hRoot == NULL )
			return NULL;


		if ( hRoot == HKEY_CLASSES_ROOT )
			(*lpszRetKey) = lpszKey + 17;

		if ( hRoot == HKEY_CURRENT_USER)
			(*lpszRetKey) = lpszKey + 17;

		if ( hRoot == HKEY_LOCAL_MACHINE)
			(*lpszRetKey) = lpszKey + 18;

		if ( hRoot == HKEY_USERS)
			(*lpszRetKey) = lpszKey + 10;

		if ( hRoot == HKEY_CURRENT_CONFIG)
			(*lpszRetKey) = lpszKey + 19;

		(*lpszRetKey)++;
		return hRoot;
	}


	static LPTSTR GetFullPath( HKEY hRoot, LPCTSTR lpszSub, LPTSTR lpszFullPath, DWORD dwCch)
	{
		if ( hRoot == HKEY_CLASSES_ROOT )
			_tcscpy_s( lpszFullPath, dwCch, _T("HKEY_CLASSES_ROOT\\"));

		if ( hRoot == HKEY_CURRENT_USER)
			_tcscpy_s( lpszFullPath, dwCch, _T("HKEY_CURRENT_USER\\"));

		if ( hRoot == HKEY_LOCAL_MACHINE)
			_tcscpy_s( lpszFullPath, dwCch, _T("HKEY_LOCAL_MACHINE\\"));

		if ( hRoot == HKEY_USERS)
			_tcscpy_s( lpszFullPath, dwCch, _T("HKEY_USERS\\"));

		if ( hRoot == HKEY_CURRENT_CONFIG)
			_tcscpy_s( lpszFullPath, dwCch, _T("HKEY_CURRENT_CONFIG\\"));

		
		_tcscat_s( lpszFullPath, dwCch - _tcslen(lpszFullPath) , lpszSub);
		return lpszFullPath;
	}

};

};//namespace msdk{;