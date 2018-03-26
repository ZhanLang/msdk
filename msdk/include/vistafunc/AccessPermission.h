
#pragma once

#include <AccCtrl.h>
#include <AclAPI.h>
#include <Lmcons.h>
class CAccessPermission  
{
public:

	CAccessPermission()
	{

	}
	~CAccessPermission()
	{

	}

	// 使用LocalFree来干掉PSID
	PSID GetSidFromUserName(LPCTSTR lpUserName)
	{
		SID_NAME_USE snu;

		TCHAR	szDomain[_MAX_PATH];
		DWORD	chDomain = _MAX_PATH;

		if(lpUserName)
		{
			DWORD cbSid = 0;
			DWORD cbDomainName = 0;

			LookupAccountName(NULL, lpUserName, NULL, &cbSid, szDomain, &chDomain, &snu);
			// 传递给系统调用的数据区域太小
			if(ERROR_INSUFFICIENT_BUFFER == GetLastError() && cbSid)
			{
				PSID lpSid = LocalAlloc(GPTR, cbSid);

				if(lpSid)
				{
					if( LookupAccountName(NULL, lpUserName, lpSid, &cbSid, szDomain, &chDomain, &snu) )
					{
						return lpSid;
					}
					LocalFree(lpSid);
				}
			}
		}

		return NULL;
	}

	// 从一个SID取得用户名
	BOOL GetUserNameFromSid(PSID pUserSid, LPTSTR szUserName, LPDWORD pcbLen)
	{
		// sanity checks and default value
		if (pUserSid == NULL || szUserName == NULL || pcbLen == NULL || *pcbLen == 0)
			return FALSE;

		SID_NAME_USE   snu;
		TCHAR          szDomain[_MAX_PATH];
		DWORD          chDomain = _MAX_PATH;
		PDWORD         pcchDomain = &chDomain;

		// Retrieve user name and domain name based on user's SID.
		return ::LookupAccountSid(NULL, pUserSid, szUserName, pcbLen, szDomain, pcchDomain, &snu);
	}

	// 注册表-设置许可权限
	BOOL Reg_GrantAccess(HKEY hKey, LPCTSTR lpSubkey, LPCTSTR lpTrusteeName, DWORD AccessPermissions=KEY_ALL_ACCESS)
	{
		LPCTSTR lpRootkey = _ConvRegRootkey(hKey);
		if(lpRootkey)
		{
			TCHAR szObjectName[1024];
			TCHAR szUserName[UNLEN + 1];

			if( lpTrusteeName == NULL )
			{
				DWORD dwSize = sizeof(szUserName);

				GetUserName(szUserName, &dwSize);
			}
			else
				_tcscpy( szUserName, lpTrusteeName );

			_sntprintf(szObjectName, sizeof(szObjectName), _T("%s\\%s"), lpRootkey, lpSubkey);
			return _GrantAccess(szObjectName, SE_REGISTRY_KEY, szUserName, AccessPermissions);
		}

		return FALSE;
	}

	// 注册表-设置拒绝权限
	BOOL Reg_DenyAccess(HKEY hKey, LPCTSTR lpSubkey, LPCTSTR lpTrusteeName, DWORD AccessPermissions=KEY_SET_VALUE)
	{
		LPCTSTR lpRootkey = _ConvRegRootkey(hKey);
		if(lpRootkey)
		{
			TCHAR szObjectName[1024];
			TCHAR szUserName[UNLEN + 1];

			if( lpTrusteeName == NULL )
			{
				DWORD dwSize = sizeof(szUserName);

				GetUserName(szUserName, &dwSize);
			}
			else
				_tcscpy( szUserName, lpTrusteeName );

			_sntprintf(szObjectName, sizeof(szObjectName), _T("%s\\%s"), lpRootkey, lpSubkey);
			return _DenyAccess(szObjectName, SE_REGISTRY_KEY, szUserName, AccessPermissions);
		}

		return FALSE;
	}

	// 文件或目录设置许可权限
	BOOL File_GrantAccess(LPCTSTR lpPath, LPCTSTR lpTrusteeName, DWORD AccessPermissions=FILE_ALL_ACCESS)
	{
		TCHAR szUserName[UNLEN + 1];

		if( lpTrusteeName == NULL )
		{
			DWORD dwSize = sizeof(szUserName);

			GetUserName(szUserName, &dwSize);
		}
		else
			_tcscpy( szUserName, lpTrusteeName );

		return _GrantAccess(lpPath, SE_FILE_OBJECT, szUserName, AccessPermissions);
	}

