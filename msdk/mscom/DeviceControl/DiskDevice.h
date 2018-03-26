#pragma once
#include "Device.h"

class CDiskDevice :
	public CDevice
{
public:
	CDiskDevice(void);
	~CDiskDevice(void);
	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_DISKDRIVE;}
	virtual DWORD GetDeviceID(){return CalcCrc32(GetSerialNumber());};

	static BOOL Scan(DeviceScanNotify* pNotify);
};
