#pragma once
#include "device.h"

class CCpuDevice :
	public CDevice
{
public:
	CCpuDevice(void);
	~CCpuDevice(void);
	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_PROCESSOR;}
	
	//¶àºËCPUµ¼ÖÂIDÖØ¸´
	virtual DWORD GetDeviceID(){return CalcCrc32(m_strDriver + m_strHID);};
	static BOOL Scan(DeviceScanNotify* pNotify);
};
