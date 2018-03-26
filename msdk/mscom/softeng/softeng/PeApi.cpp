#include "StdAfx.h"
#include "PeApi.h"

TCHAR	 g_PhysicalDrive = 'C';
CString	 g_BkDrive = 'C';
CPeApi::CPeApi(void)
{
}


CPeApi::~CPeApi(void)
{
}

BOOL CPeApi::IsPe()
{
	static TCHAR szTemp[ MAX_PATH ] = { 0 };
	if ( !_tcslen(szTemp) )
	{
		GetTempPath( MAX_PATH, szTemp );
		_tcsupr(szTemp);
	}

	return szTemp[ 0 ] != 'C';
}

BOOL CPeApi::IsWindows64()
{
	static TCHAR szPhysical[ MAX_PATH ] = { 0 };
	static BOOL  isPe64 = FALSE;
	if ( !_tcslen( szPhysical ) )
	{
		_tcscpy_s( szPhysical, MAX_PATH, GetRealPath(_T("C:\\Windows\\SysWOW64\\ntdll.dll")));
		isPe64 = PathFileExists( szPhysical );
	}
	
	return isPe64;
}

BOOL CPeApi::IsPeVistaLatter()
{
	OSVERSIONINFO  m_osver;
	m_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&m_osver);
	return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 1 ) );
}


