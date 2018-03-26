#pragma once

#include "luahost/ILuaHost.h"
#include <vector>
namespace winfunc
{

/////////////////////////////////////////////////////////////////////////////
// CWinRegKey
	typedef std::basic_string<TCHAR> TString;
	typedef std::vector<TString> TStringS;

	class CWinRegKey : public CRegKey
	{
	public:

		static BOOL BindToLua(ILuaVM* pLuaVM);

		//help for lua
		static HKEY GetHKeyByName(LPCTSTR szKeyParentName)
		{
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_CLASSES_ROOT"))) return HKEY_CLASSES_ROOT;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_CURRENT_USER"))) return HKEY_CURRENT_USER;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_LOCAL_MACHINE"))) return HKEY_LOCAL_MACHINE;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_USERS"))) return HKEY_USERS;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_PERFORMANCE_DATA"))) return HKEY_PERFORMANCE_DATA;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_PERFORMANCE_TEXT"))) return HKEY_PERFORMANCE_TEXT;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_PERFORMANCE_NLSTEXT"))) return HKEY_PERFORMANCE_NLSTEXT;
		//#if(WINVER >= 0x0400)										   ;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_CURRENT_CONFIG"))) return HKEY_CURRENT_CONFIG;
			if(0==_tcsicmp(szKeyParentName, _T("HKEY_DYN_DATA"))) return HKEY_DYN_DATA;
		//#endif
			return 0;
		};

		static REGSAM GetREGSAMByName(LPCTSTR samDesired)
		{
			if(0==_tcsicmp(samDesired, _T("KEY_READ"))) return KEY_READ;
			if(0==_tcsicmp(samDesired, _T("KEY_WRITE"))) return KEY_WRITE;
			if(0==_tcsicmp(samDesired, _T("KEY_EXECUTE"))) return KEY_EXECUTE;
			if(0==_tcsicmp(samDesired, _T("KEY_ALL_ACCESS"))) return KEY_ALL_ACCESS;
			if(0==_tcsicmp(samDesired, _T("KEY_READ_WRITE"))) return KEY_READ | KEY_WRITE;		
			return 0;
		};
		static LPTSTR GetClassByName(LPCTSTR lpszClass)
		{
			if(0==_tcsicmp(lpszClass, _T("REG_NONE"))) return (LPTSTR)REG_NONE;
			if(0==_tcsicmp(lpszClass, _T("REG_SZ"))) return (LPTSTR)REG_SZ;
			if(0==_tcsicmp(lpszClass, _T("REG_EXPAND_SZ"))) return (LPTSTR)REG_EXPAND_SZ;
		// (with environment variable references)
			if(0==_tcsicmp(lpszClass, _T("REG_BINARY"))) return (LPTSTR)REG_BINARY;
			if(0==_tcsicmp(lpszClass, _T("REG_DWORD"))) return (LPTSTR)REG_DWORD;		
			if(0==_tcsicmp(lpszClass, _T("REG_DWORD_LITTLE_ENDIAN"))) return (LPTSTR)REG_DWORD_LITTLE_ENDIAN;
			if(0==_tcsicmp(lpszClass, _T("REG_DWORD_BIG_ENDIAN"))) return (LPTSTR)REG_DWORD_BIG_ENDIAN;
			if(0==_tcsicmp(lpszClass, _T("REG_LINK"))) return (LPTSTR)REG_LINK;
			if(0==_tcsicmp(lpszClass, _T("REG_MULTI_SZ"))) return (LPTSTR)REG_MULTI_SZ;
			if(0==_tcsicmp(lpszClass, _T("REG_RESOURCE_LIST"))) return (LPTSTR)REG_RESOURCE_LIST;
			if(0==_tcsicmp(lpszClass, _T("REG_FULL_RESOURCE_DESCRIPTOR"))) return (LPTSTR)REG_FULL_RESOURCE_DESCRIPTOR;
			if(0==_tcsicmp(lpszClass, _T("REG_QWORD"))) return (LPTSTR)REG_QWORD;
			if(0==_tcsicmp(lpszClass, _T("REG_QWORD_LITTLE_ENDIAN"))) return (LPTSTR)REG_QWORD_LITTLE_ENDIAN;
			return 0;
		};

		bool Create(_In_ LPCTSTR szKeyParentName, _In_z_ LPCTSTR lpszKeyName,
			_In_opt_z_ LPCTSTR lpszClass = _T("REG_NONE"), _In_ DWORD dwOptions = REG_OPTION_NON_VOLATILE,
			_In_ LPCTSTR samDesired = _T("KEY_READ_WRITE")) throw();
		// Open an existing registry key.
		bool Open(LPCTSTR szKeyParentName, LPCTSTR lpszKeyName,	LPCTSTR samDesired =_T("KEY_READ_WRITE")) throw();
		bool Close();

		bool SetStringValue		(_In_opt_z_ LPCTSTR pszValueName, _In_opt_z_ LPCTSTR pszValue, _In_ LPCTSTR szType/* = _T("REG_SZ")*/) throw();
		bool ModifyMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszPattern) throw();

		TString		QueryStringValue		(LPCTSTR pszValueName) throw();
		TString		QueryMultiStringValue	(LPCTSTR pszValueName) throw();
		DWORD		QueryDWORDValue(_In_opt_z_ LPCTSTR pszValueName, DWORD dwDefault = 0) throw();
		ULONGLONG	QueryQWORDValue(_In_opt_z_ LPCTSTR pszValueName, ULONGLONG qwDefault = 0) throw();

		bool DeleteSubKey(LPCTSTR pszKeyName);
		bool RecurseDeleteKey(LPCTSTR pszKeyName);
		bool DeleteValue(LPCTSTR pszValueName);
	};

};//namespace winfunc