#pragma once

#include <string>
#include <util/registry.h>
namespace msdk{;
namespace msapi{;

typedef BOOL (*pEnumSubCallback)( HKEY hKey, LPCTSTR lpszKey, LPVOID lpParam);
typedef BOOL (*pEnumValueCallback)( HKEY hKey, LPCTSTR lpszName, DWORD dwType, LPVOID lpParam);
class CEnumReg
{
public:
	typedef std::basic_string<TCHAR> _tstring;
	CEnumReg(HKEY hKey)
	{
		m_hKey = hKey;
	}

	BOOL EnumSub( LPCTSTR lpszKey , pEnumSubCallback pCallback, LPVOID lpParam,  DWORD dwREGSAM = KEY_READ)
	{
		HKEY hSKey = NULL;
		LSTATUS lRet = RegOpenKeyEx(m_hKey, lpszKey, 0, dwREGSAM, &hSKey);
		if( ERROR_SUCCESS != lRet ) 
			return FALSE;

		int index = 0;
		while( TRUE )
		{
			TCHAR szName[MAX_PATH] = {0};
			lRet = RegEnumKey(hSKey, index, szName, MAX_PATH );
			if( ERROR_SUCCESS != lRet )
			{
				break;
			}
			
			if ( pCallback )
			{
				if ( !pCallback(hSKey, szName,lpParam) )
					break;
			}

			index++;
		}
		RegCloseKey(hSKey);

		return TRUE;
	}

	BOOL EnumValue(LPCTSTR lpszKey , pEnumValueCallback pCallback, LPVOID lpParam)
	{
		HKEY hSKey = NULL;
		LSTATUS lRet = RegOpenKeyEx(m_hKey, lpszKey, 0, KEY_READ, &hSKey);
		if( ERROR_SUCCESS != lRet )
			return FALSE;
		
		int nIndex = 0;
		while( TRUE) 
		{
			TCHAR szName[MAX_PATH] = {0};
			DWORD dwNameLen = MAX_PATH;
			DWORD dwType = REG_SZ;
			TCHAR szData[MAX_PATH] = {0};
			DWORD dwDatalen = MAX_PATH;
			LSTATUS lRet = RegEnumValue(hSKey, nIndex, szName, &dwNameLen, NULL, &dwType, (LPBYTE)szData, &dwDatalen);
			if( ERROR_SUCCESS != lRet )
			{
				break;
			}
			if ( pCallback )
			{
				if ( !pCallback(hSKey, szName,dwType,lpParam) )
					break;
			}
			nIndex++;
		}
		RegCloseKey(hSKey);

		return TRUE;
	}

public:
	HKEY m_hKey;
};

};};