BOOL CPeApi::IsWindowsVistaLatter()
{
	LONG lRet = 0; 
	fstring sCurrentVersion = msdk::CRegistry::GetString( 
		CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), 
		CPeApi::GetRealRegPath(CPeApi::GetFullRegPath(CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"))),
		_T("CurrentVersion"),
		FALSE,
		&lRet
		);

	if ( lRet != 0 )
	{
		GrpError( GroupName, MsgLevel_Error, _T("CPeApi::IsWindowsVistaLatter"));
	}

	if( _tcsicmp(sCurrentVersion.c_str(), L"6.0") >= 0 )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CPeApi::PathIsSystemFolder(LPCTSTR lpszPath)
{
	CString strDir = lpszPath;
	strDir = strDir.Mid(strDir.Find('\\') + 1);
	if (strDir.GetLength() == 0 || strDir.GetLength() == 2)
	{
		return TRUE;
	}

#define CMP_PATH(_X) if(strDir.CompareNoCase(_X) == 0) return TRUE;

	CMP_PATH(_T("Windows"));
	CMP_PATH(_T("Windows\\system"));
	CMP_PATH(_T("Windows\\System32"));
	CMP_PATH(_T("Windows\\SysWOW64"));

	CMP_PATH(_T("ProgramData"));
	CMP_PATH(_T("Program Files"));
	CMP_PATH(_T("Program Files (x86)"));


	CMP_PATH(_T("Documents and Settings\\All Users"));
	CMP_PATH(_T("Documents and Settings\\All Users\\Application Data"));
	CMP_PATH(_T("Users\\Administrator\\AppData\\Local"));
	CMP_PATH(_T("Documents and Settings\\Administrator\\Local Settings\\Application Data"));
	CMP_PATH(_T("Users\\Administrator\\Desktop"));
	CMP_PATH(_T("Documents and Settings\\Administrator\\桌面"));
	CMP_PATH(_T("Users\\Administrator\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu"));
	CMP_PATH(_T("Documents and Settings\\Administrator\\「开始」菜单"));
	CMP_PATH(_T("Users\\Public\\Desktop"));
	CMP_PATH(_T("Documents and Settings\\All Users\\桌面"));
	CMP_PATH(_T("ProgramData\\Microsoft\\Windows\\Start Menu"));
	CMP_PATH(_T("Documents and Settings\\All Users\\「开始」菜单"));
	CMP_PATH(_T("Users\\Administrator\\AppData\\Roaming"));
	CMP_PATH(_T("Documents and Settings\\Administrator\\Application Data"));

	CMP_PATH(_T("Users\\Administrator"));
	CMP_PATH(_T("Documents and Settings\\Administrator"));
	CMP_PATH(_T("Program Files\\Common Files"));
	CMP_PATH(_T("Users\\Administrator"));
	CMP_PATH(_T("Documents and Settings\\Administrator"));

	return FALSE;
}

CString CPeApi::GetRealPath(LPCTSTR lpszPath)
{
	if ( !(lpszPath && _tcslen(lpszPath) ) )
	{
		return _T("");
	}

	CString strRet = lpszPath;
	if ( IsPe() )
		strRet.SetAt(0, g_PhysicalDrive);
	
	return strRet;
}

CString CPeApi::GetRealRegPath(LPCTSTR lpszRegPath)
{
	if ( IsPe() ){
		return GetPeRealRegPath(lpszRegPath);
	}else{
	  return GetLocalRealRegPath(lpszRegPath);
	}
	return L"";
}


HKEY CPeApi::GetRootKey(HKEY hRoot)
{
	if ( IsPe() )
		return HKEY_LOCAL_MACHINE;

	return hRoot;
}

CString CPeApi::GetFullRegPath(HKEY hKey, LPCTSTR lpszSub)
{
	CString strFullReg;
	if ( hKey == HKEY_LOCAL_MACHINE)
		strFullReg = _T("HKEY_LOCAL_MACHINE");
	else if (HKEY_CURRENT_USER == hKey)
		strFullReg = _T("HKEY_CURRENT_USER");
	else if(HKEY_CLASSES_ROOT == hKey)
		strFullReg = _T("HKEY_CLASSES_ROOT");
	else if(HKEY_CURRENT_CONFIG == hKey)
		strFullReg = _T("HKEY_USERS");


	strFullReg.Append(_T("\\"));
	strFullReg.Append(lpszSub);

	return strFullReg;
}

CString CPeApi::GetPeRealRegPath(LPCTSTR lpszRegPath)
{
	CString subRegPath;
	HKEY hKey = NULL;
	if (_tcsnicmp(_T("HKEY_LOCAL_MACHINE\\"), lpszRegPath, _tcslen(_T("HKEY_LOCAL_MACHINE\\"))) == 0)
	{
		CString strRealPath = lpszRegPath + _tcslen(_T("HKEY_LOCAL_MACHINE\\"));
		strRealPath.MakeUpper();

		if (0 == strRealPath.Find(L"SOFTWARE"))
		{
			strRealPath = strRealPath.Right(strRealPath.GetLength()-_tcslen(L"SOFTWARE\\"));
			if ( 0 == strRealPath.Find(L"CLASSES"))
			{
				strRealPath = strRealPath.Right(strRealPath.GetLength()-_tcslen(L"CLASSES\\"));
			}

			subRegPath.Append(_T("LGPEMAC\\"));
			subRegPath.Append(strRealPath);
		}
		else if( 0 == strRealPath.Find(L"SYSTEM") )
		{
			strRealPath = strRealPath.Right(strRealPath.GetLength()-_tcslen(L"SYSTEM\\"));
			subRegPath.Append(_T("LGPESYSTEM\\"));
			subRegPath.Append(strRealPath);
		}
		else{
			subRegPath.Append(_T("LGPEMAC\\"));
			subRegPath.Append(strRealPath);
		}
	}
	else if (_tcsnicmp(_T("HKEY_CURRENT_USER\\"), lpszRegPath, _tcslen(_T("HKEY_CURRENT_USER\\"))) == 0)
	{
		CString strRealPath = lpszRegPath + _tcslen(_T("HKEY_CURRENT_USER\\"));
		strRealPath.MakeUpper();

		subRegPath.Append(_T("LGPEUSER\\"));
		subRegPath.Append(strRealPath);
	}
	else if (_tcsnicmp(_T("HKEY_CLASSES_ROOT\\"), lpszRegPath, _tcslen(_T("HKEY_CLASSES_ROOT\\"))) == 0)
	{
		CString strRealPath = lpszRegPath + _tcslen(_T("HKEY_CLASSES_ROOT\\"));
		strRealPath.MakeUpper();

		subRegPath.Append( L"LGPEMAC\\CLASSES\\");
		subRegPath.Append(strRealPath);
	}
	else if (_tcsnicmp(_T("HKEY_CURRENT_CONFIG\\"), lpszRegPath, _tcslen(_T("HKEY_CURRENT_CONFIG\\"))) == 0)
	{
		subRegPath = lpszRegPath + _tcslen(_T("HKEY_CURRENT_CONFIG\\"));
	}

	return subRegPath;
}

CString CPeApi::GetLocalRealRegPath(LPCTSTR lpszRegPath)
{
	CString subRegPath;
	HKEY hKey = NULL;
	if (_tcsnicmp(_T("HKEY_LOCAL_MACHINE\\"), lpszRegPath, _tcslen(_T("HKEY_LOCAL_MACHINE\\"))) == 0)
	{
		hKey = HKEY_LOCAL_MACHINE;
		subRegPath = lpszRegPath + _tcslen(_T("HKEY_LOCAL_MACHINE\\"));
	}
	else if (_tcsnicmp(_T("HKEY_CURRENT_USER\\"), lpszRegPath, _tcslen(_T("HKEY_CURRENT_USER\\"))) == 0)
	{
		hKey = HKEY_CURRENT_USER;
		subRegPath = lpszRegPath + _tcslen(_T("HKEY_CURRENT_USER\\"));
	}
	else if (_tcsnicmp(_T("HKEY_CLASSES_ROOT\\"), lpszRegPath, _tcslen(_T("HKEY_CLASSES_ROOT\\"))) == 0)
	{
		hKey = HKEY_CLASSES_ROOT;
		subRegPath = lpszRegPath + _tcslen(_T("HKEY_CLASSES_ROOT\\"));
	}
	else if (_tcsnicmp(_T("HKEY_CURRENT_CONFIG\\"), lpszRegPath, _tcslen(_T("HKEY_CURRENT_CONFIG\\"))) == 0)
	{
		hKey = HKEY_CURRENT_CONFIG;
		subRegPath = lpszRegPath + _tcslen(_T("HKEY_CURRENT_CONFIG\\"));
	}

	return subRegPath;
}



