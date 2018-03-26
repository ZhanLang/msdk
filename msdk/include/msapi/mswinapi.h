#pragma once

#include "Psapi.h"

#include <Aclapi.h>
#include <accctrl.h>
#include <WinIoCtl.h>
#include <winternl.h> 
#include <ntstatus.h>
#include <Tlhelp32.h>
#include <shellapi.h>
#include <shlobj.h>
#include "mspath.h"


#pragma comment(lib,"Psapi.lib")
namespace msdk{;
namespace msapi{;




//判断操作系统是否为64位
static BOOL IsWindowsX64()
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

//判断操作系统是否为VISTA之后的版本
static BOOL IsWindowVistaLatter()
{
	OSVERSIONINFO  m_osver;
	m_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&m_osver);
	return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 1 ) );
}

#include "VersionHelpers.h"
enum MicroVersion
{
	MicroVersion_Unknown		= -1,
	MicroVersion_Lower			= 4,
	MicroVersion_WinXP			= 5,
	MicroVersion_WinXP_SP1		= 6,
	MicroVersion_WinXP_SP2		= 7,
	MicroVersion_WinXP_SP3		= 8,
	MicroVersion_Vista			= 9,
	MicroVersion_Vista_SP1		= 10,
	MicroVersion_Vista_SP2		= 11,
	MicroVersion_Win7			= 12,
	MicroVersion_Win7_SP1		= 13,
	MicroVersion_Win8			= 14,
	MicroVersion_Win8_1			= 15,
	MicroVersion_Win10			= 16,
	MicroVersion_Advanced		= 100,
};

static int GetMicroVersion()
{
	static MicroVersion microVerion = MicroVersion_Unknown;
	if ( microVerion != MicroVersion_Unknown) return microVerion;

	if(	IsWindows10OrGreater()			 )			microVerion = MicroVersion_Win10;
	else if( IsWindows8Point1OrGreater() )			microVerion = MicroVersion_Win8_1;
	else if( IsWindows8OrGreater()		 )			microVerion = MicroVersion_Win8;
	else if( IsWindows7SP1OrGreater()	 )			microVerion = MicroVersion_Win7_SP1;
	else if( IsWindows7OrGreater()		 )			microVerion = MicroVersion_Win7;
	else if( IsWindowsVistaSP2OrGreater())			microVerion = MicroVersion_Vista_SP2;
	else if( IsWindowsVistaSP1OrGreater())			microVerion = MicroVersion_Vista_SP1;
	else if( IsWindowsVistaOrGreater()	 )			microVerion = MicroVersion_Vista;
	else if( IsWindowsXPSP3OrGreater()	 )			microVerion = MicroVersion_WinXP_SP3;
	else if( IsWindowsXPSP2OrGreater()	 )			microVerion = MicroVersion_WinXP_SP2;
	else if( IsWindowsXPSP1OrGreater()	 )			microVerion = MicroVersion_WinXP_SP1;
	else if( IsWindowsXPOrGreater()		 )			microVerion = MicroVersion_WinXP;
	else microVerion = MicroVersion_Advanced;
	return microVerion;
}

