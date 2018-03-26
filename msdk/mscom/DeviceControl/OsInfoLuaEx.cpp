#include "StdAfx.h"
#include "OsInfoLuaEx.h"
#include "luahost\luahost.h"
#include <sstream> 
#include "msapi/mswinapi.h"
#include "DeviceControl/IDeviceControl.h"

COsInfoLuaEx::COsInfoLuaEx(void)
{
}


COsInfoLuaEx::~COsInfoLuaEx(void)
{
}

STDMETHODIMP COsInfoLuaEx::RegisterLuaEx(ILuaVM* pLuaVm)
{
	
	GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("osinfo")
		+mluabind::Function("GetOsVersion", &COsInfoLuaEx::GetOsVersion)
		+mluabind::Function("GetOsInfo", &COsInfoLuaEx::GetOsInfo)
		+mluabind::Function("GetDiskSize", &COsInfoLuaEx::GetDiskSize)
		+mluabind::Function("GetDiskFreeSize", &COsInfoLuaEx::GetDiskFreeSize)
		+mluabind::Function("GetUserName",&COsInfoLuaEx::GetUserName)
		+mluabind::Function("GetComputerName",&COsInfoLuaEx::GetComputerName)
		+mluabind::Function("IsWow64",  &COsInfoLuaEx::IsWow64)
		+mluabind::Function("IsVM",  &COsInfoLuaEx::IsVM)
		);
	return TRUE;

	return S_OK;
}



enum OSVer
{
	OS_Unknown = -1,
	OS_Lower = 4,
	OS_WinXP = 5,
	OS_Vista = 6,
	OS_Win7 = 7,
	OS_Win8 = 8,
	OS_Advanced = 100,
};

int COsInfoLuaEx::GetOsVersion()
{
	static OSVer g_OSver = OS_Unknown;
	if (g_OSver != OS_Unknown)
	{
		return g_OSver;
	}

	g_OSver = OS_Lower;
	OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&ovi);

	if(ovi.dwMajorVersion >= 6)
	{
		g_OSver = OS_Advanced;
		if (ovi.dwMinorVersion == 2)
		{
			g_OSver = OS_Win8;
		}
		else if (ovi.dwMinorVersion == 1)
		{
			g_OSver = OS_Win7;
		}
		else if (ovi.dwMinorVersion == 0)
		{
			g_OSver = OS_Vista;
		}
	}
	else if(ovi.dwMajorVersion == 5)
	{
		g_OSver = OS_WinXP;
	}

	return g_OSver;
}

#define PRODUCT_PROFESSIONAL	0x00000030
#define VER_SUITE_WH_SERVER		0x00008000

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);


