#pragma once


#include <atlpath.h>
#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"
using namespace ATL::ATLPath;

namespace winfunc
{
	typedef std::basic_string<TCHAR> TString;
/////////////////////////////////////////////////////////////////////////////
// CWinRegKey

	class CWinPath : public CPath
	{
	public:
		//help for lua	
		CWinPath()
		{
		}
		CWinPath(LPCTSTR pszPath):CPath(pszPath)
		{
		}
		CWinPath(TString pszPath):CPath(pszPath.c_str())
		{
		}
		LPCTSTR GetData() const {return (LPCTSTR)m_strPath;}		
	};

	class CWinDir
	{
	public:

		static BOOL BindToLua(ILuaVM* pLuaVM)
		{
			GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("windir")
				+mluabind::Function("PathProviderWin", &CWinDir::PathProviderWin)
				+mluabind::Enum("DIR_WINDOWS",			CWinDir::DIR_WINDOWS)
				+mluabind::Enum("DIR_SYSTEM",				CWinDir::DIR_SYSTEM)
				+mluabind::Enum("DIR_PROGRAM_FILES",		CWinDir::DIR_PROGRAM_FILES)
				+mluabind::Enum("DIR_PROGRAM_FILESX86",	CWinDir::DIR_PROGRAM_FILESX86)
				+mluabind::Enum("DIR_IE_INTERNET_CACHE",	CWinDir::DIR_IE_INTERNET_CACHE)
				+mluabind::Enum("DIR_COMMON_START_MENU",	CWinDir::DIR_COMMON_START_MENU)
				+mluabind::Enum("DIR_START_MENU",			CWinDir::DIR_START_MENU)
				+mluabind::Enum("DIR_APP_DATA",			CWinDir::DIR_APP_DATA)
				+mluabind::Enum("DIR_PROFILE",			CWinDir::DIR_PROFILE)
				+mluabind::Enum("DIR_LOCAL_APP_DATA_LOW", CWinDir::DIR_LOCAL_APP_DATA_LOW)
				+mluabind::Enum("DIR_LOCAL_APP_DATA",		CWinDir::DIR_LOCAL_APP_DATA)
				+mluabind::Enum("DIR_COMMON_APP_DATA",	CWinDir::DIR_COMMON_APP_DATA)
				+mluabind::Enum("DIR_TEMP",	CWinDir::DIR_TEMP)
				);

			return TRUE;
		}

		CWinDir()
		{
		}
		enum DirKeyWin
		{
			PATH_WIN_START = 0,

			DIR_WINDOWS,				// Windows directory, usually "C:\Windows"
			DIR_SYSTEM,					// Usually "C:\Windows\system32"
			DIR_PROGRAM_FILES,			// Usually "C:\Program Files"
			DIR_PROGRAM_FILESX86,		// Usually "C:\Program Files" or "C:\Programs Files (x86)"

			DIR_IE_INTERNET_CACHE,		// Temporary Internet Files directory.
			DIR_COMMON_START_MENU,		// Usually "C:\Documents and Settings\All Users\Start Menu\Programs"
			DIR_START_MENU,				// Usually "C:\Documents and Settings\<user>\Start Menu\Programs"
			DIR_APP_DATA,				// Application Data directory under the user profile.
			DIR_PROFILE,				// Usually "C:\Documents and settings\<user>.
			DIR_LOCAL_APP_DATA_LOW,		// Local AppData directory for low integrity level.
			DIR_LOCAL_APP_DATA,			// "Local Settings\Application Data" directory under the user profile.
			DIR_COMMON_APP_DATA,		// W2K, XP, W2K3: "C:\Documents and settings\All Users\Application Data"
			DIR_TEMP,
			// Vista, W2K8 and above: "C:\ProgramData".
			PATH_WIN_END
		};
		static std::string PathProviderWin(int key);
	};

};//namespace winfunc