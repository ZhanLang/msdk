#include "StdAfx.h"
#include "SoftScan.h"
#include <atlmisc.h>

#include <shlobj.h>
#include "ShellLinkInfo.h"
#include <process/ParseCommand.h>
#include <util/registry.h>
#include <userguid/guidcrc.h>
#include <msapi/EnumFile.h>
#include <msapi/mspath.h>
#include <msapi/mstime.h>
#include <json/json.h>
#include <fstream>
#include <msapi/EnumReg.h>

#define FILEDB_NAME		_T("soft.db")
#define FILEDBU_NAME	_T("softu.db")
#define USERNAME_SYSTEM _T("SYSTEM")

void PrintfError(const TCHAR* sql,CppSQLite3Exception& e)
{
	CStringW strInfo;
	strInfo.Format(_T("数据库异常:Error[%d],Sql[%s]\n"),e.errorCode(),sql);
	ZM1_GrpError(GroupName, strInfo);
}

HKEY GetRegKey(LPCTSTR lpRootKey)
{
	if ( !lpRootKey )
	{
		return NULL;
	}



	HKEY hKey = NULL;
	CString strKey = lpRootKey;

	if(strKey == _T("HKEY_CLASSES_ROOT"))
		hKey = HKEY_CLASSES_ROOT;
	else if(strKey == _T("HKEY_CURRENT_USER"))
		hKey = HKEY_CURRENT_USER;
	else if(strKey == _T("HKEY_LOCAL_MACHINE"))
		hKey = HKEY_LOCAL_MACHINE;
	else if(strKey == _T("HKEY_USERS"))
		hKey = HKEY_USERS;
	else if(strKey == _T("HKEY_CURRENT_CONFIG"))
		hKey = HKEY_CURRENT_CONFIG;

	return hKey;
}

HKEY ParseRegPath(LPCTSTR lpRegPath, CString& strSubPath, CString& strKeyValue)
{
	if ( !lpRegPath )
	{
		return NULL;
	}


	CString strRegPath = lpRegPath;
	if ( strRegPath.IsEmpty() )
	{
		return NULL;
	}
	

	int nPos = strRegPath.Find(_T('\\'));
	if ( !(nPos > 0) )
	{
		return NULL;
	}
	HKEY hKey = GetRegKey(strRegPath.Left(nPos));
	if ( !hKey )
	{
		return NULL;
	}
	strRegPath.Delete(0, nPos+1);
	if ( strRegPath.IsEmpty() )
	{
		return NULL;
	}

	nPos = strRegPath.ReverseFind(_T('\\'));
	if ( !(nPos > 0) )
	{
		return NULL;
	}
	strSubPath =  strRegPath.Left(nPos);
	strKeyValue = strRegPath.Right(strRegPath.GetLength() - nPos - 1);

	return hKey;
}

BOOL IsRegExist(LPCTSTR lpRegPath, BOOL bCheckValue)
{
	if ( !lpRegPath )
	{
		return FALSE;
	}
	CString strSubPath, strKeyValue;
	HKEY hRootKey = ParseRegPath(lpRegPath, strSubPath, strKeyValue);
	if ( !hRootKey )
	{
		return FALSE;
	}
	CRegKey regKey;
	if ( !(ERROR_SUCCESS == regKey.Open(hRootKey, strSubPath, KEY_READ)) )
	{
		return FALSE;
	}
	if(bCheckValue)
	{
		//尝试打开值试试，打开成功那就算ok
		ULONG	nChars = 0;
		if(ERROR_SUCCESS == regKey.QueryValue(strKeyValue, NULL, NULL, &nChars))
			return TRUE;
	}
	else
	{
		CRegKey regKey2;
		if(ERROR_SUCCESS == regKey2.Open(regKey, strKeyValue, KEY_READ))
			return TRUE;
	}

	return FALSE;
}

template<class Tfield>
LPCTSTR DB_STRING(CppSQLite3Query& r,Tfield field)
{
	LPCTSTR lpTmp = r.getStringField(field);
	if(lpTmp) 
		return lpTmp;

	return _T("");
}

DWORD MyGetLongPathName(IN LPCWSTR lpszShortPath,
						OUT LPWSTR  lpszLongPath,
						IN DWORD    cchBuffer)
{
	DWORD (WINAPI* pGetLongPathNamen)(LPCWSTR, LPWSTR,DWORD) = NULL;
	HMODULE hKernel = GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel)
	{
		(FARPROC&)pGetLongPathNamen = GetProcAddress(hKernel,"GetLongPathName");
		if (pGetLongPathNamen)
		{
			return pGetLongPathNamen(lpszShortPath, lpszLongPath, cchBuffer);
		}
	}
	return 0L;
}

CString ExpandPath(LPCTSTR lpPath, BOOL bTrimEnd = TRUE)
{
	if ( !(lpPath && _tcslen(lpPath) > 0) )
	{
		return _T("");
	}

	CString str;
	ExpandEnvironmentStrings(lpPath, str.GetBuffer(MAX_PATH*2), MAX_PATH*2);
	MyGetLongPathName(str, str.GetBuffer(MAX_PATH*2), MAX_PATH*2);
	str.ReleaseBuffer();
	str.Replace(_T('/'),_T('\\'));

	if(bTrimEnd && _T('\\') == str.GetAt(str.GetLength()-1))
		str.Delete(str.GetLength()-1);

	return str;
}

CString GetFirstPath(LPCTSTR lpPath)
{
	if ( !(lpPath && _tcslen(lpPath) > 0) )
	{
		return _T("");
	}

	CString str = lpPath;
	str.TrimLeft();
	RASSERT(!str.IsEmpty(), _T(""));

	CString strPath = str;
	//有些路径会被引号引起来
	if(_T('"') == str.GetAt(0))
	{
		int nPos = str.Find(_T('"'), 1);
		if(nPos >= 0)
			strPath = str.Mid(1, nPos-1);
	}
	else
	{
		strPath.Replace(_T("\""), _T(""));//处理带引号的路径
		int index = strPath.Find(_T(":"), 2);
		if(index >= 0)
		{	//01234567890
			//c:\rt c:\rt
			//处理里面有多个路径的，只取第一个
			strPath.Delete(index-2, strPath.GetLength() - index + 2);
		}
	}

	return ExpandPath(strPath);
}

CSoftScan::CSoftScan(ISoftCallback* pCallback)
:m_dbOK(FALSE),m_udbOK(FALSE),m_pCallback(pCallback)
{
	TCHAR path[MAX_PATH];
	ZeroMemory(path, sizeof(TCHAR)*MAX_PATH);
	GetModuleFileName(NULL, path, MAX_PATH);
	TCHAR* pTail = _tcsrchr(path, _T('\\'));
	if (NULL != pTail)
	{
		*pTail = 0; 
	}
	m_strModulePath = path;

	CString strUserName;
	ZeroMemory(path, sizeof(TCHAR)*MAX_PATH);
	DWORD dwSize = MAX_PATH;
	::GetUserName(path, &dwSize);
	strUserName = path;
	m_bIsSvc = (0 == strUserName.CompareNoCase(USERNAME_SYSTEM));

	CString strSettigIni = m_strModulePath;
	strSettigIni+=_T("\\softeng.ini");

	m_bDebug = GetPrivateProfileInt(_T("setting"), _T("debug"), 0, strSettigIni);

}

CSoftScan::~CSoftScan(void)
{
}


STDMETHODIMP_(BOOL) CSoftScan::IsPe()
{
	return CPeApi::IsPe();
}

BOOL EnablePrivilege( LPCTSTR lpszPrivilege)
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tp;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,lpszPrivilege , &sedebugnameValue))
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = sedebugnameValue;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

static BOOL LoadPeReg( WCHAR cRoot )
{
	// 加载HKEY_LOCAL_MACHINE

	EnablePrivilege(SE_RESTORE_NAME);
	EnablePrivilege(SE_BACKUP_NAME);
	EnablePrivilege(SE_DEBUG_NAME);
	TCHAR szMacReg[] = _T("X:\\Windows\\System32\\config\\software");
	TCHAR szUsrRegWin7[] = _T("X:\\Users\\Administrator\\NTUSER.DAT");
	TCHAR szUsrRegXp[] = _T("X:\\Documents and Settings\\Administrator\\NTUSER.DAT");
	TCHAR szSysReg[] = _T("X:\\Windows\\System32\\config\\system");

	szSysReg[0] = szUsrRegXp[0] = szUsrRegWin7[0] = szMacReg[0] = cRoot;
	TCHAR szError[ MAX_PATH ] = { 0 };
	static CPERegLoader* pMacReg = new CPERegLoader(szMacReg, L"LGPEMAC");
	if ( !pMacReg->GetLoadStatus() )
	{
		ZM1_GrpError(GroupName, _T("Load Reg %s, %s"), szMacReg, GetLastErrorText(szError, MAX_PATH));
	}

	static CPERegLoader* pSysReg = new CPERegLoader(szSysReg, L"LGPESYSTEM");
	if ( !pSysReg->GetLoadStatus() )
	{
		ZM1_GrpError(GroupName, _T("Load Reg %s, %s"), szSysReg, GetLastErrorText(szError, MAX_PATH));
	}

	if ( CPeApi::IsWindowsVistaLatter() )
	{
		static CPERegLoader* m_pUserloader = new CPERegLoader(szUsrRegWin7, L"LGPEUSER");
		if ( !m_pUserloader->GetLoadStatus()  )
		{
			ZM1_GrpError(GroupName, _T("Load Reg %s, %s"), szUsrRegWin7, GetLastErrorText(szError, MAX_PATH));
		}
	}
	else
	{
		CPERegLoader* m_pUserloader = new CPERegLoader(szUsrRegXp, L"LGPEUSER");
		if ( !m_pUserloader->GetLoadStatus() )
		{
			ZM1_GrpError(GroupName, _T("Load Reg %s, %s"), szUsrRegXp, GetLastErrorText(szError, MAX_PATH));
		}
	}

	//
	

	return TRUE;
}

