#pragma once

#include <Winioctl.h>


namespace msdk{;
namespace msapi{;

 /*
	  获取分区的硬盘号：0，第0块磁盘，1：第二块磁盘
*/


static HANDLE OpenDisk( DWORD dwDiskNum)
{
	TCHAR szDrivePath[MAX_PATH] = {0};
	_stprintf_s( szDrivePath, MAX_PATH - 1, _T("\\\\.\\PhysicalDrive%d"), dwDiskNum );

	return  CreateFile (szDrivePath,  GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
}

static HANDLE OpenVolume( TCHAR letter)
{
	TCHAR volumeAccessPath[] = _T("\\\\.\\X:");
	volumeAccessPath[4] = letter;

	return CreateFile(
		volumeAccessPath,
		GENERIC_READ|GENERIC_WRITE,   // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE, 
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

}



static BOOL GetDiskDeviceNumber(HANDLE deviceHandle, DWORD* dwNum,  DWORD* dwPartNum = 0)
{
	if (deviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	STORAGE_DEVICE_NUMBER sdn;
	memset( &sdn, 0, sizeof(sdn) );
	DWORD dwRead = 0;

	BOOL bRet = DeviceIoControl(
		deviceHandle,                // handle to driver
		IOCTL_STORAGE_GET_DEVICE_NUMBER, // dwIoControlCode
		NULL,                            // lpInBuffer
		0,                               // nInBufferSize
		&sdn,           // output buffer
		sizeof(sdn),         // size of output buffer
		&dwRead,       // number of bytes returned
		NULL      // OVERLAPPED structure
		);
	if (!bRet){
		return FALSE;
	}

	if ( dwNum ) *dwNum = sdn.DeviceNumber;
	if( dwPartNum) * dwPartNum = sdn.PartitionNumber;

	return TRUE;
}

static HANDLE OpenDisk( TCHAR letter)
{
	HANDLE hDevice =  msapi::OpenVolume(letter);
	if ( hDevice == INVALID_HANDLE_VALUE)
		return hDevice;


	DWORD dwDiskNum = 0;
	if ( !GetDiskDeviceNumber(hDevice , &dwDiskNum))
	{
		CloseHandle(hDevice);
		return INVALID_HANDLE_VALUE;
	}

	return OpenDisk((DWORD)dwDiskNum);
}


//判断磁盘是否为GPT
static BOOL IsDiskGpt(HANDLE  hDriver, BOOL* bGpt)
{
	if (hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bRet = FALSE;
	DWORD    nDiskBytesRead = 0;//预设为0，当缓冲区的长度不够时，该值为所需的缓冲区的长度      
	DWORD    nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 200;
	PDRIVE_LAYOUT_INFORMATION_EX lpDiskPartInfo = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(nDiskBufferSize);
	memset(lpDiskPartInfo, 0, nDiskBufferSize);

	
	do 
	{
		bRet = DeviceIoControl(
			hDriver,
			IOCTL_DISK_GET_DRIVE_LAYOUT_EX,     
			NULL,     
			0,
			(LPVOID) lpDiskPartInfo,
			(DWORD) nDiskBufferSize,
			(LPDWORD) &nDiskBytesRead,
			NULL
			);

		if ( !bRet )
			break;

		if ( bGpt )
			*bGpt = lpDiskPartInfo->PartitionStyle == PARTITION_STYLE_GPT;

	} while (0);
	


	if ( lpDiskPartInfo )
		free(lpDiskPartInfo);

	
	return bRet;
}


static BOOL GetGptDiskNum( HANDLE hDriver, DWORD* dwPartCount)
{

	if (hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bRet = FALSE;
	DWORD    nDiskBytesRead = 0;//预设为0，当缓冲区的长度不够时，该值为所需的缓冲区的长度      
	DWORD    nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 200;
	PDRIVE_LAYOUT_INFORMATION_EX lpDiskPartInfo = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(nDiskBufferSize);
	memset(lpDiskPartInfo, 0, nDiskBufferSize);


	do 
	{
		bRet = DeviceIoControl(
			hDriver,
			IOCTL_DISK_GET_DRIVE_LAYOUT_EX,     
			NULL,     
			0,
			(LPVOID) lpDiskPartInfo,
			(DWORD) nDiskBufferSize,
			(LPDWORD) &nDiskBytesRead,
			NULL
			);

		if ( !bRet )
			break;

		if ( dwPartCount )
			*dwPartCount = lpDiskPartInfo->PartitionCount;

	} while (0);



	if ( lpDiskPartInfo )
		free(lpDiskPartInfo);


	return bRet;
}

//获取ESP分区
static BOOL GetGptDiskEspPart( HANDLE  hDriver, DWORD* dwEspPart, BOOL bDim = FALSE/*模糊匹配，大小小于300M就算*/)
{
	DWORD    nDiskBytesRead = 0;
	DWORD    nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 200;
	PDRIVE_LAYOUT_INFORMATION_EX lpDiskPartInfo = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(nDiskBufferSize);
	BOOL bRet = FALSE;

	if ( hDriver == INVALID_HANDLE_VALUE )
	{
		free(lpDiskPartInfo);
		return FALSE;
	}
		
	if ( !DeviceIoControl(hDriver,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) lpDiskPartInfo,(DWORD) nDiskBufferSize,(LPDWORD) &nDiskBytesRead,NULL) ) 
	{
		free(lpDiskPartInfo);
		return FALSE;
	}

	for ( DWORD nLoop = 0 ; nLoop < lpDiskPartInfo->PartitionCount ; nLoop++)
	{
		TCHAR szGUID[128] = { 0 };
		StringFromGUID2(lpDiskPartInfo->PartitionEntry[nLoop].Gpt.PartitionType, szGUID, 128);
		if ( _tcsicmp(_T("{C12A7328-F81F-11D2-BA4B-00A0C93EC93B}"), szGUID) == 0)
		{
			if ( dwEspPart ) 
			{
				*dwEspPart = lpDiskPartInfo->PartitionEntry[nLoop].PartitionNumber;
				(*dwEspPart)--;
			}
			bRet = TRUE;
			break;
		}
	}
	

	if ( !bRet && bDim )
	{
		for ( DWORD nLoop = 0 ; nLoop < lpDiskPartInfo->PartitionCount ; nLoop++)
		{
			if ( lpDiskPartInfo->PartitionEntry[nLoop].PartitionLength.LowPart < 1024 * 1024 * 300)
			{
				if ( dwEspPart ) 
				{
					*dwEspPart = lpDiskPartInfo->PartitionEntry[nLoop].PartitionNumber;
					(*dwEspPart)--;
				}
				bRet = TRUE;
				break;
			}
		}
	}
	free(lpDiskPartInfo);
	return bRet;
}

//获取MBR活动分区号
static BOOL GetMbrDiskBootPart(HANDLE  hDriver, DWORD* dwBootPart)
{
	DWORD    nDiskBytesRead = 0;
	DWORD    nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 200;
	PDRIVE_LAYOUT_INFORMATION_EX lpDiskPartInfo = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(nDiskBufferSize);
	BOOL bRet = FALSE;



	if ( hDriver == INVALID_HANDLE_VALUE )
		return FALSE;

	if ( !DeviceIoControl(hDriver,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID)lpDiskPartInfo,(DWORD) nDiskBufferSize,(LPDWORD) &nDiskBytesRead,NULL) ) 
	{
		free(lpDiskPartInfo);
		return FALSE;
	}

	if ( lpDiskPartInfo->PartitionStyle != PARTITION_STYLE_MBR)
	{
		free(lpDiskPartInfo);
		return FALSE;
	}
	
	 
	for ( DWORD nLoop = 0 ; nLoop < lpDiskPartInfo->PartitionCount ; nLoop++)
	{
		if ( lpDiskPartInfo->PartitionEntry[nLoop].Mbr.BootIndicator )
		{
			if ( dwBootPart )
			{
				*dwBootPart = lpDiskPartInfo->PartitionEntry[nLoop].PartitionNumber;
				(*dwBootPart)--;
			}

			bRet = TRUE;
			break;
		}
	}

	free(lpDiskPartInfo);
	return bRet;
}


static BOOL QueryDosDeviceByDiskPart( DWORD dwDisk, DWORD dwPart,  WCHAR* cDosDevice)
{
	TCHAR strFormat[ MAX_PATH ] = { 0 };
	_stprintf_s(strFormat, MAX_PATH, _T("\\Device\\Harddisk%d\\Partition%d"), dwDisk, dwPart);
	
	TCHAR lpDriveString[MAX_PATH] = {0};
	GetLogicalDriveStrings(MAX_PATH,lpDriveString);
	int nCount  = 0;  
	TCHAR * pDrive = lpDriveString;  
	for(int nlen =_tcslen(lpDriveString); nlen == 3 ;nCount++)  
	{   
		WCHAR cDocName = pDrive[0];
		if(cDocName>=L'a') cDocName-=32;

		DWORD dwDiskNum = 0;
		DWORD dwPartNum = 0;
		HANDLE hHandle = OpenDisk(cDocName);
		if ( GetDiskDeviceNumber(hHandle, &dwDiskNum, &dwPartNum ) )
		{
			if ( dwDiskNum == dwDisk && dwPartNum == dwPart )
			{
				CloseHandle(hHandle);
				if ( cDosDevice ) * cDosDevice = cDocName;
				return TRUE;
			}
			CloseHandle(hHandle);
		}
		pDrive +=4;  
		nlen = _tcslen(pDrive);  
	}  

	return FALSE;
}

static BOOL DefineDosDevice( DWORD dwDisk , DWORD dwPart, WCHAR* cDosDrive)
{
	TCHAR strFormat[ MAX_PATH ] = { 0 };
	_stprintf_s(strFormat, MAX_PATH, _T("\\Device\\Harddisk%d\\Partition%d"), dwDisk, dwPart);

	TCHAR cCanUseDrive = 0;
	DWORD unitmask = GetLogicalDrives();
	for( int j = 2; j  < 32; j ++ )
	{
		DWORD dwBit = 0x01 << j ;
		if( dwBit & unitmask )
			continue;

		cCanUseDrive = 'A' + j;
		break;
	}

	if ( !cCanUseDrive )
		return FALSE;

	TCHAR strDest[] = _T("X:"); strDest[0] = cCanUseDrive;
	BOOL bRes = ::DefineDosDevice (DDD_RAW_TARGET_PATH, strDest, strFormat);
	if( bRes && cDosDrive) *cDosDrive = cCanUseDrive;
	return bRes;
}

static BOOL DeleteAllPation(HANDLE hDriver )
{

	if ( hDriver == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD dwRead = 0;
	BOOL bRet = DeviceIoControl(hDriver, IOCTL_DISK_DELETE_DRIVE_LAYOUT, NULL, 0, NULL, 0,&dwRead, NULL);
	if ( !bRet )
		return FALSE;

	bRet = DeviceIoControl(hDriver, IOCTL_DISK_UPDATE_PROPERTIES, NULL,0,NULL,0,&dwRead, NULL);
	return bRet;
}

static BOOL GetDiskSpace( HANDLE hDevice, ULONGLONG& ullSize)
{
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DISK_GEOMETRY dgm = {0};
	DWORD dwRead = 0;
	BOOL bRes = DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL, 0,&dgm, sizeof(dgm), &dwRead, (LPOVERLAPPED) NULL);

	ullSize  = dgm.Cylinders.QuadPart * (ULONG)dgm.TracksPerCylinder *(ULONG)dgm.SectorsPerTrack * (ULONG)dgm.BytesPerSector;
	return bRes;
}


//获取磁盘的剩余空间
static BOOL GetDiskFreeSpaceEx(LPCTSTR lpszDriver, ULONGLONG& ullFreeSize)
{
	BOOL	 fResult = FALSE;
	ATLASSERT(lpszDriver != NULL);

	if (lstrlen(lpszDriver)<3)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return	false;
	}	

	TCHAR	szBoot[4]={0};
	ULARGE_INTEGER	i64FreeBytesToCaller;
	ULARGE_INTEGER	i64TotalBytes;
	ULARGE_INTEGER	i64FreeBytes;

	lstrcpyn(szBoot, lpszDriver, 3);	

	fResult = ::GetDiskFreeSpaceEx (szBoot,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	ullFreeSize = i64FreeBytes.QuadPart;

	if (!fResult)
	{   
		DWORD	dwSectPerClust=0;
		DWORD	dwBytesPerSect=0;
		DWORD	dwFreeClusters=0;
		DWORD	dwTotalClusters=0;

		fResult = ::GetDiskFreeSpace (szBoot, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);
		if (fResult) 
		{
			ullFreeSize = dwSectPerClust*dwBytesPerSect*dwFreeClusters;
		}
	}

	return fResult;
}


static BOOL IsSSD(HANDLE hDevice)
		{

			// IDE的ID命令返回的数据
			// 共512字节(256个WORD)，这里仅定义了一些感兴趣的项(基本上依据ATA/ATAPI-4)
#pragma pack(push)
#pragma pack(push, 1)
			typedef struct _tagIdentifyDevice
			{
				WORD		GeneralConfiguration;					//0
				WORD		LogicalCylinders;						//1	Obsolete
				WORD		SpecificConfiguration;					//2
				WORD		LogicalHeads;							//3 Obsolete
				WORD		Retired1[2];							//4-5
				WORD		LogicalSectors;							//6 Obsolete
				DWORD		ReservedForCompactFlash;				//7-8
				WORD		Retired2;								//9
				CHAR		SerialNumber[20];						//10-19
				WORD		Retired3;								//20
				WORD		BufferSize;								//21 Obsolete
				WORD		Obsolute4;								//22
				CHAR		FirmwareRev[8];							//23-26
				CHAR		Model[40];								//27-46
				WORD		MaxNumPerInterupt;						//47
				WORD		Reserved1;								//48
				WORD		Capabilities1;							//49
				WORD		Capabilities2;							//50
				DWORD		Obsolute5;								//51-52
				WORD		Field88and7064;							//53
				WORD		Obsolute6[5];							//54-58
				WORD		MultSectorStuff;						//59
				DWORD		TotalAddressableSectors;				//60-61
				WORD		Obsolute7;								//62
				WORD		MultiWordDma;							//63
				WORD		PioMode;								//64
				WORD		MinMultiwordDmaCycleTime;				//65
				WORD		RecommendedMultiwordDmaCycleTime;		//66
				WORD		MinPioCycleTimewoFlowCtrl;				//67
				WORD		MinPioCycleTimeWithFlowCtrl;			//68
				WORD		Reserved2[6];							//69-74
				WORD		QueueDepth;								//75
				WORD		SerialAtaCapabilities;					//76
				WORD		ReservedForFutureSerialAta;				//77
				WORD		SerialAtaFeaturesSupported;				//78
				WORD		SerialAtaFeaturesEnabled;				//79
				WORD		MajorVersion;							//80
				WORD		MinorVersion;							//81
				WORD		CommandSetSupported1;					//82
				WORD		CommandSetSupported2;					//83
				WORD		CommandSetSupported3;					//84
				WORD		CommandSetEnabled1;						//85
				WORD		CommandSetEnabled2;						//86
				WORD		CommandSetDefault;						//87
				WORD		UltraDmaMode;							//88
				WORD		TimeReqForSecurityErase;				//89
				WORD		TimeReqForEnhancedSecure;				//90
				WORD		CurrentPowerManagement;					//91
				WORD		MasterPasswordRevision;					//92
				WORD		HardwareResetResult;					//93
				WORD		AcoustricManagement;					//94
				WORD		StreamMinRequestSize;					//95
				WORD		StreamingTimeDma;						//96
				WORD		StreamingAccessLatency;					//97
				DWORD		StreamingPerformance;					//98-99
				ULONGLONG	MaxUserLba;								//100-103
				WORD		StremingTimePio;						//104
				WORD		Reserved3;								//105
				WORD		SectorSize;								//106
				WORD		InterSeekDelay;							//107
				WORD		IeeeOui;								//108
				WORD		UniqueId3;								//109
				WORD		UniqueId2;								//110
				WORD		UniqueId1;								//111
				WORD		Reserved4[4];							//112-115
				WORD		Reserved5;								//116
				DWORD		WordsPerLogicalSector;					//117-118
				WORD		Reserved6[8];							//119-126
				WORD		RemovableMediaStatus;					//127
				WORD		SecurityStatus;							//128
				WORD		VendorSpecific[31];						//129-159
				WORD		CfaPowerMode1;							//160
				WORD		ReservedForCompactFlashAssociation[7];	//161-167
				WORD		DeviceNominalFormFactor;				//168
				WORD		DataSetManagement;						//169
				WORD		AdditionalProductIdentifier[4];			//170-173
				WORD		Reserved7[2];							//174-175
				CHAR		CurrentMediaSerialNo[60];				//176-205
				WORD		SctCommandTransport;					//206
				WORD		ReservedForCeAta1[2];					//207-208
				WORD		AlignmentOfLogicalBlocks;				//209
				DWORD		WriteReadVerifySectorCountMode3;		//210-211
				DWORD		WriteReadVerifySectorCountMode2;		//212-213
				WORD		NvCacheCapabilities;					//214
				DWORD		NvCacheSizeLogicalBlocks;				//215-216
				WORD		NominalMediaRotationRate;				//217
				WORD		Reserved8;								//218
				WORD		NvCacheOptions1;						//219
				WORD		NvCacheOptions2;						//220
				WORD		Reserved9;								//221
				WORD		TransportMajorVersionNumber;			//222
				WORD		TransportMinorVersionNumber;			//223
				WORD		ReservedForCeAta2[10];					//224-233
				WORD		MinimumBlocksPerDownloadMicrocode;		//234
				WORD		MaximumBlocksPerDownloadMicrocode;		//235
				WORD		Reserved10[19];							//236-254
				WORD		IntegrityWord;							//255
			}IDENTIFY_DEVICE, *PIDENTIFY_DEVICE;
#pragma	pack(pop)

			typedef	struct _IDENTIFY_DEVICE_OUTDATA
			{
				SENDCMDOUTPARAMS	SendCmdOutParam;
				BYTE				Data[IDENTIFY_BUFFER_SIZE - 1];
			}IDENTIFY_DEVICE_OUTDATA, *PIDENTIFY_DEVICE_OUTDATA;

			DWORD dwReturnBytes = 0, dwRet = 0;

#ifndef DFP_RECEIVE_DRIVE_DATA
	#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088
#endif
			IDENTIFY_DEVICE data = {0};
			IDENTIFY_DEVICE_OUTDATA	sendCmdOutParam = {0};
			SENDCMDINPARAMS	sendCmd = {0};
			sendCmd.irDriveRegs.bCommandReg			= ID_CMD;
			sendCmd.irDriveRegs.bSectorCountReg		= 1;
			sendCmd.irDriveRegs.bSectorNumberReg	= 1;
			sendCmd.irDriveRegs.bDriveHeadReg		= 0xA0;
			sendCmd.cBufferSize						= IDENTIFY_BUFFER_SIZE;
			BOOL bRet = DeviceIoControl(
				hDevice, 
				DFP_RECEIVE_DRIVE_DATA, 
				&sendCmd, 
				sizeof(SENDCMDINPARAMS),
				&sendCmdOutParam, 
				sizeof(IDENTIFY_DEVICE_OUTDATA),
				&dwReturnBytes, 
				NULL);
			if (!bRet)
			{
				sendCmd.irDriveRegs.bDriveHeadReg = 0xB0;
				bRet = DeviceIoControl(
					hDevice, 
					DFP_RECEIVE_DRIVE_DATA, 
					&sendCmd, 
					sizeof(SENDCMDINPARAMS),
					&sendCmdOutParam, 
					sizeof(IDENTIFY_DEVICE_OUTDATA),
					&dwReturnBytes, 
					NULL);
			}
			if (bRet)
			{
				memcpy_s(&data, sizeof(IDENTIFY_DEVICE), sendCmdOutParam.SendCmdOutParam.bBuffer, sizeof(IDENTIFY_DEVICE));
				WORD nMajor = 0;

				if(data.MajorVersion == 0x0000 || data.MajorVersion == 0xFFFF)
				{
					nMajor =  0;
				}
				else{
					for(int i = 14; i > 0; i--)
					{
						if ((data.MajorVersion >> i) & 0x1)
						{
							nMajor = i;
							break;
						}
					}
				}

				if(nMajor >= 7 && data.NominalMediaRotationRate == 0x01) 
				{
					return TRUE;
				}
			}

			return FALSE;
		}

		static void AdjustString(char* str , int len)
		{
			char ch;
			int i;
			// 两两颠倒
			for (i=0;i<len;i+=2)
			{
				ch = str[i];
				str[i] = str[i+1];
				str[i+1] = ch;
			}
			// 若是右对齐的，调整为左对齐 (去掉左边的空格)
			i=0;
			while (i<len && str[i]==' ') i++;
			::memmove(str, &str[i], len-i);
			// 去掉右边的空格
			i = len - 1;
			while(i>=0 && str[i]==' ')
			{
				str[i] = '\0';
				 i--;
			}
		}
		static BOOL GetDiskModel( HANDLE hDevice,  char pModel[40])
		{
			// IDE的ID命令返回的数据
			// 共512字节(256个WORD)，这里仅定义了一些感兴趣的项(基本上依据ATA/ATAPI-4)
#pragma pack(push)
#pragma pack(push, 1)
			typedef struct _tagIdentifyDevice
			{
				WORD		GeneralConfiguration;					//0
				WORD		LogicalCylinders;						//1	Obsolete
				WORD		SpecificConfiguration;					//2
				WORD		LogicalHeads;							//3 Obsolete
				WORD		Retired1[2];							//4-5
				WORD		LogicalSectors;							//6 Obsolete
				DWORD		ReservedForCompactFlash;				//7-8
				WORD		Retired2;								//9
				CHAR		SerialNumber[20];						//10-19
				WORD		Retired3;								//20
				WORD		BufferSize;								//21 Obsolete
				WORD		Obsolute4;								//22
				CHAR		FirmwareRev[8];							//23-26
				CHAR		Model[40];								//27-46
				WORD		MaxNumPerInterupt;						//47
				WORD		Reserved1;								//48
				WORD		Capabilities1;							//49
				WORD		Capabilities2;							//50
				DWORD		Obsolute5;								//51-52
				WORD		Field88and7064;							//53
				WORD		Obsolute6[5];							//54-58
				WORD		MultSectorStuff;						//59
				DWORD		TotalAddressableSectors;				//60-61
				WORD		Obsolute7;								//62
				WORD		MultiWordDma;							//63
				WORD		PioMode;								//64
				WORD		MinMultiwordDmaCycleTime;				//65
				WORD		RecommendedMultiwordDmaCycleTime;		//66
				WORD		MinPioCycleTimewoFlowCtrl;				//67
				WORD		MinPioCycleTimeWithFlowCtrl;			//68
				WORD		Reserved2[6];							//69-74
				WORD		QueueDepth;								//75
				WORD		SerialAtaCapabilities;					//76
				WORD		ReservedForFutureSerialAta;				//77
				WORD		SerialAtaFeaturesSupported;				//78
				WORD		SerialAtaFeaturesEnabled;				//79
				WORD		MajorVersion;							//80
				WORD		MinorVersion;							//81
				WORD		CommandSetSupported1;					//82
				WORD		CommandSetSupported2;					//83
				WORD		CommandSetSupported3;					//84
				WORD		CommandSetEnabled1;						//85
				WORD		CommandSetEnabled2;						//86
				WORD		CommandSetDefault;						//87
				WORD		UltraDmaMode;							//88
				WORD		TimeReqForSecurityErase;				//89
				WORD		TimeReqForEnhancedSecure;				//90
				WORD		CurrentPowerManagement;					//91
				WORD		MasterPasswordRevision;					//92
				WORD		HardwareResetResult;					//93
				WORD		AcoustricManagement;					//94
				WORD		StreamMinRequestSize;					//95
				WORD		StreamingTimeDma;						//96
				WORD		StreamingAccessLatency;					//97
				DWORD		StreamingPerformance;					//98-99
				ULONGLONG	MaxUserLba;								//100-103
				WORD		StremingTimePio;						//104
				WORD		Reserved3;								//105
				WORD		SectorSize;								//106
				WORD		InterSeekDelay;							//107
				WORD		IeeeOui;								//108
				WORD		UniqueId3;								//109
				WORD		UniqueId2;								//110
				WORD		UniqueId1;								//111
				WORD		Reserved4[4];							//112-115
				WORD		Reserved5;								//116
				DWORD		WordsPerLogicalSector;					//117-118
				WORD		Reserved6[8];							//119-126
				WORD		RemovableMediaStatus;					//127
				WORD		SecurityStatus;							//128
				WORD		VendorSpecific[31];						//129-159
				WORD		CfaPowerMode1;							//160
				WORD		ReservedForCompactFlashAssociation[7];	//161-167
				WORD		DeviceNominalFormFactor;				//168
				WORD		DataSetManagement;						//169
				WORD		AdditionalProductIdentifier[4];			//170-173
				WORD		Reserved7[2];							//174-175
				CHAR		CurrentMediaSerialNo[60];				//176-205
				WORD		SctCommandTransport;					//206
				WORD		ReservedForCeAta1[2];					//207-208
				WORD		AlignmentOfLogicalBlocks;				//209
				DWORD		WriteReadVerifySectorCountMode3;		//210-211
				DWORD		WriteReadVerifySectorCountMode2;		//212-213
				WORD		NvCacheCapabilities;					//214
				DWORD		NvCacheSizeLogicalBlocks;				//215-216
				WORD		NominalMediaRotationRate;				//217
				WORD		Reserved8;								//218
				WORD		NvCacheOptions1;						//219
				WORD		NvCacheOptions2;						//220
				WORD		Reserved9;								//221
				WORD		TransportMajorVersionNumber;			//222
				WORD		TransportMinorVersionNumber;			//223
				WORD		ReservedForCeAta2[10];					//224-233
				WORD		MinimumBlocksPerDownloadMicrocode;		//234
				WORD		MaximumBlocksPerDownloadMicrocode;		//235
				WORD		Reserved10[19];							//236-254
				WORD		IntegrityWord;							//255
			}IDENTIFY_DEVICE, *PIDENTIFY_DEVICE;
#pragma	pack(pop)

			typedef	struct _IDENTIFY_DEVICE_OUTDATA
			{
				SENDCMDOUTPARAMS	SendCmdOutParam;
				BYTE				Data[IDENTIFY_BUFFER_SIZE - 1];
			}IDENTIFY_DEVICE_OUTDATA, *PIDENTIFY_DEVICE_OUTDATA;

			typedef struct _GETVERSIONOUTPARAMS 
			{ 
				BYTE bVersion; // Binary driver version. 
				BYTE bRevision; // Binary driver revision. 
				BYTE bReserved; // Not used. 
				BYTE bIDEDeviceMap; // Bit map of IDE devices. 
				DWORD fCapabilities; // Bit mask of driver capabilities. 
				DWORD dwReserved[4]; // For future use. 
			} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS; 

			DWORD dwReturnBytes = 0, dwRet = 0;

#ifndef DFP_RECEIVE_DRIVE_DATA
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088
#endif
			#define DFP_GET_VERSION			0x00074080 

			GETVERSIONOUTPARAMS vers;
			DWORD		bytesRtn = 0;
			ZeroMemory(&vers, sizeof(vers)); 

			if (!DeviceIoControl(hDevice, DFP_GET_VERSION, 0, 0, &vers, sizeof(vers), &bytesRtn,0))
			{ 
				return FALSE;
			} 

			if (!(vers.fCapabilities&1))
			{ 
				return FALSE;
			} 


			IDENTIFY_DEVICE data = {0};
			IDENTIFY_DEVICE_OUTDATA	sendCmdOutParam = {0};
			SENDCMDINPARAMS		sendCmd = { 0 };
			DWORD dwDiskNum = 0;
			GetDiskDeviceNumber(hDevice,&dwDiskNum);
			if (dwDiskNum & 1)
			{ 
				sendCmd.irDriveRegs.bDriveHeadReg = 0xb0; 
			}
			else
			{ 
				sendCmd.irDriveRegs.bDriveHeadReg = 0xa0; 
			} 

			if (vers.fCapabilities&(16>>dwDiskNum))
			{ 
				//We don't detect a ATAPI device. 
				return FALSE;
			}
			else
			{ 
				sendCmd.irDriveRegs.bCommandReg = 0xec; 
			} 
			sendCmd.bDriveNumber = dwDiskNum; 
			sendCmd.irDriveRegs.bSectorCountReg  = 1; 
			sendCmd.irDriveRegs.bSectorNumberReg = 1; 
			sendCmd.cBufferSize = 512; 

			BOOL bRet = DeviceIoControl(
				hDevice, 
				DFP_RECEIVE_DRIVE_DATA, 
				&sendCmd, 
				sizeof(SENDCMDINPARAMS),
				&sendCmdOutParam, 
				sizeof(IDENTIFY_DEVICE_OUTDATA),
				&dwReturnBytes, 
				NULL);
			if (!bRet)
			{
				sendCmd.irDriveRegs.bDriveHeadReg = 0xB0;
				bRet = DeviceIoControl(
					hDevice, 
					DFP_RECEIVE_DRIVE_DATA, 
					&sendCmd, 
					sizeof(SENDCMDINPARAMS),
					&sendCmdOutParam, 
					sizeof(IDENTIFY_DEVICE_OUTDATA),
					&dwReturnBytes, 
					NULL);
			}
			if (bRet)
			{
				memcpy_s(&data, sizeof(IDENTIFY_DEVICE), sendCmdOutParam.SendCmdOutParam.bBuffer, sizeof(IDENTIFY_DEVICE));
				memcpy_s( pModel, 40, data.Model, 40);

				AdjustString(pModel, 40);
			}

			return TRUE;
		}


		static std::wstring GetDiskSerialNumber(HANDLE hPhysical)
		{
			std::wstring strRet;

			if (hPhysical == INVALID_HANDLE_VALUE){
				return strRet;
			}

			DWORD dwRet;
			BYTE pBuffer[8192] = { 0 };
			STORAGE_PROPERTY_QUERY spq;
			memset(&spq, 0, sizeof(STORAGE_PROPERTY_QUERY));
			spq.PropertyId = StorageDeviceProperty;
			spq.QueryType = PropertyStandardQuery;

			if (::DeviceIoControl(hPhysical,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&spq, sizeof(STORAGE_PROPERTY_QUERY),
				pBuffer, sizeof(pBuffer),
				&dwRet, NULL))
			{
				DWORD dwStart = ((PSTORAGE_DEVICE_DESCRIPTOR)pBuffer)->SerialNumberOffset;
				char* szTmp = ((char*)pBuffer)+dwStart;
				strRet = CA2T(szTmp);
			}
			

			return strRet;
		}
};};//namespace msdk
