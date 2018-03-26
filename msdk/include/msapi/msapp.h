#pragma once

#include <string>
#include <shlobj.h>
#include <shlwapi.h>

#include <util/str_encode_embed.h>
#include <msapi/mswinapi.h>
#include <userguid/userguid.h>

#pragma comment(lib, "shlwapi.lib")

namespace msdk{;
namespace msapi{;


#ifdef UNICODE
#	define _Def_szCompanyName ustr_encode_embed<19,'0','9','2','7','4','2','A','C','-','3','D','A','E','-','4','2','A','3','-','9','D','C','C','-','9','D','D','D','6','1','D','1','8','0','9','9' >().c_str()
#	define _Def_szPublicName  ustr_encode_embed<20,'p','u','b','l','i','c','.','i','n','i'>().c_str()
#	define _Def_szDiskSerial  ustr_encode_embed<21,'D','i','s','k','S','e','r','i','a','l'>().c_str()
#	define _Def_szUserGuid    ustr_encode_embed<22,'U','s','e','r','G','u','i','d'>().c_str()
#else
#	define _Def_szCompanyName str_encode_embed<19,'0','9','2','7','4','2','A','C','-','3','D','A','E','-','4','2','A','3','-','9','D','C','C','-','9','D','D','D','6','1','D','1','8','0','9','9' >().c_str()
#	define _Def_szPublicName  str_encode_embed<20,'p','u','b','l','i','c','.','i','n','i'>().c_str()
#	define _Def_szDiskSerial  str_encode_embed<21,'D','i','s','k','S','e','r','i','a','l'>().c_str()
#	define _Def_szUserGuid    str_encode_embed<22,'U','s','e','r','G','u','i','d'>().c_str()
#endif


class CApp
{
typedef std::basic_string<TCHAR> _tstring;

public:
	


	static LPCTSTR GetPubSettingPath(LPTSTR lpszDatePath, DWORD dwCch)
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, FALSE) )
		{
			TCHAR szTemp[ MAX_PATH ] = { 0 };
			_stprintf_s( szTemp, dwCch, _T("%s\\%s\\"), szPath, _Def_szCompanyName);
			if ( !PathFileExists(szTemp) )
			{
				CreateDirectoryEx(szTemp);
				//在win10上管理员权限创建的路径，非管理员权限不能访问
				AddEveryoneAccessToFilePath(szTemp);
			}

			_stprintf_s( szTemp, dwCch, _T("%s\\%s\\%s"), szPath, _Def_szCompanyName ,_Def_szPublicName);
			_tcscpy_s(szPath, MAX_PATH, szTemp);
		}