STDMETHODIMP_(BOOL) CSoftScan::Init(/*WCHAR cRoot, LPCWSTR lpszBk*//*备份的目录*/)
{
	if ( CPeApi::IsPe() )
	{
		//LoadPeReg( cRoot );
	}


	//g_PhysicalDrive = cRoot;
	//g_BkDrive		= lpszBk;
	return TRUE;
}

STDMETHODIMP_(SOFTID) CSoftScan::FindSoft(BOOL bOnlyExist/* = TRUE*/, DWORD softLib/* = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE*/)
{
	

	ZM1_GrpError(GroupName, _T("FindSoft:%d"), softLib);
	return FindSoftEx(bOnlyExist, softLib, NULL);
}



HRESULT CSoftScan::EnumSoftInfo(BOOL bMerge)
{
	CSoftWareEnum softEnum;
	REG_SOFTWARE_INFO softinfo;

	HRESULT hr = softEnum.Enum(softinfo, FALSE, m_bIsSvc);
	while(SUCCEEDED(hr))
	{
		CheckEnumSoft(softinfo, bMerge);

		softinfo.Reset();
		hr = softEnum.EnumNext(softinfo);
	}

	for ( SOFTINFO_MAP::iterator it = m_softInfo.begin() ; it != m_softInfo.end() ; it++)
	{
		ST_SOFT_INFO& info = (it->second);
		CString strSoftName = info.kvValue[SOFT_NAME];
		CString strCompany =  info.kvValue[SOFT_COMPANY];

		BOOL bWhite = FALSE;
		//在这里匹配白名单
		for (long lLoop = 0 ;  lLoop < m_softWiteList.GetSize() ; lLoop++ )
		{
			ST_SOFT_WHITE& softWhite = m_softWiteList[lLoop];

			switch( softWhite.whiteAttr )
			{
			case SOFT_WHITE_SOFT_NAME_WHOLEWORD:
				{
					if ( strSoftName.GetLength() && strSoftName.CompareNoCase(softWhite.strValue) == 0)
						bWhite = TRUE;

					break;
				}
			case SOFT_WHITE_SOFT_NAME:
				{
					if ( strSoftName.GetLength() && strSoftName.Find(softWhite.strValue) != -1)
						bWhite = TRUE;
				}
			case SOFT_WHITE_SOFT_COMPANY_WHOLEWORD:
				{
					if ( strCompany.GetLength() && strCompany.CompareNoCase(softWhite.strValue) == 0)
						bWhite = TRUE;
					break;
				}
			case SOFT_WHITE_SOFT_COMPANY:
				{
					if ( strCompany.GetLength() && strCompany.Find(softWhite.strValue) != -1)
						bWhite = TRUE;

					break;
				}
			}

			if ( bWhite )
			{
				break;
			}
		}


		info.kvValue[SOFT_WHITE].Format(_T("%d"), bWhite);
	}

	return S_OK;
}

HRESULT CSoftScan::CheckEnumSoft(const REG_SOFTWARE_INFO &softinfo, BOOL bMerge)
{
	CString strPath = ExpandPath(softinfo.szInstallLocation);

	BOOL bAdd = FALSE;
	if(bMerge)
	{
		//看之前的库里是否有同名的
		SOFTINFO_MAP::iterator _it = m_softInfo.begin();
		for(; _it != m_softInfo.end(); _it++)
		{
			SOFTID softid = _it->first;
			if(softid&SOFTID_ENUM_MASK)
				continue;

			ST_SOFT_INFO& info = _it->second;
			KV_MAP &kvValue = info.kvValue;

			CString strName, strOldPath, strRegPath;
			if(kvValue.find(SOFT_NAME) != kvValue.end())
				strName = kvValue[SOFT_NAME];
			if(kvValue.find(SOFT_PATH) != kvValue.end())
				strOldPath = kvValue[SOFT_PATH];

			if ( kvValue.find(SOFT_REG_UNINSTALL) != kvValue.end() )
				strRegPath = kvValue[ SOFT_REG_UNINSTALL];

			//这里需要特殊处理一下，64位系统上的32位软件按理和32位上有同样的处理规则
			CString strUninstallRegPath = softinfo.szUninstallRegPath; strUninstallRegPath.MakeLower();
			strUninstallRegPath.Replace(_T("\\wow6432node"), _T(""));

			CString tempstrRegPath = strRegPath;tempstrRegPath.MakeLower();
			tempstrRegPath.Replace(_T("\\wow6432node"), _T(""));

			if( !tempstrRegPath.CollateNoCase(strUninstallRegPath))
			{
				//找到相同的了
				bAdd = TRUE;
				if(!info.bIsExist || strOldPath.IsEmpty())
				{
					info.bIsExist = TRUE;
					info.kvValue[SOFT_PATH] = strPath;
				}
				info.kvValue[SOFT_UNINSTALLSTR] = softinfo.szUninstallString;
				info.kvValue[SOFT_VER] = softinfo.szVer;
				info.kvValue[SOFT_NAME] = softinfo.szDisplayName;
			}
		}
	}

	if(!bAdd)
	{
		USES_CONVERSION;
		const char * Buf = W2A(softinfo.szSubKeyName);
		SOFTID softid = std_crc32(Buf, strlen(Buf));
		softid |= SOFTID_ENUM_MASK;

		ST_SOFT_INFO info;
		info.bIsExist	= TRUE;
		info.kvValue[SOFT_APPID] = _T("1");
		info.kvValue[SOFT_NAME] = softinfo.szDisplayName;
		info.kvValue[SOFT_PATH] = strPath;
		info.kvValue[SOFT_COMPANY] = softinfo.szPublisher;
		info.kvValue[SOFT_UNINSTALLSTR] = softinfo.szUninstallString;
		info.kvValue[SOFT_VER] = softinfo.szVer;
		info.kvValue[SOFT_REG_UNINSTALL] = softinfo.szUninstallRegPath;
		m_softInfo[softid] = info;
	}

	return S_OK;
}

STDMETHODIMP_(SOFTID) CSoftScan::FindNextSoft()
{
	if ( !(m_find != m_softInfo.end()) )
	{
		return INVALID_SOFTID;
	}


	SOFTID softid = m_find->first;
	m_find++;
	return softid;
}

STDMETHODIMP_(BOOL) CSoftScan::IsExist(SOFTID softid)
{
	SOFTINFO_MAP::const_iterator _it = m_softInfo.find(softid);
	if ( !(_it != m_softInfo.end()) )
	{
		return FALSE;
	}
	return _it->second.bIsExist;
}

STDMETHODIMP CSoftScan::ScanSoft(LPCWSTR lpExpendPath, LPCWSTR lpRegPath, LPWSTR lpPath, DWORD dwPathSize)
{

	CString strSubKey, strValueName;
	HKEY hRootKey = ParseRegPath(lpRegPath, strSubKey, strValueName);

	CString strPath;
	HRESULT hRet = ScanSoft(strPath, lpExpendPath, hRootKey, strSubKey, strValueName);
	if ( FAILED( hRet ) ) 
	{
		ZM1_GrpError(GroupName, _T("ScanSoft(%s, %s)"), lpExpendPath, lpRegPath);
		return hRet;
	}

	if ( strPath.IsEmpty() )
	{
		ZM1_GrpError(GroupName, _T("ScanSoft_1(%s, %s)"), lpExpendPath, lpRegPath);
		return E_FAIL;
	}
	
	if(dwPathSize < DWORD(strPath.GetLength()+1))
	{
		ZM1_GrpError(GroupName, _T("ScanSoft_2(%s, %s)"), lpExpendPath, lpRegPath);
		return E_OUTOFMEMORY;
	}

	wcscpy_s(lpPath, dwPathSize, strPath);
	return S_OK;
}

STDMETHODIMP CSoftScan::ScanSoftSubPath(SOFTID softid, LPCWSTR lpSubPath, SOFTFILE_LIST& fileList)
{
	if ( !lpSubPath )
	{
		return E_FAIL;
	}
	CString strSoftPath = GetSoftInfo(softid, SOFT_PATH);

	if ( strSoftPath.IsEmpty() )
	{
		return S_FALSE;
	}


	return ParseSoftFile(strSoftPath, lpSubPath, fileList);
}

//获取软件信息, 软件名称、厂商等等
STDMETHODIMP_(LPCWSTR) CSoftScan::GetSoftInfo(SOFTID softid, DWORD dwKey)
{
	SOFTINFO_MAP::const_iterator _it = m_softInfo.find(softid);
	if ( !(_it != m_softInfo.end()) )
	{
		return NULL;
	}
	const KV_MAP& kvValue = _it->second.kvValue;
	KV_MAP::const_iterator _kv = kvValue.find(dwKey);
	if ( !(_kv != kvValue.end()) )
	{
		return NULL;
	}
	

	return _kv->second;
}

