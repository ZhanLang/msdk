#pragma once
#include <list>

#define REG_DISPLAY_NAME		_T("DisplayName")
#define REG_UNINSTALL_STRING	_T("UninstallString")
#define REG_PARENT_KEY_NAME		_T("ParentKeyName")
#define REG_RELEASE_TYPE		_T("ReleaseType")
#define REG_PUBLISHER			_T("Publisher")
#define REG_EstimatedSize		_T("EstimatedSize")
#define REG_InstallLocation		_T("InstallLocation")

#define REG_NOREMOVE			_T("NoRemove")
#define REG_NOMODIFY			_T("NoModify")
#define REG_NOREPAIR			_T("NoRepair")
#define REG_WINDOWS_INSTALL		_T("WindowsInstaller")
#define REG_SYSTEM_COMPONENT	_T("SystemComponent")

typedef struct _REG_SOFTWARE_INFO
{
	CString szSubKeyName;
	CString szDisplayName;
	CString szUninstallString;
	CString szParentKeyName;
	CString szParentDisplayName;
	CString szReleaseType;
	CString szPublisher;		//¹«Ë¾
	CString szInstallLocation;  //InstallLocation
	CString szUninstallRegPath;
	DWORD	nNoRemove;
	DWORD	nNoModify;
	DWORD	nNoRepair;
	DWORD	nWindowsInstall;
	DWORD	nSystemComponent;
	DWORD	dwEstimateSize;
	INT64	i64Softwaresize;
	CString szVer;
	BOOL	bHide;
	BOOL	bUpdates;

	_REG_SOFTWARE_INFO()
	{
		Reset();
	}

	void Reset()
	{
		bUpdates = FALSE;
		szSubKeyName.Empty();
		szDisplayName.Empty();
		szUninstallString.Empty();
		szParentKeyName.Empty();
		szReleaseType.Empty();
		szPublisher.Empty();
		szInstallLocation.Empty();
		szVer.Empty();
		szUninstallRegPath.Empty();
		bHide = FALSE;
		nNoRemove = 0;
		nNoModify = 0;
		nNoRepair = 0;
		nWindowsInstall = 0;
		nSystemComponent = 0;
		dwEstimateSize = 0;
		i64Softwaresize = 0;
	}
}REG_SOFTWARE_INFO,*PREG_SOFTWARE_INFO;
//typedef CRuleBuffer<REG_SOFTWARE_INFO> SOFTWARE_LIST;

typedef std::list<REG_SOFTWARE_INFO> SOFTWARE_LIST;
class CSoftWareEnum
{
public:
	CSoftWareEnum(void);
	~CSoftWareEnum(void);

	HRESULT Enum(REG_SOFTWARE_INFO& info, BOOL bIncludeUpdates = FALSE, BOOL bIsSvc = FALSE);
	HRESULT EnumNext(REG_SOFTWARE_INFO& info);

	HRESULT EnumSoftWares(HKEY hRootKey, SOFTWARE_LIST& softList, LPCTSTR lpKeyPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), LPCTSTR lpSubPath = NULL, HKEY hReal = NULL, LPCTSTR lpszRelKeyPath = NULL);
	HRESULT GetSoftItem(HKEY hRootKey, LPCTSTR lpKeyPath, REG_SOFTWARE_INFO &info, LPCTSTR lpSubKeyName, LPCTSTR lpSubPath);

	HRESULT GetSoftInfo(HKEY hRootKey, LPCTSTR lpKeyPath, REG_SOFTWARE_INFO &info);
	BOOL CheckUserDataSoft(LPCTSTR lpMsiKeyName, REG_SOFTWARE_INFO& info);

	SOFTWARE_LIST m_softList;
	SOFTWARE_LIST::iterator m_enum;
	BOOL			m_bIncludeUpdates;
};