static BOOL GetSystemVersion( DWORD &dwMajorVersion,DWORD &dwMinorVersion )
{
	OSVERSIONINFO osvi; 
	memset(&osvi, 0, sizeof(OSVERSIONINFO)); 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
	GetVersionEx(&osvi); 
	dwMajorVersion = osvi.dwMajorVersion;
	dwMinorVersion = osvi.dwMinorVersion;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//通过进程句柄获取进程PID
//失败返回0
static DWORD GetProcessIdByHandle(HANDLE hHandle)
{
	if ( !hHandle )
		return 0;

	HMODULE hModule = 	LoadLibrary(_T("Ntdll.dll"));
	if( !hModule )
		return 0;

	typedef LONG  (WINAPI * pNtQueryInformationProcess)(
		__in       HANDLE ProcessHandle,
		__in       PROCESSINFOCLASS ProcessInformationClass,
		__out      PVOID ProcessInformation,
		__in       ULONG ProcessInformationLength,
		__out_opt  PULONG ReturnLength
		);

	pNtQueryInformationProcess pRsGetProcessId = NULL;
	pRsGetProcessId = (pNtQueryInformationProcess)GetProcAddress(hModule , "NtQueryInformationProcess");
	if( !pRsGetProcessId )
		return 0;

	PROCESS_BASIC_INFORMATION processInfo = {0};
	ULONG lReturn = 0;
	pRsGetProcessId(hHandle , ProcessBasicInformation , &processInfo , sizeof(processInfo) , &lReturn);

	FreeLibrary(hModule);
	return (DWORD)processInfo.UniqueProcessId;
}


//////////////////////////////////////////////////////////////////////////
//通过进程PID获取用户名
//dwPid: 进程PID
//lpUserName: 获取到的用户名称
//dwLen: lpUserName 缓存区长度
//return: domin\username
static BOOL GetUserNameByPid(IN DWORD dwPid , OUT LPTSTR lpUserName , IN DWORD dwLen)
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
static BOOL GetProcessNameByPid(IN DWORD dwPid , OUT LPTSTR lpName , IN DWORD dwLen)
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

static LONG GetPESize(HANDLE hFile)
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
static DWORD GetParentProcessID(DWORD dwProcessId)
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


static FARPROC GetProcAddressEx(  HMODULE hModule,  LPCSTR lpProcName)
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


static BOOL Is64PeFile(LPCTSTR lpwsFile)
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


//文件夹降权
static BOOL AddEveryoneAccessToFilePath(LPTSTR szFileName)
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


//修改注册表权限
//【MACHINE】对应【HKEY_LOCAL_MACHINE】
//【CLASSES_ROOT】 对应【HKEY_CLASSES_ROOT】
static DWORD SetRegPrivilege(LPTSTR lpszKey) 
{ 
	DWORD dwRet;

	LPTSTR SamName				= lpszKey; 
	PSECURITY_DESCRIPTOR pSD	= NULL; 
	PACL pOldDacl				= NULL; 
	PACL pNewDacl				= NULL; 
	HKEY hKey					= NULL;
	EXPLICIT_ACCESS ea			= {0}; 

	do 
	{
		// 获取SAM主键的DACL 
		dwRet = GetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDacl, NULL, &pSD);
		if (dwRet != ERROR_SUCCESS)
		{
			break;
		}

		// 创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限 
		BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

		// 将新的ACE加入DACL 
		dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl); 
		if (dwRet != ERROR_SUCCESS)
		{
			break;
		}

		// 更新SAM主键的DACL 
		dwRet = SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL); 
		if (dwRet != ERROR_SUCCESS) 
		{ 
			break;
		}



	} while (FALSE);

	
	if (pNewDacl) 
		LocalFree(pNewDacl); 
	

	/* 还原注册表权限
	if (pOldDacl) 
		SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pOldDacl, NULL); 
	if (pSD) LocalFree(pSD); 
	*/
	return dwRet; 
}

static BOOL DeletePathFile(LPCTSTR szPath)
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


