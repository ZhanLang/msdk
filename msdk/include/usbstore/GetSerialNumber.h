#pragma once

/************************************************************************/
/* 获取U盘序列号                                                        */
/************************************************************************/


#include <WinIoCtl.h>
#include <SetupAPI.h>
#include <strsafe.h>
#include <cfgmgr32.h>

#include "usb/usbiodef.h"
#include "usb/usbioctl.h"

#pragma comment(lib, "setupapi.lib")
namespace msdk{;
namespace usbstore{;

class CGetSerialNumber
{
	typedef struct _UDiskInfo
	{
		USHORT	m_nDeviceIndex;					// 设备索引号
		USHORT	m_nPlugedPort;					// 当前插入的USB端口号
		BOOL	m_bSuccessed;					// 是否获取U盘设备信息成功了
		TCHAR	m_szPID[BUFSIZ];				// 设备PID
		TCHAR	m_szVID[BUFSIZ];				// 设备VID
		TCHAR	m_cchDriverLetter;				// 设备上已经分配的所有盘符（仅记录盘符字符）
		TCHAR	m_szSerialNum[BUFSIZ];			// 设备序列号
		TCHAR	m_szDeviceName[BUFSIZ];			// 设备名称
		DWORD	m_dwStatusCode;					// 状态码
	}UDISK_INFO, *LPUDISK_INFO;


public:
	static BOOL GetSerialNumber(TCHAR cDosName, TCHAR* lpszSerialNumber, DWORD dwCchSerialNumber)
	{

		if( !IsUsbDiskDevice(cDosName) )
			return FALSE;

		TCHAR szHardwareID[MAX_PATH] = {0};
		TCHAR szInstanceID[MAX_PATH] = {0};
		TCHAR szHubDevicePath[MAX_PATH] = {0};
		TCHAR szDriverKeyName[MAX_PATH] = {0};
		
		UDISK_INFO m_ui = { 0 };
		m_ui.m_cchDriverLetter = cDosName;

		USB_DEVICE_DESCRIPTOR deviceDesc;
		bool bSerialNumber;

		if ( !FindDiskDevice(	m_ui.m_cchDriverLetter, szHubDevicePath, MAX_PATH - 1,szInstanceID,MAX_PATH - 1,m_ui.m_szDeviceName,BUFSIZ - 1,bSerialNumber) )
		{
			return ERROR_NO_MORE_ITEMS;
		}
		if ( _tcsnicmp( szInstanceID, _T("USB\\"), _tcslen(_T("USB\\")) ) == 0 )
		{
			// 如果是USB开头的设备实例
			int nPortCount = GetPortCount(szHubDevicePath);
			if ( nPortCount == 0 )
			{
				return ERROR_DEVICE_INSTALLER_NOT_READY;
			}
			for ( int i = 1; i <= nPortCount; i++ )
			{
				TCHAR szCurrentInstanceID[MAX_PATH] = {0};

				GetDriverKeyName (szHubDevicePath, i, szDriverKeyName, MAX_PATH - 1);
				FindInstanceIDByKeyName (szDriverKeyName, szHardwareID, MAX_PATH - 1, szCurrentInstanceID, MAX_PATH - 1 );
				if ( _tcsicmp( szInstanceID, szCurrentInstanceID ) == 0 )
				{
					GetDeviceDescriptor(szHubDevicePath, i, &deviceDesc );
					if (deviceDesc.iSerialNumber> 0)
					{
						GetVID( szCurrentInstanceID, m_ui.m_szVID );
						GetPID( szCurrentInstanceID, m_ui.m_szPID );
						GetStringDescriptor( szHubDevicePath, i, deviceDesc.iSerialNumber, m_ui.m_szSerialNum, BUFSIZ - 1 );
						break;
					}
				}
			}
		}
		if ( _tcslen(m_ui.m_szSerialNum) > 0 )
		{
			ToUpper(m_ui.m_szSerialNum );
			m_ui.m_bSuccessed = TRUE;

			_tcscpy_s(lpszSerialNumber, dwCchSerialNumber,m_ui.m_szSerialNum);
			return TRUE;
		}
		return FALSE;
	}


private:
	static DWORD GetStringDescriptor(LPCTSTR lpszHubPath, int iPortNumber, int iIndex, LPTSTR lpDescDataBuf, UINT nDescBufSize)
	{
		if ( lpszHubPath == NULL || lpszHubPath[0] == _T('\0') )
			return ERROR_INVALID_PARAMETER;

		if ( lpDescDataBuf == NULL || nDescBufSize <= 0 )		
			return ERROR_INVALID_PARAMETER;

		HANDLE hHub;
		DWORD dwRet = 0;
		BOOL bSuccess;
		ULONG nBytes;
		ULONG nBytesReturned;
		UCHAR stringDescReqBuf[sizeof (USB_DESCRIPTOR_REQUEST) + MAXIMUM_USB_STRING_LENGTH] = {0};
		PUSB_DESCRIPTOR_REQUEST pStringDescReq;
		PUSB_STRING_DESCRIPTOR pStringDesc;


		hHub = CreateFile(lpszHubPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		dwRet = GetLastError();

		if (hHub == INVALID_HANDLE_VALUE)
			return dwRet;

		nBytes = sizeof(stringDescReqBuf);
		pStringDescReq = (PUSB_DESCRIPTOR_REQUEST)stringDescReqBuf;
		pStringDesc = (PUSB_STRING_DESCRIPTOR)(pStringDescReq + 1);
		pStringDescReq->ConnectionIndex = iPortNumber;

		pStringDescReq->SetupPacket.wValue = (USB_STRING_DESCRIPTOR_TYPE << 8 ) | iIndex;
		pStringDescReq->SetupPacket.wIndex = 0x409;
		pStringDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

		bSuccess = DeviceIoControl(hHub, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,	pStringDescReq, nBytes, pStringDescReq, nBytes, &nBytesReturned, NULL);
		dwRet = GetLastError();
		CloseHandle (hHub);

		if (!bSuccess)
			return dwRet;
		if (nBytesReturned < 2)
			return dwRet;
		if (pStringDesc-> bDescriptorType != USB_STRING_DESCRIPTOR_TYPE)
			return dwRet;
		if (pStringDesc-> bLength != nBytesReturned - sizeof(USB_DESCRIPTOR_REQUEST))
			return dwRet;
		if (pStringDesc-> bLength % 2 != 0)
			return dwRet;
#ifdef _UNICODE
		StringCchCopyW( (WCHAR *)lpDescDataBuf, nDescBufSize - 1, pStringDesc->bString );
#else
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, (WCHAR *)(pStringDesc->bString), wcslen((WCHAR *)pStringDesc->bString), lpDescDataBuf, nDescBufSize - 1, NULL, NULL);
#endif
		return 0;
	}

