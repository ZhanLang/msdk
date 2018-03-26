// QuickLinkMgr.cpp: implementation of the CWinQuickLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinQuickLink.h"
#include <strsafe.h>
#include <intshcut.h>
#include <SHLOBJ.H>
#include "shellapi.h"
#include "luahost/luahost.h"
#if defined(_MEM_CHECK_)

		#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
		#endif

#endif

#ifdef _DEBUG
#include <assert.h>
#endif

namespace winfunc
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinQuickLink::CWinQuickLink()
{
}

CWinQuickLink::~CWinQuickLink()
{
}



static wchar_t *__conv_to_unicode(const char *_from)
{
	// adjust values below to reach threshold you need
	const int buffdepth = 16384;
	const int buffsize  = 8;

	static wchar_t buffer[buffsize][buffdepth];
	static int currentindex = 0;

#ifdef _DEBUG
	if(strlen(_from) >= buffdepth)
		assert(0);
#endif

	int out = currentindex;
	++currentindex;
	if(currentindex == buffsize)
		currentindex = 0;

	MultiByteToWideChar(
		CP_ACP,
		0,
		_from,
		-1,
		&buffer[out][0],
		buffdepth);

	return buffer[out];
}


bool CWinQuickLink::CreateLink(LPCTSTR lpszLinkFilePath, LPCTSTR lpszTarget, LPCTSTR lpszArguments/* = NULL*/, LPCTSTR lpszRemark/* = NULL*/,
							   LPCTSTR lpszIcon)
{
    HRESULT hres;
    IShellLink* pShellLink;
    IPersistFile* ppf;

	// Get a pointer to the IShellLink interface.

	::CoInitialize(NULL);

    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hres))
	{
		//	set start position WorkingDirectory
		TCHAR*	pPos = NULL;

		TCHAR	szWorkingDir[MAX_PATH + 1]={0};
		lstrcpyn(szWorkingDir, lpszTarget, sizeof(szWorkingDir));

		if(pPos = _tcsrchr(szWorkingDir, _T('\\')))
			*pPos = _T('\0');

		pShellLink->SetWorkingDirectory(szWorkingDir);
		if (lpszIcon && lstrlen(lpszIcon))
		{
			pShellLink->SetIconLocation(lpszIcon, 0);
		}
		// create link file's Directory
		TCHAR	szDir[MAX_PATH + 1]={0};
		TCHAR*	lpChr;
		TCHAR*	lpHead;

		lstrcpyn(szDir, lpszLinkFilePath, sizeof(szDir));
		if(pPos = _tcsrchr(szDir, _T('\\')))
			*pPos = _T('\0');

		lpHead = szDir;
		while(1){
			lpChr = _tcschr(lpHead, _T('\\'));
			if(NULL == lpChr){
				CreateDirectory(szDir, NULL);
				break;
			}
			else{
				*lpChr = NULL;
				CreateDirectory(szDir, NULL);
				*lpChr = _T('\\');
				lpHead = lpChr+1;
			}
		}
		
		if(lpszRemark)
		{
			LPCTSTR pPos = _tcsrchr(lpszRemark, _T('\\'));
			if(pPos)	// 修改描述中夹杂目录的错误
			{
				pShellLink->SetDescription(pPos + 1);
			}
			else
				pShellLink->SetDescription(lpszRemark);
		}
		if(lpszArguments)
		{
			pShellLink->SetArguments(lpszArguments);
		}

		pShellLink->SetPath(lpszTarget);

        hres = pShellLink->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);

        if(SUCCEEDED(hres))
		{
            				
#ifdef _UNICODE
			hres = ppf->Save(lpszLinkFilePath, true);
#else
			const wchar_t *wstr = __conv_to_unicode(lpszLinkFilePath);
			if(wstr) hres = ppf->Save(wstr, true);
			else	 hres = E_FAIL;
#endif
            ppf->Release();
        }
        pShellLink->Release(); 
    }

	::CoUninitialize();

	// return depend on COM call result
	if(SUCCEEDED(hres))
		return true;
	return false;
}