//导出资源文件到磁盘文件中
static BOOL ExportResourceToFile(HINSTANCE hRes, DWORD dwID, LPCTSTR lpszType, LPCTSTR lpszSave)
{
	HRSRC hResource = ::FindResource(hRes, MAKEINTRESOURCE(dwID), lpszType);
	if (!hResource)
		return FALSE;

	DWORD	dwSize  = ::SizeofResource(hRes, hResource);
	HGLOBAL hGlobal = ::LoadResource(hRes, hResource);

	if( !(hGlobal != NULL && dwSize) ) 
	{
		::FreeResource(hResource);
		return FALSE;
	}

	msapi::CreateDirectoryEx(lpszSave);
	HANDLE hSaveFile = CreateFile(lpszSave, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( INVALID_HANDLE_VALUE == hSaveFile)
	{
		::FreeResource(hResource);
		return FALSE;
	}

	DWORD dwWriteSize = 0;
	BOOL bRet = WriteFile(hSaveFile, ::LockResource(hGlobal), dwSize, &dwWriteSize, NULL);

	::FreeResource(hResource);
	CloseHandle(hSaveFile);

	return (bRet && dwWriteSize == dwSize);
}



#ifndef DEFULT_IO_OPERATION_BYTES
#	define DEFULT_IO_OPERATION_BYTES	1024
#endif

static DWORD Execute(LPCTSTR appfile, LPCTSTR commandline, BOOL waitforexit, BOOL showconcle, INT waittime)
{
	TCHAR _errinfo[DEFULT_IO_OPERATION_BYTES * 2 + 1] = {0};

	_errinfo[0] = NULL;
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	sa.bInheritHandle		= TRUE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength				= sizeof(sa);

	HANDLE hread  = NULL,hwrite = NULL,herrin = NULL,herrout= NULL;

	DWORD  result = -1;

	do {
		if (waitforexit
			&& ::CreatePipe(&hread, &hwrite, &sa,	DEFULT_IO_OPERATION_BYTES * 200)
			&& ::CreatePipe(&herrin, &herrout, &sa, DEFULT_IO_OPERATION_BYTES * 200)
			&& ::SetHandleInformation(herrin, HANDLE_FLAG_INHERIT, 0)
			)
		{
			si.dwFlags	  = STARTF_USESTDHANDLES;
			si.hStdError  = herrout;
			si.hStdOutput = hwrite;
			si.hStdInput  = hread;
		}

		DWORD createflag = ::GetPriorityClass(::GetCurrentProcess());
		if (!showconcle) createflag |= CREATE_NO_WINDOW;
		BOOL err = ::CreateProcess(appfile, (LPTSTR)commandline,
			NULL, NULL, TRUE, 
			createflag, NULL, NULL,
			&si, &pi
			);
		CloseHandle(hwrite);
		CloseHandle(herrout);
		if (!err) {
			break;
		}


		if (!waitforexit) {
			result = 0;
			break;
		}

		DWORD r = ::WaitForSingleObject(pi.hProcess, waittime);
		if (r != WAIT_OBJECT_0) {
			::TerminateProcess(pi.hProcess, -1);
		}
		::GetExitCodeProcess(pi.hProcess, &result);

		DWORD outlen = 0,
			errlen = 0;
		if (hread != NULL && herrin != NULL) {
			::ReadFile(hread,  _errinfo,		  DEFULT_IO_OPERATION_BYTES, &outlen, NULL);
			::ReadFile(herrin, _errinfo + outlen, DEFULT_IO_OPERATION_BYTES, &errlen, NULL);
		}
		_errinfo[errlen + outlen] = NULL;

	} while (0);

	CloseHandle(hread);
	CloseHandle(herrin);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	_errinfo[500] = NULL;
	return result;
}


//通过名称结束进程
static DWORD TerminateProcessAsName( LPCTSTR lpszName )
{
	if (!(lpszName && _tcslen(lpszName)) )
	{
		return -1 ;
	}

	PROCESSENTRY32 pe32 = {sizeof(pe32)} ;
	HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
	if ( hProcessShot == INVALID_HANDLE_VALUE )
	{
		return -1 ;
	}

	//USES_CONVERSION;
	LPCTSTR lpszNameW = lpszName;
	DWORD dwCount = 0;
	if ( Process32First ( hProcessShot, &pe32 ) )
	{
		do
		{
			if(_tcsicmp(pe32.szExeFile, lpszNameW) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
				if (hProcess && TerminateProcess(hProcess, 0)){
					dwCount++;
				}else{
					
				}
				if (hProcess)
				{
					CloseHandle(hProcess);
				}
			}
		}while ( Process32Next ( hProcessShot, &pe32 ) ) ;  
	}
	CloseHandle ( hProcessShot) ;  
	return dwCount;
}


//通过文件名获取进程ID
static DWORD GetProcessIdAsName( LPCTSTR lpszName )
{

	if (!(lpszName && _tcslen(lpszName)) )
	{
		return -1 ;
	}

	PROCESSENTRY32 pe32 = {sizeof(pe32)} ;
	HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
	if ( hProcessShot == INVALID_HANDLE_VALUE )
	{
		return -1 ;
	}

	LPCTSTR lpszNameW = lpszName;
	DWORD dwPid = -1;
	if ( Process32First ( hProcessShot, &pe32 ) )
	{
		do
		{
			if(_tcsicmp(pe32.szExeFile, lpszNameW) == 0)
			{
				dwPid = pe32.th32ProcessID;
				break;
			}
		}while ( Process32Next ( hProcessShot, &pe32 ) ) ;  
	}
	CloseHandle ( hProcessShot) ;  
	return dwPid;
}


static LPTSTR GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize)
{
	LPTSTR lpszTemp = 0;
	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		0,
		GetLastError(),
		LANG_NEUTRAL, //默认的语言种类
		(LPTSTR)&lpszTemp,
		0,
		0
		);

	if (!dwRet || (dwSize < dwRet + 14))
		lpszBuf[0] = TEXT('\0');
	else {
		lpszTemp[_tcsclen(lpszTemp) - 2] = TEXT('\0');  //remove cr/nl characters
		_tcscpy_s(lpszBuf, dwSize, lpszTemp);
	}

	if (lpszTemp)
		LocalFree(HLOCAL(lpszTemp));

	return lpszBuf;
}


