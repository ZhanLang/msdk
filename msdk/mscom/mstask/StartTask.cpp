#include "StdAfx.h"
#include "starttask.h"
#include "gtasks.h"
//#include "irfwlog.h"
//#include "strsafe.h"
#include "atlstr.h"

typedef DWORD (WINAPI * FnGetActiveConsoleSessionId)();
typedef  BOOL (WINAPI *FnDuplicateTokenEx)(
	           HANDLE hExistingToken,
			   DWORD dwDesiredAccess,
			   LPSECURITY_ATTRIBUTES lpTokenAttributes,
			   SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
			   TOKEN_TYPE TokenType,
			   PHANDLE phNewToken);
typedef  BOOL (WINAPI * FnOpenProcessToken)(
	           HANDLE ProcessHandle,
			   DWORD DesiredAccess,
			   PHANDLE TokenHandle );
typedef  BOOL (WINAPI * FnSetTokenInformation)(
	           HANDLE TokenHandle,
			   TOKEN_INFORMATION_CLASS TokenInformationClass,
			   LPVOID TokenInformation,
			   DWORD TokenInformationLength );
typedef  BOOL (WINAPI *FnDuplicateToken)(
	           HANDLE hExistingToken,
			   SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
			   PHANDLE phNewToken);

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CStartTask::StartDllProgram(TCHAR* aszAppName,DWORD dwSpecific)
{
	RASSERTP(aszAppName,FALSE);
	HMODULE lhDll = NULL;

	if (dwSpecific==0)
	{
		TCHAR lszFunName[128] = {0};//函数的名称
		TCHAR lszFileName[128] = {0};//文件的名称
		TCHAR lszDllFilePath[MAX_PATH+20] = {0};//的的全路径
		TCHAR *lpTag = NULL;
		//拆出文件名称
		lpTag =  _tcschr(aszAppName,_T(';'));
		RASSERTP(lpTag,FALSE);

		_tcsncpy_s(lszFileName,128,aszAppName,lpTag - aszAppName);
		aszAppName = lpTag + 1;
		_stprintf_s(lszDllFilePath,_T("%s"),lszFileName);	

		//加载DLL文件
		//strcpy(lszDllFilePath,lszDllFilePath+1);
		lhDll = ::LoadLibrary(lszDllFilePath);
		RASSERTP(lhDll,FALSE);

		typedef void (WINAPI *PFUN_RAV_LOG_DLL)(UINT);
		PFUN_RAV_LOG_DLL lpFun = NULL;
		lstrcpy(lszFunName,aszAppName);
		//得到函数,然后启动
#ifdef _UNICODE
		USES_CONVERSION;
		lpFun = (PFUN_RAV_LOG_DLL)GetProcAddress(lhDll,W2A(lszFunName));
#else
		lpFun = (PFUN_RAV_LOG_DLL)GetProcAddress(lhDll,lszFunName);
#endif
		if(lpFun)
		{
			lpFun(0);
		}
	}
	else
	{  //防火墙特殊化，以后让他们该掉
		//lhDll = LoadLibrary(_T("rfwlog.dll"));
		//if(NULL == lhDll)
		//{
		//	RSLOG(RSLOG_ALERT, "LoadLibrary rfwlog.dll Error=%d",GetLastError());
		//	return FALSE;
		//}
		//HRESULT (STDAPICALLTYPE *pDllGetClassObject)(IN REFCLSID , IN REFIID , OUT LPVOID FAR* ) = 0;
		//(FARPROC&)pDllGetClassObject = GetProcAddress(lhDll, "DllGetClassObject");
		//if(!pDllGetClassObject)
		//	return FALSE;
		//{
		//	CComPtr<IRSClassFactory> iCFact;
		//	pDllGetClassObject(CLSID_IRSClassFactory, __uuidof(IRSClassFactory), (void**)&iCFact);
		//	CComPtr<IRfwDELLog> iRfwDel;

		//	iCFact->CreateInstance(NULL,__uuidof(IRfwDELLog),(void**)&iRfwDel);
		//	if (iRfwDel)
		//	{
		//		iRfwDel->TimeClean(0); //删除11天之前的日志		
		//	}
		//	iRfwDel.Release();
		//	iCFact.Release();
		//}
	}
	if(lhDll)
	FreeLibrary(lhDll);
	return TRUE;
}

