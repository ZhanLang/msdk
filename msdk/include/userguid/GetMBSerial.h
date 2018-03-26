/*******************************************************************************
*

*
*******************************************************************************/
#ifndef _RSGETMBSERIAL_H
#define _RSGETMBSERIAL_H

#include <comdef.h>
#include <Wbemidl.h>
#include <winternl.h>
# pragma comment(lib, "wbemuuid.lib")


// typedef struct _UNICODE_STRING {
// 	USHORT  Length;//长度
// 	USHORT  MaximumLength;//最大长度
// 	PWSTR  Buffer;//缓存指针，访问物理内存时，此处指向UNICODE字符串"\device\physicalmemory"
// } UNICODE_STRING,*PUNICODE_STRING;


// typedef struct _OBJECT_ATTRIBUTES {
// 	ULONG Length;//长度 18h
// 	HANDLE RootDirectory;//  00000000
// 	PUNICODE_STRING ObjectName;//指向对象名的指针
// 	ULONG Attributes;//对象属性00000040h
// 	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR，0
// 	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE，0
// } OBJECT_ATTRIBUTES;
// typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef DWORD  (__stdcall *ZWOS)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
typedef DWORD  (__stdcall *ZWMV)(HANDLE,HANDLE,PVOID,ULONG,ULONG,PLARGE_INTEGER,PSIZE_T,DWORD,ULONG,ULONG);
typedef DWORD  (__stdcall *ZWUMV)(HANDLE,PVOID);
typedef UINT   (WINAPI *LPFUNGetSystemFirmwareTable)(DWORD,DWORD,PVOID,DWORD);

#pragma pack(1)
typedef struct _SMBIOS_HEADER
{
	DWORD dwAnchorString; //"_SM_"
	BYTE  bEPSCheckSum;
	BYTE  bEntryPointLen;
	BYTE  bSmbiosMajorVer;
	BYTE  bSmbiosMinorVer;
	WORD  wMaxStructSize;
	BYTE  bEntryPointResvison;
	BYTE  bResvered[5];
	BYTE  bIntermediateAuthorString[5]; //"_DMI_"
	BYTE  bIntermediateChecksum;
	WORD  wStructTableLen;
	DWORD dwStructTableAddress;
	WORD  wNuberOfSmbiosStruct;
	BYTE  bSmbiosBcdRevision;
}SMBIOS_HEADER,*PSMBIOS_HEADER;
typedef struct _SMBIOS_STRUCT_HEADER
{
	BYTE bStructType;
	BYTE bHeaderSize;
	WORD wHandle; 
}SMBIOS_STRUCT_HEADER,*PSMBIOS_STRUCT_HEADER;
typedef struct _SMBIOS_STRUCT_SYSTEMINFO
{
	SMBIOS_STRUCT_HEADER stStructHeader;
	BYTE bManuFacturerString;
	BYTE bProductNameString;
	BYTE bVersionString;
	BYTE bSerialNumberString;
	BYTE bUUID[0x10];
	BYTE bWakeupType;

}SMBIOS_STRUCT_SYSTEMINFO,*PSMBIOS_STRUCT_SYSTEMINFO;

#pragma pack()


namespace msdk {

#define RSMB_SIZE	100

#pragma warning( push)
#pragma warning( disable:4996 )


	

	class CGetMainBoardInfo
	{
	public:
		static bool GetMainBoardInfo(char mainBoardInfo[RSMB_SIZE], bool bFromWmi = true)
		{
			bool bGet = false;

			CHAR szSerialNumber[100];
			ZeroMemory(szSerialNumber, sizeof(szSerialNumber));
			if(bFromWmi)
			{
				// 取主板序列号
				//bGet = GetSerialNumber("Win32_BaseBoard", szSerialNumber);

				// 取BIOS序列号
				bGet = GetSerialNumber("Win32_BIOS", szSerialNumber);
			}
			else
			{
				DWORD dwSize = 100;
				bGet = GetBiosSerial(szSerialNumber, dwSize);
			}
			if(bGet)
				bGet = ValidateSerial(szSerialNumber);

			memcpy(mainBoardInfo, szSerialNumber, strlen(szSerialNumber));

			return bGet;
		}

