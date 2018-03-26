#pragma once
#include "device.h"

class CNetCardDevice :
	public CDevice
{
public:
	CNetCardDevice(void);
	~CNetCardDevice(void);

	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_NETCARD;}
	virtual DWORD GetDeviceID(){return CalcCrc32(GetSerialNumber());};
	
	virtual LPCWSTR GetDeviceModel(){return m_strFName.c_str();}
	static BOOL Scan(DeviceScanNotify*pNotift);
};
