// RandRun.cpp: implementation of the CRandRun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RandRun.h"
//#include "CfgMgr.h"

#include <stdlib.h>
#include <atltime.h>

#define WM_RANDRUN (WM_USER+234)
#define WAIT_CHECK_TIMER	( 60*1000 )	//1分钟
//#define WAIT_CHECK_TIMER	60*1000	//5分钟

CRandRun::CRandRun()
{
	m_pStart = NULL;
	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	ZeroMemory(m_szSmartupCmd, MAX_PATH);

	m_bOpenUpdate = FALSE;
	TCHAR szRsAppMgr[MAX_PATH] = {0};
	RsGetCurentPath(szRsAppMgr,MAX_PATH);
	_stprintf_s(m_szNetConfig,MAX_PATH,_T("%s\\NetConfig.ini"), szRsAppMgr);

}

CRandRun::~CRandRun()
{
	if(m_hEventStop)
		CloseHandle(m_hEventStop);
}


BOOL CRandRun::Init(CStartTask*  pStart)
{
	m_pStart = pStart;
	ResetEvent(m_hEventStop);

	if( !CreateMemberThread<CRandRun>(this,&CRandRun::DoRunNewverThread,m_RunNewverThread,0)) 
	{
		RSLOG(RSLOG_ACTION,_T("创建CRandRun Thread失败! \n"));
		return FALSE;
	}

	return TRUE;
}

void CRandRun::UnInit()
{
	SetEvent(m_hEventStop);
	m_RunNewverThread.SetTimeOut(3*1000);
	m_RunNewverThread.TerminateThread();
}

int CRandRun::ReadNetConfigUpdate()
{
	TCHAR szTmp[100];
	int iRc = UPDATE_NO;

	int iReadCount = GetPrivateProfileString(_T("Update"), _T("Time"), _T(""), szTmp, 100, m_szNetConfig);
	if(iReadCount > 0)
	{
		if(iReadCount >= 19)
		{
			SYSTEMTIME lastUpdateTime;
			CTime	timeSystem, timeLastUpdate;

			timeSystem = CTime::GetCurrentTime();

			ConvertToSystemtime(szTmp,&lastUpdateTime);
			//sscanf(szTmp, "%d-%d-%d %d:%d:%d"
			//	, &lastUpdateTime.wYear
			//	, &lastUpdateTime.wMonth
			//	, &lastUpdateTime.wDay
			//	, &lastUpdateTime.wHour
			//	, &lastUpdateTime.wMinute
			//	, &lastUpdateTime.wSecond);
			timeLastUpdate = CTime(lastUpdateTime);

			CTimeSpan timeSpan = timeSystem - timeLastUpdate;
			DWORD dwValue =(DWORD) timeSpan.GetTotalSeconds();
			if(dwValue >= GetPrivateProfileInt(_T("Update"), _T("IntervalTime"), 0, m_szNetConfig))
			{
				//已经到时间了
				iRc = UPDATE_NOW;
				WritePrivateProfileString(_T("Update"), _T("Time"), _T(""), m_szNetConfig);
				WritePrivateProfileString(_T("Update"), _T("IntervalTime"), _T(""), m_szNetConfig);
			}
			else
			{
				iRc = UPDATE_WAIT;
			}
		}
		else
		{
			WritePrivateProfileString(_T("Update"), _T("Time"), _T(""), m_szNetConfig);
			WritePrivateProfileString(_T("Update"), _T("IntervalTime"), _T(""), m_szNetConfig);
			iRc = UPDATE_NO;
		}
	}
	return iRc;
}



BOOL CRandRun::SetRandRun(LPCTSTR lpszCmd)
{
	if (NULL == lpszCmd) 
		return FALSE;

	TCHAR* lpTemp = new TCHAR[_tcslen(lpszCmd)+1];
	_tcsnccpy_s(lpTemp,_tcslen(lpszCmd)+1,lpszCmd,_tcslen(lpszCmd));
	
	m_RunNewverThread.PostThreadMessage(WM_RANDRUN,(WPARAM)(LPVOID)lpTemp,NULL);

	return TRUE;
}

BOOL CRandRun::SetSmartupCmd(LPCTSTR lpszCmd, BOOL bOpen)
{
	ZeroMemory(m_szSmartupCmd, MAX_PATH);
	_tcsnccpy_s(m_szSmartupCmd,MAX_PATH,lpszCmd,_tcslen(lpszCmd)+1);

	m_bOpenUpdate = bOpen;
	return TRUE;
}


