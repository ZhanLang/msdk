////////////////////////////////////////////////////////////////////////////////
/**
* @file
* 文件简要说明
* <p>项目名称：瑞星程序开发库
* <br>文件名称：listsession.h
* <br>实现功能：多session的功能
* <br>作    者：onlyu
* <br>编写日期：2008-7-19
* <br>
* <br>修改历史（自下而上填写 内容包括：日期  修改人  修改说明）
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _LISTSESSIONS_INCLUDE_H_
#define _LISTSESSIONS_INCLUDE_H_

#pragma once

#include <Wtsapi32.h>
#include <util\callapi.h>

#include <vector>
using namespace std;

#pragma comment(lib, "Wtsapi32.lib")

class CEnumSessions
{
public:
	class CWtsapi32 : public msdk::tImpModuleMid<CWtsapi32>
	{
	public:	
		HANDLE (WINAPI *WTSOpenServerA)(IN LPSTR pServerName);
		BOOL (WINAPI *WTSEnumerateSessionsA)(IN HANDLE hServer, IN DWORD Reserved, IN DWORD Version, OUT PWTS_SESSION_INFOA * ppSessionInfo, OUT DWORD * pCount);
		BOOL (WINAPI *WTSQuerySessionInformationA)(IN HANDLE hServer, IN DWORD SessionId, IN WTS_INFO_CLASS WTSInfoClass, OUT LPSTR * ppBuffer, OUT DWORD * pBytesReturned);
		VOID (WINAPI *WTSFreeMemory)(IN PVOID pMemory);
		VOID (WINAPI *WTSCloseServer)(IN HANDLE hServer);

		DECLARE_FUN_BEGIN(CWtsapi32, "Wtsapi32.dll")
		DECLARE_FUN(WTSOpenServerA)
		DECLARE_FUN(WTSEnumerateSessionsA)
		DECLARE_FUN(WTSQuerySessionInformationA)
		DECLARE_FUN(WTSFreeMemory)
		DECLARE_FUN(WTSCloseServer)
		DECLARE_FUN_END()
	}m_hWtsapi32;

	vector<int> m_listSid;
public:
	CEnumSessions()
	{
		m_hWtsapi32.Load();
	}
	~CEnumSessions()
	{

	}

	VOID ClearSession()
	{

	}
	HRESULT ListSessionID()
	{
		m_listSid.clear();

		// open a handle to the server
		//hOpenTS = WTSOpenServer("servername");
		HANDLE hOpenTS = m_hWtsapi32.WTSOpenServerA(NULL);	
		if(hOpenTS != NULL)
		{		
			PWTS_SESSION_INFOA ppSessionInfo;
			DWORD dwPCount;
			
			BOOL bWTSEnumSess;
			BOOL bWTSQuerySess;			
			LPSTR ppBuffer = NULL;			
			DWORD dwppBuffer = 0 ;
			bWTSEnumSess = m_hWtsapi32.WTSEnumerateSessionsA(hOpenTS, 0,1, &ppSessionInfo, &dwPCount);
			if(bWTSEnumSess != 0)
			{				
				// for each session				
				for(DWORD i = 0; i<dwPCount;i++)
				{
					bWTSQuerySess = m_hWtsapi32.WTSQuerySessionInformationA(hOpenTS, i, WTSSessionId, &ppBuffer, &dwppBuffer);
					DWORD dwErr;
					dwErr = GetLastError();
					
					// if query session succeeded and the session is found
					// Q186548 Terminal Server Error Messages: 3900 to 7999
					if((bWTSQuerySess != 0) && (dwErr != 7022))
					{						
						m_listSid.push_back(*(int*)ppBuffer);					
					}//if((bWTSQuerySess != 0) && (dwErr != 7022))					
					else
					{
						DWORD dwErr;
						dwErr = GetLastError();						
						
						m_hWtsapi32.WTSFreeMemory(ppBuffer);
						m_hWtsapi32.WTSCloseServer( hOpenTS);
						
						return (GetLastError());
					}					
				}//for
			}//if(bWTSEnumSess != 0)
			else
			{			
				m_hWtsapi32.WTSFreeMemory(ppBuffer);
				m_hWtsapi32.WTSCloseServer( hOpenTS);
				
				return (GetLastError());
			}// everything worked just fine.. clean up and say Okay..

			m_hWtsapi32.WTSFreeMemory(ppBuffer);
			m_hWtsapi32.WTSCloseServer( hOpenTS);
			
			return S_OK;
		}	//if(hOpenTS != NULL)
		else
		{			
			return (GetLastError());
		}
	}
};


#endif // _LISTSESSIONS_INCLUDE_H_
