#pragma once
#include "stdafx.h"
//
#include "msapi/mswinapi.h"
#include "Winternl.h"
#include "Psapi.h"
#include <tchar.h>

#include <winternl.h> 
#include <ntstatus.h>
#pragma comment(lib,"Psapi.lib")


BOOL GetDiskDeviceNumber(TCHAR letter)
{
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
		return -1;
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
	CloseHandle(deviceHandle);
	if (!bRet){
		return -1;
	}

	return sdn.DeviceNumber;
}


BOOL IsUsbDiskDevice(TCHAR letter)
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

//判断操作系统是否为64位
BOOL IsWindowsX64()
{
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	SYSTEM_INFO si = {0};
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	PGNSI pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGNSI)
		pGNSI(&si);
	else 
		GetSystemInfo(&si); 

	if (PROCESSOR_ARCHITECTURE_IA64 == si.wProcessorArchitecture ||  
		PROCESSOR_ARCHITECTURE_AMD64 == si.wProcessorArchitecture)
	{
		return TRUE;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//通过进程句柄获取进程PID
//失败返回0
DWORD MsGetProcessId(HANDLE hHandle)
{
	RASSERT(hHandle , 0);

	HMODULE hModule = 	LoadLibrary(_T("Ntdll.dll"));
	RASSERT(hModule , 0);

	typedef NTSTATUS  (WINAPI * pNtQueryInformationProcess)(
		__in       HANDLE ProcessHandle,
		__in       PROCESSINFOCLASS ProcessInformationClass,
		__out      PVOID ProcessInformation,
		__in       ULONG ProcessInformationLength,
		__out_opt  PULONG ReturnLength
		);

	pNtQueryInformationProcess pRsGetProcessId = NULL;
	pRsGetProcessId = (pNtQueryInformationProcess)GetProcAddress(hModule , "NtQueryInformationProcess");
	RASSERT(pRsGetProcessId , 0);

	PROCESS_BASIC_INFORMATION processInfo = {0};
	ULONG lReturn = 0;
	pRsGetProcessId(hHandle , ProcessBasicInformation , &processInfo , sizeof(processInfo) , &lReturn);

	FreeLibrary(hModule);
	return processInfo.UniqueProcessId;
}


//////////////////////////////////////////////////////////////////////////
//通过进程PID获取用户名
//dwPid: 进程PID
//lpUserName: 获取到的用户名称
//dwLen: lpUserName 缓存区长度
//return: domin\username
BOOL GetUserNameByPid(IN DWORD dwPid , OUT LPTSTR lpUserName , IN DWORD dwLen)
{
	if (!lpUserName)
		return FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return FALSE;

	HANDLE hTok = NULL;
	PTOKEN_USER ptu = NULL;
	if(OpenProcessToken( hProcess, TOKEN_QUERY, &hTok ) )
	{
		DWORD dwNeeded = 0;
		GetTokenInformation( hTok, TokenUser, NULL, 0, &dwNeeded );
		ptu = (PTOKEN_USER)LocalAlloc( LPTR, dwNeeded );

		if (!ptu)
		{
			if (hTok)
				CloseHandle(hTok);
			if (hProcess)
				CloseHandle(hProcess);
			return FALSE;
		}

		if (!GetTokenInformation( hTok, TokenUser, ptu, dwNeeded, &dwNeeded ))
		{
			if (ptu)
				LocalFree(ptu);
			if (hTok)
				CloseHandle(hTok);
			if (hProcess)
				CloseHandle(hProcess);

			return FALSE;
		}

		PSID psid = ptu->User.Sid;
		TCHAR szUser[MAX_PATH] = {0};
		TCHAR szDomain[MAX_PATH] = {0};

		DWORD cchUser = sizeof( szUser ), cchDomain = sizeof( szDomain );

		SID_NAME_USE use;
		BOOL bRes = LookupAccountSid( NULL, psid, szUser, &cchUser, szDomain, &cchDomain, &use );
		if( bRes )
		{
			_stprintf_s(lpUserName , dwLen , _T("%s\\%s") , szUser , szDomain);
		}

		if (ptu) //彻底失败
			LocalFree(ptu);
		if (hTok)
			CloseHandle(hTok);
		if (hProcess)
			CloseHandle(hProcess);

		return bRes;
	}

	if (hTok)
		CloseHandle(hTok);
	if (hProcess)
		CloseHandle(hProcess);
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//通过进程ID获取进程名称
BOOL GetProcessNameByPid(IN DWORD dwPid , OUT LPTSTR lpName , IN DWORD dwLen)
{
	if (!lpName)
		return FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return FALSE;

	BOOL bRet = FALSE;

	DWORD cbNeeded = 0;
	HMODULE hModule = 0;
	if(EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded) &&
		GetModuleFileNameEx(hProcess, hModule, lpName, dwLen * sizeof(TCHAR)))
	{
		bRet = TRUE;
	}

	if (hProcess)
	{
		CloseHandle(hProcess);
	}

	return bRet;
}


/*
功能：获取PE文件的大小
返回：-1失败
*/

#ifndef PE_BUF_SIZE
#	define PE_BUF_SIZE 1024*1024
#endif

static BYTE	BUFFER[PE_BUF_SIZE];

LONG GetPESize(HANDLE hFile)
{
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	DWORD dwReadBytes;
	if(FALSE == ReadFile(hFile, BUFFER, PE_BUF_SIZE, &dwReadBytes, NULL))//Read file error
		return -1;

	PIMAGE_DOS_HEADER	pImageDosHeader = (PIMAGE_DOS_HEADER)BUFFER;
	if(IMAGE_DOS_SIGNATURE != pImageDosHeader->e_magic)
		return -1;

	DWORD dwPEHead = *(LPDWORD)(BUFFER + 0x3c);

	if(IMAGE_NT_SIGNATURE != *(LPDWORD)(BUFFER + dwPEHead))
		return -1;

	PIMAGE_FILE_HEADER			pFileHeader		= (PIMAGE_FILE_HEADER)(BUFFER + dwPEHead + 4);
	PIMAGE_OPTIONAL_HEADER32	pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)(pFileHeader + 1);
	PIMAGE_SECTION_HEADER		pSectionHeader	= (PIMAGE_SECTION_HEADER)(pOptionalHeader + 1);

	DWORD	dwFileSize = 0;
	for(short i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		DWORD dwSectionSize = pSectionHeader->PointerToRawData + pSectionHeader->SizeOfRawData;
		if(dwSectionSize > dwFileSize)	dwFileSize = dwSectionSize;

		pSectionHeader++;
	}

	return dwFileSize;
}

