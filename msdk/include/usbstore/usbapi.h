#pragma once

#include <WinIoCtl.h>

namespace msdk{;
namespace usbstore{;



static BOOL IsUsbDiskDevice(TCHAR letter)
{
	BOOL bRet = FALSE;
	TCHAR volumeAccessPath[] = _T("\\\\.\\X:");
	volumeAccessPath[4] = letter;

	HANDLE deviceHandle = CreateFile(
		volumeAccessPath,
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE, 
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (deviceHandle == INVALID_HANDLE_VALUE){
		return bRet;
	}

	STORAGE_PROPERTY_QUERY query;
	memset(&query, 0, sizeof(query));
	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	DWORD bytes;
	STORAGE_DEVICE_DESCRIPTOR devd;

	TCHAR strdir[] = _T("X:");
	strdir[0] = letter;

	bRet = GetDriveType(strdir)==DRIVE_REMOVABLE;

	if (DeviceIoControl(deviceHandle,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&query, sizeof(query),
		&devd, sizeof(devd),
		&bytes, NULL))
	{
		bRet = bRet && BusTypeUsb == devd.BusType;
	}

	CloseHandle(deviceHandle);

	return bRet;
}


};};