	static DWORD GetDeviceDescriptor( LPCTSTR lpszHubPath, int iPortNumber, PUSB_DEVICE_DESCRIPTOR pDeviceDescriptor )
	{
		if ( lpszHubPath == NULL || lpszHubPath[0] == _T('\0') )	
			return ERROR_INVALID_PARAMETER;

		if ( pDeviceDescriptor == NULL )	
			return ERROR_INVALID_PARAMETER;

		HANDLE hHub;
		DWORD dwRet = 0;
		BYTE pbBuffer[500] = {0};
		PUSB_NODE_CONNECTION_INFORMATION pConnectionInfo = (PUSB_NODE_CONNECTION_INFORMATION)pbBuffer;
		PUSB_DEVICE_DESCRIPTOR pDescriptor = (PUSB_DEVICE_DESCRIPTOR) pbBuffer;


		DWORD dwNumBytes = 0;
		hHub = CreateFile(lpszHubPath,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING, 0, NULL);
		dwRet = GetLastError();

		if (hHub == INVALID_HANDLE_VALUE)
			return dwRet;

		pConnectionInfo->ConnectionIndex = iPortNumber;

		BOOL bRet = DeviceIoControl(hHub,IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,pConnectionInfo,sizeof(pbBuffer),pConnectionInfo,sizeof (pbBuffer),&dwNumBytes, NULL);
		dwRet = GetLastError();

		if ( bRet )
			memcpy(pDeviceDescriptor, &pConnectionInfo->DeviceDescriptor, sizeof (USB_DEVICE_DESCRIPTOR));

		CloseHandle(hHub);
		return dwRet;
	}

