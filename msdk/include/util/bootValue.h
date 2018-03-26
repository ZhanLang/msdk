#pragma once
#include <Shlobj.h>

namespace msdk{

//用于保存重启前有效数据
typedef struct SYSTEM_TIME_INFORMATION
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;
#define SystemTimeInformation                 0x3

#define BOOT_APPNAME	_T("values")

class CBootValue
{
public:
	BOOL SetValue(LPCTSTR lpName, LPCTSTR lpString)
	{
		if(!lpName || !lpString || !IsInit()) return FALSE;

		TCHAR szNameFlag[100] = {0};
		_stprintf_s(szNameFlag, 100, _T("%s_tag_"), lpName);
		if(!WritePrivateProfileString(BOOT_APPNAME, szNameFlag, m_bootFlag, m_iniFile))
			return FALSE;

		return WritePrivateProfileString(BOOT_APPNAME, lpName, lpString, m_iniFile);
	}

	DWORD GetVaule(LPCTSTR lpName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize)
	{
		if(!lpName || !lpReturnedString || !IsInit()) return 0;

		TCHAR szNameFlag[100] = {0};
		_stprintf_s(szNameFlag, 100, _T("%s_tag_"), lpName);

		TCHAR bootFlag[100] = {0};
		if(0 == GetPrivateProfileString(BOOT_APPNAME, szNameFlag, NULL, bootFlag, 100, m_iniFile)
			|| _tcsicmp(bootFlag, m_bootFlag))
		{
			if(lpDefault)
			{
				_tcscpy_s(lpReturnedString, nSize, lpDefault);
				return _tcslen(lpDefault);
			}
			return 0;
		}

		//一切ok，读上去有效
		return GetPrivateProfileString(BOOT_APPNAME, lpName, lpDefault, lpReturnedString, nSize, m_iniFile);
	}

	CBootValue():m_bInit(false){Init();}
private:
	bool m_bInit;
	TCHAR m_iniFile[MAX_PATH];
	TCHAR m_bootFlag[100];

	typedef LONG (WINAPI* pfnNtQuerySystemInfomation)(UINT, PVOID, ULONG, PULONG);
	pfnNtQuerySystemInfomation m_fnNtQuerySystemInformation;

	BOOL Init()
	{
		ZeroMemory(m_iniFile, sizeof(m_iniFile));
		ZeroMemory(m_bootFlag, sizeof(m_bootFlag));

		//%programdata%
		TCHAR buf[MAX_PATH] = {0};
		::SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
		_stprintf_s(m_iniFile, MAX_PATH, _T("%s\\msbootvalue.ini"), buf);

		m_fnNtQuerySystemInformation = (pfnNtQuerySystemInfomation)GetProcAddress(GetModuleHandle(_T("NTDLL")), "NtQuerySystemInformation");
		if(0 == m_fnNtQuerySystemInformation)
			return FALSE;

		SYSTEM_TIME_INFORMATION Sti;
		ULONG ret_len = 0;
		LONG nRet = (*m_fnNtQuerySystemInformation)(SystemTimeInformation,&Sti,sizeof(Sti),&ret_len);
		if(ret_len != sizeof(Sti))
			return FALSE;

		LONGLONG QuadPart = Sti.liKeBootTime.QuadPart;
		_stprintf_s(m_bootFlag, sizeof(m_bootFlag)/sizeof(TCHAR), _T("%I64u"), QuadPart);

		m_bInit = true;
		return TRUE;
	}

	bool IsInit(){return m_bInit;}
};

};//namespace msdk