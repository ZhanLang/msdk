#include "StdAfx.h"
#include "WinPath.h"

#include <shlobj.h>
#include <string>
#include <shlwapi.h>
#include <atlstr.h>

#if defined(_MEM_CHECK_)

		#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
		#endif

#endif

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace winfunc
{
	std::string CWinDir::PathProviderWin( int key )
	{
		CHAR system_buffer[MAX_PATH];
		system_buffer[0] = 0;

		switch (key)
		{
		case DIR_WINDOWS:
			::GetWindowsDirectoryA(system_buffer, MAX_PATH);
			break;

		case DIR_SYSTEM:
			::GetSystemDirectoryA(system_buffer, MAX_PATH);
			break;

		case DIR_PROGRAM_FILESX86:
			{
				SYSTEM_INFO system_info = { 0 };
				::GetNativeSystemInfo(&system_info);
				if (system_info.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
					::SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILESX86, NULL, SHGFP_TYPE_CURRENT, system_buffer);
				break;
			}
			
		case DIR_PROGRAM_FILES:
			::SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_IE_INTERNET_CACHE:
			::SHGetFolderPathA(NULL, CSIDL_INTERNET_CACHE, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_COMMON_START_MENU:
			::SHGetFolderPathA(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_START_MENU:
			::SHGetFolderPathA(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_APP_DATA:
			::SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_COMMON_APP_DATA:
			::SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_PROFILE:
			::SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;
		case DIR_TEMP:
			{
				CHAR strPath[MAX_PATH] = { 0 };
				GetTempPathA(MAX_PATH, strPath);
				return strPath;
			}
			break;
		case DIR_LOCAL_APP_DATA_LOW:
			//::SHGetFolderPathA(NULL, CSIDL_LOCAL_DATA_LOW, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		case DIR_LOCAL_APP_DATA:
			::SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer);
			break;

		default:
			break;
		}
		std::string strPath = system_buffer;
		return strPath;
	}

};//namespace winfunc