/*获取PE文件的详细信息*/
static INT GetPeFileDetail(  LPCTSTR lpszPeFile					, LPCTSTR lpszCodePage				,//"080404b0"中文
							 LPTSTR lpszFileVer					, DWORD dwCchFileVer				,
							 LPTSTR lpszFileDesc			= 0	, DWORD dwCchFileDesc			= 0 ,
							 LPTSTR lpszProductName			= 0	, DWORD dwCchProductName		= 0 ,
							 LPTSTR lpszProductVer			= 0	, DWORD dwCchProductVer			= 0 ,
							 LPTSTR lpszProductCopyRight	= 0	, DWORD dwCchProductCopyRight	= 0	,
							 LPTSTR lpszOriginalFileName	= 0	, DWORD dwCchrOriginalFileName	= 0	)
{

	DWORD dwSize = GetFileVersionInfoSize(lpszPeFile, NULL);
	if( !dwSize ) return FALSE;

	LPVOID pBlock = malloc(dwSize);
	if( !pBlock ) return FALSE;

	BOOL bRet = GetFileVersionInfo(lpszPeFile,0,dwSize,pBlock);
	if( !bRet )
	{
		free(pBlock);
		return FALSE;
	}

	TCHAR* pVerValue = NULL;
	TCHAR szInfo[1000] = { 0 };
	UINT cbBlockSize = 0;
	INT  nRet = 0;
	// 描述 
	_stprintf_s(szInfo, _countof(szInfo),_T("\\StringFileInfo\\%s\\Comments"),lpszCodePage); 
	VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize); 

	// 公司名
	_stprintf_s(szInfo, _countof(szInfo),_T("\\StringFileInfo\\%s\\CompanyName"),lpszCodePage); 
	VerQueryValue(pBlock, szInfo, (LPVOID*)&pVerValue, &cbBlockSize); 


	if ( lpszFileDesc && dwCchFileDesc )
	{
		_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\FileDescription"), lpszCodePage);
		if( VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize) )
		{
			_tcscpy_s(lpszFileDesc, dwCchFileDesc, pVerValue);
			nRet++;
		}
	}


	//文件版本
	if ( lpszFileVer && dwCchFileVer )
	{
		_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\FileVersion"), lpszCodePage);
		if( VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize) )
		{
			_tcscpy_s(lpszFileVer, dwCchFileVer, pVerValue);
			nRet++;
		}
	}

	//产品版本
	if ( lpszProductVer && dwCchProductVer )
	{
		_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\ProductVersion"), lpszCodePage);
		if( VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize) )
		{
			_tcscpy_s(lpszProductVer, dwCchProductVer, pVerValue);
			nRet++;
		}
	}


	// 内部名称 
	_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\InternalName"), lpszCodePage);
	VerQueryValue(pBlock,szInfo,(LPVOID*)&pVerValue,&cbBlockSize); 

	// 版权声明
	if ( lpszProductCopyRight && dwCchProductCopyRight )
	{
		_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\LegalCopyright"), lpszCodePage);
		if( VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize) )
		{
			_tcscpy_s(lpszProductCopyRight, dwCchProductCopyRight, pVerValue);
			nRet++;
		}
	}


	//产品名称
	if ( lpszProductName && dwCchProductName )
	{
		_stprintf_s(szInfo, _countof(szInfo), _T("\\StringFileInfo\\%s\\ProductName"), lpszCodePage);
		if( VerQueryValue(pBlock,szInfo, (LPVOID*)&pVerValue, &cbBlockSize) )
		{
			_tcscpy_s(lpszProductName, dwCchProductName, pVerValue);
			nRet++;
		}
	}
	

	free(pBlock);
	return nRet;
}

//强制重新启动系统
static BOOL ReBootSystem(BOOL bPowerOff = FALSE)
{
	const int SE_SHUTDOWN_PRIVILEGE = 0x13;
	typedef int (__stdcall *PFN_RtlAdjustPrivilege)( INT, BOOL, BOOL, INT*);
	typedef int (__stdcall *PFN_ZwShutdownSystem)(INT);
	HMODULE hModule = ::LoadLibrary(_T("ntdll.dll"));
	if( hModule != NULL)
	{
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress( hModule, "RtlAdjustPrivilege");
		PFN_ZwShutdownSystem pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress( hModule,"NtShutdownSystem");
		if( pfnRtl != NULL && pfnShutdown != NULL )
		{
			int en = 0;
			int nRet= pfnRtl( SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if( nRet == 0x0C000007C )
				nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en);
			//const int POWEROFF = 1;
			//SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
			nRet = pfnShutdown(bPowerOff ? 2 : 1);
			return TRUE;
		}
	}
	
	return FALSE;
}


