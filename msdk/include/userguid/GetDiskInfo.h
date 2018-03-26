#pragma once
#ifndef _MSGETDISKINFO_H
#define _MSGETDISKINFO_H


#include <vector>
#include <winioctl.h>


#define  MAX_IDE_DRIVES  16

//  IOCTL commands
#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition


#define SMART_GET_VERSION               CTL_CODE(IOCTL_DISK_BASE, 0x0020, METHOD_BUFFERED, FILE_READ_ACCESS)
#define SMART_SEND_DRIVE_COMMAND        CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

#pragma  pack(1)
//  GETVERSIONOUTPARAMS contains the data returned from the 
//  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

// The following struct defines the interesting part of the IDENTIFY
// buffer:
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

//
// IDENTIFY data (from ATAPI driver source)
//
typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid:1;        // 6A  53
	USHORT Reserved3:15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;
#pragma pack()

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE


typedef struct _rt_IdeDInfo_
{
	BYTE IDEExists[4];
	BYTE DiskExists[8];
	WORD DisksRawInfo[8*256];
} rt_IdeDInfo, *pt_IdeDInfo;


// (* IdeDinfo "data fields" *)
typedef struct _rt_DiskInfo_
{
	BOOL DiskExists;
	BOOL ATAdevice;
	BOOL RemovableDevice;
	WORD TotLogCyl;
	WORD TotLogHeads;
	WORD TotLogSPT;
	char SerialNumber[20];
	char FirmwareRevision[8];
	char ModelNumber[40];
	WORD CurLogCyl;
	WORD CurLogHeads;
	WORD CurLogSPT;
} rt_DiskInfo;


#define  m_cVxDFunctionIdesDInfo  1

#define	 RSDISK_SIZE	50+1

using namespace std;

namespace msdk {
#pragma warning( push)
#pragma warning( disable:4996 )
	class CGetDiskInfo
	{
	public:
		~CGetDiskInfo()
		{
			CleanList();
		}

		static bool GetDiskInfo(char diskInfo[RSDISK_SIZE])
		{
			CGetDiskInfo tag;
			return tag.NewInfo(diskInfo);
		}


	protected:
		BOOL ReadPhysicalDriveInNT()
		{
			BOOL done = FALSE;
			int drive = 0;
			BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
			for (drive = 0; drive < MAX_IDE_DRIVES && !IsHaveOneInfo(); drive++)
			{
				HANDLE hPhysicalDriveIOCTL = 0;
				CHAR driveName [256];
				sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

				hPhysicalDriveIOCTL = CreateFileA (driveName,
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					OPEN_EXISTING, 0, NULL);

				//在SSD下获取的ID有可能会重复，当计算机中有多块硬盘的时候取非SSD的第一块硬盘，如果只有一块SSD硬盘，则获取ID
				if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE /*&& !IsSSD(hPhysicalDriveIOCTL)*/)
				{
					GETVERSIONOUTPARAMS VersionParams;
					DWORD               cbBytesReturned = 0;

					// Get the version, etc of PhysicalDrive IOCTL
					memset ((void*) &VersionParams, 0, sizeof(VersionParams));
					if (  DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
						NULL, 
						0,
						&VersionParams,
						sizeof(VersionParams),
						&cbBytesReturned, NULL) )
					{         
						// If there is a IDE device at number "i" issue commands
						// to the device
						if (VersionParams.bIDEDeviceMap > 0)
						{
							BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
							SENDCMDINPARAMS  scip;

							// Now, get the ID sector for all IDE devices in the system.
							// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
							// otherwise use the IDE_ATA_IDENTIFY command
							bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

							memset (&scip, 0, sizeof(scip));
							memset (IdOutCmd, 0, sizeof(IdOutCmd));

							if ( DoIdentify (hPhysicalDriveIOCTL, 
								&scip, 
								(PSENDCMDOUTPARAMS)&IdOutCmd, 
								(BYTE) bIDCmd,
								(BYTE) drive,
								&cbBytesReturned))
							{
								USHORT *pIdSector = (USHORT *)((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
								AddIfNew(pIdSector);	  
								done = TRUE;
							}
						}
					}

					CloseHandle (hPhysicalDriveIOCTL);
				}
			}

			return done;
		}

		BOOL ReadIdeDriveAsScsiDriveInNT()
		{
			BOOL done = FALSE;
			int controller = 0;
			for (controller = 0; controller < MAX_IDE_DRIVES && !IsHaveOneInfo(); controller++)
			{
				HANDLE hScsiDriveIOCTL = 0;
				CHAR   driveName [256];

				//  Try to get a handle to PhysicalDrive IOCTL, report failure
				//  and exit if can't.
				sprintf (driveName, "\\\\.\\Scsi%d:", controller);

				//  Windows NT, Windows 2000, any rights should do
				hScsiDriveIOCTL = CreateFileA (driveName,
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					OPEN_EXISTING, 0, NULL);


				if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
				{
					int drive = 0;

					for (drive = 0; drive < 2; drive++)
					{
						char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
						SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
						SENDCMDINPARAMS *pin =
							(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
						DWORD dummy;

						memset (buffer, 0, sizeof (buffer));
						p -> HeaderLength = sizeof (SRB_IO_CONTROL);
						p -> Timeout = 10000;
						p -> Length = SENDIDLENGTH;
						p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
						strncpy ((char *) p -> Signature, "SCSIDISK", 8);

						pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
						pin -> bDriveNumber = drive;

						if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
							buffer,
							sizeof (SRB_IO_CONTROL) +
							sizeof (SENDCMDINPARAMS) - 1,
							buffer,
							sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
							&dummy, NULL))
						{
							SENDCMDOUTPARAMS *pOut =
								(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
							IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
							if (pId -> sModelNumber [0])
							{
								USHORT *pIdSector = (USHORT *) pId;
								AddIfNew(pIdSector);	
								done = TRUE;
							}
						}
					}
					CloseHandle (hScsiDriveIOCTL);
				}
			}

			return done;
		}