	// 文件或目录设置拒绝权限
	BOOL File_DenyAccess(LPCTSTR lpPath, LPCTSTR lpTrusteeName, DWORD AccessPermissions=FILE_GENERIC_WRITE)
	{
		TCHAR szUserName[UNLEN + 1];

		if( lpTrusteeName == NULL )
		{
			DWORD dwSize = sizeof(szUserName);

			GetUserName(szUserName, &dwSize);
		}
		else
			_tcscpy( szUserName, lpTrusteeName );

		return _DenyAccess(lpPath, SE_FILE_OBJECT, szUserName, AccessPermissions);
	}

protected:
	// 根据类型或名字删除ACE
	void _DeleteAce(PACL pAcl, ACCESS_MODE AccessMode, LPCTSTR lpTrusteeName)
	{
		if(pAcl)
		{
			ULONG lCount = 0;
			ULONG lDels = 0;
			PEXPLICIT_ACCESS lpea;
			PSID  lpSid = NULL;

			//		if(lpTrusteeName)
			//			lpSid = GetSidFromUserName(lpTrusteeName);

			if( ERROR_SUCCESS == GetExplicitEntriesFromAcl(pAcl, &lCount, &lpea) )
			{
				for(ULONG i=0; i<lCount; ++i)
				{
					if(lpea[i].grfAccessMode == AccessMode )
					{
#if 1
						DeleteAce(pAcl, i-lDels);
						lDels ++;
#else
						if(lpSid)
						{
							if(lpea[i].Trustee.TrusteeForm == TRUSTEE_IS_SID )
							{
								if( EqualSid(GetTrusteeName(&lpea[i].Trustee), lpSid) )
								{
									DeleteAce(pAcl, i-lDels);
									lDels ++;
								}
							}
							else
							{
								// 不可能是其他值
								ATLASSERT(FALSE);
							}
						}
						else
						{
							DeleteAce(pAcl, i-lDels);
							lDels ++;
						}
#endif
					}
				}
			}

			if(lpSid)
				LocalFree(lpSid);
		}
	}

	// 设置权限
	BOOL _SetAccess(LPCTSTR pObjectName, SE_OBJECT_TYPE ObjectType, ACCESS_MODE accessMode, LPCTSTR lpTrusteeName, DWORD AccessPermissions)
	{
		if( pObjectName == NULL ||
			lpTrusteeName == NULL)
			return FALSE;

		BOOL bRet = FALSE;
		PSECURITY_DESCRIPTOR pSD = NULL;
		PACL pOldDacl = NULL; 

		// 取得DACL
		if( ERROR_SUCCESS == GetNamedSecurityInfo(
			(LPTSTR)pObjectName, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDacl, NULL, &pSD) )
		{
			if( accessMode == SET_ACCESS ||
				accessMode == GRANT_ACCESS)
			{
				// 删除访问模式为DENY_ACCESS的名字为lpTrusteeName的权限
				_DeleteAce(pOldDacl, DENY_ACCESS, lpTrusteeName);
			}

			// 创建一个新的ACE
			PACL pNewDacl = NULL;
			EXPLICIT_ACCESS ea;
			ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));

			BuildExplicitAccessWithName(&ea, (LPTSTR)lpTrusteeName, AccessPermissions, accessMode, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

			// 将新的ACE加入DACL
			if( ERROR_SUCCESS == SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl) )
			{
				// 更新DACL
				bRet = (ERROR_SUCCESS == SetNamedSecurityInfo(
					(LPTSTR)pObjectName, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL) );

				if(pNewDacl)
					LocalFree(pNewDacl);
			}

			if(pSD)
				LocalFree(pSD);
		}

		return bRet;
	}

	// 设置拒绝权限
	inline BOOL _DenyAccess(LPCTSTR pObjectName, SE_OBJECT_TYPE ObjectType, LPCTSTR lpTrusteeName, DWORD AccessPermissions)
	{
		return _SetAccess(pObjectName, ObjectType, DENY_ACCESS, lpTrusteeName, AccessPermissions);
	}

	// 设置许可权限
	inline BOOL _GrantAccess(LPCTSTR pObjectName, SE_OBJECT_TYPE ObjectType, LPCTSTR lpTrusteeName, DWORD AccessPermissions)
	{
		return _SetAccess(pObjectName, ObjectType, SET_ACCESS, lpTrusteeName, AccessPermissions);
	}

	inline LPCTSTR _ConvRegRootkey(HKEY hRootkey)
	{
		LPCTSTR lpRootkey = NULL;

		switch((LONG)hRootkey)
		{
		case HKEY_CLASSES_ROOT:
			lpRootkey = _T("CLASSES_ROOT");
			break;

		case HKEY_CURRENT_USER:
			lpRootkey = _T("CURRENT_USER");
			break;

		case HKEY_LOCAL_MACHINE:
			lpRootkey = _T("MACHINE");
			break;

		case HKEY_USERS:
			lpRootkey = _T("USERS");
			break;

		default:
			break;
		}

		return lpRootkey;
	}
};
