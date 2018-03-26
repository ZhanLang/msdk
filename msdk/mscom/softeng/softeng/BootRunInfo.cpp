#include "StdAfx.h"
#include "BootRunInfo.h"
using namespace BootRunInfo;

#include "Sddl.h"
#include <lm.h>

#include "comutil.h"

#include "shobjidl.h"

#include <initguid.h>
#include <Mstask.h>

#pragma comment(lib, "taskschd.lib")
#include <ShellAPI.h>
#include <regex>


#define MAX_VALUENAME		128

CBootRunInfo::CBootRunInfo(void)
{
	GetAllUserSid();
}

CBootRunInfo::~CBootRunInfo(void)
{
	Reset();
}

void CBootRunInfo::Reset()
{
	if(!m_vtBootItems.empty())
	{
		std::vector<PST_BootRunItem>::iterator ite;
		for (ite=m_vtBootItems.begin(); ite!=m_vtBootItems.end(); ite++)
		{
			PST_BootRunItem p = *ite;
			delete p;
		}
		m_vtBootItems.clear();
	}

}

BOOL CBootRunInfo::TestUserSid(WCHAR * strSid)
{
	//测试在 HKEY_USERS 下,是否对应的 SID 项

	if(!strSid)
		return FALSE;

	LONG	lRet = 0;
	HKEY	hOpenKey = NULL;

	lRet = RegOpenKeyExW(CPeApi::GetRootKey(HKEY_USERS), CPeApi::GetFullRegPath( HKEY_USERS, strSid), 0, KEY_READ, &hOpenKey);
	if(ERROR_SUCCESS != lRet)
	{
		return FALSE;
	}

	if(hOpenKey)
		RegCloseKey(hOpenKey);
	return TRUE;
}