//---------------------------------------------------------------
//----------------------------------------------------------------
typedef struct ThreadPara{
	TCHAR  szPath[MAX_PATH];
	TCHAR  szCmdLine[MAX_PATH+100];
	DWORD  dwType;
	DWORD  dwIndex;
	DWORD  dwIsStop;
	DWORD  dwRight;
	DWORD  dwSessionID;
	DWORD  dwClassID;
	CGTasks *  lpClass;
	CStartTask *lpThis;
}ThreadPara;

#define WM_THREAD_QUIT  (WM_USER+0x800)
#define WM_OUR_QUIT     (WM_USER+0x801)

CStartTask::CStartTask(void)
{
	RsGetCurentPath(m_szPath,MAX_PATH);
	m_bXpAbove = IsXPAbove();
	//m_iLang = (IUnknown*)NULL;
	//if(m_cldr.LoadRSCom(_T("rslang.dll")))
	//	 m_cldr.CreateInstance(NULL, __uuidof(IRsLanguage2), (void**)&m_iLang);
}

CStartTask::~CStartTask(void)
{
}
DWORD CStartTask::TaskProcessXP(LPVOID lpParameter)
{

	ThreadPara *lpData = (ThreadPara* )lpParameter;
	RASSERTP(lpData,E_FAIL);

	BOOL             bReturnVal = FALSE;

	STARTUPINFO		 stStartUpInfo;
	ZeroMemory(&stStartUpInfo, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.lpDesktop  = _T("WinSta0\\Default");      // Default desktop
	stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = SW_SHOWNORMAL;

	PROCESS_INFORMATION		stProcessInfo;
	stProcessInfo.hProcess=NULL;
	stProcessInfo.hThread=NULL;

	SECURITY_DESCRIPTOR     sDesc = {0};
	SECURITY_ATTRIBUTES     sa;
	LPSECURITY_ATTRIBUTES  lpAttributes = NULL;
	DWORD dwExplorerID = 0;

	//---------------------------------------------------------
	DWORD dwActiveSessionID = 0;

	FnGetActiveConsoleSessionId pGetActiveSessionId = (FnGetActiveConsoleSessionId) GetProcAddress(
		GetModuleHandle(_T("kernel32.dll")), "WTSGetActiveConsoleSessionId");
	FnDuplicateTokenEx pDuplicateTokenEx = (FnDuplicateTokenEx) GetProcAddress(
		GetModuleHandle(_T("Advapi32.dll")), "DuplicateTokenEx");

	//FnDuplicateToken pDuplicateToken = (FnDuplicateToken) GetProcAddress(
	//	GetModuleHandle("Advapi32.dll"), "DuplicateToken");
	FnOpenProcessToken pOpenProcessToken = (FnOpenProcessToken) GetProcAddress(
		GetModuleHandle(_T("Advapi32.dll")), "OpenProcessToken");
	FnSetTokenInformation pSetTokenInformation = (FnSetTokenInformation) GetProcAddress(
		GetModuleHandle(_T("Advapi32.dll")), "SetTokenInformation");

	//RSLOG(RSLOG_ACTION, "TaskProcessXP WTSGetActiveConsoleSessionId=%d,DuplicateTokenEx=%d ,OpenProcessToken=%d ,SetTokenInformation=%d \n",
	//	pGetActiveSessionId,pDuplicateTokenEx,pOpenProcessToken,pSetTokenInformation);

	dwActiveSessionID = 0;
	if(lpData->dwSessionID == 0 && pGetActiveSessionId)
	{
		dwActiveSessionID = pGetActiveSessionId();//dwActiveSessionID不是本session!
		if(dwActiveSessionID == 0xFFFFFFFF)
		{
			dwActiveSessionID = 0;
		}
	}
	else
	{
		dwActiveSessionID = lpData->dwSessionID ;
	}

	if(lpData->dwRight == 0)
	{
		//取活动Session下的explorer.exe ID
		DWORD dwTest = m_check.CheckExplorer(dwActiveSessionID); 
		//RSLOG(RSLOG_ALERT, "dwRight=0: explorer id=%d,session id=%d", dwTest,dwActiveSessionID);

		if(dwTest == 0) 
		{	
			//没取到,搜一个活动的explorer.exe ID
			dwTest  = m_check.GetActiveExplorer(dwActiveSessionID);
			//RSLOG(RSLOG_ALERT, "dwRight=0 22: explorer id=%d,session id=%d", dwTest,dwActiveSessionID);
			if(dwTest == 0)
			{
				RSLOG(RSLOG_ALERT, _T("TaskProcessXP : InitializeSecurityDescriptor FAILED, Err(%d)"), GetLastError());
				return FALSE;
			}
			//当前活动的session下没有explorer,改用搜到explorer启动
			dwExplorerID  = dwTest;
		}

		if (!InitializeSecurityDescriptor(&sDesc, SECURITY_DESCRIPTOR_REVISION))
		{
			RSLOG(RSLOG_ALERT,_T("TaskProcessXP : InitializeSecurityDescriptor FAILED, Err(%d)\n"), GetLastError());
			return FALSE;
		}

		if (!SetSecurityDescriptorDacl(&sDesc, TRUE, (PACL) NULL, FALSE))	  
		{
			RSLOG(RSLOG_ALERT, _T("TaskProcessXP : SetSecurityDescriptorDacl FAILED : Err(%d)"), GetLastError());
			return FALSE;
		}
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sDesc;
		sa.bInheritHandle = TRUE;
		lpAttributes = &sa;
	}
	else if(lpData->dwRight ==1)
	{
		//取活动Session下的explorer.exe ID
		dwExplorerID = m_check.CheckExplorer(dwActiveSessionID); 
		//RSLOG(RSLOG_ALERT, "TaskProcessXP:explorer id=%d,session id=%d", dwExplorerID,dwActiveSessionID);

		if(dwExplorerID == 0) 
		{	
			//没取到,搜一个活动的explorer.exe ID
			dwExplorerID  = m_check.GetActiveExplorer(dwActiveSessionID);
			//RSLOG(RSLOG_ALERT, "TaskProcessXP2:explorer id=%d,session id=%d", dwExplorerID,dwActiveSessionID);

			if(dwExplorerID == 0 )
			{			
				//RSLOG(RSLOG_ALERT, "TaskProcessXP :没发现explorer,任务没法启动: Err(%d)", GetLastError());
				return FALSE;
			}
		}
		lpAttributes = NULL;
	}


	if( pDuplicateTokenEx
		&& pOpenProcessToken
		&& pSetTokenInformation
		)
	{
		HANDLE hTokenThis = NULL;
		HANDLE hTokenDup = NULL;
		HANDLE hThisProcess = 0; //GetCurrentProcess();
		if(dwExplorerID==0)
		{
			hThisProcess = GetCurrentProcess();
		}
		else
		{
			hThisProcess = OpenProcess (PROCESS_ALL_ACCESS, //PROCESS_QUERY_INFORMATION
				FALSE, dwExplorerID); 
		}

		BOOL bRet = FALSE;
		bRet = pOpenProcessToken(hThisProcess, TOKEN_ALL_ACCESS, &hTokenThis);

		BOOL bRet2 =  pDuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, 
			TokenPrimary, &hTokenDup);
	
		if(bRet && bRet2)
		{
			//if(((DWORD)(sessionid))!=dwActiveSessionID)
			{
				//设置Session ID，如果设置不成功，就会用本进程Session
				bRet = pSetTokenInformation(hTokenDup, TokenSessionId, &dwActiveSessionID, sizeof(DWORD));
				//RSLOG(RSLOG_ACTION, "SetTokenInformation return value:%d,  errCode:%d\n",  bRet, GetLastError());
			}

			LPVOID pEnv = NULL;
			DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

			LPVOID lpEnvironment; 
			HINSTANCE hDllInst = LoadLibrary(_T("Userenv.DLL"));
			if(hDllInst	)
			{
				typedef DWORD (WINAPI * FnCreateEnvironmentBlock)(LPVOID, HANDLE, BOOL);
				typedef DWORD (WINAPI * FnDestroyEnvironmentBlock)(LPVOID);

				FnCreateEnvironmentBlock pCreateEnvironmentBlock = (FnCreateEnvironmentBlock) GetProcAddress(
					hDllInst, "CreateEnvironmentBlock");
				FnDestroyEnvironmentBlock pDestroyEnvironmentBlock = (FnDestroyEnvironmentBlock) GetProcAddress(
					hDllInst, "DestroyEnvironmentBlock");

				RSLOG(RSLOG_ACTION, _T("CreateEnvironmentBlock:%d,  DestroyEnvironmentBlock:%d\n"), 
					pCreateEnvironmentBlock, pDestroyEnvironmentBlock);

				if( pCreateEnvironmentBlock
					&& pDestroyEnvironmentBlock
					&& pCreateEnvironmentBlock(&lpEnvironment, hTokenThis, FALSE))
				{

					DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
					BOOL bRet = CreateProcessAsUser(hTokenDup, NULL, lpData->szCmdLine, lpAttributes, lpAttributes, FALSE,
						dwCreationFlag, lpEnvironment, lpData->szPath, &stStartUpInfo, &stProcessInfo);
				
					RSLOG(RSLOG_ACTION, _T("Task启动程序 CreateProcessAsUser=%s,result=%d"),lpData->szCmdLine,bRet);
					if(bRet)
					{
						if(lpData->lpClass)
						{
							if(lpData->dwIsStop == 0)
							{
								SYSTEMTIME lnowTime;
								memset(&lnowTime,0,sizeof(lnowTime));
								GetLocalTime(&lnowTime);
								lpData->lpClass->ModifyTime(lpData->dwIndex,&lnowTime);
								lpData->lpClass->ModifyRunStaus(lpData->dwType, lpData->dwIndex,(DWORD)stProcessInfo.hProcess);
							}
							else{
								lpData->lpClass->ModifyRunStaus(lpData->dwType, lpData->dwIndex,0);
							}
						}

						if(lpData->lpClass && lpData->dwClassID != RS_RAVTASK_CLASS_IDLE_SCAN )
						{

						}

					}else{
					
						RSLOG(RSLOG_ACTION, _T("CreateProcessAsUser return errCode:%d\n"), GetLastError());
					}

					pDestroyEnvironmentBlock(lpEnvironment);
					if(bRet)
					{
						bReturnVal = TRUE;
						if(stProcessInfo.hThread)
							CloseHandle(stProcessInfo.hThread);
						if(stProcessInfo.hProcess)
							CloseHandle(stProcessInfo.hProcess);
					}
				}
			}

			if(hDllInst)
				FreeLibrary(hDllInst);

			if(hTokenThis)
				CloseHandle(hTokenThis);

			if(hTokenDup)
				CloseHandle(hTokenDup);

			if(hThisProcess)
				CloseHandle(hThisProcess);
		}
	}
	return bReturnVal;	
}

