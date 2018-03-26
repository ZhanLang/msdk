#include "StdAfx.h"
#include "NetCardDevice.h"

CNetCardDevice::CNetCardDevice(void)
{
}

CNetCardDevice::~CNetCardDevice(void)
{
}

BOOL CNetCardDevice::Scan(DeviceScanNotify*pNotify)
{
	RASSERT(pNotify, FALSE);
	SP_DEVINFO_DATA hDeviceInfoData = {0};
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_NET, 0, 0, DIGCF_PRESENT);
	RASSERT(hDevInfo != INVALID_HANDLE_VALUE, FALSE);

	hDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (int i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&hDeviceInfoData);i++)
	{
		//过滤掉系统的东西
		if(IsRootDevice(hDevInfo, hDeviceInfoData))
		{
			continue;
		}


		CNetCardDevice* pDiskDevice = new CNetCardDevice();
		pDiskDevice->SetClass(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_CLASS));			// 设备安装类名称 用来构建树 系统设备、显示卡
		pDiskDevice->SetClassGuid(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_CLASSGUID));	// 设备安装类的GUID
		pDiskDevice->SetDesc(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_DEVICEDESC));		// 设备描述
		pDiskDevice->SetFName(GetDeviceProperty(hDevInfo,hDeviceInfoData,  SPDRP_FRIENDLYNAME));	// 友好(易读)名称
		pDiskDevice->SetHID(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_HARDWAREID));		// 硬件设备ID
		pDiskDevice->SetDriver(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_DRIVER));			// 返回驱动信息在注册表中的位置
		pDiskDevice->SetManufacturer(GetDeviceProperty(hDevInfo, hDeviceInfoData, SPDRP_MFG));				// 设备制造商
		pDiskDevice->SetCompatibleID(GetDeviceProperty(hDevInfo,hDeviceInfoData, SPDRP_COMPATIBLEIDS));
		pDiskDevice->SetServiceName( GetDeviceProperty(hDevInfo,hDeviceInfoData, SPDRP_SERVICE));
		pDiskDevice->SetNumeratorName(GetDeviceProperty(hDevInfo,hDeviceInfoData, SPDRP_ENUMERATOR_NAME)); 

		pNotify->OnDeviceScanNotify(pDiskDevice);
		
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return TRUE;
}