//获取IExplorer的路径
static BOOL GetIEPath(LPTSTR szBuf, int bufBytes)
{
	BOOL bRet = FALSE;
	DWORD dwType = 0;
	DWORD cbData = bufBytes;
	HKEY hRoot = NULL, hSubKey = NULL;
	if(RegOpenKey(HKEY_LOCAL_MACHINE, NULL, &hRoot) == ERROR_SUCCESS)
	{
		if(RegOpenKeyEx(hRoot, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE"),
			0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hSubKey, NULL, NULL, &dwType, (LPBYTE)szBuf, &cbData) == ERROR_SUCCESS)
				bRet = TRUE;
			RegCloseKey(hSubKey);
		}
		RegCloseKey(hRoot);
	}
	return bRet;
}

//获取IE内核的版本
static BOOL GetIEVersion(LPTSTR lpszVersion, UINT uSize)
{
	//////////////////////////////////////////////////////////////////////////
	TCHAR szFileName[MAX_PATH] = { 0 };
	HMODULE hModule = ::GetModuleHandle(_T("WININET.DLL"));
	DWORD dwHandle = 0;
	if (hModule == 0)
		return FALSE;
	DWORD dwLen = GetModuleFileName(hModule, szFileName, sizeof(szFileName) / sizeof(szFileName[0]));
	if (dwLen <= 0)
		return FALSE;

	dwLen = GetFileVersionInfoSize(szFileName, &dwHandle);
	if (dwLen <= 0)
		return FALSE;
	BYTE *pVersionInfo = new BYTE[dwLen];
	if (!::GetFileVersionInfo(szFileName, dwHandle, dwLen, pVersionInfo))
	{
		delete pVersionInfo;
		return FALSE;
	}

	VS_FIXEDFILEINFO *pFixed = NULL;
	UINT uLen = 0;
	if (!VerQueryValue(pVersionInfo, _T("\\"), (LPVOID*)&pFixed, &uLen))
	{
		delete pVersionInfo;

		return FALSE;
	}

	//wsprintf(lpszVersion, _T("MSIE %d.%d"), verInfo.dwMajorVersion, verInfo.dwMinorVersion);
	wsprintf(lpszVersion,
		_T("MSIE %d.%d"), 
		HIWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/),
		LOWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/));	

	delete pVersionInfo;

	return TRUE;
}


//删除图标缓存
static BOOL DelIconCache()
{
	TCHAR szAppData[ MAX_PATH ] = { 0 };
	if ( !SHGetSpecialFolderPath(NULL, szAppData, CSIDL_LOCAL_APPDATA, FALSE) )
		return FALSE;

	_tcscat_s( szAppData, MAX_PATH, _T("\\IconCache.db"));

	if ( PathFileExists(szAppData) )
		return DeleteFile( szAppData);

	return TRUE;
}

/*
static BOOL GetShotcutPath(LPTSTR szFilePath, LPTSTR strPath, DWORD dwCch)
{
	//创建IShellLink实例
	IShellLink *psl = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
	if( FAILED(hr) )
	{
		return FALSE;
	}

	//获取IPersistFile接口
	IPersistFile* ppf = NULL;
	hr = psl->QueryInterface( IID_IPersistFile, (LPVOID*)&ppf);
	if( FAILED(hr) )
	{
		psl->Release();
		return FALSE;
	}


	CHAR szPath[MAX_PATH] = { 0 };

	USES_CONVERSION;
	strcpy_s(szPath, MAX_PATH, T2A(szFilePath));
	//加载快捷方式
	hr = ppf->Load(T2W(szFilePath), STGM_READ | STGM_SHARE_DENY_NONE);
	if( FAILED(hr) )
	{
		ppf->Release();
		psl->Release();
		return FALSE;
	}

	//解析快捷方式
	hr = psl->Resolve( NULL, SLR_ANY_MATCH|SLR_NO_UI );
	if( FAILED(hr) )
	{
		ppf->Release();
		psl->Release();
		return FALSE;
	}


	// 获取目标路径（可能含有环境变量）
	TCHAR szTargetPath[MAX_PATH] = {0};
	hr = psl->GetPath(szTargetPath, MAX_PATH, NULL, SLGP_UNCPRIORITY);
	if( FAILED(hr) )
	{
		psl->Release();
		ppf->Release();
		return FALSE;
	}

	_tcscpy_s( strPath, dwCch, szTargetPath);

	ppf->Release();
	psl->Release();

	if (_tcslen(strPath) == 0){
		return FALSE;
	}

	if (PathFileExists(strPath)){
		return TRUE;
	}

	return TRUE;
}
*/