DWORD CStartTask::TaskProcess(LPVOID lpParameter)
{
	PROCESS_INFORMATION		pi;
	pi.hProcess=NULL;
	pi.hThread=NULL;

	ThreadPara *lpData = (ThreadPara* )lpParameter;
	RASSERTP(lpData,E_FAIL);

	STARTUPINFO				stStartUpInfo;
	ZeroMemory(&stStartUpInfo, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.lpDesktop  = _T("WinSta0\\Default");      // Default desktop
	stStartUpInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE); // Standard handles for input
	stStartUpInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE); // and error
	stStartUpInfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = SW_SHOWDEFAULT;

	try
	{
		if ( CreateProcess(NULL, lpData->szCmdLine,NULL, NULL,
			FALSE, 0, NULL,	lpData->szPath, &stStartUpInfo,&pi) )
		{
			if(lpData->lpClass)
			{
				if(lpData->dwIsStop == 0)
				{
					SYSTEMTIME lnowTime;
					memset(&lnowTime,0,sizeof(lnowTime));
					GetLocalTime(&lnowTime);
					lpData->lpClass->ModifyTime(lpData->dwIndex,&lnowTime);
					lpData->lpClass->ModifyRunStaus(lpData->dwType, lpData->dwIndex,(DWORD)pi.hProcess);
				}
				else{
					lpData->lpClass->ModifyRunStaus(lpData->dwType, lpData->dwIndex,0);
				}
			}

			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			if(lpData->lpClass && lpData->dwClassID != RS_RAVTASK_CLASS_IDLE_SCAN)
			{
			}
		}
		else{
			RSLOG(RSLOG_DETAIL, _T("任务%d创建失败"),lpData->dwIndex);
		}
	}
	catch(...)
	{
		RSLOG(RSLOG_FATAL, _T("TaskProcess Catch exceptions!"));
	}


	return 0;	
}