	static DWORD FindInstanceIDByKeyName( LPCTSTR lpszDriverKeyName,LPTSTR lpszHardwareID,UINT nHardwareIDBusSize,LPTSTR lpInstanceID,UINT nInstanceIDBufSize)
	{
		if ( lpszDriverKeyName == NULL || lpszDriverKeyName[0] == _T('\0') )	
			return ERROR_INVALID_PARAMETER;

		if ( lpszHardwareID == NULL || nHardwareIDBusSize <= 0 )	
			return ERROR_INVALID_PARAMETER;

		if ( lpInstanceID == NULL || nInstanceIDBufSize <= 0 )
			return ERROR_INVALID_PARAMETER;

		HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
		BOOL bSuccess;
		int i = 0;
		DWORD dwRet = 0;
		LPCTSTR lpszKeyName = NULL;
		SP_DEVINFO_DATA devinfoData = {0};
		DWORD dwNumBytes = 0;
		BYTE pbBuffer[BUFSIZ] = {0};
		lpszHardwareID = _T("");

		hDevInfo = SetupDiGetClassDevs (0,_T("USB"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
		dwRet = GetLastError();
		if (hDevInfo == INVALID_HANDLE_VALUE)
			return dwRet;

		while (true)
		{
			devinfoData.cbSize = sizeof(devinfoData);
			bSuccess = SetupDiEnumDeviceInfo(hDevInfo, i, &devinfoData);
			dwRet = GetLastError();
			if ( !bSuccess)
				break;

			lpszKeyName = _T ("");
			if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &devinfoData, SPDRP_DRIVER, NULL, pbBuffer,sizeof(pbBuffer), &dwNumBytes) )
				lpszKeyName = (TCHAR *)pbBuffer;

			if ( lpszKeyName && _tcsicmp( lpszKeyName, lpszDriverKeyName ) == 0 )
			{
				SetupDiGetDeviceInstanceId(	hDevInfo, &devinfoData, (LPTSTR)lpInstanceID, nInstanceIDBufSize,&dwNumBytes);
				if (SetupDiGetDeviceRegistryProperty(hDevInfo, &devinfoData, SPDRP_HARDWAREID,NULL, (PBYTE)lpszHardwareID,nHardwareIDBusSize, &dwNumBytes) )
					break;
			}
			i++;
		}

		SetupDiDestroyDeviceInfoList (hDevInfo);
		return 0;
	}


	static int GetPortCount( LPCTSTR lpszHubDevicePath)
	{
		if ( lpszHubDevicePath == NULL || lpszHubDevicePath[0] == _T('\0') )	
			return 0;

		int nCount = 0;
		HANDLE hHub = INVALID_HANDLE_VALUE;
		DWORD dwNumBytes = 0;
		USB_NODE_INFORMATION nodeInfo;
		memset( &nodeInfo, 0, sizeof(nodeInfo) );

		hHub = CreateFile(lpszHubDevicePath, GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_EXISTING, 0, NULL);
		if (hHub == INVALID_HANDLE_VALUE)
			return nCount;

		nodeInfo. NodeType = UsbHub;
		if ( DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, &nodeInfo,sizeof (nodeInfo),&nodeInfo, sizeof(nodeInfo), &dwNumBytes, NULL) )
			nCount = nodeInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;