//获取父进程PID，失败返回 -1
//单独引用需要包含 


#ifndef ProcessBasicInformation
#	define ProcessBasicInformation 0
#endif

typedef LONG (__stdcall *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
DWORD GetParentProcessID(DWORD dwProcessId)
{
	NTSTATUS 					status		=	STATUS_SUCCESS;
	DWORD						dwParentPID =	DWORD(-1);
	HANDLE						hProcess	=	NULL;
	PROCESS_BASIC_INFORMATION	pbi			=	{ 0 };

	PROCNTQSIP NtQueryInformationProcess	= (PROCNTQSIP)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationProcess"); 
	if (!NtQueryInformationProcess)
		return DWORD(-1);


	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE, dwProcessId);
	if (!hProcess)
		return DWORD(-1);

	status = NtQueryInformationProcess( hProcess, ProcessBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	if(status == STATUS_SUCCESS)
		dwParentPID = DWORD(pbi.Reserved3);

	CloseHandle (hProcess);

	return dwParentPID;
}


FARPROC GetProcAddressEx(  HMODULE hModule,  LPCSTR lpProcName)
{
	if (!(hModule && lpProcName))
		return NULL;

	PIMAGE_DOS_HEADER           pDosHeard = NULL;
	PIMAGE_NT_HEADERS32         pNtHeard32 = NULL;
	PIMAGE_NT_HEADERS64         pNtHeard64 = NULL;
	PIMAGE_DATA_DIRECTORY       pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY     pExportBlack = NULL;

	unsigned short             *pAddressOfNameOrdinals = NULL;
	unsigned long              *pAddressOfNames = NULL;
	unsigned long              *pAddressOfFunctions = NULL;
	unsigned int                i = 0;


	ULONG                      Num = 0;
	ULONG                      Funstart = 0;
	ULONG                      FunEnd = 0;



	pDosHeard = (PIMAGE_DOS_HEADER)hModule;
	pNtHeard32 = (PIMAGE_NT_HEADERS32)(pDosHeard->e_lfanew + (UCHAR *)pDosHeard);

	if (pDosHeard->e_magic != IMAGE_DOS_SIGNATURE || pNtHeard32->Signature != IMAGE_NT_SIGNATURE)
	{
		return NULL;
	}


	switch (pNtHeard32->OptionalHeader.Magic)
	{
	case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
		{
			pDataDirectory = &pNtHeard32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		}
		break;
	case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
		{
			pNtHeard64 = (PIMAGE_NT_HEADERS64)pNtHeard32;
			pDataDirectory = &pNtHeard64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		}
		break;
	default:
		{
			return NULL;
		}
		break;
	}

	pExportBlack = (PIMAGE_EXPORT_DIRECTORY)((UCHAR *)hModule + pDataDirectory->VirtualAddress);
	if (!pExportBlack || !pDataDirectory->Size)
	{
		return NULL;
	}


	pAddressOfNameOrdinals = (unsigned short *)((UCHAR*)hModule + pExportBlack->AddressOfNameOrdinals);
	pAddressOfNames = (unsigned long *)((char *)hModule + pExportBlack->AddressOfNames);
	pAddressOfFunctions = (unsigned long *)((UCHAR*)hModule + pExportBlack->AddressOfFunctions);


	for (i = 0; i < pExportBlack->NumberOfNames; i++)
	{
		LPCSTR lpszAddrName = (LPCSTR)hModule + *pAddressOfNames;

		if (strcmp(lpszAddrName, lpProcName) == 0)
		{
			Num = *pAddressOfFunctions;
			Funstart = pDataDirectory->VirtualAddress;
			FunEnd = pDataDirectory->VirtualAddress + pDataDirectory->Size;
			return (FARPROC)(*(pAddressOfFunctions + *pAddressOfNameOrdinals) + (ULONG_PTR)hModule);
		}
		pAddressOfNames++;
		pAddressOfNameOrdinals++;
	}

	return NULL;
}


BOOL Is64PeFile(LPCTSTR lpwsFile)
{
	BOOL bRtn = FALSE;
	HANDLE hFile = CreateFile(lpwsFile, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{		
		DWORD dwReaded = 0;
		IMAGE_DOS_HEADER dosHeader = {0};
		if(ReadFile(hFile, &dosHeader, sizeof(dosHeader), &dwReaded, NULL))
		{
			if(dwReaded == sizeof(dosHeader))
			{
				if(dosHeader.e_magic == IMAGE_DOS_SIGNATURE)
				{
					LONG lOffset = dosHeader.e_lfanew;
					lOffset += 4;//定位到IMAGE_FILE_HEADER ??? 不考虑特殊文件
					if(lOffset == SetFilePointer(hFile, lOffset, NULL, FILE_BEGIN))
					{
						IMAGE_FILE_HEADER ifHeader = {0};
						if(ReadFile(hFile, &ifHeader, sizeof(IMAGE_FILE_HEADER), &dwReaded, NULL))
						{
							if(dwReaded == sizeof(IMAGE_FILE_HEADER))
							{
								bRtn = (IMAGE_FILE_MACHINE_AMD64 == ifHeader.Machine
									|| IMAGE_FILE_MACHINE_IA64 == ifHeader.Machine);
							}
						}
					}					
				}
			}
		}
		CloseHandle(hFile);
	}
	return bRtn;
}

BOOL AddEveryoneAccessToFilePath(LPTSTR szFileName)
{
	BOOL bResult = FALSE;
	PACL pOldDacl=NULL;
	PACL pNewDacl=NULL;
	DWORD dRet;
	EXPLICIT_ACCESS eia;
	PSECURITY_DESCRIPTOR pSID=NULL;
	dRet = GetNamedSecurityInfo(szFileName,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,&pOldDacl,NULL,&pSID);// 获取文件的DACL 
	if(dRet==ERROR_SUCCESS)
	{
		//创建一个ACE,允许Everyone组成员完全控制对象,并允许子对象继承此权限
		ZeroMemory(&eia,sizeof(EXPLICIT_ACCESS));
		BuildExplicitAccessWithName(&eia,_T("Everyone"),GENERIC_ALL|WRITE_DAC|GENERIC_EXECUTE|GENERIC_WRITE|GENERIC_READ,SET_ACCESS,SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		// 将新的ACE加入DACL 
		dRet = SetEntriesInAcl(1,&eia,pOldDacl,&pNewDacl);
		if(dRet==ERROR_SUCCESS)
		{
			// 更新文件的DACL 
			dRet = SetNamedSecurityInfo(szFileName,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,pNewDacl,NULL);
			if(dRet==ERROR_SUCCESS)
			{
				bResult=TRUE;
			}
		}
	}

	//释放DACL和SID
	if(pNewDacl)LocalFree(pNewDacl);
	if(pSID)LocalFree(pSID);
	return bResult;
}

BOOL DeletePathFile(LPCTSTR szPath)
{
	if( szPath && PathFileExists(szPath) )
	{
		AddEveryoneAccessToFilePath((LPTSTR)(LPCTSTR)szPath);

		TCHAR szBuffer[1024] = {0};
		_tcsncpy(szBuffer, szPath, _tcslen(szPath));

		SHFILEOPSTRUCT shfos = { 0 };
		shfos.hwnd = NULL;
		shfos.wFunc = FO_DELETE;
		shfos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
		shfos.pFrom = szBuffer;
		shfos.pTo = NULL;
		shfos.lpszProgressTitle = NULL;
		int nRet = SHFileOperation(&shfos);	
		return ( 0 == nRet)?TRUE:FALSE;
	}
	return FALSE;
}