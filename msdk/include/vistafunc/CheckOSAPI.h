#include "stdafx.h"

#include "Winnt.h"
class CCheckOSAPI
{
public:
	CCheckOSAPI()
	{
		m_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&m_osver);
	}

	BOOL IsWindowXP()
	{
		return ( m_osver.dwMajorVersion  == 5 &&  m_osver.dwMinorVersion == 1 );
	}

	BOOL IsWindowVista()
	{
		return ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion == 0 );
	}

	BOOL IsWindow7()
	{
		return ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion == 1 );
	}

	BOOL IsWindow8()
	{
		return ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion == 2 );
	}

	BOOL IsWindowBlue()
	{
		return ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion == 3 );
	}

	BOOL IsWindow9()
	{
		return ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion == 4 );
	}

	BOOL IsWindowVistaLatter()
	{
		return ( m_osver.dwMajorVersion  >= 6 );
	}

	BOOL IsWindow7Latter()
	{
		return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 1 ) );
	}

	BOOL IsWindow8Latter()
	{
		return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 2 ) );
	}

	BOOL IsWindowBlueLatter()
	{
		return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 3 ) );
	}

	BOOL IsWindow9Latter()
	{
		return ( ( m_osver.dwMajorVersion > 6 ) || ( m_osver.dwMajorVersion  == 6 &&  m_osver.dwMinorVersion >= 4 ) );
	}


	BOOL IsWindowServer()
	{
		OSVERSIONINFOEX	osif;

		osif.dwOSVersionInfoSize = sizeof(osif);
		GetVersionEx((LPOSVERSIONINFOW)&osif);
		if( osif.wProductType == VER_NT_SERVER )
			return TRUE;

		return FALSE;
	}


#ifndef PROCESSOR_ARCHITECTURE_AMD64
#define PROCESSOR_ARCHITECTURE_AMD64 9
#endif

	BOOL IsWindow64() 
	{ 
		typedef void (__stdcall * PFNGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo); 
		SYSTEM_INFO si = {0}; 
		PFNGetNativeSystemInfo pGNSI; 
		pGNSI = (PFNGetNativeSystemInfo) GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetNativeSystemInfo"); 
		if(NULL != pGNSI) 
			pGNSI(&si); 
		else
			GetSystemInfo(&si);

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) 
			return TRUE; 

		return FALSE; 
	}

	BOOL Is64bitDLL(LPCWSTR lpwsFile)
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

private:
	OSVERSIONINFO  m_osver;
};



