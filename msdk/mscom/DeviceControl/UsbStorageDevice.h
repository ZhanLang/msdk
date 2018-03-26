#pragma once
#include "Device.h"

class CUsbStorageDevice
	:public CDevice
{
public:
	CUsbStorageDevice(void);
	~CUsbStorageDevice(void);
	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_USBSTORAGE;}
	virtual DWORD GetDeviceID(){return CalcCrc32(GetSerialNumber());};

	static BOOL Scan(DeviceScanNotify* pNotify);
};