BOOL CStartTask::ConvertCommandline(IN TCHAR* szOldCmd,OUT  TCHAR* szCmdLine,OUT  TCHAR* szPath)
{
	if(szOldCmd==NULL ||szCmdLine ==NULL || szPath==NULL )
		return FALSE;
	TCHAR szTemp[MAX_PATH];
	ZeroMemory(szTemp,sizeof(szTemp));

	//%ProgramFiles%
	ExpandEnvironmentStrings(szOldCmd,szTemp,MAX_PATH);

	CString strFull = szTemp;
	strFull.Trim();

	strFull.Replace(_T("%INSTALL%"),m_szPath);
	if(strFull.Find(_T("%CURRENTLANG%")) >0 )
	{
		//if(m_iLang)
		//{
		//	int nCode = m_iLang->GetCurrentLanguage();
		//	char szCodePage[100] ={0};
		//	itoa(nCode,szCodePage,10);
		//	strFull.Replace(_T("%CURRENTLANG%"),szCodePage);
		//}
		//else
			strFull.Replace(_T("%CURRENTLANG%"),_T(""));
	}

	if(strFull.GetLength() > MAX_PATH)
		return FALSE;
	int nPos = strFull.ReverseFind(_T('\\'));
	if(nPos > 0)
	{
		memcpy(szCmdLine,strFull.GetBuffer(),strFull.GetLength()*sizeof(TCHAR));
		CString strPath = strFull.Left(nPos);
		if(strPath.GetLength() < MAX_PATH)
			memcpy(szPath,strPath.GetBuffer(),strPath.GetLength()*sizeof(TCHAR));
	}
	else
	{
		CString szPathTmp;
		szPathTmp.Format(_T("%s\\%s"),m_szPath,strFull);
		if(szPathTmp.GetLength() > MAX_PATH)
			return FALSE;
		memcpy(szCmdLine,szPathTmp.GetBuffer(),szPathTmp.GetLength()*sizeof(TCHAR));
		lstrcpy(szPath,m_szPath);
	}
	return TRUE;
}