void CBootRunInfo::GetAllUserSid()
{
	//////////////////////////////////////////////////////////////////////////
	HMODULE hNetapi = NULL;
	hNetapi = ::LoadLibrary(_T("Netapi32.dll"));
	if(NULL == hNetapi)
		return;

	typedef NET_API_STATUS (NET_API_FUNCTION * NETUSERENUM)(
		__in     LPCWSTR servername,
		__in     DWORD level,
		__in     DWORD filter,
		__out    LPBYTE* bufptr,
		__in     DWORD prefmaxlen,
		__out    LPDWORD entriesread,
		__out    LPDWORD totalentries,
		__inout  LPDWORD resume_handle
		);

	typedef NET_API_STATUS (NET_API_FUNCTION * NETAPIBUFFERFREE)(
		IN LPVOID Buffer
		);

	NETUSERENUM FnNetUserEnum = NULL;
	FnNetUserEnum = (NETUSERENUM)GetProcAddress(hNetapi, ("NetUserEnum"));
	if(!FnNetUserEnum)
		goto err_return;

	NETAPIBUFFERFREE FnNetApiBufferFree = NULL;
	FnNetApiBufferFree = (NETAPIBUFFERFREE)GetProcAddress(hNetapi, ("NetApiBufferFree"));
	if(!FnNetApiBufferFree)
		goto err_return;

	//////////////////////////////////////////////////////////////////////////

	LPUSER_INFO_0 pBuf = NULL;
	LPUSER_INFO_0 pTmpBuf;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	DWORD i;
	DWORD dwTotalCount = 0;
	NET_API_STATUS nStatus;
	LPWSTR pszServerName = L"\\\\127.0.0.1";

	do 
	{
		nStatus = FnNetUserEnum(pszServerName,
			dwLevel,
			FILTER_NORMAL_ACCOUNT, // global users
			(LPBYTE*)&pBuf,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);

		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				for (i = 0; (i < dwEntriesRead); i++)
				{
					//	assert(pTmpBuf != NULL);

					if (pTmpBuf == NULL)
					{
						break;
					}

					// #if defined(UNICODE) || defined(_UNICODE)
					// 					RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | UserName: %s"), pTmpBuf->usri0_name);
					// #else
					// 					USES_CONVERSION;
					// 					RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | UserName: %s"), W2A(pTmpBuf->usri0_name));
					// #endif					

					//////////////////////////////////////////////////////////////////////////
					PSID psid = NULL;
					DWORD szpsid = 0;
					WCHAR szReferencedDomainName[256] = {0};
					DWORD szRefDomainName = 256;
					SID_NAME_USE sid_name_use = SID_NAME_USE(1);
					int nRet = 0;

					nRet = LookupAccountNameW(NULL, pTmpBuf->usri0_name, psid, &szpsid, szReferencedDomainName, &szRefDomainName, &sid_name_use);
					//	RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | LookupAccountNameW - 1 | Ret:%d  LastErr:%u"), nRet, GetLastError());
					if(szpsid>0)
					{
						//		RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | LookupAccountNameW OK - 1"));
						psid = new BYTE[szpsid];
						if(psid)
						{
							nRet = LookupAccountNameW(NULL, pTmpBuf->usri0_name, psid, &szpsid, szReferencedDomainName, &szRefDomainName, &sid_name_use);
							//		RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | LookupAccountNameW - 2 | Ret:%d  LastErr:%u"), nRet, GetLastError());
							if(nRet)
							{
								//		RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | LookupAccountNameW OK - 2"));
								if(IsValidSid(psid) && SidTypeUser == sid_name_use)
								{
									//		RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | IsValidSid OK "));
									WCHAR * pStrSid = NULL;
									if(ConvertSidToStringSidW(psid, &pStrSid))
									{
										// #if defined(UNICODE) || defined(_UNICODE)
										// 										RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | Get Sid: %s"), pStrSid);
										// #else
										// 										USES_CONVERSION;
										// 										RSLOG(RSLOG_ACTION, _T("[KRFC] CKernelReinforce::GetAllUserSid | Get Sid: %s"), W2A(pStrSid));
										// #endif

										if(TestUserSid(pStrSid))
										{
											//	m_mapSids[std::wstring(pStrSid)] = i;
											//	m_vectSid.push_back(pStrSid);

											ST_UserInfo userinfo;
											userinfo.strUserName = std::wstring(pTmpBuf->usri0_name);
											userinfo.strSID = std::wstring(pStrSid);
											userinfo.dwIndex = i;
											m_vtUserInfo.push_back(userinfo);

#ifdef _DEBUG
											//TCHAR szdbg[512] = {0};
											//wsprintf(szdbg, _T("[UserInfo][0x%08x] %s | %s\n"), userinfo.dwIndex, userinfo.strSID.c_str(), userinfo.strUserName.c_str());
											//::OutputDebugString(szdbg);
#endif

										}
										LocalFree(pStrSid);
									}
								}
							}
							delete [] psid;
						}
					}

					pTmpBuf++;
					dwTotalCount++;
				}
			}
		}
		else
		{
			GetSidBySpecialWay();
			//找注册表
			GetLastError();
		}

		if (pBuf != NULL)
		{
			FnNetApiBufferFree(pBuf);
			pBuf = NULL;
		}

	} while (nStatus == ERROR_MORE_DATA);

	//////////////////////////////////////////////////////////////////////////
err_return:
	::FreeLibrary(hNetapi);
}

//[高峰 2013/1/11]
//采取一个投机取巧的办法查找sid
//那就是遍历HKEY_USERS下的所有项
//符合正则表达式规则的名字即认为是sid
void CBootRunInfo::GetSidBySpecialWay()
{
	CRegKey enumKey;
	if (ERROR_SUCCESS != enumKey.Open(CPeApi::GetRootKey(HKEY_USERS), NULL))
	{
		return;
	}

	UINT nIndex = 0;
	for (;;)
	{
		TCHAR   keyName[MAX_PATH];
		DWORD	keyLen = MAX_PATH;
		if(ERROR_SUCCESS != enumKey.EnumKey(nIndex++, keyName, &keyLen))
			break;

		//标准用户sid串规则
		std::tr1::wregex re_sid(L"S(-[0-9]+){7}"); 
		//szKeyName是待匹配串
		if(regex_match(keyName, re_sid))
		{
			//认为匹配了
			ST_UserInfo sUser;
			sUser.strSID = keyName;
			m_vtUserInfo.push_back(sUser);
		}
	}

}


