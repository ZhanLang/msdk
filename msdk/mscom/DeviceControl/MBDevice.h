#pragma once
#include "device.h"

class CMBDevice :
	public CDevice
{
public:
	CMBDevice(void);
	~CMBDevice(void);
	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_BASEBOARD;}

	virtual LPCWSTR GetDeviceModel(){return m_strDeviceModel.c_str();}
	static BOOL Scan(DeviceScanNotify* pNotify);
};