std::string COsInfoLuaEx::GetOsInfo()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX)); 
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}


	PGNSI pGNSI = (PGNSI) GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll"))
		, "GetNativeSystemInfo"
		);
	if(NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si); 
	// Check for unsupported OS
	if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId || osvi.dwMajorVersion <= 4 ) 
	{
		return false;
	} 
	std::stringstream os;
	os << "Microsoft "; 
	// Test for the specific product. 
	//6： 系列
	if ( osvi.dwMajorVersion == 6 )
	{
		if( osvi.dwMinorVersion == 0 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				os << "Windows Vista ";
			else os << "Windows Server 2008 ";
		}
		else if ( osvi.dwMinorVersion == 1 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				os << "Windows 7 ";
			else os << "Windows Server 2008 R2 ";
		}  
		else if(osvi.dwMinorVersion == 2)
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				os << "Windows 8 ";
			else os << "Windows 8 Server ";

		}
		PGPI pGPI = (PGPI) GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll"))
			, "GetProductInfo"
			);
		pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType); 
		switch( dwType )
		{
		case PRODUCT_ULTIMATE:
			os << "Ultimate Edition";
			break;
		case PRODUCT_PROFESSIONAL:
			os << "Professional";
			break;
		case PRODUCT_HOME_PREMIUM:
			os << "Home Premium Edition";
			break;
		case PRODUCT_HOME_BASIC:
			os << "Home Basic Edition";
			break;
		case PRODUCT_ENTERPRISE:
			os << "Enterprise Edition";
			break;
		case PRODUCT_BUSINESS:
			os << "Business Edition";
			break;
		case PRODUCT_STARTER:
			os << "Starter Edition";
			break;
		case PRODUCT_CLUSTER_SERVER:
			os << "Cluster Server Edition";
			break;
		case PRODUCT_DATACENTER_SERVER:
			os << "Datacenter Edition";
			break;
		case PRODUCT_DATACENTER_SERVER_CORE:
			os << "Datacenter Edition (core installation)";
			break;
		case PRODUCT_ENTERPRISE_SERVER:
			os << "Enterprise Edition";
			break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:
			os << "Enterprise Edition (core installation)";
			break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:
			os << "Enterprise Edition for Itanium-based Systems";
			break;
		case PRODUCT_SMALLBUSINESS_SERVER:
			os << "Small Business Server";
			break;
		case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			os << "Small Business Server Premium Edition";
			break;
		case PRODUCT_STANDARD_SERVER:
			os << "Standard Edition";
			break;
		case PRODUCT_STANDARD_SERVER_CORE:
			os << "Standard Edition (core installation)";
			break;
		case PRODUCT_WEB_SERVER:
			os << "Web Server Edition";
			break;
		}
	} 

	//5-2: 系列
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
	{
		if( GetSystemMetrics(/*SM_SERVERR2*/89) )
			os <<  "Windows Server 2003 R2, ";
		else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
			os <<  "Windows Storage Server 2003";
		else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
			os <<  "Windows Home Server";
		else if( osvi.wProductType == VER_NT_WORKSTATION &&
			si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		{
			os <<  "Windows XP Professional x64 Edition";
		}
		else os << "Windows Server 2003, ";  

		// Test for the server type.
		if ( osvi.wProductType != VER_NT_WORKSTATION )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					os <<  "Datacenter Edition for Itanium-based Systems";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					os <<  "Enterprise Edition for Itanium-based Systems";
			}   
			else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					os <<  "Datacenter x64 Edition";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					os <<  "Enterprise x64 Edition";
				else os <<  "Standard x64 Edition";
			}   
			else
			{
				if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					os <<  "Compute Cluster Edition";
				else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					os <<  "Datacenter Edition";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					os <<  "Enterprise Edition";
				else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
					os <<  "Web Edition";
				else os <<  "Standard Edition";
			}
		}
	} 
	//5-1: 系列
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
	{
		os << "Windows XP ";
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			os <<  "Home Edition";
		else os <<  "Professional";
	}
	//5-0: 系列
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
	{
		os << "Windows 2000 "; 
		if ( osvi.wProductType == VER_NT_WORKSTATION )
		{
			os <<  "Professional";
		}
		else 
		{
			if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				os <<  "Datacenter Server";
			else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				os <<  "Advanced Server";
			else os <<  "Server";
		}
	} 
	//windows NT
	if ( osvi.dwMajorVersion <= 4  && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		os << "Windows NT ";
	}
	// Windows 98/me
	if(osvi.dwMajorVersion == 4 && (osvi.dwMinorVersion == 10 || osvi.dwMinorVersion == 90) &&  osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)	
	{		
		os << "Windows 98";
	}
	// Windows 95
	if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0 && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)			
	{
		os << "Windows 95";
	}


	// Include service pack (if any) and build number. 
	if(wcslen(osvi.szCSDVersion) > 0) 
	{
		os << " " << osvi.szCSDVersion;
	} 
	os << L" (build " << osvi.dwBuildNumber << L")"; 

	//64bit or 32bit
	if ( osvi.dwMajorVersion >= 6 ) 
	{
		if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			os <<  ", 64-bit";
		else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			os << ", 32-bit";
	} 

	std::string s = os.str();
	return s;
}

std::string COsInfoLuaEx::GetDiskSize()
{
	ULARGE_INTEGER m_TotalNumberOfBytes = {0};
	ULARGE_INTEGER m_TotalNumberOfFreeBytes = {0};
	GetDiskFreeSpaceEx(NULL, NULL, &m_TotalNumberOfBytes, &m_TotalNumberOfFreeBytes);

	char buffer[MAX_PATH] = {0};
	return _i64toa(m_TotalNumberOfBytes.QuadPart,buffer, 10);
}

std::string COsInfoLuaEx::GetDiskFreeSize()
{
	ULARGE_INTEGER m_TotalNumberOfBytes = {0};
	ULARGE_INTEGER m_TotalNumberOfFreeBytes = {0};
	GetDiskFreeSpaceEx(NULL, NULL, &m_TotalNumberOfBytes, &m_TotalNumberOfFreeBytes);

	char buffer[MAX_PATH] = {0};
	return _i64toa(m_TotalNumberOfFreeBytes.QuadPart,buffer, 10);
}

std::string COsInfoLuaEx::GetUserName()
{
	CHAR szUser[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	::GetUserNameA(szUser, &dwSize);
	return szUser;
}

std::string COsInfoLuaEx::GetComputerName()
{
	CHAR szUser[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	::GetComputerNameA(szUser, &dwSize);
	return szUser;
}


bool COsInfoLuaEx::IsWow64()
{
	return IsWindowsX64() ? true : false;
}

bool COsInfoLuaEx::IsVM()
{
	IDeviceManage* pDeviceMgr = ::CreateDeviceManage(NULL);
	RASSERT(pDeviceMgr, false);
	bool bRet =  pDeviceMgr->IsVM() ? true : false;
	::DestoryDeviceManage(pDeviceMgr);

	return bRet;
}
