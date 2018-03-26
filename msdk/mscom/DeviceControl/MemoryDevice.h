#pragma once
#include "device.h"

class CMemoryDevice :
	public CDevice
{
public:
	CMemoryDevice(void);
	~CMemoryDevice(void);

	static BOOL Scan(DeviceScanNotify* pNotify);
};