		//空串或者全是一样的也认为是无效的
		static bool ValidateSerial(const char* szClassName)
		{
			if(NULL == szClassName || 0x00 == *szClassName)
				return false;

			char chFirst = *szClassName;

			size_t len = strlen(szClassName);
			size_t nCheckCount = 0;

			while(*szClassName && *szClassName++ == chFirst)
			{
				++nCheckCount;
			}

			return len != nCheckCount;
		}

		// 公共函数
		static bool GetSerialNumber(const CHAR* szClassName, CHAR* szSerialNumber)
		{
			//HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
			HRESULT hres = CoInitialize(NULL);
			if (FAILED(hres))
				return false;                  
			hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,   
				RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL); 

			//RPC_E_TOO_LATE：CoInitializeSecurity has already been called. 
			if(RPC_E_TOO_LATE == hres)
				hres = S_FALSE;

			if (FAILED(hres))
			{
				CoUninitialize();
				return false;                  
			}
			IWbemLocator *pLoc = NULL;
			hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID*)&pLoc);
			if (FAILED(hres))
			{
				CoUninitialize();
				return false;                
			}
			IWbemServices *pSvc = NULL;
			hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
			if (FAILED(hres))
			{
				pLoc->Release();    
				CoUninitialize();
				return false;              
			}
			hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
				RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,EOAC_NONE);
			if (FAILED(hres))
			{
				pSvc->Release();
				pLoc->Release();    
				CoUninitialize();
				return false;              
			}
			CHAR szWQL[1024] = {0};
			sprintf(szWQL, "SELECT * FROM %s", szClassName);
			IEnumWbemClassObject* pEnumerator = NULL;
			hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(szWQL), WBEM_FLAG_FORWARD_ONLY | 
				WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
			if (FAILED(hres))
			{
				pSvc->Release();
				pLoc->Release();
				CoUninitialize();
				return false;             
			}
			while (pEnumerator)
			{
				ULONG uReturn = 0;
				IWbemClassObject *pclsObj = NULL;
				HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (uReturn == 0 || !pclsObj)
				{
					if(pclsObj)
						pclsObj->Release();
					break;  
				}
				VARIANT vtProp;
				pclsObj->Get(L"SerialNumber", 0, &vtProp, NULL, NULL);
				sprintf(szSerialNumber, "%ws", V_BSTR(&vtProp));
				VariantClear(&vtProp);
				pclsObj->Release();
			}
			pSvc->Release();
			pLoc->Release();
			pEnumerator->Release();
			CoUninitialize(); 
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		//非wmi模式
		static bool GetBiosSerial(CHAR* pszSerial, DWORD &dwSize)
		{
			bool bSuccess = false;

			switch(GetWindowsVer())
			{
			case 1:
				bSuccess=GetLowVersionBIosSerial(pszSerial,dwSize);
				break;
			case 2:
				bSuccess=GetHighVersionBIosSerial(pszSerial,dwSize);
				break;
			default:
				break;
			}

			return bSuccess;
		}

		static BYTE* SrchBlk(BYTE *Dest, BYTE *Blk, DWORD DestLen, DWORD BlkLen)
		{
			if(DestLen<BlkLen)
				return NULL;
			BYTE *DestEnd=Dest+DestLen;
			//BYTE *DestBak,*BlkBak;
			//BYTE *DelBak,*DelBak1;


			DWORD i;
			BYTE *pHead, *pCur, *psrc;

			while(BlkLen<=DestLen)
			{
				psrc=Blk;
				pCur=pHead=(BYTE*)memchr(Dest,*psrc,DestEnd-Dest);
				i=BlkLen-1;
				while(pCur && i)
				{
					i--;
					pCur++;
					psrc++;
					if (DestEnd <= pCur)
						pCur=NULL;
					else
						pCur=(BYTE*)memchr(pCur,*psrc,DestEnd-pCur);
				}
				if (!pCur)
					break;
				if ((DWORD)(pCur-pHead)==(BlkLen-1))
				{
					return pHead;   //OK
				}
				Dest=(BYTE*)(pCur-BlkLen+1);
				DestLen=(DWORD)(DestEnd-Dest);
			}
			
			return NULL;
		}
		static CHAR* GetSerialFromDmiDataBuf(BYTE* pBuf,DWORD dwBufSize)
		{
			BYTE* pStructStart;
			WORD  wTemp=0;

			PSMBIOS_STRUCT_HEADER psmBiosHeader;
			PSMBIOS_STRUCT_SYSTEMINFO psSystemInfo;
			if((pBuf==0)||(dwBufSize<0x10))
				return NULL;
			pStructStart=pBuf;
			do
			{
				psmBiosHeader=(PSMBIOS_STRUCT_HEADER)pStructStart;
				switch(psmBiosHeader->bStructType)
				{
				case 1: //systeminfo;
					psSystemInfo=(PSMBIOS_STRUCT_SYSTEMINFO)psmBiosHeader;
					if(psSystemInfo->bSerialNumberString)//序列号字符串存在
					{
						pStructStart+=psSystemInfo->stStructHeader.bHeaderSize;
						wTemp=1;
						while((wTemp<=psSystemInfo->bSerialNumberString)&&(pStructStart<pBuf+dwBufSize))
						{
							if(wTemp==psSystemInfo->bSerialNumberString)
								return (CHAR*)pStructStart;
							if(pStructStart[0]==0)
								wTemp++;
							pStructStart++;
						}

					}
					return NULL;
					break;
				default:
					pStructStart+=psmBiosHeader->bHeaderSize;
					if(pStructStart>=pBuf+dwBufSize)
						break;
					pStructStart=SrchBlk(pStructStart,(BYTE*)(&wTemp),dwBufSize-(pStructStart-pBuf),2);
					if(pStructStart==NULL)
						return NULL;
					pStructStart+=2;
					break;
				}

			}while(pStructStart<(pBuf+dwBufSize));
			
			return NULL;
		}

		static CHAR* RawBiosBufGetSerial(BYTE* pBuf,DWORD dwBufSize)
		{
			BYTE* pSmBiosBegin;
			//DWORD dwPos;
			PSMBIOS_HEADER pSmBiosHeader;
			if((pBuf==0)||(dwBufSize<0x20))
				return NULL;
			pSmBiosBegin=pBuf;
			pSmBiosHeader=NULL;
			do 
			{
				pSmBiosBegin=SrchBlk(pSmBiosBegin,(BYTE*)"_SM_",dwBufSize-(pSmBiosBegin-pBuf),4);
				if(pSmBiosBegin==NULL)
					break;
				pSmBiosHeader=(PSMBIOS_HEADER)pSmBiosBegin;
				if(memcmp(pSmBiosHeader->bIntermediateAuthorString,"_DMI_",5))
				{
					pSmBiosBegin++;
					pSmBiosHeader=NULL;
				}
				else
				{
					break;
				}
			} while (pSmBiosBegin<(pBuf+dwBufSize));
			if(pSmBiosHeader==NULL)
				return NULL;
			if(dwBufSize<(pSmBiosHeader->dwStructTableAddress&0xffff))
				return NULL;
			return GetSerialFromDmiDataBuf(pBuf+(pSmBiosHeader->dwStructTableAddress&0xffff),dwBufSize-(pSmBiosHeader->dwStructTableAddress&0xffff));
		}

		static bool GetLowVersionBIosSerial(CHAR* pszBuf,DWORD &dwBufSize)
		{
			//MSG msg;
			//HACCEL hAccelTable;
			UNICODE_STRING struniph;
			OBJECT_ATTRIBUTES obj_ar;
			ZWOS ZWopenS;
			ZWMV ZWmapV;
			ZWUMV ZWunmapV;
			HANDLE hSection;
			HMODULE hinstLib;
			DWORD ba;
			LARGE_INTEGER so;
			SIZE_T ssize;
			CHAR* pszSerial;
			bool bRet=false;
			so.LowPart=0x000f0000;//物理内存的基址，就是f000:0000
			so.HighPart=0x00000000;
			ssize=0xffff;
			wchar_t strPH[30]=L"\\device\\physicalmemory";

			// 初始化全局字符串
			//变量初始化
			ba=0;//联系后的基址将在这里返回
			struniph.Buffer=strPH;
			struniph.Length=0x2c;//注意大小是按字节算
			struniph.MaximumLength =0x2e;//也是字节
			obj_ar.Attributes =64;//属性
			obj_ar.Length =24;//OBJECT_ATTRIBUTES类型的长度
			obj_ar.ObjectName=&struniph;//指向对象的指针
			obj_ar.RootDirectory=0;
			obj_ar.SecurityDescriptor=0;
			obj_ar.SecurityQualityOfService =0;
			//读入ntdll.dll,得到函数地址
			hinstLib = LoadLibraryA("ntdll.dll");
			ZWopenS=(ZWOS)GetProcAddress(hinstLib,"ZwOpenSection");
			ZWmapV=(ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection");
			ZWunmapV=(ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection");
			if((ZWopenS==NULL)||(ZWmapV==NULL)||(ZWunmapV==NULL))
				return bRet;
			//调用函数，对物理内存进行映射
			if(ZWopenS(&hSection,4,&obj_ar)<0)
			{
				FreeLibrary(hinstLib);
				return bRet;
			}
			if(ZWmapV((HANDLE)hSection,(HANDLE)0xffffffff,&ba,0,0xffff,&so,&ssize,1,0,2)<0)
			{
				FreeLibrary(hinstLib);
				return bRet;
			}
		/*
			f1=fopen("bios.mem","wb+");
			fwrite((void*)ba,65536,1,f1);
			fclose(f1);*/
			pszSerial=RawBiosBufGetSerial((BYTE*)ba,ssize);
			if(pszSerial)
			{
				if(dwBufSize>strlen(pszSerial))
				{
					strncpy(pszBuf,pszSerial,dwBufSize);
					bRet=TRUE;
				}
				dwBufSize=strlen(pszSerial);

			}
			ZWunmapV(hSection,(PVOID)ba);
			CloseHandle(hSection);
			FreeLibrary(hinstLib);
			//MessageBox(NULL,"Bios saved to bios.mem!","Save OK",MB_OK);
			return bRet;

		}

		static bool GetHighVersionBIosSerial(CHAR* pszBuf,DWORD &dwBufSize)
		{
			DWORD dwSize;
			BYTE*   pbBuf;
			CHAR* pszSerial;
			bool bRet=FALSE;
			LPFUNGetSystemFirmwareTable lpfun;
			HMODULE hinstLib;

			typedef struct RawSMBIOSData
			{
				BYTE	Used20CallingMethod;
				BYTE	SMBIOSMajorVersion;
				BYTE	SMBIOSMinorVersion;
				BYTE	DmiRevision;
				DWORD	Length;
				BYTE	SMBIOSTableData[1];
			}STRawSMBIOSData,*PSTRawSMBIOSData;
			PSTRawSMBIOSData pstRawBios;


			hinstLib=LoadLibraryA("kernel32.dll");
			if(hinstLib==INVALID_HANDLE_VALUE)
				return FALSE;
			lpfun=(LPFUNGetSystemFirmwareTable)GetProcAddress(hinstLib,"GetSystemFirmwareTable");
			if(lpfun==NULL)
			{
				FreeLibrary(hinstLib);
				return bRet;
			}

			dwSize=lpfun('RSMB',0,NULL,0);
			pbBuf=new BYTE[dwSize];
			dwSize=lpfun('RSMB',0,pbBuf,dwSize);
			pstRawBios=(PSTRawSMBIOSData)pbBuf;
			pszSerial=GetSerialFromDmiDataBuf(pstRawBios->SMBIOSTableData,pstRawBios->Length);
			if(pszSerial)
			{
				if(dwBufSize>strlen(pszSerial))
				{
					strncpy(pszBuf,pszSerial,dwBufSize);
					bRet=TRUE;
				}
				dwBufSize=strlen(pszSerial);

			}
			//MessageBox(NULL,"Bios saved to bios.mem!","Save OK",MB_OK);
			delete[] pbBuf;
			return bRet;

		}
		static INT GetWindowsVer()
		{
			OSVERSIONINFOEX osvi;
			SYSTEM_INFO si;
			BOOL bOsVersionInfoEx;

			ZeroMemory(&si, sizeof(SYSTEM_INFO));
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

			// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
			// If that fails, try using the OSVERSIONINFO structure.

			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
			{
				osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
				if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
					return 0;
			}
			if(osvi.dwMajorVersion<=4)
			{
				return 0;
			}
			else if(osvi.dwMajorVersion<=5)
			{
				if(osvi.dwMinorVersion >= 2) //xp professional x64 及 2003
					return 2;

				return 1;
			}
			return 2;
		}
	};
#pragma warning( pop )

} //namespace msdk

#endif	//_RSGETMBSERIAL_H