BOOL CStartTask::StartProgram(VOID* lpClass, ST_RSTASK * lpTask,UINT uType,DWORD dwSessionID)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szCmdLine[MAX_PATH+100];
	TCHAR szOldCmd[MAX_PATH+100];

	ZeroMemory(szPath,MAX_PATH*sizeof(TCHAR));
	ZeroMemory(szCmdLine,(MAX_PATH+100)*sizeof(TCHAR));
	ZeroMemory(szOldCmd,(MAX_PATH+100)*sizeof(TCHAR));

	ThreadPara *lpData= new ThreadPara;
	RASSERTP(lpData,FALSE);
	ZeroMemory(lpData,sizeof(ThreadPara));

	if(uType)
	{
		lstrcpy(szOldCmd,lpTask->task_cmd.end);
		lpData->dwIsStop = 1;
	}
	else
	{
		lstrcpy(szOldCmd,lpTask->task_cmd.cmd);
		lpData->dwIsStop = 0;
	}

    //_strupr(szOldCmd);
	ConvertCommandline(szOldCmd,szCmdLine,szPath);

	if(_tcsstr(szCmdLine,_T(".dll") ) || _tcsstr(szCmdLine,_T(".DLL")))
	{
		if(lpData) delete lpData;
		return StartDllProgram(szCmdLine,lpTask->task_specific.wSpecificMethod);

	}
	//StartProcess(szCmdLine,m_szPath,lpTask->dwProgressID);

	lpData->lpClass =(CGTasks *) lpClass;
	lpData->lpThis = this;
	lpData->dwType = lpTask->task_attrib.uType;
	lpData->dwClassID = lpTask->task_attrib.uClassID;
	lpData->dwIndex = lpTask->task_attrib.uIndex;
	lpData->dwSessionID = dwSessionID;
	lpData->dwRight = lpTask->task_specific.wRight;

	lstrcpy(lpData->szCmdLine,szCmdLine);
	lstrcpy(lpData->szPath,szPath);


	if(m_bXpAbove)
		TaskProcessXP(lpData);
	else
		TaskProcess(lpData);

	if(lpData)
	{
		delete lpData;
	}
	//RSLOG(RSLOG_DETAIL, "StartProgram:任务%d,ThreadHandle=%d,ThreadID=%d",lpTask->task_attrib.uIndex,hThreadHandle,dwThreadID);
	return TRUE;
}