bool CWinQuickLink::CreateInternetLink(LPCTSTR lpszLinkFilePath, LPCTSTR lpszUrl) {
	HRESULT hres;
	IUniformResourceLocator	*pShellLink;
	IPersistFile			*ppf;

	// Get a pointer to the IShellLink interface.

	::CoInitialize(NULL);

	hres = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, 
							IID_IUniformResourceLocator, 
							(LPVOID*)&pShellLink);
	if (SUCCEEDED(hres)) {
		// create link file's Directory
		TCHAR	szDir[MAX_PATH + 1]={0};
		TCHAR*	pPos   = NULL;
		TCHAR*	lpChr  = NULL;
		TCHAR*	lpHead = NULL;

		lstrcpyn(szDir, lpszLinkFilePath, sizeof(szDir));
		if(pPos = _tcsrchr(szDir, _T('\\')))
			*pPos = _T('\0');

		lpHead = szDir;
		while(1){
			lpChr = _tcschr(lpHead, _T('\\'));
			if(NULL == lpChr){
				CreateDirectory(szDir, NULL);
				break;
			}
			else{
				*lpChr = NULL;
				CreateDirectory(szDir, NULL);
				*lpChr = _T('\\');
				lpHead = lpChr+1;
			}
		}
		pShellLink->SetURL(lpszUrl, 0);

		hres = pShellLink->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);
		if(SUCCEEDED(hres))	{
#ifdef _UNICODE
			hres = ppf->Save(lpszLinkFilePath, true);
#else
			const wchar_t *wstr = __conv_to_unicode(lpszLinkFilePath);
			if(wstr)	hres = ppf->Save(wstr, true);
			else		hres = E_FAIL;
#endif
			ppf->Release();
		}
		pShellLink->Release(); 
	}

	::CoUninitialize();

	// return depend on COM call result
	if(SUCCEEDED(hres))
		return true;
	return false;
}

bool CWinQuickLink::GetStartMenuPos(LPTSTR lpszPath, DWORD dwMaxLen)
{
	int		Ret;
	HKEY	hKey;
	DWORD	dwType = REG_SZ;

	//if the System is WinNT...
	OSVERSIONINFO OsVer;

	OsVer.dwOSVersionInfoSize= sizeof( OSVERSIONINFO );
	GetVersionEx(&OsVer);

	if(VER_PLATFORM_WIN32_NT == OsVer.dwPlatformId)
	{
		return GetAllUserProgramsPath(lpszPath, dwMaxLen);
	}

	// Prepare to navigate to the CurrentVersion key.
	Ret= RegOpenKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
			0,KEY_READ|KEY_WRITE,&hKey);

	if(Ret==ERROR_SUCCESS)
	{
		Ret=RegQueryValueEx(hKey,_T("Programs"),
			NULL,&dwType,(LPBYTE)lpszPath,&dwMaxLen);

		RegCloseKey(hKey);
	}

	Ret=lstrlen(lpszPath);
	if(Ret==0){
		Ret=ERROR_SUCCESS+1;
	}
	else{
		Ret=ERROR_SUCCESS+2;
	}

	if(Ret == ERROR_SUCCESS+1)
	{
		GetWindowsDirectory(lpszPath,dwMaxLen);
		StringCchCat(lpszPath, dwMaxLen, _T("\\Start Menu\\Programs"));
	}
	return true;
}

bool CWinQuickLink::GetDesktopPos(LPTSTR lpszPath, DWORD dwMaxLen)
{
	TCHAR szAllUserDesk[MAX_PATH + 1] = {0};
	TCHAR szDesk[MAX_PATH + 1] = {0};
	
	SHGetSpecialFolderPath(NULL, szAllUserDesk, CSIDL_COMMON_DESKTOPDIRECTORY, false);
	SHGetSpecialFolderPath(NULL, szDesk, CSIDL_DESKTOPDIRECTORY, false);
	
	OSVERSIONINFO OsVer;
	OsVer.dwOSVersionInfoSize= sizeof( OSVERSIONINFO );
	GetVersionEx(&OsVer);
	if(VER_PLATFORM_WIN32_NT  == OsVer.dwPlatformId)
	{
		if (szAllUserDesk[0])
		{
			lstrcpyn(lpszPath, szAllUserDesk, dwMaxLen);
			return true;
		}
		return GetAllUserDeskTopPath(lpszPath, dwMaxLen);
	}

	if (szDesk[0])
	{
		lstrcpyn(lpszPath, szDesk, dwMaxLen);
		return true;
	}

	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	int Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
		0, KEY_READ, &hKey);

	if (Ret==ERROR_SUCCESS)
	{
		Ret=RegQueryValueEx(hKey,_T("Desktop"),	NULL, &dwType, (LPBYTE)lpszPath, &dwMaxLen);
		RegCloseKey(hKey);
	}

	if(Ret!=ERROR_SUCCESS || !lpszPath[0])
	{
		GetWindowsDirectory(lpszPath, dwMaxLen);	//9x
		StringCchCat(lpszPath, dwMaxLen, _T("\\Desktop"));
		return true;
	}
	return false;
}

