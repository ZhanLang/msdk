#include "StdAfx.h"
#include "MemoryDevice.h"

CMemoryDevice::CMemoryDevice(void)
{
}

CMemoryDevice::~CMemoryDevice(void)
{
}

BOOL CMemoryDevice::Scan(DeviceScanNotify* pNotify)
{
	RASSERT(pNotify, FALSE);
	return TRUE;
}