void CRandRun::DoRunNewverThread()
{
	DWORD dwResult;
	MSG msg;	
	
	HANDLE hStop[1];
	hStop[0] = m_hEventStop;

	RANDRUNINFOLIST RandRunList;

	DWORD dwTickCount = 0;//准备给当netconfig的升级时间为空的时候自动一小时一次
	DWORD dwCheckTimer = GetTickCount();
	
	while(TRUE)
	{
		dwResult =  MsgWaitForMultipleObjects(1,hStop,FALSE,1000,QS_ALLEVENTS); 
		
		if (WAIT_TIMEOUT == dwResult)
        {
			if(m_bOpenUpdate && (GetTickCount()-dwCheckTimer >= WAIT_CHECK_TIMER))//需要有个时间间隔去检测netconfig.ini
			{
				dwCheckTimer = GetTickCount();

				BOOL bNeedUpdate = FALSE;
				//判断是否需要按网络返回时间升级 add by ysb 07/08/06
				int iUpdateType = ReadNetConfigUpdate();
				switch(iUpdateType)
				{
				case CRandRun::UPDATE_NOW://需要升级了
					{
						bNeedUpdate = TRUE;
					}
					break;
				case CRandRun::UPDATE_NO:
					if(0 == dwTickCount)
					{
						dwTickCount = GetTickCount();
					}
					else
					{
						if((GetTickCount() - dwTickCount) > 30*60*1000)//半小时到了
						{
							bNeedUpdate = TRUE;
						}
					}
					break;
				case CRandRun::UPDATE_WAIT:
				default:
					dwTickCount = 0;
					break;
				}

				if(bNeedUpdate)
				{
					dwTickCount = 0;
					if(_tcslen(m_szSmartupCmd) > 0)
					{
						RANDRUNINFO info = {0};
						_tcsnccpy_s(info.szCmd,MAX_PATH,m_szSmartupCmd,_tcslen(m_szSmartupCmd));

						//算随机数据
						srand(GetTickCount());						
						info.nCount = (int)(((DWORD)rand()) % 60);
						RandRunList.push_back(info);

					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			RANDRUNINFOLIST::iterator endIT = RandRunList.end();
			RANDRUNINFOLIST::iterator beginIT = RandRunList.begin();
			
			while( beginIT != endIT)
			{
				BOOL bCancelRun = FALSE;
				RANDRUNINFO& theRef = *beginIT;

				if(0 == _tcsncmp(theRef.szCmd, m_szSmartupCmd, _tcslen(m_szSmartupCmd)))//是升级
				{
					if(m_bOpenUpdate //不是即时升级是可以的，也不需要读netconfig的配置
						&& CRandRun::UPDATE_NO != ReadNetConfigUpdate())//只有在配置里没有下次启动时间的时候才生效
					{
						bCancelRun = TRUE;
					}
				}

				if(bCancelRun)
				{
					RandRunList.erase(beginIT++);
					break;
				}
				else
				{
					if( --(theRef.nCount) <= 0)
					{
						//extern CCenter g_CCenter;
						//g_CCenter.StartProcess(START_PROGRAM_INDEX_SMARTUP, theRef.szCmd, g_cfg.GetAppPath());
						if(m_check.CheckExplorer() && m_pStart)
						{
							m_pStart->StartProcess(theRef.szCmd,NULL);
						}
						else{
							//RSLOG(RSLOG_ACTION,"到了即时升级任务的时间,但未登陆,不启动!");
						}
						RandRunList.erase(beginIT++);

					}
					else
						++beginIT;
				}
			}
        }
		else if (WAIT_OBJECT_0 +1 == dwResult) 
		{
			if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{	
				if (msg.message == WM_RANDRUN) 
				{
					if (msg.wParam) 
					{
						TCHAR* lpTemp = (TCHAR*)(LPVOID)msg.wParam;
						RANDRUNINFO info = {0};
						lstrcpy(info.szCmd,lpTemp);
						delete[] lpTemp;
						
						//算随机数据
						srand(GetTickCount());						
						info.nCount = (int)(((DWORD)rand()) % 60);
						RandRunList.push_back(info);

					}
				}

				DispatchMessage(&msg);
			}		
		}
		else if (WAIT_OBJECT_0 == dwResult)
		{
			return;
		}		
	}
}