static HANDLE ShellProcess( LPCTSTR lpszFile, LPCTSTR lpszCmd, BOOL bShow)
{
	SHELLEXECUTEINFO shellInfo = { 0 };
	shellInfo.cbSize = sizeof( shellInfo );
	shellInfo.lpVerb = _T("open");
	shellInfo.hwnd = NULL;
	shellInfo.lpFile =(LPCTSTR)lpszFile;

	shellInfo.lpParameters = lpszCmd;
	shellInfo.nShow = bShow ? SW_SHOWNORMAL : SW_HIDE;
	shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	if ( ShellExecuteEx( &shellInfo ) )
		return shellInfo.hProcess;
	return NULL;
}



static DWORD ShellProcess( LPCTSTR lpszFile, DWORD dwWait, LPCTSTR lpszCmd, BOOL bShow)
{
	DWORD dwExitCode = -1;
	HANDLE hHandle = ShellProcess(lpszFile, lpszCmd, bShow);
	if ( hHandle == NULL)
		return dwExitCode;

	if ( WaitForSingleObject(hHandle, dwWait) == WAIT_TIMEOUT )
	{
		TerminateProcess(hHandle, dwExitCode);
		CloseHandle(hHandle);
		return dwExitCode;
	}

	GetExitCodeProcess( hHandle, &dwExitCode);
	CloseHandle(hHandle);
	return dwExitCode;
}

static DWORD ShellCommand( LPCTSTR lpszExe, DWORD dwWait, TCHAR const *pCmd, ...)
{
	TCHAR pTempBuffer[256] = {0};
	DWORD dwExitCode = -1;
	if ( pCmd )
	{
		va_list args;
		va_start( args, pCmd );
		_vstprintf_s( pTempBuffer, _countof(pTempBuffer), pCmd, args );
		va_end( args );
	}
	
#ifdef _DEBUG
	OutputDebugString( pTempBuffer);
#endif

	HANDLE hHandle = ShellProcess(lpszExe, pTempBuffer, FALSE);
	if ( hHandle == NULL)
		return dwExitCode;

	if ( WaitForSingleObject(hHandle, dwWait) == WAIT_TIMEOUT )
	{
		CloseHandle(hHandle);
		return dwExitCode;
	}
	
	GetExitCodeProcess( hHandle, &dwExitCode);
	CloseHandle(hHandle);
	return dwExitCode;
}


static HWND GetWindowHwndByPorcessID(DWORD dwProcessID)  
{  
	DWORD dwPID = 0;  
	HWND hwndRet = NULL;  
	// 取得第一个窗口句柄  
	HWND hwndWindow = ::GetTopWindow(0);  
	while (hwndWindow)  
	{  
		dwPID = 0;  
		// 通过窗口句柄取得进程ID  
		DWORD dwTheardID = ::GetWindowThreadProcessId(hwndWindow, &dwPID);  
		if (dwTheardID != 0)  
		{  
			// 判断和参数传入的进程ID是否相等  
			if (dwPID == dwProcessID)  
			{  
				// 进程ID相等，则记录窗口句柄  
				hwndRet = hwndWindow;  
				break;  
			}  
		}  
		// 取得下一个窗口句柄  
		hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT);  
	}  
	// 上面取得的窗口，不一定是最上层的窗口，需要通过GetParent获取最顶层窗口  
	HWND hwndWindowParent = NULL;  
	// 循环查找父窗口，以便保证返回的句柄是最顶层的窗口句柄  
	while (hwndRet != NULL)  
	{  
		hwndWindowParent = ::GetParent(hwndRet);  
		if (hwndWindowParent == NULL)  
		{  
			break;  
		}  
		hwndRet = hwndWindowParent;  
	}  
	// 返回窗口句柄  
	return hwndRet;  
}  
};
};//namespace msdk