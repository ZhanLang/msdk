#include "StdAfx.h"
#include "SoftWareEnum.h"
#include <set>
#include <process//ParseCommand.h>
#include <msapi/EnumReg.h>
#include <util/registry.h>
#include "BootRunInfo.h"
//扫描软件

CSoftWareEnum::CSoftWareEnum(void):m_bIncludeUpdates(FALSE)
{
}

CSoftWareEnum::~CSoftWareEnum(void)
{
}

HRESULT CSoftWareEnum::Enum(REG_SOFTWARE_INFO& info, BOOL bIncludeUpdates/* = FALSE*/, BOOL bIsSvc/*FALSE*/)
{
	m_softList.clear();


	EnumSoftWares(
		CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), m_softList, 
		CPeApi::GetRealRegPath( 
			CPeApi::GetFullRegPath( HKEY_LOCAL_MACHINE, _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"))
			),NULL, CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));


	EnumSoftWares(
		CPeApi::GetRootKey( HKEY_LOCAL_MACHINE ), m_softList,
		CPeApi::GetRealRegPath(
			CPeApi::GetFullRegPath( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"))
			),NULL, CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
		);


	EnumSoftWares(
		CPeApi::GetRootKey(HKEY_CURRENT_USER), m_softList, 
		CPeApi::GetRealRegPath( 
		CPeApi::GetFullRegPath( HKEY_CURRENT_USER, _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"))
		),NULL, HKEY_CURRENT_USER, _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));




	EnumSoftWares(
		CPeApi::GetRootKey( HKEY_CURRENT_USER ), m_softList,
		CPeApi::GetRealRegPath(
		CPeApi::GetFullRegPath( HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"))
		),NULL, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
		);

	EnumSoftWares(
		CPeApi::GetRootKey( HKEY_LOCAL_MACHINE ) , m_softList, 
		CPeApi::GetRealRegPath(
			CPeApi::GetFullRegPath( HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\S-1-5-18\\Products")) 
			)
			,_T("InstallProperties")
			, CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\S-1-5-18\\Products")
		);


	//HKEY_USERS\\S-1-5-21-93264243-3190661538-3854843050-1000

	if (/*TRUE == bIsSvc*/ FALSE) //目前的清理是不可能运行到服务状态下的
	{
		/*
		//运行在服务中查找另一个注册表
		BootRunInfo::CBootRunInfo bri;
		bri.GetAllUserSid();

		CString strPath;
		std::vector<BootRunInfo::ST_UserInfo>::const_iterator itId;
		for (itId = bri.m_vtUserInfo.begin(); itId != bri.m_vtUserInfo.end(); ++itId)
		{
			strPath.Format(_T("%s\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), itId->strSID.c_str());
			EnumSoftWares(HKEY_USERS, m_softList, strPath);
		}
		*/
	}
	else
	{
		//EnumSoftWares(HKEY_CURRENT_USER, m_softList);
	}

	typedef struct _ylb{
		bool operator()(REG_SOFTWARE_INFO& p1, REG_SOFTWARE_INFO& p2) {
			return _stricmp(CW2A(p1.szDisplayName), CW2A(p2.szDisplayName)) < 0;
			//return p1.szDisplayName.Compare(p2.szDisplayName) < 0; //unicode对中文比较有问题
		}
	}ylb;
	ylb hard_info_sort;
	m_softList.sort(hard_info_sort);

	/*int k=0;
	std::set<CString> mapProducts;
	while(1)
	{
	TCHAR	szBuffer[512];
	int iret = MsiEnumProducts(k++, szBuffer);
	if (iret	==	ERROR_SUCCESS)
	{
	mapProducts.insert(szBuffer);
	continue;

	}
	else
	{
	break;
	}
	}

	SOFTWARE_LIST::iterator _it = m_softList.begin();
	for(; _it != m_softList.end(); _it++)
	{
	if (mapProducts.find((*_it).szSubKeyName) != mapProducts.end())
	{
	if ((*_it).nSystemComponent!=1 && (*_it).szUninstallString.IsEmpty())
	{
	(*_it).bHide = FALSE;
	}
	}
	}*/

	m_enum = m_softList.begin();
	return EnumNext(info);
}

HRESULT CSoftWareEnum::EnumNext(REG_SOFTWARE_INFO& info)
{
	HRESULT hr = E_FAIL;
	while(m_enum != m_softList.end())
	{
		const REG_SOFTWARE_INFO& item = *m_enum;
		if((!item.bHide) && (!item.bUpdates || (item.bUpdates && m_bIncludeUpdates)))
		{
			info = item;
			m_enum++;
			return S_OK;
		}
		m_enum++;
	}

	return E_FAIL;
}

HRESULT CSoftWareEnum::EnumSoftWares(HKEY hRootKey, SOFTWARE_LIST& softList, LPCTSTR lpKeyPath, LPCTSTR lpSubPath, HKEY hReal, LPCTSTR lpRelKeyPath)
{
	CRegKey enumKey;
	RASSERT(ERROR_SUCCESS == enumKey.Open(hRootKey, lpKeyPath, KEY_READ ), E_FAIL);

	DWORD iIndex = 0;
	while(1)
	{
		TCHAR   keyName[MAX_PATH];
		DWORD	keyLen = MAX_PATH;
		if(ERROR_SUCCESS != enumKey.EnumKey(iIndex++, keyName, &keyLen))
			break;

		CString strKeyPath,strRelKeyPath;
		strKeyPath.Format(_T("%s\\%s"), lpKeyPath, keyName);
		if(lpSubPath && _tcslen(lpSubPath) > 0)
		{
			strKeyPath += _T("\\");
			strKeyPath += lpSubPath;
		}

		strRelKeyPath.Format(_T("%s\\%s"), lpRelKeyPath, keyName);
		if(lpSubPath && _tcslen(lpSubPath) > 0)
		{
			strRelKeyPath += _T("\\");
			strRelKeyPath += lpSubPath;
		}

		REG_SOFTWARE_INFO info;
		if(SUCCEEDED(GetSoftItem(hRootKey, strKeyPath, info, keyName, lpSubPath)) && !info.bHide)
		{
			TCHAR szFullPath[ 1024 ] = { 0 };

			info.szSubKeyName = keyName;
			info.szUninstallRegPath =  CRegistry::GetFullPath(hReal, strRelKeyPath, szFullPath, _countof(szFullPath) );
			//ZM1_GrpInfo(GroupName, _T("info.szUninstallRegPath:%s"), info.szUninstallRegPath);

		

			softList.push_back(info);
		}
	}

	return S_OK;
}

LONG QueryStringValue(HKEY hKey, LPCSTR pszValueName, LPSTR pszValue, ULONG* pnChars) throw()
{
	LONG lRes;
	DWORD dwType;
	ULONG nBytes;

	ATLASSUME(hKey != NULL);
	ATLASSERT(pnChars != NULL);

	nBytes = (*pnChars)*sizeof(CHAR);
	*pnChars = 0;
	lRes = ::RegQueryValueExA(hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue),
		&nBytes);

	if (lRes != ERROR_SUCCESS)
	{
		return lRes;
	}

	if(dwType != REG_SZ && dwType != REG_EXPAND_SZ)
	{
		return ERROR_INVALID_DATA;
	}

	if (pszValue != NULL)
	{
		if(nBytes!=0)
		{
#pragma warning(suppress:6385) // suppress noisy code analysis warning due to annotation on RegQueryValueEx
			if ((nBytes % sizeof(CHAR) != 0) || (pszValue[nBytes / sizeof(CHAR) -1] != 0))
			{
				return ERROR_INVALID_DATA;
			}
		}
		else
		{
			pszValue[0]=_T('\0');
		}
	}

	*pnChars = nBytes/sizeof(CHAR);

	return ERROR_SUCCESS;
}
#pragma warning(pop)

LONG GetRegKeyValue(CString& strValue, CRegKey& regKey, LPCTSTR lpName, ULONG nChars = MAX_PATH)
{
	CStringA strTemp;
	LONG lRc = QueryStringValue(regKey, CW2A(lpName), strTemp.GetBuffer(nChars), &nChars);
	strTemp.ReleaseBuffer();

	if ( !(ERROR_SUCCESS == lRc) )
	{
		return lRc;
	}
	strValue = CA2W(strTemp);
	return lRc;
}

#define LO4BIT(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xf))
#define HI4BIT(w)           ((BYTE)((((DWORD_PTR)(w)) >> 4) & 0xf))

CString GetInstallerKeyNameFromGuid(LPCTSTR lpGuid)
{
	GUID guid = S2GUID(lpGuid);

	CString str;
	str.Format(_T("%X%X%X%X%X%X%X%X")
		_T("%X%X%X%X")
		_T("%X%X%X%X")
		_T("%X%X%X%X")
		_T("%X%X%X%X")
		_T("%X%X%X%X")
		_T("%X%X%X%X"),
		LO4BIT(LOBYTE(LOWORD(guid.Data1))), HI4BIT(LOBYTE(LOWORD(guid.Data1))),
		LO4BIT(HIBYTE(LOWORD(guid.Data1))), HI4BIT(HIBYTE(LOWORD(guid.Data1))),
		LO4BIT(LOBYTE(HIWORD(guid.Data1))), HI4BIT(LOBYTE(HIWORD(guid.Data1))),
		LO4BIT(HIBYTE(HIWORD(guid.Data1))), HI4BIT(HIBYTE(HIWORD(guid.Data1))),

		LO4BIT(LOBYTE(guid.Data2)), HI4BIT(LOBYTE(guid.Data2)),
		LO4BIT(HIBYTE(guid.Data2)), HI4BIT(HIBYTE(guid.Data2)),

		LO4BIT(LOBYTE(guid.Data3)), HI4BIT(LOBYTE(guid.Data3)),
		LO4BIT(HIBYTE(guid.Data3)), HI4BIT(HIBYTE(guid.Data3)),

		LO4BIT(guid.Data4[0]), HI4BIT(guid.Data4[0]),
		LO4BIT(guid.Data4[1]), HI4BIT(guid.Data4[1]),
		LO4BIT(guid.Data4[2]), HI4BIT(guid.Data4[2]),
		LO4BIT(guid.Data4[3]), HI4BIT(guid.Data4[3]),
		LO4BIT(guid.Data4[4]), HI4BIT(guid.Data4[4]),
		LO4BIT(guid.Data4[5]), HI4BIT(guid.Data4[5]),
		LO4BIT(guid.Data4[6]), HI4BIT(guid.Data4[6]),
		LO4BIT(guid.Data4[7]), HI4BIT(guid.Data4[7])
		);
	return str;
}

BOOL IsBkStr(LPCTSTR lpStr)
{
	RASSERT(lpStr, FALSE);

	int nLen = _tcslen(lpStr);
	RASSERT(nLen > 0, FALSE);

	RASSERT(0 == _tcsnicmp(lpStr, _T("KB"), 2), FALSE);

	LPCTSTR subStr = lpStr+2;
	int i = 0;
	for(; i < 6 && subStr[i]; i++)
	{
		TCHAR ch = subStr[i];
		if(ch < _T('0') || ch > _T('9'))
			return FALSE;
	}

	return TRUE;
}

/*
1.uninstall WindowsInstaller：1 没有DisplayName 那就去看HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Installer\UserData\S-1-5-18\Products下的数据
2、枚举HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Installer\UserData\S-1-5-18\Products下的项，且取出来*\InstallProperties UninstallString的值，得到guid，如果不在uninstall下，则添加
*/

HRESULT CSoftWareEnum::GetSoftItem(HKEY hRootKey, LPCTSTR lpKeyPath, REG_SOFTWARE_INFO &info, LPCTSTR lpSubKeyName, LPCTSTR lpSubPath)
{
	BOOL bMon = FALSE;
	if(0 == _tcsicmp(lpSubKeyName, _T("KB2378111_WM9")))
	{
		bMon = TRUE;
	}

	// 读取子键下的各个Value的值
	info.bHide = FALSE;
	RFAILED(GetSoftInfo(hRootKey, lpKeyPath, info));

	if(lpSubPath)
	{
		RASSERT(!info.nSystemComponent && !info.szDisplayName.IsEmpty() && !info.szUninstallString.IsEmpty() && info.nWindowsInstall, E_FAIL);
		RASSERT(info.szUninstallString.GetLength() >= 52 && info.szUninstallString.Find(_T("MsiExec.exe")) >= 0, E_FAIL);

		int nLeft = info.szUninstallString.Find(_T('{'));
		int nRight = info.szUninstallString.Find(_T('}'));
		RASSERT(nLeft > 0 && nRight > 0 && (nRight-nLeft) == 37, E_FAIL);

		CString strProductGuid = info.szUninstallString.Mid(nLeft, 38);
		CString strUninstallPath;
		strUninstallPath.Format(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), strProductGuid);

		CRegKey regKey;
		if(ERROR_SUCCESS == regKey.Open( CPeApi::GetRootKey( HKEY_LOCAL_MACHINE), CPeApi::GetRealRegPath(CPeApi::GetFullRegPath(CPeApi::GetRootKey(HKEY_LOCAL_MACHINE),strUninstallPath )) ) ||
			ERROR_SUCCESS == regKey.Open( CPeApi::GetRootKey(HKEY_CURRENT_USER), CPeApi::GetRealRegPath( CPeApi::GetFullRegPath(CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), strUninstallPath) ) ))	//在正常的卸载列表里，那就不要了
			return E_FAIL;

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	info.bHide = TRUE;	//新的扫描方式，默认是不算的

	if(!info.nSystemComponent)
	{
		if(!info.nWindowsInstall)
		{
			if(
				IsBkStr(lpSubKeyName)||
				!info.szParentKeyName.IsEmpty() ||
				info.szReleaseType==_T("Hotfix")||
				info.szReleaseType==_T("Security Update")||
				info.szReleaseType==_T("Update Rollup")
				)
			{
				//系统更新
				info.bHide		= FALSE;
				info.bUpdates	= TRUE;
			}
			else
			{
				if(!info.szUninstallString.IsEmpty() && !info.szDisplayName.IsEmpty())
				{
					info.bHide = FALSE;
				}
			}
		}
		else	//If WindowsInstaller
		{
			CString strMsiKeyName = GetInstallerKeyNameFromGuid(lpSubKeyName);
			CString strMsiKeyPath;
			strMsiKeyPath.Format(_T("Software\\Classes\\Installer\\Products\\%s"), strMsiKeyName);
			CRegKey classesKey;
			if(ERROR_SUCCESS == classesKey.Open( 
				CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), CPeApi::GetRealRegPath( CPeApi::GetFullRegPath( CPeApi::GetRootKey(HKEY_LOCAL_MACHINE), strMsiKeyPath))
					, KEY_READ)
				
				)
			{
				CString strInstallRegPath;
				strInstallRegPath.Format(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\S-1-5-18\\Products\\%s\\InstallProperties"), strMsiKeyName);
				GetSoftInfo(
					CPeApi::GetRootKey( HKEY_LOCAL_MACHINE) , CPeApi::GetRealRegPath( CPeApi::GetFullRegPath( CPeApi::GetRootKey( HKEY_LOCAL_MACHINE) , strInstallRegPath)), info
					);
				
				info.bHide = FALSE;
			}
			else if(CheckUserDataSoft(strMsiKeyName, info))
			{
				info.bHide = FALSE;
			}
		}
	}
	else
	{
		info.nSystemComponent = TRUE;
	}
	return S_OK;

	//////////////////////////////////////////////////////////////////////////
	/*if((info.nSystemComponent==1) ||
	info.szReleaseType==_T("Hotfix")||
	info.szReleaseType==_T("Security Update")||
	info.szReleaseType==_T("Update")||
	info.szReleaseType==_T("Software Update")||
	info.szReleaseType==_T("Service Pack") ||
	info.szReleaseType==_T("Update Rollup") ||
	info.szUninstallString.IsEmpty() ||
	!info.szParentKeyName.IsEmpty()
	)
	{
	info.bHide = TRUE;
	}

	if(!info.szParentKeyName.CompareNoCase(_T("OperatingSystem")) ||
	!info.szDisplayName.CompareNoCase(_T("Microsoft")) ||
	!info.szDisplayName.CompareNoCase(_T("Microsoft Corporation")) ||
	!info.szDisplayName.CompareNoCase(_T("Microsoft")) ||
	!info.szDisplayName.CompareNoCase(_T("Microsoft")) ||
	!info.szDisplayName.CompareNoCase(_T("Microsoft")) ||
	!_tcsnicmp(info.szDisplayName, _T("Windows XP 安全更新"), _tcsclen(_T("Windows XP 安全更新"))) ||
	!_tcsnicmp(info.szDisplayName, _T("Windows XP 更新"), _tcsclen(_T("Windows XP 更新"))) ||
	!_tcsnicmp(info.szDisplayName, _T("Windows XP 修补程序"), _tcsclen(_T("Windows XP 修补程序"))) ||
	!info.szReleaseType.CompareNoCase(_T("Security Update")) ||
	!info.szReleaseType.CompareNoCase(_T("Update")) ||
	info.nNoRemove == 1 ||
	( 
	!_tcsnicmp(info.szUninstallString, _T("MsiExec.exe"), _tcsclen(_T("MsiExec.exe"))) && 
	!info.szPublisher.CompareNoCase(_T("Microsoft Corporation")) &&
	info.nSystemComponent == 1
	)
	)
	{
	info.bHide = 4;
	}*/

	return S_OK;
}

