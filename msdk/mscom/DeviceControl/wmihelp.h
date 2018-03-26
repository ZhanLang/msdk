#pragma once
#include <string>
#include <devguid.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <algorithm>
#pragma comment(lib,"setupapi.lib")


enum  OSTypeEnum
{
	WIN7 = 0,							// Win7
	WinXP,
	WinServer2008,
	WinVista,
	Win2008,
	Win2000,
	OnKnown
};

// 获取操作系统版本
static OSTypeEnum GetSystemVersion()
{
	OSTypeEnum  osType = OnKnown;

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return OnKnown;
	}

	if( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				osType = WIN7;
				return osType;
			}
			else
			{
				osType = WinServer2008;
				return osType;
			}
		}
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				osType = WinVista;
				return osType;
			}
			else
			{
				osType = Win2008;
				return osType;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			osType = WinXP;
			return osType;
		}
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			osType = Win2000;
			return osType;
		}
	}
	return osType;
}


// 获取指定设备的指定属性
static std::wstring GetDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA &hDeviceInfoData,DWORD dwProperty)
{
	std::wstring	strProperty;
	DWORD	dwData;
	DWORD	dwSize = 4096;
	LPTSTR	buffer = (WCHAR *)LocalAlloc(LPTR, dwSize);	

	while (!SetupDiGetDeviceRegistryProperty( hDevInfo,
		&hDeviceInfoData,
		dwProperty,
		&dwData,
		(PBYTE)buffer,
		dwSize,
		&dwSize) )
	{	
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (buffer != NULL)
				LocalFree(buffer);

			buffer = (WCHAR *)LocalAlloc(LPTR, dwSize*2);
		}
		else
		{		
			break;
		}
	}

	strProperty = buffer;

	if (buffer != NULL)
		LocalFree(buffer);

	// 转换成大写
	std::transform( strProperty.begin(), strProperty.end(),
		strProperty.begin(), toupper);

	return strProperty;
}

// 判断某一设备是否已被停用
static BOOL IsDeviceDisable(SP_DEVINFO_DATA hDeviceInfoData)
{
	DWORD dwStatus			= -1;
	DWORD dwProblenNumber	= -1;
	DWORD dwResult;
	BOOL  bReturn = FALSE;

	dwResult = CM_Get_DevNode_Status(&dwStatus, &dwProblenNumber, hDeviceInfoData.DevInst, 0);

	if (CR_SUCCESS == dwResult)
	{
		if (dwProblenNumber == CM_PROB_DISABLED || (dwStatus & DN_HAS_PROBLEM) != 0)
			bReturn = TRUE;
	}

	return bReturn;
}

// 改变某一设备的状态
static BOOL ChangeDeviceState(HDEVINFO hDevInfo, 
					   SP_DEVINFO_DATA hDeviceInfoData, 
					   DWORD dwState)
{

	BOOL bResult = FALSE;
	SP_PROPCHANGE_PARAMS pcpParams;
	OSTypeEnum	OsType = OnKnown;

	OsType = GetSystemVersion();

	// 判定设备是否已经被停用
	BOOL bStop = IsDeviceDisable(hDeviceInfoData);

	// 过滤重复操作
	if( bStop == TRUE && dwState == DICS_DISABLE)
		return TRUE;

	if( bStop == FALSE && dwState == DICS_ENABLE )
		return TRUE;

	// Win7上启用
	if( OsType == WIN7 ||
		OsType == WinVista )
	{
		pcpParams.Scope = DICS_FLAG_GLOBAL; 
	}
	else  // XP以前 
	{
		pcpParams.Scope = DICS_FLAG_CONFIGSPECIFIC;
		pcpParams.HwProfile = 0;
	}

	pcpParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcpParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcpParams.StateChange = dwState;

	// 设置或清空某一设备信息集或者某一设备信息元素中的安装参数
	// 若
	bResult = SetupDiSetClassInstallParams( hDevInfo, 
		&hDeviceInfoData, // 为NULL则设置信息集相关的所有设备
		(PSP_CLASSINSTALL_HEADER)&pcpParams, //存储安装信息
		sizeof(pcpParams) // 结构大小
		);

	if(bResult == FALSE)
		return bResult;

	// 改变指定设备状态
	bResult = SetupDiChangeState(hDevInfo, &hDeviceInfoData);

	return bResult;
}


static BOOL IsRootDevice(HDEVINFO hDevInfo, SP_DEVINFO_DATA& lpDevInfo)
{
	BOOL bRet = FALSE;
	std::wstring	strRoot = L"ROOT";
	std::wstring	strEnumRatorName;


	// 获取该设备的枚举器名称
	strEnumRatorName = GetDeviceProperty( hDevInfo, lpDevInfo, SPDRP_ENUMERATOR_NAME );

	// 全部转换成大写
	transform(strEnumRatorName.begin(), strEnumRatorName.end(), 
		strEnumRatorName.begin(), toupper);



	if( !strEnumRatorName.empty() )
	{
		// 执行比对, 如果对比相等
		if( !strEnumRatorName.compare( 0, strRoot.size(), strRoot ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}