		CloseHandle (hHub);
		return nCount;
	}

	static DWORD GetDriverKeyName(LPCTSTR lpszHubPath, int iPortNumber, LPTSTR lpszDriverKeyName, UINT nDriverKeyNameBufSize)
	{
		if ( lpszHubPath == NULL || lpszHubPath[0] == _T('\0') )	
			return ERROR_INVALID_PARAMETER;
		
		if ( lpszDriverKeyName == NULL || nDriverKeyNameBufSize <= 0 )	
			return ERROR_INVALID_PARAMETER;

		HANDLE hHub = INVALID_HANDLE_VALUE;
		DWORD dwNumBytes = 0;
		DWORD dwRet = 0;
		USB_NODE_CONNECTION_INFORMATION nodeInfo = {0};
		PBYTE pbBuffer[500] = {0};

		hHub = CreateFile(lpszHubPath, GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_EXISTING, 0,NULL);
		dwRet = GetLastError();

		if ( hHub != INVALID_HANDLE_VALUE )
		{
			nodeInfo.ConnectionIndex = iPortNumber;
			if (DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,&nodeInfo, sizeof(nodeInfo), &nodeInfo, sizeof(nodeInfo), &dwNumBytes, NULL))
			{
				if ( nodeInfo.ConnectionStatus == DeviceConnected )
				{
					PUSB_NODE_CONNECTION_DRIVERKEY_NAME pDriverKeyName = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME)pbBuffer;
					pDriverKeyName->ConnectionIndex = iPortNumber;
					if (DeviceIoControl(hHub,IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,pDriverKeyName,sizeof(pbBuffer), pDriverKeyName,sizeof(pbBuffer), 	&dwNumBytes, NULL)) 
					{
#ifdef _UNICODE
						StringCchCopyW( lpszDriverKeyName, nDriverKeyNameBufSize - 1, (TCHAR *)pDriverKeyName->DriverKeyName );
#else
						TCHAR Res[256] = {0};
						WideCharToMultiByte(	CP_ACP, WC_COMPOSITECHECK, (WCHAR *)pDriverKeyName->DriverKeyName, wcslen((WCHAR *)pDriverKeyName->DriverKeyName), 
							lpszDriverKeyName, nDriverKeyNameBufSize - 1, NULL, NULL);
#endif
					}
				}
			}
			dwRet = GetLastError();
			CloseHandle( hHub );
		}
		return dwRet;
	}

	static bool FindDiskDevice( TCHAR cchDriveLetter, 
		LPTSTR lpszHubDevicePath,UINT nHubDevicePathBufSize,
		LPTSTR lpszInstanceID, UINT nInstanceIDBufSize,
		LPTSTR lpszDeviceName, 	UINT nDeviceNameBufSize,
		bool & bSerialNumber)
	{
		if ( (cchDriveLetter < _T('a') || cchDriveLetter > _T('z')) && (cchDriveLetter < _T('A') || cchDriveLetter > _T('Z')) )	
			return false;

		if ( lpszHubDevicePath == NULL || nHubDevicePathBufSize <= 0 )	
			return false;

		if ( lpszInstanceID == NULL || nInstanceIDBufSize <= 0 )	
			return false;

		if ( lpszDeviceName == NULL || nDeviceNameBufSize <= 0 )	
			return false;

		bool bReturn = false;
		BOOL bSuccess;
		int iDeviceNumber;
		HDEVINFO hSetup;
		int i;
		DWORD dwNumBytes;
		SP_DEVICE_INTERFACE_DATA interfaceData;
		SP_DEVINFO_DATA devinfoData;
		BYTE pbBuffer[800] = {0};
		PSP_DEVICE_INTERFACE_DETAIL_DATA pDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)&pbBuffer[0];
		TCHAR szDevicePath[MAX_PATH] = {0};
		StringCchPrintf( szDevicePath, MAX_PATH - 1, _T("\\\\.\\%C:"), cchDriveLetter );
		bSerialNumber = false;
		iDeviceNumber = GetDeviceNumber(szDevicePath);
		if ( iDeviceNumber == -1 )
			return false;

		hSetup = SetupDiGetClassDevs((LPGUID)&GUID_DEVINTERFACE_DISK,NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (hSetup == INVALID_HANDLE_VALUE)
			return false;

		i = 0;
		while (true)
		{
			interfaceData.cbSize = sizeof(interfaceData);
			bSuccess = SetupDiEnumDeviceInterfaces(hSetup, NULL,(LPGUID)&GUID_DEVINTERFACE_DISK, i,&interfaceData);
			if (!bSuccess)
				break;

			devinfoData.cbSize = sizeof(devinfoData);
			pDetailData-> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			if ( SetupDiGetDeviceInterfaceDetail(hSetup, &interfaceData, pDetailData, sizeof(pbBuffer), &dwNumBytes, &devinfoData) != 0 )
			{
				if (GetDeviceNumber(pDetailData-> DevicePath) == iDeviceNumber )
				{
					DEVINST devPrev;
					TCHAR szTemp[BUFSIZ] = {0};
					WCHAR szGUID[64] = {0};
					DWORD dwCap;
					SetupDiGetDeviceRegistryProperty(hSetup, &devinfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)lpszDeviceName, nDeviceNameBufSize/sizeof(TCHAR), &dwNumBytes);
					if (SetupDiGetDeviceRegistryProperty (hSetup, &devinfoData, SPDRP_CAPABILITIES, NULL, (PBYTE) &dwCap, sizeof (dwCap), &dwNumBytes))
						bSerialNumber = (dwCap && CM_DEVCAP_UNIQUEID) != 0;
					CM_Get_Parent (&devPrev, devinfoData. DevInst, 0);
					CM_Get_Device_ID(devPrev, (TCHAR *)lpszInstanceID, nInstanceIDBufSize/sizeof(TCHAR), 0);
					CM_Get_Parent (&devPrev, devPrev, 0);
					CM_Get_Device_ID(devPrev, szTemp, BUFSIZ/sizeof(TCHAR), 0);

					for ( size_t j = 0; j < _tcslen(szTemp); j++ )
						if (szTemp[j] == _T('\\'))
							szTemp[j] = _T('#');

					GUID GuidDevIntUsbHub = {0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, 0xa0, 0xc9, 0x06, 0xbe, 0xd8};	//GUID_DEVINTERFACE_USB_HUB;
					StringFromGUID2( GuidDevIntUsbHub, (LPOLESTR)(PBYTE)szGUID, sizeof (szGUID) / sizeof (WCHAR) );
#ifdef _UNICODE
					StringCchPrintfW( lpszHubDevicePath, nHubDevicePathBufSize - 1, L"\\\\.\\%s#%s", szTemp, szGUID );
#else
					CHAR Res[256] = {0};
					WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK, (WCHAR *)szGUID, wcslen ((WCHAR *)szGUID), Res, 256, NULL, NULL);
					StringCchPrintfA( lpszHubDevicePath, nHubDevicePathBufSize - 1, "\\\\.\\%s#%s", szTemp, Res );
#endif
					bReturn = true;
					break;
				}
			}
			i++;
		}
		SetupDiDestroyDeviceInfoList(hSetup);
		return bReturn;
	}

	static int	GetDeviceNumber( LPCTSTR lpszDeviceRootPath )
	{
		if ( lpszDeviceRootPath == NULL || lpszDeviceRootPath[0] == _T('\0') )		return -1;

		int iDeviceNumber = -1;
		HANDLE hVolume;
		hVolume = CreateFile(lpszDeviceRootPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if ( hVolume != INVALID_HANDLE_VALUE )
		{
			STORAGE_DEVICE_NUMBER deviceNumber;
			DWORD dwNumBytes;
			if ( DeviceIoControl(hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL,0, &deviceNumber, 	sizeof(deviceNumber), &dwNumBytes, NULL) != 0)
				iDeviceNumber = (deviceNumber.DeviceType <<8 ) + deviceNumber.DeviceNumber;

			CloseHandle (hVolume);
		}
		return iDeviceNumber;
	}

	static int GetVID(LPCTSTR lpInstancePath, LPTSTR lpszVid, USHORT nBufSize = 5)
	{
		if ( lpInstancePath == NULL || lpInstancePath[0] == _T('\0') )	
			return 0;

		if ( lpszVid == NULL || nBufSize <= 0 )	
			return 0;

		int nLen = 0;
		for ( size_t i = 0; i < _tcslen(lpInstancePath) - 2; i++ )
		{
			if ( (lpInstancePath[i] == _T('V') || lpInstancePath[i] == _T('v')) && 
				(lpInstancePath[i+1] == _T('I') || lpInstancePath[i+1] == _T('i')) && 
				(lpInstancePath[i+2] == _T('D') || lpInstancePath[i+2] == _T('d')) )
			{
				StringCchCopyN( lpszVid, nBufSize, lpInstancePath + i + 4, 4 );
				break;
			}
		}
		return _tcslen(lpszVid);
	}


	static int GetPID(LPCTSTR lpInstancePath, LPTSTR lpszPid, USHORT nBufSize = 5)
	{
		if ( lpInstancePath == NULL || lpInstancePath[0] == _T('\0') )	
			return 0;

		if ( lpszPid == NULL || nBufSize <= 0 )	return 0;

		int nLen = 0;
		for ( size_t i = 0; i < _tcslen(lpInstancePath) - 2; i++ )
		{
			if ( (lpInstancePath[i] == _T('P') || lpInstancePath[i] == _T('p')) && 
				(lpInstancePath[i+1] == _T('I') || lpInstancePath[i+1] == _T('i')) && 
				(lpInstancePath[i+2] == _T('D') || lpInstancePath[i+2] == _T('d')) )
			{
				StringCchCopyN( lpszPid, nBufSize, lpInstancePath + i + 4, 4 );
				break;
			}
		}
		return _tcslen(lpszPid);
	}

	static size_t ToUpper( LPTSTR lpString )
	{
		if ( lpString == NULL || lpString[0] == _T('\0') )	
			return 0;

		size_t nCount = 0;
		for ( size_t i = 0; i < _tcslen(lpString); i++ )
		{
			if ( lpString[i] >= _T('a') && lpString[i] <= _T('z') )
			{
				lpString[i] = lpString[i] - 32;
				nCount++;
			}
		}
		return nCount;
	}

	static size_t ToLower( LPTSTR lpString )
	{
		if ( lpString == NULL || lpString[0] == _T('\0') )	
			return 0;

		size_t nCount = 0;
		for ( size_t i = 0; i < _tcslen(lpString); i++ )
		{
			if ( lpString[i] >= _T('A') && lpString[i] <= _T('Z') )
			{
				lpString[i] = lpString[i] + 32;
				nCount++;
			}
		}
		return nCount;
	}

	static BOOL IsUsbDiskDevice(TCHAR letter)
	{
		BOOL bRet = FALSE;
		TCHAR volumeAccessPath[] = _T("\\\\.\\X:");
		volumeAccessPath[4] = letter;

		HANDLE deviceHandle = CreateFile(volumeAccessPath,0,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0, NULL);

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

		if (DeviceIoControl(deviceHandle,IOCTL_STORAGE_QUERY_PROPERTY,&query, sizeof(query),&devd, sizeof(devd),&bytes, NULL))
		{
			bRet = bRet && BusTypeUsb == devd.BusType;
		}

		CloseHandle(deviceHandle);

		return bRet;
	}


};

};};