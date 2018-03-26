#include "stdafx.h"
#include "Device.h"

BOOL CDevice::StartDevice()
{
	SP_DEVINFO_DATA				hDeviceInfoData;
	BOOL						bRet = FALSE;

	if( m_strHID.empty() )
		return bRet;

	// 转换成大写
	transform( m_strHID.begin(), m_strHID.end(),
		m_strHID.begin(), toupper );

	// 获取指定类型的设备信息集
	HDEVINFO hDevInfo = SetupDiGetClassDevs( (LPGUID)&(S2GUIDW(m_strClassGuid.c_str())), 
		0,
		0, 
		DIGCF_PRESENT  );

	if ( hDevInfo == INVALID_HANDLE_VALUE )
		return FALSE;

	hDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// 枚举设备信息集中得所有设备信息元素
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &hDeviceInfoData); i++)
	{
		std::wstring strDeviceID = GetDeviceProperty(hDevInfo,hDeviceInfoData, SPDRP_HARDWAREID);

		// 转换成大写
		transform( strDeviceID.begin(), strDeviceID.end(),
			strDeviceID.begin(), toupper );

		// 使用设备硬件ID筛选出指定设备
		if( !strDeviceID.compare(m_strHID) )
		{
			// 改变指定设备的状态
			bRet = ChangeDeviceState(hDevInfo, hDeviceInfoData, DICS_ENABLE);
			break;
		}
	}

	if( hDevInfo)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}

// 停用某一个设备
BOOL CDevice::StopDevice()
{
	SP_DEVINFO_DATA	hDeviceInfoData;
	BOOL			bRet = FALSE;

	if( m_strHID.empty() )
		return bRet;

	// 获取指定类型设备的信息集
	HDEVINFO hDevInfo = SetupDiGetClassDevs( (LPGUID)&(S2GUIDW(m_strClassGuid.c_str())), // All Classes
		0,
		0, 
		DIGCF_PRESENT );

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return FALSE;

	hDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// 枚举信息集中得所有设备信息元素
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &hDeviceInfoData); i++)
	{
		// 使用硬件设备ID来筛选出指定设备
		if( GetDeviceProperty(hDevInfo,hDeviceInfoData, SPDRP_HARDWAREID).compare(m_strHID) == 0 )
		{
			// 判断设备是否已经被停用
			if( IsDeviceDisable(hDeviceInfoData) )	// 判定设备是否停止，如果停止将不再处理
			{
				bRet = TRUE;
				goto ErrorHandle;
			}

			// 改变设备状态为停用
			bRet = ChangeDeviceState(hDevInfo,hDeviceInfoData, DICS_DISABLE);
			break;
		}
	}

ErrorHandle:
	if( hDevInfo)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}