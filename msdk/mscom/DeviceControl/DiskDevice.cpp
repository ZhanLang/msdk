#include "StdAfx.h"
#include "DiskDevice.h"

CDiskDevice::CDiskDevice(void)
{
}

CDiskDevice::~CDiskDevice(void)
{
}


/*
GUID_DEVCLASS_FDC软盘控制器
GUID_DEVCLASS_DISPLAY显示卡
GUID_DEVCLASS_CDROM光驱
GUID_DEVCLASS_KEYBOARD键盘
GUID_DEVCLASS_COMPUTER计算机
GUID_DEVCLASS_SYSTEM系统
GUID_DEVCLASS_DISKDRIVE磁盘驱动器
GUID_DEVCLASS_MEDIA声音、视频和游戏控制器
GUID_DEVCLASS_MODEMMODEM
GUID_DEVCLASS_MOUSE鼠标和其他指针设备
GUID_DEVCLASS_NET网络设备器
GUID_DEVCLASS_USB通用串行总线控制器
GUID_DEVCLASS_FLOPPYDISK软盘驱动器
GUID_DEVCLASS_UNKNOWN未知设备
GUID_DEVCLASS_SCSIADAPTERSCSI 和 RAID 控制器
GUID_DEVCLASS_HDCIDE ATA/ATAPI 控制器
GUID_DEVCLASS_PORTS端口（COM 和 LPT）
GUID_DEVCLASS_MONITOR监视器
*/


BOOL CDiskDevice::Scan(DeviceScanNotify* pNotify)
{
	RASSERT(pNotify, FALSE);
	SP_DEVINFO_DATA hDeviceInfoData = {0};
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_DISKDRIVE, 0, 0, DIGCF_PRESENT);
	RASSERT(hDevInfo != INVALID_HANDLE_VALUE, FALSE);
 
	hDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	
	for (int i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&hDeviceInfoData);i++)
	{
		
		CDiskDevice* pDiskDevice = new CDiskDevice();
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