		_tcscpy_s(lpszDatePath,dwCch, szPath);
		return lpszDatePath;
	}


	static BOOL GetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszDef, LPTSTR lpszData, DWORD dwCch)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetPubSettingPath(szSettingFile, MAX_PATH))
		{
			if (GetPrivateProfileString(lpszNode, lpszKey, lpszDef, lpszData, dwCch, szSettingFile) )
				return TRUE;
		}

		return FALSE;
	}

	static DWORD GetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetPubSettingPath(szSettingFile, MAX_PATH))
		{
			return GetPrivateProfileInt(lpszNode, lpszKey, dwDef , szSettingFile);
		}

		return FALSE;
	}

	static BOOL SetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetPubSettingPath(szSettingFile, MAX_PATH))
		{
			return WritePrivateProfileString(lpszNode, lpszKey, lpszData, szSettingFile) ? TRUE : FALSE;
		}

		return FALSE;
	}

	static BOOL SetPublicData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwData)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetPubSettingPath(szSettingFile, MAX_PATH))
		{
			TCHAR szData[ MAX_PATH ] = { 0 };
			_itot_s(dwData, szData,MAX_PATH, 10);
			return WritePrivateProfileString(lpszNode, lpszKey,szData , szSettingFile) ? TRUE : FALSE;
		}

		return FALSE;
	}


	//获取设置项
	static BOOL GetPubSetting(LPCTSTR lpszKey, LPCTSTR lpszDef, LPTSTR lpszData, DWORD dwCch)
	{
		return GetPublicData(_T("setting"), lpszKey,lpszDef, lpszData, dwCch);
	}

	static DWORD GetPubSetting(LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return GetPublicData(_T("setting"), lpszKey, dwDef);
	}

	static BOOL SetPubSetting(LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return SetPublicData(_T("setting"), lpszKey, lpszData);
	}

	static BOOL SetPubSetting(LPCTSTR lpszKey, DWORD dwData)
	{
		return SetPublicData(_T("setting"), lpszKey, dwData);
	}


	//获取磁盘序列号
	static LPCTSTR GetDiskSerial( LPTSTR lpszDiskSerial, DWORD dwCch)
	{
		if ( GetPubSetting( _Def_szDiskSerial, _T(""),lpszDiskSerial, dwCch) && _tcslen( lpszDiskSerial) )
			return lpszDiskSerial;
		
		CHAR szDisk[ RSDISK_SIZE ] = { 0 };
		if ( CGetDiskInfo::GetDiskInfo(szDisk) && strlen(szDisk) < dwCch)
		{
			for ( int n = 0 ; n < RSDISK_SIZE ; n++)
				lpszDiskSerial[n] = szDisk[n];

			SetPubSetting(_Def_szDiskSerial, lpszDiskSerial);
		}

		return lpszDiskSerial;
	}

	static std::basic_string<TCHAR> GetDiskSerial()
	{
		TCHAR lpszDiskSerial[ MAX_PATH ] = { 0 };
		return GetDiskSerial(lpszDiskSerial, _countof(lpszDiskSerial));
	}


	//获取UserID
	static LPCTSTR GetUserGuid( LPTSTR lpszUserGuid, DWORD dwCch)
	{
		if ( dwCch < sizeof( GUID ))
			return lpszUserGuid;


		if ( GetPubSetting( _Def_szUserGuid, _T(""),lpszUserGuid, dwCch) && _tcslen( lpszUserGuid) )
			return lpszUserGuid;


		GUID guid = CUserGuid::GetGuid();
		_stprintf(lpszUserGuid,_T("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		SetPubSetting(_Def_szUserGuid, lpszUserGuid);
		return lpszUserGuid;
	}

	static std::basic_string<TCHAR> GetUserGuid()
	{
		TCHAR lpszUserGuid[ MAX_PATH ] = { 0 };
		return GetUserGuid(lpszUserGuid, _countof(lpszUserGuid));
	}


	//////////////////////////////////////////////////////////////////////////

	CApp(LPCTSTR lpszAppName)
	{
		if( lpszAppName )
			m_strAppName = lpszAppName;
	}


	LPCTSTR GetSetupPath( LPTSTR lpszSetupPath, DWORD dwCch)
	{
		 GetPrivateData(_T("setting"), _T("path"), _T(""), lpszSetupPath, dwCch);
		 return lpszSetupPath;
	}

	BOOL SetSetupPath(LPCTSTR lpszPath)
	{
		return SetPrivateData(_T("setting"), _T("path"), lpszPath);
	}

	LPTSTR GetVersion(LPTSTR lpszVersion, DWORD dwCch)
	{
		 GetPrivateData(_T("setting"), _T("version"), _T(""), lpszVersion, dwCch);
		 return lpszVersion;
	}

	std::basic_string<TCHAR> GetVersion()
	{
		TCHAR lpszVersion[ MAX_PATH ] = { 0 };
		return GetVersion(lpszVersion, _countof(lpszVersion));
	}

	BOOL SetVersion(LPTSTR lpszVersion)
	{
		return SetPrivateData(_T("setting"), _T("version"), lpszVersion);
	}

	BOOL GetDataPath(LPTSTR lpszDatePath, DWORD dwCch)
	{
		if ( m_strDatePath.length() )
		{
			_tcscpy_s(lpszDatePath, dwCch, m_strDatePath.c_str());
			return TRUE;
		}

		TCHAR szPath[ MAX_PATH ] = { 0 };
		if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, TRUE) )
		{
			_stprintf_s( lpszDatePath, dwCch, _T("%s\\%s\\%s\\"), szPath, _Def_szCompanyName,m_strAppName.c_str() );
			m_strDatePath = lpszDatePath;
			if ( GetFileAttributes(m_strDatePath.c_str()) == -1 )
			{
				CreateDirectoryEx(lpszDatePath);
				//在win10上管理员权限创建的路径，非管理员权限不能访问
				AddEveryoneAccessToFilePath(lpszDatePath);
			}
			
			return TRUE;
		}
	
		return FALSE;
	}

	std::basic_string<TCHAR> GetDataPath()
	{
		TCHAR szPath[ MAX_PATH ] = { 0 };
		GetDataPath(szPath, _countof(szPath));
		return szPath;
	}

	BOOL GetSettingFile( LPTSTR lpszSettingFile, DWORD dwCch)
	{
		if( m_strSettingFile.length() )
		{
			_tcscpy_s(lpszSettingFile, dwCch, m_strSettingFile.c_str());
			return TRUE;
		}

		TCHAR szDatePath[ MAX_PATH ] = { 0 };
		if ( GetDataPath( szDatePath, dwCch) )
		{
			_stprintf_s( lpszSettingFile, dwCch, _T("%s%s.ini"), szDatePath, m_strAppName.c_str() );
			m_strSettingFile = lpszSettingFile;
			return TRUE;
		}

		return FALSE;
	}

	
	BOOL GetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszDef, LPTSTR lpszData, DWORD dwCch)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetSettingFile(szSettingFile, MAX_PATH))
		{
			return GetPrivateProfileString(lpszNode, lpszKey, lpszDef, lpszData, dwCch, szSettingFile)? TRUE : FALSE;
		}

		return FALSE;
	}

	DWORD GetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetSettingFile(szSettingFile, MAX_PATH))
			return GetPrivateProfileInt(lpszNode, lpszKey, dwDef , szSettingFile);
		
		return FALSE;
	}

	//获取设置项
	LPCTSTR GetSetting(LPCTSTR lpszKey, LPCTSTR lpszDef, LPTSTR lpszData, DWORD dwCch)
	{
		 GetPrivateData(_T("setting"), lpszKey,lpszDef, lpszData, dwCch);
		 return lpszData;
	}
	std::basic_string<TCHAR> GetSetting(const std::basic_string<TCHAR>& lpszKey, const std::basic_string<TCHAR>& lpszDef)
	{
		TCHAR lpszData[ 1024 ] = { 0 };
		return GetSetting(lpszKey.c_str(), lpszDef.c_str(), lpszData, _countof(lpszData));
	}

	DWORD GetSetting(LPCTSTR lpszKey, DWORD dwDef = 0)
	{
		return GetPrivateData(_T("setting"), lpszKey, dwDef);
	}

	BOOL SetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetSettingFile(szSettingFile, MAX_PATH))
			return WritePrivateProfileString(lpszNode, lpszKey, lpszData, szSettingFile) ? TRUE : FALSE;
		
		return FALSE;
	}

	BOOL SetPrivateData( LPCTSTR lpszNode, LPCTSTR lpszKey, DWORD dwData)
	{
		TCHAR szSettingFile[ MAX_PATH ] = { 0 };
		if (GetSettingFile(szSettingFile, MAX_PATH))
		{
			TCHAR szData[ MAX_PATH ] = { 0 };
			_itot_s(dwData, szData,MAX_PATH, 10);
			return WritePrivateProfileString(lpszNode, lpszKey,szData , szSettingFile) ? TRUE : FALSE;
		}

		return FALSE;
	}

	BOOL SetSetting(LPCTSTR lpszKey, LPCTSTR lpszData)
	{
		return SetPrivateData(_T("setting"), lpszKey, lpszData);
	}

	BOOL SetSetting(LPCTSTR lpszKey, DWORD dwData)
	{
		return SetPrivateData(_T("setting"), lpszKey, dwData);
	}

private:
	static BOOL CreateDirectoryEx(const TCHAR* szPath)
	{
		if ( !(szPath && lstrlen(szPath)) )
			return FALSE;

		TCHAR szMyPath[MAX_PATH ] = {0};
		_tcscpy_s(szMyPath ,MAX_PATH , szPath);
		TCHAR szCurPath[MAX_PATH] = {0};
		for (UINT i = 0; i < _tcslen(szMyPath); i++){
			szCurPath[i] = szMyPath[i];
			if (szCurPath[i] == _T('\\')){
				if (!PathFileExists(szCurPath)){
					CreateDirectory(szCurPath, NULL);
				}
			}
		}

		DWORD dwFileAtt = ::GetFileAttributes(szPath);
		return ((INVALID_FILE_ATTRIBUTES != dwFileAtt) && (0 != (dwFileAtt & FILE_ATTRIBUTE_DIRECTORY)));
	}
private:
	_tstring m_strAppName;
	_tstring m_strDatePath;
	_tstring m_strSetupPath;
	_tstring m_strSettingFile;
};

};};