BOOL CSoftWareEnum::CheckUserDataSoft(LPCTSTR lpMsiKeyName, REG_SOFTWARE_INFO& info)
{
	RASSERT(lpMsiKeyName && _tcslen(lpMsiKeyName) > 0, FALSE);

	LPCTSTR lpUserData = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");
	CRegKey enumKey;
	RASSERT(ERROR_SUCCESS == enumKey.Open( 
		CPeApi::GetRootKey(HKEY_LOCAL_MACHINE),
		CPeApi::GetRealRegPath(
			CPeApi::GetFullRegPath( CPeApi::GetRootKey(HKEY_LOCAL_MACHINE) , lpUserData)
			)
			, KEY_READ
		), 
		FALSE
	);

	DWORD iIndex = 0;
	while(1)
	{
		TCHAR   keyName[MAX_PATH];
		DWORD	keyLen = MAX_PATH;
		if(ERROR_SUCCESS != enumKey.EnumKey(iIndex++, keyName, &keyLen))
			break;

		if(0 == _tcsicmp(keyName, _T("S-1-5-18")))
			continue;

		CString strKeyPath;
		strKeyPath.Format(_T("%s\\%s\\Products\\%s"), lpUserData, keyName, lpMsiKeyName);

		if(SUCCEEDED(GetSoftInfo( 
			CPeApi::GetRootKey( HKEY_LOCAL_MACHINE) , 
			CPeApi::GetRealRegPath( 
				CPeApi::GetFullRegPath( 
						CPeApi::GetRootKey( HKEY_LOCAL_MACHINE), strKeyPath)
					), info)) && !info.nSystemComponent)
			return TRUE;
	}

	return FALSE;
}