void CBootRunInfo::Nt6Enum(ITaskFolder* pRootFolder)
{
	ATLASSERT(pRootFolder);

	CComPtr<ITaskFolderCollection> pSubFolders = NULL;
	if (SUCCEEDED(pRootFolder->GetFolders(0, &pSubFolders)) && pSubFolders)
	{
		LONG nCount = 0;
		if (SUCCEEDED(pSubFolders->get_Count(&nCount)))
		{
			for (LONG nTemp = 0; nTemp < nCount; nTemp++)
			{
				CComPtr<ITaskFolder> pSubFolder = NULL;
				if (SUCCEEDED(pSubFolders->get_Item(CComVariant(nTemp + 1), &pSubFolder)) && pSubFolder)
				{
					Nt6Enum(pSubFolder);
				}
			}
		}
	}

	CComPtr<IRegisteredTaskCollection> pRegTaskCollection = NULL;
	HRESULT hr = pRootFolder->GetTasks(TASK_ENUM_HIDDEN, &pRegTaskCollection);

	if (FAILED(hr) || pRegTaskCollection == NULL)
	{
		return;
	}

	LONG nNumOfTasks = 0;
	hr = pRegTaskCollection->get_Count(&nNumOfTasks);

	if (FAILED(hr) || 0 == nNumOfTasks)
	{
		return;
	}

	for(LONG nTemp = 0; nTemp < nNumOfTasks; nTemp++)
	{
		CComPtr<IRegisteredTask> pRegTask;
		hr = pRegTaskCollection->get_Item(CComVariant(nTemp + 1), &pRegTask);
		if (FAILED(hr) || pRegTask == NULL)
		{
			continue;
		}

		CComPtr<ITaskDefinition> pTaskDef;
		if (FAILED(pRegTask->get_Definition(&pTaskDef)) || !pTaskDef)
		{
			continue;
		}

		CComPtr<IActionCollection> pActions = NULL;
		if (FAILED(pTaskDef->get_Actions(&pActions)) || pActions == NULL)
		{
			continue;
		}

		LONG nCount = 0;
		pActions->get_Count(&nCount);

		CString strImage(_T(""));
		for (LONG nTemp = 0; nTemp < nCount; nTemp++)
		{
			CComPtr<IAction> pAction = NULL;
			if (FAILED(pActions->get_Item(nTemp + 1, &pAction)) || pAction == NULL)
			{
				continue;
			}

			CComPtr<IExecAction> pExeAction = NULL;
			TASK_ACTION_TYPE actType;
			if (SUCCEEDED(pAction->get_Type(&actType)) && actType == TASK_ACTION_EXEC)
			{
				if (SUCCEEDED(pAction->QueryInterface(IID_IExecAction, (void**)&pExeAction)))
				{
					CComBSTR bstrPath;
					pExeAction->get_Path(&bstrPath);
					strImage = bstrPath;
					::DoEnvironmentSubst(strImage.GetBufferSetLength(MAX_PATH), MAX_PATH);
					strImage.ReleaseBuffer();
				}

				break;
			}
		}

		if(strImage.IsEmpty())
			continue;

		PST_BootRunItem pbrItem = new ST_BootRunItem(LPCWSTR(strImage), NULL, NULL, br_type_task, -1);
		if(pbrItem)
		{
#ifdef _DEBUG
			WCHAR szdbg[1024] = {0};
			wsprintf(szdbg, _T("[RunKey][User:0x%08x][Ver:%s][Publisher:%s] %s\n"), pbrItem->dwUser, pbrItem->strVer.c_str(), pbrItem->strPublisher.c_str(), pbrItem->strProName.c_str());
			::OutputDebugString(szdbg);
#endif
			m_vtBootItems.push_back(pbrItem);
		}
	}
}

void CBootRunInfo::EnumTaskOnNt6()
{
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
	{
		return;
	}

	hr = ::CoInitializeSecurity(NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		0,
		NULL);

	CComPtr<ITaskService> pTaskService = NULL;
	hr = ::CoCreateInstance(CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)&pTaskService);  
	if (FAILED(hr) || pTaskService == NULL)
	{
		::CoUninitialize();
		return;
	}

	hr = pTaskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
	{
		::CoUninitialize();
		return;
	}

	CComPtr<ITaskFolder> pRootFolder = NULL;
	hr = pTaskService->GetFolder(CComBSTR(L"\\") , &pRootFolder);

	if (FAILED(hr) || pRootFolder == NULL)
	{
		::CoUninitialize();
		return;
	}

	Nt6Enum(pRootFolder);

	::CoUninitialize();
}
