#pragma once


#include <vector>

#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"
namespace winfunc
{
	
	typedef std::basic_string<TCHAR> TString;
	typedef std::vector<TString> TStringS;
//	manage Quick Link: Program Menu, Desktop Link, Quick Launch Bar

	class CWinQuickLink  
	{
	public:
		CWinQuickLink();
		~CWinQuickLink();

		static BOOL BindToLua(ILuaVM* pLuaVM)
		{

			GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("quicklink")

				+mluabind::Function("GetQuickLaunchPos",&CWinQuickLink::GetQuickLaunchPosLua)
				+mluabind::Function("GetDesktopPos", &CWinQuickLink::GetDesktopPosLua)
				+mluabind::Function("GetStartMenuPos", &CWinQuickLink::GetStartMenuPosLua)
				+mluabind::Function("GetAllUserDeskTopPath", &CWinQuickLink::GetAllUserDeskTopPathLua)
				+mluabind::Function("GetAllUserProgramsPath", &CWinQuickLink::GetAllUserProgramsPathLua)
				+mluabind::Function("GetSpecialFolder", &CWinQuickLink::GetSpecialFolder)
				+mluabind::Function("GetTempFileName", &CWinQuickLink::GetTempFileName)
				+mluabind::Function("GetSystemTempDir", &CWinQuickLink::GetSystemTempDir)
				+mluabind::Function("GetSystemDir", &CWinQuickLink::GetSystemDir)
				+mluabind::Function("GetWindowsDir", &CWinQuickLink::GetWindowsDir)
				+mluabind::Function("GetProgramsDir", &CWinQuickLink::GetProgramsDir)
				+mluabind::Function("GetCommonDir", &CWinQuickLink::GetCommonDir)
				+mluabind::Function("GetCurrentDir",		&CWinQuickLink::GetCurrentDir)
				+mluabind::Function("CreateLink",			&CWinQuickLink::CreateLink)
				+mluabind::Function("CreateInternetLink",	&CWinQuickLink::CreateInternetLink)
				+mluabind::Function("CreateTaskBar",	&CWinQuickLink::CreateTaskBar)
				+mluabind::Function("DeleteTaskBar",	&CWinQuickLink::DeleteTaskBar)
				);

			return TRUE;
		}

		static bool GetQuickLaunchPos(LPTSTR lpszPath, DWORD dwMaxLen);
		static bool GetDesktopPos(LPTSTR lpszPath, DWORD dwMaxLen);
		static bool GetStartMenuPos(LPTSTR lpszPath, DWORD dwMaxLen);

		static bool CreateLink(LPCTSTR lpszLinkFilePath, 
			LPCTSTR lpszTarget, 
			LPCTSTR lpszArguments, 
			LPCTSTR lpszRemark,
			LPCTSTR lpszIcon = NULL);


		static bool CreateTaskBar(LPCTSTR lpszLink, 
			LPCTSTR lpszTarget, 
			LPCTSTR lpszArguments, 
			LPCTSTR lpszRemark,
			LPCTSTR lpszIcon = NULL);


		static bool DeleteTaskBar(LPCTSTR lpszLink);

		static bool CreateInternetLink(LPCTSTR lpszLinkFilePath, LPCTSTR lpszUrl);
		
		static bool GetAllUserDeskTopPath(LPTSTR lpDeskTopPath, int Size);
		static bool GetAllUserProgramsPath(LPTSTR lpProgramsPath,int SizeOfPath);

		//for lua
		static TString GetQuickLaunchPosLua();
		static TString GetDesktopPosLua();
		static TString GetStartMenuPosLua();
		static TString GetAllUserDeskTopPathLua();
		static TString GetAllUserProgramsPathLua();
		static TString GetSpecialFolder(LONG lMacro);

		static TString	GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, UINT uUnique);
		static TString GetSystemTempDir();
		static TString GetSystemDir();
		static TString GetWindowsDir();
		static TString GetProgramsDir();
		static TString GetCommonDir();	
		static TString GetCurrentDir();
	};

};//namespace winfunc