bool CWinQuickLink::GetQuickLaunchPos(LPTSTR lpszPath, DWORD dwMaxLen)
{
	int		Ret;
	HKEY	hKey;
	DWORD	dwType = REG_SZ;

	DWORD	SizeOfBuffer = MAX_PATH;
	TCHAR	szDir[MAX_PATH + 1]={0};

	// Prepare to navigate to the CurrentVersion key.

	Ret= RegOpenKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
			0,KEY_READ,&hKey);

	if(Ret==ERROR_SUCCESS)
	{
		Ret=RegQueryValueEx(hKey,_T("AppData"),
			NULL,&dwType,(LPBYTE)szDir,&SizeOfBuffer);

		RegCloseKey(hKey);
	}

	Ret=lstrlen(szDir)>0?ERROR_SUCCESS:Ret;

	if(Ret!=ERROR_SUCCESS)	//winnt ?
	{
		::GetWindowsDirectory(szDir, dwMaxLen);
		StringCchCat(szDir, sizeof(szDir), _T("\\Application Data"));
	}

	StringCchCat(szDir, sizeof(szDir), _T("\\Microsoft\\Internet Explorer\\Quick Launch"));
	lstrcpyn(lpszPath, szDir, dwMaxLen);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	函数名:				GetAllUserDeskTopPath
//	用  途:				获取用户桌面路径
//	对全局变量的影响:	无
//	参数说明:			lpDeskTopPath:	桌面路径保存指针
//						Size		:	路径保存大小
//	返回结果:			无
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CWinQuickLink::GetAllUserDeskTopPath(LPTSTR lpDeskTopPath, int Size)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\Windows\\CurrentVersion")
		_T("\\Explorer\\Shell Folders"),
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD SizeOfBuffer = Size;
		long lRet=RegQueryValueEx(hKey, _T("Common Programs"),
			NULL, &dwType, (LPBYTE)lpDeskTopPath, &SizeOfBuffer);
		RegCloseKey(hKey);
		if ((lRet == ERROR_SUCCESS) && lpDeskTopPath[0])
			return true;
	}
	if (!GetWindowsDirectory(lpDeskTopPath, Size))
		return false;
	StringCchCat(lpDeskTopPath, Size, _T("\\Profiles\\All Users\\Start Menu\\Programs"));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	函数名:				GetAllUserProgramsPath
//	用  途:				获取用户程序组
//	对全局变量的影响:	无
//	参数说明:			lpProgramsPath:	用户程序组路径保存指针
//						SizeOfPath	  :	路径保存大小
//	返回结果:			无
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CWinQuickLink::GetAllUserProgramsPath(LPTSTR lpProgramsPath,int SizeOfPath)
{
	int		Ret;
	HKEY	hKey;
	DWORD	dwType = REG_SZ;
	DWORD	SizeOfBuffer;

	SizeOfBuffer = SizeOfPath;

	// Prepare to navigate to the CurrentVersion key.
	Ret= RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
			0,KEY_READ,&hKey);

	if(Ret==ERROR_SUCCESS)
	{
		Ret=RegQueryValueEx(hKey,_T("Common Programs"),
			NULL,&dwType,(LPBYTE)lpProgramsPath,&SizeOfBuffer);

		RegCloseKey(hKey);
	}

	Ret=lstrlen(lpProgramsPath);
	if(Ret)	Ret=ERROR_SUCCESS;

	if(Ret!=ERROR_SUCCESS)
	{
		GetWindowsDirectory(lpProgramsPath,SizeOfPath);
		StringCchCat(lpProgramsPath, SizeOfPath, _T("\\Profiles\\All Users\\Start Menu\\Programs"));
	}
	return true;
}

