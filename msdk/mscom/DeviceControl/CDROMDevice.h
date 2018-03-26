#pragma once
#include "Device.h"

class CCDROMDevice
	:public CDevice
{
public:
	CCDROMDevice(void);
	~CCDROMDevice(void);

	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_CDROM;}
	virtual DWORD GetDeviceID(){return CalcCrc32(GetSerialNumber());};

	static BOOL Scan(DeviceScanNotify* pNotify);
};