BOOL EnumInstallPathCallback( HKEY hKey, LPCTSTR lpszName, DWORD dwType, LPVOID lpParam)
{
	if ( !( lpszName && _tcslen(lpszName) ) )
		return TRUE;

	if ( _tcsicmp(_T("UninstallString") , lpszName) == 0 
		  )
	{
		long lRet = ERROR_SUCCESS;
		std::wstring sRet = CRegistry::GetString(hKey, _T(""), lpszName, FALSE,&lRet);

		if (!( sRet.length() > 3 && sRet.at(1) == ':' ) )
			return TRUE;



		CParseCommand ParseCmd;
		if ( !ParseCmd.ParseCmd(sRet.c_str()) )
			return TRUE;

		LPCTSTR lpszFilePath = ParseCmd.GetFilePath();

		CString& sPath = *(CString*)(lpParam);
		sPath = lpszFilePath;

		
		sPath = sPath.Left(sPath.ReverseFind('\\'));
			
		return FALSE;
	}

	if (  _tcsicmp(_T("DisplayIcon") , lpszName) == 0)
	{
		long lRet = ERROR_SUCCESS;
		std::wstring sRet = CRegistry::GetString(hKey, _T(""), lpszName,FALSE, &lRet);

		if (!( sRet.length() > 3 && sRet.at(1) == ':' ) )
			return TRUE;


		CString& sPath = *(CString*)(lpParam);
		sPath = sRet.c_str();
		sPath = sPath.Left(sPath.ReverseFind('\\'));
		return FALSE;
	}

	if ( _tcsicmp(_T("InstallSource") , lpszName) == 0 )
	{
		long lRet = ERROR_SUCCESS;
		std::wstring sRet = CRegistry::GetString(hKey, _T(""), lpszName,FALSE, &lRet);

		if (!( sRet.length() > 3 && sRet.at(1) == ':' ) )
			return TRUE;

		CString& sPath = *(CString*)(lpParam);
		sPath = sRet.c_str();

		return FALSE;
	}

	return TRUE;
}