		BOOL ReadPhysicalDriveInNTUsingSmart()
		{
			BOOL done = FALSE;
			int drive = 0;

			for (drive = 0; drive < MAX_IDE_DRIVES && !IsHaveOneInfo(); drive++)
			{
				HANDLE hPhysicalDriveIOCTL = 0;

				//  Try to get a handle to PhysicalDrive IOCTL, report failure
				//  and exit if can't.

				CHAR driveName [256];
				sprintf  (driveName, "\\\\.\\PhysicalDrive%d", drive);

				//  Windows NT, Windows 2000, Windows Server 2003, Vista
				hPhysicalDriveIOCTL = CreateFileA (driveName,
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
					NULL, OPEN_EXISTING, 0, NULL);

				if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
				{
					GETVERSIONINPARAMS GetVersionParams;
					DWORD cbBytesReturned = 0;

					// Get the version, etc of PhysicalDrive IOCTL
					memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));

					if (  DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
						NULL, 
						0,
						&GetVersionParams, sizeof (GETVERSIONINPARAMS),
						&cbBytesReturned, NULL) )
					{         
						// Allocate the command buffer
						ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
						PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) malloc (CommandSize);
						// Retrieve the IDENTIFY data
						// Prepare the command

						Command -> irDriveRegs.bCommandReg = ID_CMD;
						DWORD BytesReturned = 0;
						if ( DeviceIoControl (hPhysicalDriveIOCTL, 
							SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
							Command, CommandSize,
							&BytesReturned, NULL) )

						{
							// Print the IDENTIFY data
							USHORT *pIdSector = (USHORT *)
								(PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;


							AddIfNew(pIdSector);

							done = TRUE;
						}
						// Done
						CloseHandle (hPhysicalDriveIOCTL);
						free (Command);
					}
				}
			}
			return done;
		}

	private:
		typedef vector< WORD* > LSTDATA;
		LSTDATA m_list;

		bool NewInfo(char diskInfo[RSDISK_SIZE])
		{
			CleanList();
			
			if(!IsHaveOneInfo()) //只要一个就够了
				ReadPhysicalDriveInNTUsingSmart();


			if(!IsHaveOneInfo()) //只要一个就够了
				ReadPhysicalDriveInNT();

			if(!IsHaveOneInfo()) //只要一个就够了
				ReadIdeDriveAsScsiDriveInNT();

			

			if( m_list.size() <= 0)
				return false;

			BYTE dwBufSize = RSDISK_SIZE;
			return SUCCEEDED(ConvertToString(m_list[0],diskInfo, dwBufSize, 10,19));
		}

		void CleanList()
		{
			for(UINT i = 0; i< m_list.size(); i++)
				delete m_list[i];
			m_list.clear();
		}

		bool IsHaveOneInfo()
		{
			return m_list.size() > 0;
		}

		bool AddIfNew(USHORT *pIdSector)
		{
			bool bAdd = true;
			for(UINT i =0; i< m_list.size();i++)
			{
				if(memcmp(pIdSector,m_list[i],256 * sizeof(WORD)) == 0)
				{
					bAdd = false;
					break;
				}
			}
			if(bAdd)
			{
				WORD* diskdata = new WORD[256];
				::memcpy(diskdata,pIdSector,256*sizeof(WORD));
				m_list.push_back(diskdata);
			}
			return bAdd;
		}

		bool DoIdentify (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
			PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
			PDWORD lpcbBytesReturned)
		{
			// Set up data structures for IDENTIFY command.
			pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
			pSCIP -> irDriveRegs.bFeaturesReg = 0;
			pSCIP -> irDriveRegs.bSectorCountReg = 1;
			pSCIP -> irDriveRegs.bSectorNumberReg = 1;
			pSCIP -> irDriveRegs.bCylLowReg = 0;
			pSCIP -> irDriveRegs.bCylHighReg = 0;

			// Compute the drive number.
			pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

			// The command can either be IDE identify or ATAPI identify.
			pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
			pSCIP -> bDriveNumber = bDriveNum;
			pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

			return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
				(LPVOID) pSCIP,
				sizeof(SENDCMDINPARAMS) - 1,
				(LPVOID) pSCOP,
				sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
				lpcbBytesReturned, NULL) )?true:false;
		}

		HRESULT  ConvertToString(WORD dwDiskData[256], CHAR* pszString ,BYTE &dwBufSize,int nFirstIndex, int nLastIndex)
		{
			char szResBuf[256];
			char ss[256];
			int nIndex = 0;
			int nPosition = 0;
			for(nIndex = nFirstIndex; nIndex <= nLastIndex; nIndex++)
			{
				ss[nPosition] = (char)(dwDiskData[nIndex] / 256);
				nPosition++;

				// Get low BYTE for 2nd character
				ss[nPosition] = (char)(dwDiskData[nIndex] % 256);
				nPosition++;
			}

			// End the string
			ss[nPosition] = '\0';

			int i, index=0;
			for(i=0; i<nPosition; i++)
			{
				if(ss[i]==0 || ss[i]==32)	continue;
				szResBuf[index]=ss[i];
				index++;
			}
			szResBuf[index]=0;
			if((dwBufSize<=index)||(pszString==NULL))
			{
				dwBufSize=index+1;
				return E_OUTOFMEMORY;
			}
			strcpy(pszString,szResBuf);
			dwBufSize=index;
			return S_OK;
		}


		
	};

#pragma warning( pop )
} //namespace msdk

#endif //_MSGETDISKINFO_H