BOOL CStartTask::StartProcess(TCHAR* lpCmdLine, TCHAR* lpPath)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szCmdLine[MAX_PATH+100];
	TCHAR szOldCmd[MAX_PATH+100];
	ZeroMemory(szPath,sizeof(szPath));
	ZeroMemory(szCmdLine,sizeof(szCmdLine));
	ZeroMemory(szOldCmd,sizeof(szOldCmd));

	lstrcpy(szOldCmd,lpCmdLine);
	ConvertCommandline(szOldCmd,szCmdLine,szPath);

	STARTUPINFO				stStartInfo;
	PROCESS_INFORMATION		stProcessInfo;

	if(m_bXpAbove)
	{
		BOOL bReturnVal = FALSE;
		ZeroMemory(&stStartInfo, sizeof(STARTUPINFO));
		stStartInfo.cb = sizeof(STARTUPINFO);
		stStartInfo.lpDesktop  = _T("WinSta0\\Default");      // Default desktop
		stStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE); // Standard handles for input
		stStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE); // and error
		stStartInfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
		stStartInfo.wShowWindow = SW_SHOWDEFAULT;

		stProcessInfo.hProcess=NULL;
		stProcessInfo.hThread=NULL;
		SECURITY_DESCRIPTOR     sDesc = {0};
		SECURITY_ATTRIBUTES     sa;
		if (!InitializeSecurityDescriptor(&sDesc, SECURITY_DESCRIPTOR_REVISION))
		{
			RSLOG(RSXML_ALERT, _T("InitializeSecurityDescriptor FAILED, Err(%d)\n"), GetLastError());
			return FALSE;
		}
		if (!SetSecurityDescriptorDacl(&sDesc, TRUE, (PACL) NULL, FALSE))	  
		{
			RSLOG(RSXML_ALERT, _T("SetSecurityDescriptorDacl FAILED : Err(%d)\n"), GetLastError());
			return FALSE;
		}
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sDesc;
		sa.bInheritHandle = TRUE;

		//---------------------------------------------------------
		DWORD dwActiveSessionID = 0;
		FnGetActiveConsoleSessionId pGetActiveSessionId = (FnGetActiveConsoleSessionId) GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")), "WTSGetActiveConsoleSessionId");
		FnDuplicateTokenEx pDuplicateTokenEx = (FnDuplicateTokenEx) GetProcAddress(
			GetModuleHandle(_T("Advapi32.dll")), "DuplicateTokenEx");
		FnOpenProcessToken pOpenProcessToken = (FnOpenProcessToken) GetProcAddress(
			GetModuleHandle(_T("Advapi32.dll")), "OpenProcessToken");
		FnSetTokenInformation pSetTokenInformation = (FnSetTokenInformation) GetProcAddress(
			GetModuleHandle(_T("Advapi32.dll")), "SetTokenInformation");

		dwActiveSessionID = 0;
		if( pGetActiveSessionId)
		{
			dwActiveSessionID = pGetActiveSessionId();//dwActiveSessionID不是本session!
			if(dwActiveSessionID == 0xFFFFFFFF)
			{
				dwActiveSessionID = 0;
			}
		}
		DWORD dwExplorerID = 0;

		//取活动Session下的explorer.exe ID
		DWORD dwTest = m_check.CheckExplorer(dwActiveSessionID); 
		if(dwTest == 0) 
		{	
			//没取到,搜一个活动的explorer.exe ID
			dwTest  = m_check.GetActiveExplorer(dwActiveSessionID);
			if(dwTest == 0)
			{
				return FALSE;
			}
			//当前活动的session下没有explorer,改用搜到explorer启动
			dwExplorerID  = dwTest;
		}


		if( pDuplicateTokenEx
			&& pOpenProcessToken
			&& pSetTokenInformation
			)
		{
			HANDLE hTokenThis = NULL;
			HANDLE hTokenDup = NULL;
			HANDLE hThisProcess = 0; 
				
			if(dwExplorerID==0)
			{
				hThisProcess = GetCurrentProcess();
			}
			else
			{
				hThisProcess = OpenProcess (PROCESS_ALL_ACCESS, //PROCESS_QUERY_INFORMATION
					FALSE, dwExplorerID); 
			}

			BOOL bRet = FALSE;
			bRet = pOpenProcessToken(hThisProcess, TOKEN_ALL_ACCESS, &hTokenThis);
			BOOL bRet2 =  pDuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, 
				TokenPrimary, &hTokenDup);

			if(bRet && bRet2)
			{

				//if(((DWORD)(sessionid))!=dwActiveSessionID)
				{
					//设置Session ID，如果设置不成功，就会用本进程Session
					bRet = pSetTokenInformation(hTokenDup, TokenSessionId, &dwActiveSessionID, sizeof(DWORD));
				}

				LPVOID pEnv = NULL;
				DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

				LPVOID lpEnvironment; 
				HINSTANCE hDllInst = LoadLibrary(_T("Userenv.DLL"));
				if(hDllInst	)
				{
					typedef DWORD (WINAPI * FnCreateEnvironmentBlock)(LPVOID, HANDLE, BOOL);
					typedef DWORD (WINAPI * FnDestroyEnvironmentBlock)(LPVOID);

					FnCreateEnvironmentBlock pCreateEnvironmentBlock = (FnCreateEnvironmentBlock) GetProcAddress(
						hDllInst, "CreateEnvironmentBlock");
					FnDestroyEnvironmentBlock pDestroyEnvironmentBlock = (FnDestroyEnvironmentBlock) GetProcAddress(
						hDllInst, "DestroyEnvironmentBlock");
					//RSLOG(RSLOG_ACTION, "CreateEnvironmentBlock:%d,  DestroyEnvironmentBlock:%d\n", 
					//	pCreateEnvironmentBlock, pDestroyEnvironmentBlock);

					if( pCreateEnvironmentBlock
						&& pDestroyEnvironmentBlock
						&& pCreateEnvironmentBlock(&lpEnvironment, hTokenThis, FALSE))
					{

						DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
						BOOL bRet = CreateProcessAsUser(hTokenDup, NULL, szCmdLine, &sa, &sa, FALSE,
							dwCreationFlag, lpEnvironment, szPath, &stStartInfo, &stProcessInfo);

						if(!bRet)
						{
							RSLOG(RSLOG_ACTION, _T("CreateProcessAsUser return errCode:%d\n"), GetLastError());
						}

						pDestroyEnvironmentBlock(lpEnvironment);
						if(bRet)
						{
							bReturnVal = TRUE;
							if(stProcessInfo.hThread)
								CloseHandle(stProcessInfo.hThread);
							if(stProcessInfo.hProcess)
								CloseHandle(stProcessInfo.hProcess);
						}
					}
				}

				if(hDllInst)
					FreeLibrary(hDllInst);

				if(hTokenThis)
					CloseHandle(hTokenThis);

				if(hTokenDup)
					CloseHandle(hTokenDup);

				if(hThisProcess)
					CloseHandle(hThisProcess);
			}
		}

		return bReturnVal;	


	}
	else
	{
		ZeroMemory(&stStartInfo, sizeof(STARTUPINFO));
		stStartInfo.cb = sizeof(STARTUPINFO);
		//stStartInfo.dwFlags = STARTF_USESHOWWINDOW;
		//stStartInfo.wShowWindow = SW_SHOWDEFAULT;

		BOOL bRet = CreateProcess(NULL, szCmdLine, 
			NULL, NULL, FALSE, 0, NULL,	szPath,	&stStartInfo,&stProcessInfo);
		if (bRet) 
		{
			CloseHandle(stProcessInfo.hThread);
			CloseHandle(stProcessInfo.hProcess);
		}
		return bRet;
	}
	return -1;
}