TString CWinQuickLink::GetQuickLaunchPosLua()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	GetQuickLaunchPos(&strRet[0], strRet.size());
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}

TString CWinQuickLink::GetDesktopPosLua()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	GetDesktopPos(&strRet[0], strRet.size());
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetStartMenuPosLua()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	GetStartMenuPos(&strRet[0], strRet.size());
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetAllUserDeskTopPathLua()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	GetAllUserDeskTopPath(&strRet[0], strRet.size());
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetAllUserProgramsPathLua()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	GetAllUserProgramsPath(&strRet[0], strRet.size());
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}

TString CWinQuickLink::GetSpecialFolder(LONG lMacro)
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	SHGetSpecialFolderPath(NULL, &strRet[0], lMacro, false);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}

TString	CWinQuickLink::GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, UINT uUnique)
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	::GetTempFileName(lpPathName, lpPrefixString, uUnique, &strRet[0]);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetSystemTempDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	::GetTempPath(MAX_PATH, &strRet[0]);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetSystemDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	SHGetSpecialFolderPath(NULL, &strRet[0], CSIDL_SYSTEM, false);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetWindowsDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	SHGetSpecialFolderPath(NULL, &strRet[0], CSIDL_WINDOWS, false);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetProgramsDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	SHGetSpecialFolderPath(NULL, &strRet[0], CSIDL_PROGRAM_FILES, false);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetCommonDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	SHGetSpecialFolderPath(NULL, &strRet[0], CSIDL_PROGRAM_FILES_COMMON, false);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}
TString CWinQuickLink::GetCurrentDir()
{
	TString strRet;
	strRet.assign(MAX_PATH, 0);
	::GetCurrentDirectory(MAX_PATH, &strRet[0]);
	strRet.erase(lstrlen(strRet.c_str()));
	return strRet;
}




BOOL IsWindowVistaLatter()
{
	OSVERSIONINFO  osver;
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx(&osver);
	return ( osver.dwMajorVersion  >= 6 );
}

bool CWinQuickLink::CreateTaskBar(
	LPCTSTR lpszLink, 
	LPCTSTR lpszTarget, 
	LPCTSTR lpszArguments, 
	LPCTSTR lpszRemark,
	LPCTSTR lpszIcon/* = NULL*/)
{
	TCHAR strTaskBar[MAX_PATH] = {0};
	RASSERT(GetQuickLaunchPos(strTaskBar,MAX_PATH),FALSE);

	if (IsWindowVistaLatter())
	{
		lstrcat(strTaskBar, _T("User Pinned\\TaskBar"));
	}

	lstrcat(strTaskBar,lpszLink);
	if (PathFileExists(strTaskBar))
	{
		::DeleteFile(strTaskBar);
	}
	BOOL bRet = CreateLink(strTaskBar,lpszTarget,lpszArguments,lpszRemark,lpszIcon);
	RASSERT(bRet,false);
	if (IsWindowVistaLatter())
	{
		try{
			::ShellExecute(NULL, _T("taskbarpin"), strTaskBar, NULL, NULL, 0);
		}catch(...){}
	}

	return true;
}

bool CWinQuickLink::DeleteTaskBar( LPCTSTR lpszLink )
{
	TCHAR strTaskBar[MAX_PATH] = {0};
	RASSERT(GetQuickLaunchPos(strTaskBar,MAX_PATH),FALSE);

	if (IsWindowVistaLatter())
	{
		lstrcat(strTaskBar, _T("User Pinned\\TaskBar"));
	}

	lstrcat(strTaskBar,lpszLink);
	if (IsWindowVistaLatter())
	{
		try{
			ShellExecute(NULL, _T("taskbarunpin"), strTaskBar, NULL, NULL, 0);
		}catch(...){}
	}

	return msapi::DeleteFileEx(strTaskBar) ? true : false;
}

};//namespace winfunc