STDMETHODIMP_(LPCWSTR) CSoftScan::EnumSoftInfo(SOFTID softid, DWORD& dwKey, DWORD& enumPos)
{
	SOFTINFO_MAP::const_iterator _it = m_softInfo.find(softid);
	if ( !(_it != m_softInfo.end()) )
	{
		return NULL;
	}


	const KV_MAP& kvValue = _it->second.kvValue;
	KV_MAP::const_iterator _kv;
	if(-1 == enumPos)
	{
		_kv = kvValue.begin();
	}
	else
	{
		_kv = kvValue.find(enumPos);
		if ( !(_kv != kvValue.end()) )
		{
			return NULL;
		}
		
		_kv++;
	}
	if ( !(_kv != kvValue.end()) )
	{
		return NULL;
	}
	dwKey = _kv->first;
	enumPos = dwKey;
	return _kv->second;
}

STDMETHODIMP_(LPCWSTR) CSoftScan::GetSoftClass(DWORD softClass)
{
	KV_MAP::const_iterator _it = m_softClass.find(softClass);
	if ( !(_it != m_softClass.end()) )
	{
		return NULL;
	}
	return _it->second;
}

static BOOL GetShotcutPath(LPCTSTR szFilePath, CString &strPath, TCHAR cSys, BOOL bIs64)
{
	//创建IShellLink实例
	IShellLink *psl = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
	if( FAILED(hr) )
	{
		return FALSE;
	}

	//获取IPersistFile接口
	IPersistFile* ppf = NULL;
	hr = psl->QueryInterface( IID_IPersistFile, (LPVOID*)&ppf);
	if( FAILED(hr) )
	{
		psl->Release();
		return FALSE;
	}


	TCHAR szPath[MAX_PATH] = { 0 };
	_tcscpy_s(szPath, MAX_PATH, szFilePath);
	//加载快捷方式
	hr = ppf->Load(szPath, STGM_READWRITE | STGM_SHARE_DENY_NONE);
	if( FAILED(hr) )
	{
		ppf->Release();
		psl->Release();
		return FALSE;
	}

	//解析快捷方式
	hr = psl->Resolve( NULL, SLR_ANY_MATCH|SLR_NO_UI );
	if( FAILED(hr) )
	{
		ppf->Release();
		psl->Release();
		return FALSE;
	}


	// 获取目标路径（可能含有环境变量）
	TCHAR szTargetPath[MAX_PATH] = {0};
	hr = psl->GetPath(szTargetPath, MAX_PATH, NULL, SLGP_UNCPRIORITY);
	if( FAILED(hr) )
	{
		psl->Release();
		ppf->Release();
		return FALSE;
	}
	strPath = szTargetPath;


	ppf->Release();
	psl->Release();

	if (strPath.IsEmpty()){
		return FALSE;
	}

	//strPath.SetAt(0, cSys);
	if (PathFileExists(strPath)){
		return TRUE;
	}

	/*
	// WINPE系统与所安装系统位数不同, 可能导致问题
	if (bIs64 && strPath.Find(_T("\\Program Files\\")) != -1)
	{
		strPath.Replace(_T("\\Program Files\\"), _T("\\Program Files (x86)\\"));
		return TRUE;
	}

	if (!bIs64 && strPath.Find(_T("\\Program Files (x86)\\")) != -1)
	{
		strPath.Replace(_T("\\Program Files (x86)\\"), _T("\\Program Files\\"));
		return TRUE;
	}
	*/

	return TRUE;
}

BOOL FileEnumLnkFunction(LPCTSTR lpszLink, LPVOID lpParam)
{
	SOFTFILE_LIST& fileList = *(SOFTFILE_LIST*)lpParam;
	if ( _tcsicmp( PathFindExtension( lpszLink ), _T(".lnk")) != 0)
		return TRUE;
	
	CString strPath;
	CShellLinkInfo shellInfo;
	if ( shellInfo.Open(lpszLink) )
	{
		USES_CONVERSION;
		strPath = A2T(shellInfo.GetRealFilePath());
	}
	else
	{
		if ( !GetShotcutPath(lpszLink, strPath, 'c', FALSE) )
			return TRUE;
	}

	BOOL bFind = FALSE;
	for ( long n = 0 ; n < fileList.GetSize() ; n++)
	{
		ST_SOFTFILE& s = fileList[n];
		
		if ( strPath.CompareNoCase( s.strFile) == 0)
		{
			bFind = TRUE;
			break;
		}
	}
	if ( bFind )
	{
		
		ST_SOFTFILE softFile;
		wcscpy_s(softFile.strFile, sizeof(softFile.strFile)/sizeof(WCHAR), lpszLink);
		fileList.push_back(softFile);
		GrpMsg(GroupName, MsgLevel_Msg, _T("找到快捷方式:%s"), lpszLink);
	}


	return TRUE;
}