HRESULT CSoftWareEnum::GetSoftInfo(HKEY hRootKey, LPCTSTR lpKeyPath, REG_SOFTWARE_INFO &info)
{
	RASSERT(lpKeyPath, E_FAIL);

	CRegKey regKey;
	RASSERT(ERROR_SUCCESS == regKey.Open(hRootKey, lpKeyPath, KEY_READ), E_FAIL);

	if(info.szDisplayName.IsEmpty())
		GetRegKeyValue(info.szDisplayName, regKey, REG_DISPLAY_NAME);
	info.szDisplayName.Trim();

	if(info.szUninstallString.IsEmpty())
		GetRegKeyValue(info.szUninstallString, regKey, REG_UNINSTALL_STRING, MAX_PATH*2);

	if(info.szParentKeyName.IsEmpty())
		GetRegKeyValue(info.szParentKeyName, regKey, REG_PARENT_KEY_NAME);

	if(info.szParentDisplayName.IsEmpty())
		GetRegKeyValue(info.szParentDisplayName, regKey, _T("ParentDisplayName"));

	if(info.szReleaseType.IsEmpty())
		GetRegKeyValue(info.szReleaseType, regKey, REG_RELEASE_TYPE);

	if(info.szPublisher.IsEmpty())
		GetRegKeyValue(info.szPublisher, regKey, REG_PUBLISHER);

	if(info.szInstallLocation.IsEmpty())
	{
		GetRegKeyValue(info.szInstallLocation, regKey, REG_InstallLocation);
		info.szInstallLocation.Replace(_T("\""), _T(""));
		if ( info.szInstallLocation.IsEmpty() )
		{
			//不能使用这种方法
			GetRegKeyValue(info.szInstallLocation, regKey, REG_UNINSTALL_STRING);
			info.szInstallLocation.Replace(_T("\""), _T(""));

			CParseCommand ParseCmd;
			if (ParseCmd.ParseCmd(info.szInstallLocation) )
			{
				LPCTSTR lpszFilePath = ParseCmd.GetFilePath();

				info.szInstallLocation = info.szInstallLocation.Left(info.szInstallLocation.ReverseFind('\\') + 1);
			}


			if (  info.szInstallLocation.IsEmpty() )
			{
				GetRegKeyValue(info.szInstallLocation, regKey, _T("DisplayIcon"));
				info.szInstallLocation.Replace(_T("\""), _T(""));
				info.szInstallLocation = info.szInstallLocation.Left(info.szInstallLocation.ReverseFind('\\') + 1);
			}
			if ( info.szInstallLocation.IsEmpty() )
			{
				msapi::CEnumReg(hRootKey).EnumValue(lpKeyPath, EnumInstallPathCallback, &info.szInstallLocation);
			}
		}
	}


	info.szInstallLocation.Replace(_T("\\\\"), _T("\\"));
	info.szInstallLocation.Replace(_T("/"), _T("\\"));	//有些目录中是以/为分隔符

	if(info.szVer.IsEmpty())
		GetRegKeyValue(info.szVer, regKey, _T("Inno Setup: Setup Version"));

	if(info.szVer.IsEmpty())
		GetRegKeyValue(info.szVer, regKey, _T("DisplayVersion"));

	if(info.szVer.IsEmpty())
		GetRegKeyValue(info.szVer, regKey, _T("Version"));

	regKey.QueryDWORDValue(REG_NOREMOVE, info.nNoRemove);
	regKey.QueryDWORDValue(REG_NOMODIFY, info.nNoModify);
	regKey.QueryDWORDValue(REG_NOREPAIR, info.nNoRepair);
	regKey.QueryDWORDValue(REG_WINDOWS_INSTALL, info.nWindowsInstall);
	regKey.QueryDWORDValue(REG_SYSTEM_COMPONENT, info.nSystemComponent);
	regKey.QueryDWORDValue(REG_EstimatedSize, info.dwEstimateSize);

	return S_OK;
}