STDMETHODIMP CSoftScan::GetSoftFile(SOFTID softid, SOFTFILE_LIST& fileList)
{

	CFuncTime(_T("CSoftScan"), _T("GetSoftFile"));
	LPCWSTR lpPath = GetSoftInfo(softid, SOFT_PATH);
	RASSERT(lpPath, E_FAIL);

	CString sql;
	sql.Format(_T("select * from softfile where softid=%d"), softid);

	BOOL bGet = FALSE;
	try
	{
		int i=2;
		while(i-- > 0)
		{
			if((1 == i && !m_dbOK)
				|| (0 == i && !m_udbOK))
				continue;

			CppSQLite3DB& db = (i==1?m_DB:m_uDB);
			CppSQLite3Query qResult = db.execQuery(sql);

			if(!qResult.eof() && !bGet)
				bGet = TRUE;

			while (!qResult.eof())
			{	
				LPCTSTR lpFile = qResult.getStringField(_T("file"));
				if(lpFile)
				{
					ParseSoftFile(lpPath, lpFile, fileList);
				}
				qResult.nextRow();
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return E_FAIL;
	}

	if(!bGet && lpPath && _tcslen(lpPath) > 0)	//库里没有配置文件，则找*.*
	{
		ParseSoftFile(lpPath, _T(".exe"), fileList);
	}


	

	if ( !CPeApi::IsPe() )
	{
		int shGuid[] = { //插入快捷方式
			CSIDL_COMMON_DESKTOPDIRECTORY, 
			CSIDL_DESKTOP,
			CSIDL_COMMON_STARTMENU,
			CSIDL_STARTMENU,
		};


		if ( fileList.GetSize() )
		{
			for ( int nLoop = 0 ; nLoop < _countof(shGuid) ; nLoop++)
			{
				TCHAR szDeskTop[ MAX_PATH ] = { 0 };
				if ( SHGetSpecialFolderPath(NULL, szDeskTop, shGuid[nLoop], FALSE) )
				{
					msapi::CEnumFile(FileEnumLnkFunction, (LPVOID)&fileList,TRUE).EnumFile(szDeskTop, _T("*.*"));
				}
			}
		}
	}
	else
	{
		TCHAR sShortCut[][MAX_PATH]  = { 
			{_T("C:\\Users\\")} ,
			{_T("C:\\Users\\Administrator\\Desktop\\")} ,
			{_T("C:\\Users\\All Users\\Desktop\\")} ,
			{_T("C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\")} ,
			{_T("C:\\Users\\Public\\Desktop\\")} ,
			{_T("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\")} ,
			{_T("C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch\\")} ,
			{_T("C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu\\")} ,
			{_T("C:\\Documents and Settings\\Administrator\\Desktop\\")} ,
			{_T("C:\\Documents and Settings\\Administrator\\桌面\\")} ,
			{_T("C:\\Documents and Settings\\Administrator\\Start Menu\\")} ,
			{_T("C:\\Documents and Settings\\Administrator\\「开始」菜单\\")} ,
			{_T("C:\\Documents and Settings\\All Users\\Desktop\\")} ,
			{_T("C:\\Documents and Settings\\All Users\\桌面\\")} ,
			{_T("C:\\Documents and Settings\\All Users\\Start Menu\\")} ,
			{_T("C:\\Documents and Settings\\All Users\\「开始」菜单\\")} ,
			{_T("C:\\Documents and Settings\\Administrator\\Application Data\\Microsoft\\Internet Explorer\\Quick Launch\\")} ,
			{_T("C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\")} ,
			{_T("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\")} ,
	
		};
		
		for ( int nLoop = 0 ; nLoop < _countof(sShortCut) ; nLoop++ )
		{
			//if ( PathFileExists( sShortCut[nLoop]))
			//{
			ZM1_GrpDbg(GroupName, _T("扫描快捷方式目录:%s"), sShortCut[nLoop]);
				msapi::CEnumFile(FileEnumLnkFunction, (LPVOID)&fileList,TRUE).EnumFile(CPeApi::GetRealPath(sShortCut[nLoop]), _T("*.*"));
			//}
		}
	}
	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftPath(SOFTID softid, SOFTPATH_LIST& pathList)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftPath"));
	LPCWSTR lpPath = GetSoftInfo(softid, SOFT_PATH);

	CString sql;
	sql.Format(_T("select * from softpath where softid=%d"), softid);

	BOOL bGet = FALSE;
	try
	{
		int i=2;
		while(i-- > 0)
		{
			if((1 == i && !m_dbOK)
				|| (0 == i && !m_udbOK))
				continue;

			CppSQLite3DB& db = (i==1?m_DB:m_uDB);
			CppSQLite3Query qResult = db.execQuery(sql);

			if(!qResult.eof() && !bGet)
				bGet = TRUE;

			while (!qResult.eof())
			{	
				int nType		= qResult.getIntField(_T("type"));
				LPCTSTR lpBuf	= qResult.getStringField(_T("path"));
				if(lpBuf)
				{
					ParseSoftPath(lpPath, lpBuf, nType, pathList);
				}
				qResult.nextRow();
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return E_FAIL;
	}

	if(!bGet && lpPath && _tcslen(lpPath) > 0)
	{
		ParseSoftPath(lpPath, lpPath, PATHTYPE_FILE, pathList);
	}

	LPCWSTR lpszUnstPath = GetSoftInfo(softid, SOFT_REG_UNINSTALL);
	if ( lpszUnstPath && _tcslen( lpszUnstPath ))
	{
		ST_SOFTPATH softPath = { 0};
		softPath.nType = PATHTYPE_REG_KEY;
		_tcscpy_s( softPath.strPath, _countof(softPath.strPath), lpszUnstPath);
		pathList.push_back(softPath);
	}
	
	return S_OK;
}

HRESULT CSoftScan::ParseSoftFile(LPCTSTR lpPath, LPCTSTR lpFile, SOFTFILE_LIST& fileList)
{
	RASSERT(lpPath && lpFile, E_FAIL);

	//先看lpFile是不是解出来的绝对路径
	CString strFile = ExpandPath(lpFile);
	if(!strFile.IsEmpty() && strFile.Find(':') > 0)//绝对路径
	{
		return InsertFile(strFile, fileList);
	}

	CString strFind;
	strFind.Format(_T("%s\\%s"), lpPath, _T("*.*"));

	CFindFile findFile;
	BOOL bFind = findFile.FindFile(strFind);
	while(bFind)
	{
		CString strPathFile = findFile.GetFilePath();
		CString strExt = PathFindExtension(strPathFile);
		CString strFileName = findFile.GetFileName();

		if ( strFileName.Compare(_T(".")) == 0 || strFileName.Compare(_T("..")) == 0)
		{
			bFind = findFile.FindNextFile();
			continue;
		}

		if ( findFile.IsDirectory() )
		{
			ParseSoftFile(strPathFile, lpFile, fileList);
		}

		
		else if ( strExt.CompareNoCase(lpFile) == 0 )
		{
		
			
			
				InsertFile(findFile.GetFilePath(), fileList);
			
		}
		
		bFind = findFile.FindNextFile();
	}


	


	return S_OK;
}

//
HRESULT CSoftScan::InsertFile(LPCTSTR lpFile, SOFTFILE_LIST& fileList)
{
	RASSERT(lpFile, E_FAIL);
	TCHAR szPathTemp[MAX_PATH] = { 0 };
	if ( CPeApi::PathIsSystemFolder(msapi::GetPathFilePath(lpFile, szPathTemp, MAX_PATH)))
	{
		ZM1_GrpError(GroupName, _T("系统目录，不能删除:%s"), lpFile);
		return S_OK;
	}

	DWORD dwAttribute = GetFileAttributes(lpFile);
	RASSERT(INVALID_FILE_ATTRIBUTES != dwAttribute && !(dwAttribute&FILE_ATTRIBUTE_DIRECTORY), E_FAIL);

	ST_SOFTFILE softFile;
	wcscpy_s(softFile.strFile, sizeof(softFile.strFile)/sizeof(WCHAR), lpFile);
	fileList.push_back(softFile);

	return S_OK;
}

HRESULT CSoftScan::ParseSoftPath(LPCTSTR lpPath, LPCTSTR lpBuf, int nType, SOFTPATH_LIST& pathList)
{
	//考虑那种已卸载但是卸载不干净的，即lpPath为NULL，这情况也得继续检查下去
	RASSERT(lpBuf, E_FAIL);

	if(PATHTYPE_NOFILE < nType)	//非文件类型
	{
		return InsertPath(lpBuf, nType, pathList);
	}

	//先看lpBuf是不是解出来的绝对路径
	CString strPath = ExpandPath(lpBuf);
	if(!strPath.IsEmpty() && strPath.Find(':') > 0)//绝对路径
	{
		return InsertPath(strPath, nType, pathList);
	}

	RASSERT(lpPath, S_FALSE);
	CString strFind;
	if(!strPath.IsEmpty())
		strFind.Format(_T("%s\\%s"), lpPath, lpBuf);
	else
		strFind = lpPath;

	DWORD dwAttribute = GetFileAttributes(strFind);
	if(INVALID_FILE_ATTRIBUTES != dwAttribute && dwAttribute&FILE_ATTRIBUTE_DIRECTORY)
	{
		//是文件夹
		return InsertPath(strFind, nType, pathList);
	}

	CFindFile findFile;
	BOOL bFind = findFile.FindFile(strFind);
	while(bFind)
	{
		if(!findFile.IsDots())
		{
			InsertPath(findFile.GetFilePath(), nType, pathList);
		}

		bFind = findFile.FindNextFile();
	}

	return S_OK;
}

HRESULT CSoftScan::InsertPath(LPCTSTR lpPath, int nType, SOFTPATH_LIST& pathList)
{
	RASSERT(lpPath, E_FAIL);

	if(PATHTYPE_NOFILE > nType)
	{
		//DWORD dwAttribute = GetFileAttributes(lpPath);
		//RASSERT(INVALID_FILE_ATTRIBUTES != dwAttribute, E_FAIL);
	}
	else if(PATHTYPE_REG_KEY == nType || PATHTYPE_REG_VALUE == nType)	//注册表
	{
		//那校验一下注册表在不在
		RASSERT(IsRegExist(lpPath, PATHTYPE_REG_VALUE == nType), E_FAIL);
	}

	ST_SOFTPATH softPath = { 0 };
	softPath.nType = nType;

	if(PATHTYPE_NOFILE > nType)
	{
		//DWORD dwAttribute = GetFileAttributes(lpPath);
		//RASSERT(INVALID_FILE_ATTRIBUTES != dwAttribute, E_FAIL);
		if ( !CPeApi::PathIsSystemFolder(lpPath) )
		{
			wcscpy_s(softPath.strPath, sizeof(softPath.strPath)/sizeof(WCHAR), CPeApi::GetRealPath(lpPath) );
		}
		else
			ZM1_GrpError(GroupName, _T("系统目录，不能删除:%s"), softPath.strPath);
		
	}
	else
	{
		wcscpy_s(softPath.strPath, sizeof(softPath.strPath)/sizeof(WCHAR), lpPath);
	}
	pathList.push_back(softPath);

	return S_OK;
}

HRESULT CSoftScan::RefushSoftInfo(BOOL bOnlyExist, LPCWSTR lpCondition)
{
	m_softInfo.clear();
	m_find = m_softInfo.begin();

	SOFTRULES rules;
	RFAILED(GetSoftRules(rules, lpCondition));

	SOFTRULES::const_iterator _it = rules.begin();
	for(; _it != rules.end(); _it++)
	{
		BOOL bIsExist = FALSE;
		const ST_SOFT_RULE& rule = *_it;

		CString strPath;
		if(SUCCEEDED(ScanSoft(strPath, rule.strExpendPath, CPeApi::GetRootKey(rule.hRootKey), 
				CPeApi::GetRealRegPath(CPeApi::GetFullRegPath(CPeApi::GetRootKey(rule.hRootKey),rule.strSubKey )), rule.strValueName))
				&& !strPath.IsEmpty()
				)
		{
			bIsExist = TRUE;
		}
	
		if(!bIsExist && bOnlyExist)
			continue;

		SOFTINFO_MAP::iterator _it = m_softInfo.find(rule.softid);
		if(_it != m_softInfo.end())	//之前就已经有了
		{
			//看之前的记录是标识已安装还是未安装，未安装且这次是已安装，那就用这次的覆盖，否则不管
			ST_SOFT_INFO &info = (_it->second);

			CString strOldPath;
			if(info.kvValue.find(SOFT_PATH) != info.kvValue.end())
			{
				strOldPath = info.kvValue[SOFT_PATH];
			}

			if(bIsExist && (!info.bIsExist || strOldPath.IsEmpty()))
			{
				//只需要修改路径就可以了，其他属性字段那就是上次就读好了的
				info.bIsExist = TRUE;
				info.kvValue[SOFT_PATH] = CPeApi::GetRealPath(strPath) ;
				//ZM1_GrpInfo(GroupName, _T("找到软件(%s)"), strPath);
				TCHAR szFullPath[ 1024 ] = { 0 };
				info.kvValue[SOFT_REG_UNINSTALL] = CRegistry::GetFullPath(rule.hRootKey, rule.strSubKey, szFullPath, _countof(szFullPath) );
			}
		}
		else
		{
			//之前没有，那一定是插入一条新的
			ST_SOFT_INFO info;
			info.bIsExist	= bIsExist;

			GetSoftAttribute(rule.softid, info.kvValue, (0 == rule.appid)?SOFTLIB_DEFAULE:SOFTLIB_USER);
			if(!strPath.IsEmpty())
				info.kvValue[SOFT_PATH] = CPeApi::GetRealPath(strPath);

			//ZM1_GrpInfo(GroupName, _T("找到软件,新插入(%s)"), strPath);

			CString str;
			str.Format(_T("%d"), rule.appid);
			info.kvValue[SOFT_APPID] = str;
			TCHAR szFullPath[ 1024 ] = { 0 };
			info.kvValue[SOFT_REG_UNINSTALL] = CRegistry::GetFullPath(rule.hRootKey, rule.strSubKey, szFullPath, _countof(szFullPath) );
			


			m_softInfo[rule.softid] = info;
		}
	}


	//
	for ( SOFTINFO_MAP::iterator it = m_softInfo.begin() ; it != m_softInfo.end() ; it++)
	{
		ST_SOFT_INFO& info = (it->second);
		CString strSoftName = info.kvValue[SOFT_NAME];
		CString strCompany =  info.kvValue[SOFT_COMPANY];

		BOOL bWhite = FALSE;
		//在这里匹配白名单
		for (long lLoop = 0 ;  lLoop < m_softWiteList.GetSize() ; lLoop++ )
		{
			ST_SOFT_WHITE& softWhite = m_softWiteList[lLoop];

			switch( softWhite.whiteAttr )
			{
			case SOFT_WHITE_SOFT_NAME_WHOLEWORD:
				{
					if ( strSoftName.GetLength() && strSoftName.CompareNoCase(softWhite.strValue) == 0)
						bWhite = TRUE;

					break;
				}
			case SOFT_WHITE_SOFT_NAME:
				{
					if ( strSoftName.GetLength() && strSoftName.Find(softWhite.strValue) != -1)
						bWhite = TRUE;
				}
			case SOFT_WHITE_SOFT_COMPANY_WHOLEWORD:
				{
					if ( strCompany.GetLength() && strCompany.CompareNoCase(softWhite.strValue) == 0)
						bWhite = TRUE;
					break;
				}
			case SOFT_WHITE_SOFT_COMPANY:
				{
					if ( strCompany.GetLength() && strCompany.Find(softWhite.strValue) != -1)
						bWhite = TRUE;

					break;
				}
			}

			if ( bWhite )
			{
				break;
			}
		}


		info.kvValue[SOFT_WHITE].Format(_T("%d"), bWhite);
	}

	return S_OK;
}

HRESULT CSoftScan::RefushSoftClass()
{
	m_softClass.clear();
	LPCTSTR sql = _T("select * from class");
	try
	{
		int i=2;
		while(i-- > 0)
		{
			if((1 == i && !m_dbOK)
				|| (0 == i && !m_udbOK))
				continue;

			CppSQLite3DB& db = (i==1?m_DB:m_uDB);
			CppSQLite3Query qResult = db.execQuery(sql);
			while (!qResult.eof())
			{	
				DWORD   dwClass	= qResult.getIntField(_T("class"));
				LPCTSTR lpName	= qResult.getStringField(_T("name"));
				if(lpName)
				{
					m_softClass[dwClass] = lpName;
				}

				qResult.nextRow();
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}
	return S_OK;
}

HRESULT CSoftScan::GetSoftRules(SOFTRULES& rules, LPCWSTR lpCondition)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftRules"));
	CString sql = _T("select * from soft");
	if(lpCondition && wcslen(lpCondition) > 0)
	{
		sql += _T(" WHERE (");
		sql += lpCondition;
		sql += _T(")");
	}

	sql += _T(" order by softid");

	try
	{
		int i=2;
		while(i-- > 0)
		{
			if((1 == i && !m_dbOK)
				|| (0 == i && !m_udbOK))
				continue;

			CppSQLite3DB& db = (i==1?m_DB:m_uDB);
			CppSQLite3Query qResult = db.execQuery(sql);
			while (!qResult.eof())
			{	
				ST_SOFT_RULE Item ;

				Item.softid			= (SOFTID)qResult.getIntField(_T("softid"));
				Item.strExpendPath	= DB_STRING(qResult, _T("path"));
				Item.hRootKey       = ParseRegPath(DB_STRING(qResult, _T("regpath")), Item.strSubKey, Item.strValueName);
				if(i==1)
				{
					Item.appid = 0;
				}
				else
				{
					Item.appid = qResult.getIntField(_T("appid"));
				}

				rules.push_back(Item);

				qResult.nextRow();
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;

	}
	return S_OK;
}

BOOL EnumInstallPathCallback( HKEY hKey, LPCTSTR lpszName, DWORD dwType, LPVOID lpParam);
HRESULT CSoftScan::ScanSoft(CString &strPath, LPCTSTR lpExpendPath, HKEY hRootKey, LPCTSTR lpSubKey, LPCTSTR lpValueName)
{
	//ZM1_GrpInfo(GroupName, _T("扫描软件：%s, %s"), lpSubKey, lpValueName);
	CString strExpendPath = ExpandPath(lpExpendPath, FALSE);

	if(!strExpendPath.IsEmpty() && strExpendPath.Find(':') > 0)//绝对路径
	{
		return ScanPath(strPath, strExpendPath);
	}
	else if(!strExpendPath.IsEmpty() && NULL == hRootKey)
	{
		//不是绝对路径，又没有注册表，那当服务进行一把扫描
		return ScanService(strPath, lpExpendPath);
	}

	//通过注册表找
	RASSERT(hRootKey && lpSubKey && lpValueName, E_INVALIDARG);
	CString strSubKey = lpSubKey;

	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Open(hRootKey, lpSubKey, KEY_READ))
	{
		TCHAR	szPath[_MAX_PATH]={0};
		ULONG	nChars = _MAX_PATH;
		if(ERROR_SUCCESS == regKey.QueryValue(lpValueName, NULL, szPath, &nChars))
		{
			CString strGetPath = GetFirstPath(szPath);

			if(strExpendPath.GetLength() > 0)
			{
				int iCount = strExpendPath.Replace(_T("..\\"), _T(""));
				if(iCount > 0)
				{
					while(iCount-- > 0)
					{
						int iIndex = strGetPath.ReverseFind(_T('\\'));
						if(iIndex >= 0)
						{
							strGetPath.Delete(iIndex, strGetPath.GetLength() - iIndex);
						}
					}
				}

				if(strExpendPath.GetLength() > 0)
				{
					strGetPath += _T("\\");
					strGetPath += strExpendPath;
				}
			}
			strPath = strGetPath;

			//特殊处理一下路径
			while(1)
			{
				DWORD dwAttribute = GetFileAttributes(CPeApi::GetRealPath(strPath));
				if(INVALID_FILE_ATTRIBUTES == dwAttribute)
				{
					//可能是类似E:\Program Files\Tencent\RTXC\RTX.exe,-0这种
					int iIndex = strPath.Find(_T(".exe"));
					if(iIndex < 0)
						iIndex = strPath.Find(_T(".dll"));

					if(strPath.Find(_T(':')) > 0 && iIndex > 0 && (strPath.GetLength() > (iIndex+4)))
					{
						iIndex += 4; //偏移.exe路径
						strPath.Delete(iIndex, strPath.GetLength() - iIndex);
						continue;
						//这里不能break，故意留着等待下次循环
					}
					else
					{
						//strPath.Empty();
					}
				}
				else if(!(dwAttribute&FILE_ATTRIBUTE_DIRECTORY))	//不是文件夹，是文件
				{
					//自动去掉最后一层
					int iIndex = strPath.ReverseFind(_T('\\'));
					if(iIndex >= 0)
					{
						strPath.Delete(iIndex, strPath.GetLength() - iIndex);
					}
				}

				break;
			}
		}

		if ( strPath.IsEmpty() )
		{
			msapi::CEnumReg(hRootKey).EnumValue(lpSubKey, EnumInstallPathCallback, &strPath);
		}
	}
	else
	{
		TCHAR szErr[ MAX_PATH ] = { 0 };
		//ZM1_GrpError(GroupName, _T("CSoftScan.ScanSoft Open(%s, %s)"), CPeApi::GetFullRegPath(hRootKey,lpSubKey), GetLastErrorText(szErr, MAX_PATH));
	}

	return S_OK;
}

HRESULT CSoftScan::ScanPath(CString &strPath, LPCTSTR lpExpendPath)
{
	RASSERT(lpExpendPath, E_INVALIDARG);
	CString strExpendPath = lpExpendPath;
	RASSERT(!strExpendPath.IsEmpty() && strExpendPath.Find(':') > 0, E_INVALIDARG);

	if(strExpendPath.GetAt(0) == _T('?'))	//第一个是问号
	{
		//枚举驱动器，且看哪个路径存在
		TCHAR szBuf[100];
		int nCount = GetLogicalDriveStrings(100, szBuf);
		RASSERT(nCount > 0, E_FAIL);

		int i = 0;
		for(; i < nCount; i++)
		{
			TCHAR ch = szBuf[i];
			if(ch >= _T('C') && ch <= 'Z') //不处理A、B盘
			{
				i+=3;
				strExpendPath.SetAt(0, ch);

				DWORD dwAttribute = GetFileAttributes(strExpendPath);
				if(INVALID_FILE_ATTRIBUTES != dwAttribute/* && dwAttribute&FILE_ATTRIBUTE_DIRECTORY*/)
				{
					strPath = strExpendPath;
					break;
				}
			}
		}
	}
	else
	{
		DWORD dwAttribute = GetFileAttributes(strExpendPath);
		if(INVALID_FILE_ATTRIBUTES != dwAttribute/* && dwAttribute&FILE_ATTRIBUTE_DIRECTORY*/)
			strPath = strExpendPath;
	}
	return S_OK;
}


HRESULT CSoftScan::ScanService(CString &strPath, LPCTSTR lpSrvName)
{
	RASSERT(lpSrvName && _tcslen(lpSrvName), E_INVALIDARG);

	CString strSrvReg;
	strSrvReg.Format(_T("SYSTEM\\CurrentControlSet\\services\\%s"), lpSrvName);

	CRegKey regKey;
	RASSERT(ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strSrvReg, KEY_READ), E_FAIL);

	CString strValue;
	ULONG ulLen = MAX_PATH*2;
	RASSERT(ERROR_SUCCESS == regKey.QueryStringValue(_T("ImagePath"), strValue.GetBuffer(ulLen), &ulLen), E_FAIL);
	strValue.ReleaseBuffer();

	CString strServerPath = ExpandPath(strValue);

	if(strServerPath.GetAt(0) != _T('"'))
	{
		int iIndex = strServerPath.Find(_T(".exe"));
		if(iIndex < 0)
			iIndex = strServerPath.Find(_T(".dll"));

		if(iIndex > 0)
		{
			iIndex += 4; //偏移.exe路径
			strServerPath.Insert(0, _T('"'));
			strServerPath.Insert(iIndex+1, _T('"'));
		}
	}

	strPath = strServerPath;
	return S_OK;
}

HRESULT CSoftScan::GetSoftAttribute(SOFTID softid, KV_MAP &kvValue, DWORD softLib)
{
	CString sql;
	sql.Format(_T("select * from attribute where softid=%d order by key"), softid);

	try
	{
		CppSQLite3DB& db = (softLib==SOFTLIB_DEFAULE?m_DB:m_uDB);
		CppSQLite3Query qResult = db.execQuery(sql);
		while (!qResult.eof())
		{	
			int k = qResult.getIntField(_T("key"));
			if ( k == SOFT_CLEAN_LEAVE)
			{
				int a = 0;
			}
			kvValue[k] = DB_STRING(qResult, _T("value"));

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return E_FAIL;
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
#define IsUserSoftid(softid) (softid >= SOFTID_USER_BASE)

STDMETHODIMP_(SOFTID) CSoftScan::FindSoftEx(BOOL bOnlyExist, DWORD softLib, LPCWSTR lpCondition/* = NULL*/)
{
	Open(softLib);
	RefushSoftClass();
	RefushSoftInfo(bOnlyExist, lpCondition);

	if(softLib&SOFTLIB_ENUM)
		EnumSoftInfo(softLib&SOFTLIB_MERGE);

	m_find = m_softInfo.begin();
	return FindNextSoft();
}

STDMETHODIMP CSoftScan::GetCurMaxSoftid(SOFTID* pdefSoftid, SOFTID* puserSoftid)
{
	if(pdefSoftid && m_dbOK)
	{
		GetMaxSoftid(m_DB, *pdefSoftid);
	}

	if(puserSoftid && m_udbOK)
	{
		GetMaxSoftid(m_uDB, *puserSoftid);
	}
	return S_OK;
}

HRESULT CSoftScan::GetMaxSoftid(CppSQLite3DB& db, SOFTID &softid)
{
	CFuncTime(_T("CSoftScan"), _T("GetMaxSoftid"));
	RASSERT(db.mpDB, E_FAIL);

	softid = 0;
	CString sql = _T("select max(softid) from soft");
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		if(!qResult.eof())
		{
			softid= qResult.getIntField(0);
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CSoftScan::DelSoft(SOFTID softid)
{
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	DWORD softlib = IsUserSoftid(softid)?SOFTLIB_USER:SOFTLIB_DEFAULE;
	BeginTrans(softlib);

	HRESULT hr = S_OK;
	CString sql;
	try
	{
		sql.Format(_T("DELETE FROM attribute WHERE softid=%d"), softid);
		db.execDML(sql);

		sql.Format(_T("DELETE FROM softfile WHERE softid=%d"), softid);
		db.execDML(sql);

		sql.Format(_T("DELETE FROM softpath WHERE softid=%d"), softid);
		db.execDML(sql);

		sql.Format(_T("DELETE FROM soft WHERE softid=%d"), softid);
		db.execDML(sql);
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		hr = S_FALSE;
	}

	hr = EndTrans(softlib);
	if(FAILED(hr))
		RollBackTrans(softlib);

	return hr;
}

STDMETHODIMP_(BOOL) CSoftScan::IsHave(SOFTID softid)
{
	CFuncTime(_T("CSoftScan"), _T("IsHave"));
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	sql.Format(_T("select softid from soft where softid=%d"), softid);
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		if(!qResult.eof())
			return TRUE;
		else
			return FALSE;
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return FALSE;
	}

	return FALSE;
}

STDMETHODIMP CSoftScan::GetSoftRule(SOFTID softid, SOFTRULE_LIST& ruleList)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftRule"));
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	sql.Format(_T("select * from soft where softid=%d"), softid);
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		while (!qResult.eof())
		{	
			ST_SOFTRULE rule;
			ZeroMemory(&rule, sizeof(rule));

			LPCTSTR lpPath	= qResult.getStringField(_T("path"));
			if(lpPath)
				_tcscpy_s(rule.strPath, sizeof(rule.strPath)/sizeof(WCHAR), lpPath);

			LPCTSTR lpRegPath	= qResult.getStringField(_T("regpath"));
			if(lpRegPath)
				_tcscpy_s(rule.strRegPath, sizeof(rule.strRegPath)/sizeof(WCHAR), lpRegPath);

			rule.dwAppid	= qResult.getIntField(_T("appid"));
			ruleList.push_back(rule);

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftRule(SOFTID softid, SOFTRULE_LIST& ruleList)
{
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	try
	{
		sql.Format(_T("DELETE FROM soft WHERE softid=%d"), softid);//先删除
		db.execDML(sql);

		long lSize = ruleList.GetSize();
		long i = 0;
		while(i < lSize)
		{
			const ST_SOFTRULE& rule = ruleList[i];
			sql.Format(_T("INSERT INTO soft(softid,path,regpath,appid) VALUES(%d,'%s','%s',%d)"),
				softid, rule.strPath, rule.strRegPath, rule.dwAppid);
			db.execDML(sql);

			i++;
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftAttribute(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftAttribute"));
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	sql.Format(_T("select * from attribute where softid=%d order by key"), softid);
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		while (!qResult.eof())
		{	
			ST_SOFTATTRIBUTE rule;
			ZeroMemory(&rule, sizeof(rule));

			rule.dwKey		= qResult.getIntField(_T("key"));
			LPCTSTR lpValue	= qResult.getStringField(_T("value"));
			if(lpValue)
				_tcscpy_s(rule.strValue, sizeof(rule.strValue)/sizeof(WCHAR), lpValue);

			atttibuteList.push_back(rule);

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftAttribute(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList)
{
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	try
	{
		sql.Format(_T("DELETE FROM attribute WHERE softid=%d"), softid);//先删除
		db.execDML(sql);

		long lSize = atttibuteList.GetSize();
		long i = 0;
		while(i < lSize)
		{
			const ST_SOFTATTRIBUTE& rule = atttibuteList[i];
			sql.Format(_T("INSERT INTO attribute(softid,key,value) VALUES(%d,%d,'%s')"),
				softid, rule.dwKey, rule.strValue);
			db.execDML(sql);

			i++;
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftFileRule(SOFTID softid, SOFTFILE_LIST& fileList)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftFileRule"));
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	sql.Format(_T("select * from softfile where softid=%d"), softid);
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		while (!qResult.eof())
		{	
			ST_SOFTFILE rule;
			ZeroMemory(&rule, sizeof(rule));

			LPCTSTR lpFile	= qResult.getStringField(_T("file"));
			if(lpFile)
				_tcscpy_s(rule.strFile, sizeof(rule.strFile)/sizeof(WCHAR), lpFile);

			fileList.push_back(rule);

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftFileRule(SOFTID softid, SOFTFILE_LIST& fileList)
{
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	try
	{
		sql.Format(_T("DELETE FROM softfile WHERE softid=%d"), softid);//先删除
		db.execDML(sql);

		long lSize = fileList.GetSize();
		long i = 0;
		while(i < lSize)
		{
			const ST_SOFTFILE& rule = fileList[i];
			sql.Format(_T("INSERT INTO softfile(softid,file) VALUES(%d,'%s')"),
				softid, rule.strFile);
			db.execDML(sql);

			i++;
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftPathRule(SOFTID softid, SOFTPATH_LIST& pathList)
{
	CFuncTime(_T("CSoftScan"), _T("GetSoftPathRule"));
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);

	CString sql;
	sql.Format(_T("select * from softpath where softid=%d"), softid);
	try
	{
		CppSQLite3Query qResult = db.execQuery(sql);
		while (!qResult.eof())
		{	
			ST_SOFTPATH rule;
			ZeroMemory(&rule, sizeof(rule));

			rule.nType		= qResult.getIntField(_T("type"));
			LPCTSTR lpPath	= qResult.getStringField(_T("path"));
			if(lpPath)
				_tcscpy_s(rule.strPath, sizeof(rule.strPath)/sizeof(WCHAR), lpPath);

			pathList.push_back(rule);

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftPathRule(SOFTID softid, SOFTPATH_LIST& pathList)
{
	CppSQLite3DB& db = (IsUserSoftid(softid)?m_uDB:m_DB);
	RASSERT(db.mpDB, E_FAIL);
	DWORD softlib = IsUserSoftid(softid)?SOFTLIB_USER:SOFTLIB_DEFAULE;

	CString sql;
	try
	{
		//BeginTrans(softlib);
		sql.Format(_T("DELETE FROM softpath WHERE softid=%d"), softid);//先删除
		db.execDML(sql);

		long lSize = pathList.GetSize();
		long i = 0;
		while(i < lSize)
		{
			const ST_SOFTPATH& rule = pathList[i];
			sql.Format(_T("INSERT INTO softpath(softid,type,path) VALUES(%d,%d,'%s')"),
				softid, rule.nType, rule.strPath);
			db.execDML(sql);

			i++;
		}
		//EndTrans(softlib);
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftWhite(SOFT_WHITE_LIST& softWhite)
{
	if ( m_dbOK )
		SetSoftWhite(SOFTLIB_DEFAULE, softWhite);
	if ( m_udbOK )
		SetSoftWhite(SOFTLIB_USER, softWhite);

	return S_OK;
}

STDMETHODIMP CSoftScan::SetSoftWhite(DWORD softLib, SOFT_WHITE_LIST& softWhite)
{
	CppSQLite3DB& db = softLib & SOFTLIB_DEFAULE ? m_DB : m_uDB;

	//先删除
	//DELETE FROM WHITE WHERE attr = 1 and value = 'MaxDos'
	BeginTrans(softLib);
	for ( long nLoop = 0 ; nLoop < softWhite.GetSize() ; nLoop++)
	{
		ST_SOFT_WHITE& soft = softWhite[nLoop];
		CString strDel; strDel.Format(_T("DELETE FROM WHITE WHERE attr = %d and value = '%s'"), soft.whiteAttr, soft.strValue);

		try//先删除再插入
		{
			db.execDML(strDel);
		}
		catch (CppSQLite3Exception& e)
		{
			PrintfError(strDel,e);
			continue;
		}

		CString strInst; strInst.Format(_T("INSERT INTO WHITE(attr,value) VALUES(%d,'%s')"), soft.whiteAttr, soft.strValue);
		try//先删除再插入
		{
			db.execDML(strInst);
		}
		catch (CppSQLite3Exception& e)
		{
			PrintfError(strInst,e);
			continue;
		}
	}
	EndTrans(softLib);

	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftWhite(SOFT_WHITE_LIST& softWhite)
{
	
	if ( m_dbOK )
		GetSoftWhite(SOFTLIB_DEFAULE, softWhite);
	if ( m_udbOK )
		GetSoftWhite(SOFTLIB_USER, softWhite);

	return S_OK;
}

STDMETHODIMP CSoftScan::GetSoftWhite(DWORD softLib, SOFT_WHITE_LIST& softWhite)
{
	TCHAR szSelWhite[] = _T("SELECT * FROM WHITE;");
	CppSQLite3DB& db = softLib & SOFTLIB_DEFAULE ? m_DB : m_uDB;
	try
	{
		CppSQLite3Query qResult =  db.execQuery(szSelWhite);
		while (!qResult.eof())
		{	
			DWORD   dwAttr	= qResult.getIntField(_T("attr"));
			LPCWSTR lpName	= qResult.getStringField(_T("value"));
			if(lpName)
			{
				ST_SOFT_WHITE w = { 0 };
				w.whiteAttr = dwAttr;

				wcscpy_s( w.strValue, _countof(w.strValue), lpName );
				softWhite.push_back( w );
			}

			qResult.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(szSelWhite,e);
		return E_FAIL;
	}

	return S_OK;
}

/*
BOOL CSoftScan::DelSoftFiles(Json::Value& root, DWORD dwSoftID,SOFTFILE_LIST& files)
{
	LPCWSTR lpszSoftName = GetSoftInfo(dwSoftID, SOFT_NAME);


	for ( long lLoop = 0 ; lLoop < files.GetSize() ; lLoop++ )
	{
		ST_SOFTFILE& file = files[lLoop];
		CString strBkPath = file.strFile;
		strBkPath = strBkPath.Mid(3);

		CString strPath;strPath.AppendFormat(_T("%s\\%s\\lgbk\\%s"), g_BkDrive, lpszSoftName, strBkPath);
		msapi::CopyPath(file.strFile, strPath, FALSE);
		if ( !m_bDebug )
		{
			BOOL bRet = msapi::DeleteFileEx(file.strFile);
		}
	
		msapi::TyrRemoveDirectoryEx(file.strFile); //尝试删除空目录
		ZM1_GrpError(GroupName, _T("删除文件:%s"), file.strFile);
	}

	return TRUE;
}

BOOL CopyRegPath(Json::Value& root, LPCTSTR lpszKey)
{
	if ( !(lpszKey && wcslen(lpszKey)))
	{
		return ERROR_SUCCESS;
	}

	CString strRootKey = lpszKey;
	LPCTSTR lpszSubKey = NULL;

	HKEY hRoot = CRegistry::GetKey(lpszKey,&lpszSubKey);

	HKEY hRet = 0;
	CString strRealRegPath = CPeApi::GetRealRegPath(lpszKey);
	LONG lRet = RegOpenKeyEx(CPeApi::GetRootKey(hRoot), strRealRegPath,0,KEY_ALL_ACCESS, &hRet);
	if ( lRet == ERROR_SUCCESS && hRet)
	{
		int index = 0;
		while(TRUE)
		{
			TCHAR szName[ MAX_PATH ] = { 0 };
			DWORD Reserved = 0, dwType = 0,dwBufLen = 1024, dwNameLen = MAX_PATH;
			BYTE buffer[ 1024 ] = { 0 };
			//REG_SZ  REG_EXPAND_SZ*
			lRet = RegEnumValue(hRet, index, szName, &dwNameLen, NULL, &dwType, buffer, &dwBufLen);
			if( !(ERROR_SUCCESS == lRet && _tcslen(szName)))
			{
				ZM1_GrpError(GroupName, _T("RegEnumValue_备份注册表失败:%s,,%s"), strRealRegPath, lpszKey);
				break;
			}

			
			if ( dwType == REG_SZ || dwType == REG_EXPAND_SZ)
			{
				root[index]["path"] = lpszKey;
				root[index]["key"] = szName;
				root[index]["value"] = (LPCWSTR)buffer;
			}

			index++;
		}
	}
	else
		ZM1_GrpError(GroupName, _T("备份注册表失败:%s,,%s"), strRealRegPath, lpszKey);
	

	return TRUE;
}

BOOL CSoftScan::DelSoftPaths(Json::Value& root, DWORD dwSoftID,SOFTPATH_LIST& paths)
{
	LPCWSTR lpszSoftName = GetSoftInfo(dwSoftID, SOFT_NAME);
	for ( long lLoop = 0 ; lLoop < paths.GetSize() ; lLoop++ )
	{
		ST_SOFTPATH& path = paths[lLoop];
		switch( path.nType )
		{
		case PATHTYPE_FILE:
			{
				ZM1_GrpInfo(GroupName,  _T("11"));
				if ( GetFileAttributes( path.strPath) & FILE_ATTRIBUTE_DIRECTORY )
				{
					CString strBkPath = path.strPath;
					strBkPath = strBkPath.Mid(3);

					


					CString strPath;strPath.AppendFormat(_T("%s\\%s\\lgbk\\%s"), g_BkDrive, lpszSoftName, strBkPath);

				

					msapi::CopyPath(path.strPath, strPath, FALSE);


					

					if ( !m_bDebug )
						msapi::DeleteFileEx(path.strPath);

					ZM1_GrpInfo(GroupName,  _T("删除目录:%s"), path.strPath);
				}
				else
				{
				

					CString strBkPath = path.strPath;
					strBkPath = strBkPath.Mid(3);

					ZM1_GrpInfo(GroupName,  _T("16"));

					CString strPath;strPath.AppendFormat(_T("%s\\%s\\lgbk\\%s"), g_BkDrive, lpszSoftName, strBkPath);
					msapi::CopyPath(path.strPath, strPath, FALSE);

					ZM1_GrpInfo(GroupName,  _T("17"));

					if ( !m_bDebug )
						msapi::DeleteFileForce(path.strPath);

					ZM1_GrpInfo(GroupName,  _T("删除文件:%s"), path.strPath);
				}
				
				
				break;
			}
		case PATHTYPE_COM:
			{
				int n = 0;
				break;
			}
		case PATHTYPE_NOFILE:
			{
				int n = 0;
				break;
			}
		case PATHTYPE_REG_KEY:
			{
				LPCTSTR lpszKey = NULL;
				HKEY hRoot = CRegistry::GetKey(path.strPath,&lpszKey);
				if ( hRoot && lpszKey && _tcslen( lpszKey ))
				{
					int nSize = root["regs"].size();
					CopyRegPath(root["regs"][nSize], path.strPath);
					
					if( !m_bDebug )
						CRegistry::DeleteKey(CPeApi::GetRootKey(hRoot),CPeApi::GetRealRegPath(path.strPath));
					ZM1_GrpInfo(GroupName,  _T("删除注册表:%s"), path.strPath);
				}
				
				break;
			}
		case PATHTYPE_REG_VALUE:
			{
				ZM1_GrpInfo(GroupName,  _T("删除注册表值:%s"), path.strPath);
				break;
			}
		}
	}

	return TRUE;
}
*/
//注册表的备份
/*
STDMETHODIMP CSoftScan::Clean(DWORD dwSoftID, DWORD dwClean)
{
	DWORD dwTick = GetTickCount();
	LPCWSTR lpszSoftName = GetSoftInfo(dwSoftID, SOFT_NAME);
	ZM1_GrpInfo(GroupName,  _T("开始清理:%u %s"), dwSoftID, lpszSoftName);

	Json::Value root;
	Json::Value info;
	

	//获取安装安装目录
	SOFTFILE_LIST softFile;
	if ( SUCCEEDED(GetSoftFile(dwSoftID, softFile)) )
	{
	
		DelSoftFiles(root,dwSoftID, softFile);
	}

	

	SOFTPATH_LIST softPath;
	if ( SUCCEEDED(GetSoftPath(dwSoftID, softPath)) )
	{
		

		DelSoftPaths(root, dwSoftID,softPath);
	}
	
	

	TCHAR szTime[MAX_PATH] = { 0 };
	
	info["name"] = lpszSoftName;
	info["time"] = msapi::GetCurrentDateStr(szTime, MAX_PATH);
	
	CString strPath;strPath.AppendFormat(_T("%s\\%s\\lgbk.json"), g_BkDrive, lpszSoftName);
	root["info"] = info;
	Json::StyledStreamWriter().write(std::ofstream(strPath),root);

	ZM1_GrpInfo(GroupName,  _T("结束清理:%u %s 耗时:%d"), dwSoftID, lpszSoftName, GetTickCount() - dwTick);


	return S_OK;
}

STDMETHODIMP CSoftScan::DelTemp()
{
	if ( CPeApi::IsPe() && !m_bDebug)
	{
		msapi::DeleteFileEx(CPeApi::GetRealPath(_T("C:\\Windows\\Temp\\")));
		msapi::DeleteFileEx(CPeApi::GetRealPath(_T("C:\\Users\\Administrator\\AppData\\Local\\Temp\\")));
		msapi::DeleteFileEx(CPeApi::GetRealPath(_T("C:\\Documents and Settings\\Administrator\\Local Settings\\Temp\\")));
	}
	

	return S_OK;
}

STDMETHODIMP CSoftScan::DelRun()
{
	if ( CPeApi::IsPe() && !m_bDebug)
	{
		CRegistry::DeleteKey(CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), 
			CPeApi::GetRealRegPath( CPeApi::GetFullRegPath(HKEY_LOCAL_MACHINE,  _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"))));

		CRegistry::DeleteKey(CPeApi::GetRootKey(HKEY_CURRENT_USER), 
			CPeApi::GetRealRegPath( CPeApi::GetFullRegPath(HKEY_CURRENT_USER,  _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"))));

		CRegistry::DeleteKey(CPeApi::GetRootKey(HKEY_CURRENT_USER), 
			CPeApi::GetRealRegPath( CPeApi::GetFullRegPath(HKEY_CURRENT_USER,  _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"))));

		CRegistry::DeleteKey(CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), 
			CPeApi::GetRealRegPath( CPeApi::GetFullRegPath(HKEY_LOCAL_MACHINE,  _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"))));
	}
	

	return S_OK;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
BOOL CSoftScan::Open(DWORD softLib)
{
	BOOL bOpen = FALSE;

	if(softLib&SOFTLIB_DEFAULE)	//需要打开默认库
	{
		if(OpenDB(SOFTLIB_DEFAULE))
			bOpen = TRUE;
		else
			ZM1_GrpError(GroupName, _T("打开官方库失败"));
	}
	else
	{
		CloseDB(SOFTLIB_DEFAULE);
	}

	if(softLib&SOFTLIB_USER)	//需要打开用户库
	{
		if(OpenDB(SOFTLIB_USER))
			bOpen = TRUE;
		else
			ZM1_GrpError(GroupName, _T("打开用户库失败"));
	}
	else
	{
		CloseDB(SOFTLIB_USER);
	}

	return bOpen;
}


BOOL FileEnumSoftDb(LPCTSTR lpszLink, LPVOID lpParam)
{
	CString& fileList = *(CString*)lpParam;
	if ( _tcsicmp( PathFindExtension( lpszLink ), _T(".db")) != 0)
		return TRUE;

	CString strFilePath = lpszLink;strFilePath.MakeLower();
	if ( _tcsstr(PathFindFileName(strFilePath), _T("soft")))
	{
		ZM1_GrpInfo(GroupName, _T("找到soft配置库:%s"), lpszLink);
		fileList = lpszLink;
	}

	return TRUE;
}

BOOL CSoftScan::OpenDB(DWORD softLib)
{
	if((softLib&SOFTLIB_DEFAULE && m_dbOK)
		|| (softLib&SOFTLIB_USER && m_udbOK))
	{
		//已打开
		return TRUE;
	}

	CString strDBFile;
	strDBFile.Format(_T("%s\\%s"), m_strModulePath, softLib==SOFTLIB_DEFAULE?FILEDB_NAME:FILEDBU_NAME);
	ZM1_GrpError(GroupName, strDBFile);

	DWORD dwAttribute = GetFileAttributes(strDBFile);
	if(INVALID_FILE_ATTRIBUTES == dwAttribute || dwAttribute&FILE_ATTRIBUTE_DIRECTORY)
		return FALSE;


	//遍历EFI分区，找到配置文件
	if ( softLib == SOFTLIB_DEFAULE)
	{
		msapi::CEnumFile(FileEnumSoftDb, (LPVOID)&strDBFile,TRUE).EnumFile(_T("[:"), _T("*.*"));
	}

	
	try
	{
		CppSQLite3DB& db = (SOFTLIB_DEFAULE == softLib?m_DB:m_uDB);
		db.open(strDBFile,TRUE);
	}
	catch (CppSQLite3Exception& e)
	{
		PrintfError(_T("打开数据库"),e);
		return FALSE;
	}

	if(SOFTLIB_DEFAULE == softLib)
	{
		GetSoftWhite(softLib, m_softWiteList);
		m_dbOK = TRUE;
	}
	else if(SOFTLIB_USER == softLib)
	{
		GetSoftWhite(softLib, m_softWiteList);
		m_udbOK = TRUE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
VOID CSoftScan::CloseDB(DWORD softLib)
{
	switch(softLib)
	{
	case SOFTLIB_DEFAULE:
		if (m_dbOK)
		{
			m_DB.close();
			m_dbOK = FALSE;
		}
		break;
	case SOFTLIB_USER:
		if (m_udbOK)
		{
			m_uDB.close();
			m_udbOK = FALSE;
		}
		break;
	}
}

HRESULT CSoftScan::BeginTrans(DWORD softLib)
{
	HRESULT hr = S_OK;
	try
	{

		CppSQLite3DB& db = (SOFTLIB_DEFAULE == softLib?m_DB:m_uDB);
		db.execDML(_T("BEGIN TRANSACTION RESTOREPOINT;"));
	}
	catch (...) 
	{
		hr = E_UNEXPECTED;
	}

	return hr;
}

HRESULT CSoftScan::EndTrans(DWORD softLib, BOOL bFailedRollBack/* = TRUE*/) 
{
	HRESULT hr = S_OK;
	try
	{
		CppSQLite3DB& db = (SOFTLIB_DEFAULE == softLib?m_DB:m_uDB);
		db.execDML(_T("COMMIT TRANSACTION RESTOREPOINT;"));
	}
	catch (...) 
	{
		hr = E_UNEXPECTED;
	}

	if(FAILED(hr) && bFailedRollBack)
		RollBackTrans(softLib);

	return hr;
}

HRESULT CSoftScan::RollBackTrans(DWORD softLib)
{

	HRESULT hr = S_OK;
	try
	{
		CppSQLite3DB& db = (SOFTLIB_DEFAULE == softLib?m_DB:m_uDB);
		db.execDML(_T("ROLLBACK TRANSACTION RESTOREPOINT;"));
	}
	catch (...) 
	{
		hr = E_UNEXPECTED;
	}

	return hr;
}

HRESULT CSoftScan::execDML(const TCHAR* sql, DWORD softLib)
{

	HRESULT hr = E_FAIL;
	try
	{
		CppSQLite3DB& db = (SOFTLIB_DEFAULE == softLib?m_DB:m_uDB);
		hr = db.execDML(sql);
	}
	catch(CppSQLite3Exception& e)
	{
		PrintfError(sql,e);
		hr = E_FAIL;
	}
	return hr;	
}
