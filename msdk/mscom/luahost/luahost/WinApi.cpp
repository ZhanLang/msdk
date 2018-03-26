#include "StdAfx.h"
#include "WinApi.h"


#include "luahost/luahost.h"
#include "shellapi.h"
#include <tlhelp32.h>
#include "msapi/mswinapi.h"
#include "util/utility_ex.h"
#include <util/registry.h>

using namespace UTILEX;
CWinApi::CWinApi(void)
{
}


CWinApi::~CWinApi(void)
{
}

std::string CWinApi::GetCommandLine()
{
	return ::GetCommandLineA();
}


std::string CWinApi::GetCurrentPath()
{
	char szPath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	char* pTail = strrchr(szPath, _T('\\'));
	if (NULL != pTail){
		*pTail = 0;
	}
	return szPath;
}

std::string CWinApi::GetCurrentFullPath()
{
	char szPath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	return szPath;
}


static LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
	LPTSTR lpszTemp = 0;
	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		0,
		GetLastError(),
		LANG_NEUTRAL, //默认的语言种类
		(LPTSTR)&lpszTemp,
		0,
		0
		);

	if( !dwRet || (dwSize < dwRet+14) )
		lpszBuf[0] = TEXT('\0');
	else {
		lpszTemp[_tcsclen(lpszTemp)-2] = TEXT('\0');  //remove cr/nl characters
		_tcscpy_s(lpszBuf, dwSize, lpszTemp);
	}

	if( lpszTemp )
		LocalFree(HLOCAL(lpszTemp));

	return lpszBuf;
}


#define DEFULT_IO_OPERATION_BYTES	1024
DWORD CWinApi::Execute(LPCTSTR appfile, LPCTSTR commandline, bool waitforexit, bool showconcle, int waittime)
{
	
	RASSERT(appfile,-1);

	TCHAR _errinfo[DEFULT_IO_OPERATION_BYTES * 2 + 1] = {0};

	_errinfo[0] = NULL;
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	sa.bInheritHandle		= TRUE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength				= sizeof(sa);

	HANDLE hread  = NULL,hwrite = NULL,herrin = NULL,herrout= NULL;

	DWORD  result = -1;

	do {
		if (waitforexit
			&& ::CreatePipe(&hread, &hwrite, &sa,	DEFULT_IO_OPERATION_BYTES * 200)
			&& ::CreatePipe(&herrin, &herrout, &sa, DEFULT_IO_OPERATION_BYTES * 200)
			&& ::SetHandleInformation(herrin, HANDLE_FLAG_INHERIT, 0)
			)
		{
			si.dwFlags	  = STARTF_USESTDHANDLES;
			si.hStdError  = herrout;
			si.hStdOutput = hwrite;
			si.hStdInput  = hread;
		}

		DWORD createflag = ::GetPriorityClass(::GetCurrentProcess());
		if (!showconcle) createflag |= CREATE_NO_WINDOW;
		BOOL err = ::CreateProcess(appfile, (LPTSTR)commandline,
			NULL, NULL, TRUE, 
			createflag, NULL, NULL,
			&si, &pi
			);
		CloseHandle(hwrite);
		CloseHandle(herrout);
		if (!err) {
			TCHAR szErr[MAX_PATH] = {0};
			GrpError(GRP_NAME,MsgLevel_Error, _T("执行进程失败了,file:[%s];cmd:[%s],err:[%s]"),appfile,commandline,::GetLastErrorText(szErr,MAX_PATH));
			break;
		}
	

		if (!waitforexit) {
			result = 0;
			break;
		}

		DWORD r = ::WaitForSingleObject(pi.hProcess, waittime);
		if (r != WAIT_OBJECT_0)
		{
			::TerminateProcess(pi.hProcess, -1);
			GrpError(GRP_NAME, MsgLevel_Error, _T("Execute TerminateProcess(%s), as %d"), appfile?appfile:_T("null"), r);
		}
		::GetExitCodeProcess(pi.hProcess, &result);
		
		DWORD outlen = 0,
			errlen = 0;
		if (hread != NULL && herrin != NULL) {
			//::ReadFile(hread,  _errinfo,		  DEFULT_IO_OPERATION_BYTES, &outlen, NULL);
			//::ReadFile(herrin, _errinfo + outlen, DEFULT_IO_OPERATION_BYTES, &errlen, NULL);
		}
		_errinfo[errlen + outlen] = NULL;

	} while (0);

	CloseHandle(hread);
	CloseHandle(herrin);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	_errinfo[500] = NULL;

	GrpError(GRP_NAME, MsgLevel_Error, _T("进程执行结果:[%x]"), result);
	return result;

}

void CWinApi::Sleep(int nTickCount)
{
	::Sleep(nTickCount);
}

DWORD CWinApi::_ShellExecute(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd , BOOL bWait)
{
	DWORD dwResult = 0;
	SHELLEXECUTEINFO si={0};
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.fMask=SEE_MASK_NOCLOSEPROCESS; 
	si.lpFile= lpFile; 
	si.lpVerb= lpOperation;
	si.lpDirectory = lpDirectory;
	si.lpParameters = lpParameters;
	si.nShow=nShowCmd;
	if (ShellExecuteEx(&si))
	{
		if ( bWait )
		{
			WaitForSingleObject(si.hProcess, -1);
			::GetExitCodeProcess(si.hProcess, &dwResult);
		}
	}

	return dwResult;
}

DWORD CWinApi::TerminateProcessAsName( LPCSTR lpszName )
{
	if (!(lpszName && strlen(lpszName)) )
	{
		GrpErrorA(GroupNameA, MsgLevel_Error, "CWinApi::TerminateProcessAsName(%s)::[!(lpszName && strlen(lpszName))]", lpszName);
		return -1 ;
	}

	PROCESSENTRY32 pe32 = {sizeof(pe32)} ;
	HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
	if ( hProcessShot == INVALID_HANDLE_VALUE )
	{
		GrpErrorA(GroupNameA, MsgLevel_Error, "CWinApi::TerminateProcessAsName(%s)::CreateToolhelp32Snapshot", lpszName);
		return -1 ;
	}

	USES_CONVERSION;
	LPCWSTR lpszNameW = A2W(lpszName);
	DWORD dwCount = 0;
	if ( Process32First ( hProcessShot, &pe32 ) )
	{
		do
		{
			if(wcsicmp(pe32.szExeFile, lpszNameW) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
				if (hProcess && TerminateProcess(hProcess, 0)){
					dwCount++;
				}else{
					GrpErrorA(GroupNameA, MsgLevel_Error, "CWinApi::TerminateProcessAsName(%s)::OpenProcess", lpszName);
				}
				if (hProcess)
				{
					CloseHandle(hProcess);
				}
			}
		}while ( Process32Next ( hProcessShot, &pe32 ) ) ;  
	}
	CloseHandle ( hProcessShot) ;  
	return dwCount;
}

DWORD CWinApi::GetProcessIdAsName( LPCSTR lpszName )
{
	HANDLE hHandle = GetProcesssHandleByName( lpszName );
	if ( hHandle == NULL)
		return 0;

	DWORD dwPid = msapi::GetProcessIdByHandle(hHandle);
	CloseHandle(hHandle);
	return dwPid;
}

HANDLE CWinApi::GetProcesssHandleByName(LPCSTR lpszName)
{
	if (!(lpszName && strlen(lpszName)) )
		return NULL ;
	

	PROCESSENTRY32 pe32 = {sizeof(pe32)} ;
	HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
	if ( hProcessShot == INVALID_HANDLE_VALUE )
		return NULL;

	USES_CONVERSION;
	LPCWSTR lpszNameW = A2W(lpszName);
	HANDLE hHandle = 0;
	if ( Process32First ( hProcessShot, &pe32 ) )
	{
		do
		{
			if(wcsicmp(pe32.szExeFile, lpszNameW) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
				if (hProcess)
				{
					hHandle = hProcess;
					break;
				}

			}
		}while ( Process32Next ( hProcessShot, &pe32 ) ) ;  
	}
	CloseHandle ( hProcessShot) ;  
	return hHandle;
}

HANDLE CWinApi::GetProcesssHandleByID(DWORD dwPid)
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
}

DWORD CWinApi::GetTickCount()
{
	return ::GetTickCount();
}

LONG CWinApi::GetPeSize(LPCTSTR lpszPeFile)
{
	UTIL::sentry<HANDLE, UTIL::handle_sentry> hFileHandle = CreateFile(lpszPeFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	RASSERT(INVALID_HANDLE_VALUE != hFileHandle, 0);

	return msapi::GetPESize(hFileHandle);
}

bool CWinApi::CheckOutFileFromPe(LPCTSTR lpszFileFrome, LPCTSTR lpszFileTo)
{
	RASSERT(lpszFileFrome && lpszFileTo, false);

	UTIL::sentry<HANDLE, UTIL::handle_sentry> hFileHandle = CreateFile(lpszFileFrome, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	RASSERT(INVALID_HANDLE_VALUE != hFileHandle, false);

	LONG dwSize = msapi::GetPESize(hFileHandle);
	RASSERT(dwSize != -1, false);

	SetFilePointer(hFileHandle, dwSize, NULL, FILE_BEGIN);
	UTIL::sentry<HANDLE, UTIL::handle_sentry> hTargetHandle = CreateFile(lpszFileTo, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	RASSERT(hTargetHandle != INVALID_HANDLE_VALUE, false);

	BYTE lpBuffer[4096] = {0};

	while (1)
	{
		DWORD dwReadFromPe = 0;
		ReadFile(hFileHandle,lpBuffer,4096,&dwReadFromPe,NULL);
		if (dwReadFromPe == 0) break;
		DWORD dwWrite = 0;
		WriteFile(hTargetHandle,lpBuffer,dwReadFromPe,&dwWrite,NULL);
	}

	return true;
}


bool CWinApi::CheckOutFileFromeResource(LPCTSTR lpszModuleFileName, LPCTSTR lpszResType, int nResID, LPCTSTR lpszFileTo)
{
	UTIL::sentry<HMODULE, UTILEX::hmodule_sentry> hResModule = LoadLibraryEx(lpszModuleFileName, NULL, LOAD_LIBRARY_AS_DATAFILE );
	RASSERT(hResModule, false);

	

	HRSRC hResource = ::FindResource(hResModule, MAKEINTRESOURCE(nResID), lpszResType);

	if( hResource == NULL )
		return false;


	DWORD dwSize = ::SizeofResource(hResModule, hResource);
	if( dwSize == 0 )
	{
		::FreeResource(hResource);
		return false;
	}

	
	HGLOBAL hGlobal = ::LoadResource(hResModule, hResource);
	if( hGlobal == NULL ) 
	{
		::FreeResource(hResource);
		return false;
	}

	UTIL::sentry<HANDLE, UTIL::handle_sentry> hTargetHandle = CreateFile(lpszFileTo, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hTargetHandle == INVALID_HANDLE_VALUE)
	{
		::FreeResource(hResource);
		return false;
	}
	
	DWORD dwWrite = 0;
	BOOL bRet =WriteFile(hTargetHandle,  (LPBYTE)::LockResource(hGlobal), dwSize, &dwWrite, NULL);
	
	bRet &= (dwWrite == dwSize);
	::FreeResource(hResource);
	return bRet == TRUE;
}

std::string CWinApi::GetLastErrorText()
{
	CHAR lpszBuf[1024] = {0};
	LPSTR lpszTemp = 0;
	DWORD dwRet = ::FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		0,
		GetLastError(),
		LANG_NEUTRAL, //默认的语言种类
		(LPSTR)&lpszTemp,
		0,
		0
		);

	if( !dwRet || (1024 < dwRet+14) )
		lpszBuf[0] = '\0';
	else {
		lpszTemp[strlen(lpszTemp)-2] = '\0';  //remove cr/nl characters
		strcpy_s(lpszBuf, 1024, lpszTemp);
	}

	if( lpszTemp )
		LocalFree(HLOCAL(lpszTemp));

	return lpszBuf;
}

VOID* CWinApi::GetModuleHandle(LPCSTR lpszModule)
{
	return GetModuleHandleA(lpszModule);
}

std::string CWinApi::GetModuleFileName(VOID* hModule)
{
	CHAR szModule[MAX_PATH] = { 0 };
	GetModuleFileNameA((HMODULE)hModule, szModule, MAX_PATH);
	return szModule;
}


std::string CWinApi::GetModuleFilePath(VOID* hModule)
{
	CHAR szModule[MAX_PATH] = { 0 };
	GetModuleFileNameA((HMODULE)hModule, szModule, MAX_PATH);
	CHAR* pTail = strrchr(szModule, '\\');
	if (NULL != pTail){
		*pTail = 0;
	}
	return szModule;
}

bool CWinApi::AppendStringToPeFile(LPCTSTR lpszFile, LPCSTR lpszString)
{
	RASSERT(lpszString && strlen(lpszString), false);

	UTIL::sentry<HANDLE, UTIL::handle_sentry> hFileHandle = CreateFile(lpszFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	RASSERT(INVALID_HANDLE_VALUE != hFileHandle, false);

	LONG dwSize = msapi::GetPESize(hFileHandle);
	RASSERT(dwSize != -1, false);

	SetFilePointer(hFileHandle, dwSize, NULL, FILE_BEGIN);
	DWORD dwWrite = 0;
	return WriteFile(hFileHandle,lpszString,strlen(lpszString) + 1,&dwWrite,NULL) ? true : false;
}

tstring CWinApi::PathFileName(LPCTSTR lpszFileName)
{
	return ::PathFindFileName(lpszFileName);
}

bool CWinApi::DllRegisterServer(LPCTSTR lpszDllPath, bool bInst)
{
	RSLOG(GroupName, MsgLevel_Error, _T("DllRegisterServer"));
	if ( !PathFileExists(lpszDllPath) )
	{
		GrpError(GroupName, MsgLevel_Error, _T("DllRegisterServer(%s)"), _T("文件不存在"));
		return false;
	}

	GrpMsg(GroupName, MsgLevel_Msg, _T("DllRegisterServer (%s,%d)"), lpszDllPath, bInst);

	BOOL b64PeFile = msapi::Is64PeFile(lpszDllPath);
	if ( b64PeFile )
		GrpError(GroupName, MsgLevel_Error, _T("DllRegisterServer(%s)"), _T("64位DLL"));
	


	BOOL bWin64 = msapi::IsWindowsX64();

	if ( !bWin64 && b64PeFile)
	{
		GrpError(GroupName, MsgLevel_Error, _T("在32位机器上注册64位Dll"));
		return false;
	}
	//暂时不考虑自身是否64位的情况

	LPVOID pWow64FsRedirection = NULL;
	if ( bWin64 && b64PeFile)//需要重定向
		vistafunc::DisableWow64FsRedirection(&pWow64FsRedirection);
	
	CString strCmd; strCmd.Format(bInst ? _T("%s \"%s\" /s") : _T("%s \"%s\" /s /u"),_T("C:\\Windows\\System32\\regsvr32.exe"), lpszDllPath);

	DWORD dwRet = msapi::Execute(_T("C:\\Windows\\System32\\regsvr32.exe"), strCmd, TRUE, FALSE, -1);
	if ( dwRet )
		GrpError(GroupName, MsgLevel_Error, _T("DllRegisterServer(%s) 失败[%d]"), lpszDllPath, dwRet);
	
	if ( pWow64FsRedirection )
		vistafunc::RevertWow64FsRedirection(pWow64FsRedirection);

	return dwRet == 0;
}

bool CWinApi::_SetDefProgram(LPCTSTR lpszPord, LPCTSTR lpszProg, LPCTSTR lpszExt, LPCTSTR lpszDes, LPCTSTR lpszParme , LPCTSTR lpszIco)
{

	SetDefProgram(HKEY_CURRENT_USER, lpszPord, lpszProg, lpszExt, lpszDes, lpszParme, lpszIco);
	SetDefProgram(HKEY_LOCAL_MACHINE, lpszPord, lpszProg, lpszExt, lpszDes, lpszParme, lpszIco);

	return true;
	
}

DWORD CWinApi::WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	return ::WaitForSingleObject( hHandle, dwMilliseconds);
}

VOID CWinApi::CloseHandle(__in HANDLE hObject)
{
	::CloseHandle(hObject);
}

VOID* CWinApi::FindWindowEx(VOID* hwndParent, VOID* hwndChildAfter,LPCSTR lpszClass,LPCSTR lpszWindow)
{
	HWND hWnd = ::FindWindowExA((HWND)hwndParent, (HWND)hwndChildAfter, lpszClass, lpszWindow);
	//GrpErrorA("LuaHost", MsgLevel_Error, "FindWindowEx(%s,%s, %x) %s", lpszClass, lpszWindow, hWnd, GetLastErrorText().c_str());
	return hWnd;
}

VOID* CWinApi::FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName)
{
	return ::FindWindowA( lpClassName, lpWindowName);
}

VOID* CWinApi::EnumWindow(int enumWnd, LPCTSTR lpClassName, LPCTSTR lpWindowName)
{
	HWND hWnd = msapi::CEnumWnd(enumWnd, GetDesktopWindow()).Enum( lpClassName, lpWindowName );
	//GrpError(L"LuaHost", MsgLevel_Error, L"EnumWindow(%s,%s, %x)", lpClassName, lpWindowName, hWnd);

	return hWnd;
}

BOOL CWinApi::ShowWindow(VOID* hWnd, int nCmdShow)
{
	return ::ShowWindow((HWND)hWnd, nCmdShow);
}

BOOL CWinApi::CenterWindow(VOID* hWnd )
{
	HWND m_hWnd = (HWND)hWnd;
	if (!(m_hWnd &&::IsWindow(m_hWnd)))
		return FALSE;

	// determine owner window to center against


	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;

	// don't center against invisible or minimized windows

	// center within screen coordinates
#if WINVER < 0x0500
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
	HMONITOR hMonitor = NULL;
	hMonitor = ::MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);


	MONITORINFO minfo;
	minfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &minfo);

	rcArea = minfo.rcWork;
#endif

	::GetWindowRect(GetDesktopWindow(), &rcCenter);



	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// if the dialog is outside the screen, move it inside
	if(xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if(xLeft < rcArea.left)
		xLeft = rcArea.left;

	if(yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if(yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

bool CWinApi::SetDefProgram(HKEY hKey, LPCTSTR lpszPord, LPCTSTR lpszProg, LPCTSTR lpszExt, LPCTSTR lpszDes, LPCTSTR lpszParme , LPCTSTR lpszIco)
{
	CRegistry().SetString(hKey, CString(_T("Software\\Classes\\")) + CString(lpszPord) + CString(lpszExt), _T(""), lpszDes);
	CRegistry().SetString(hKey, _T("Software\\Classes\\") +CString(lpszPord) + CString(lpszExt) +CString(_T("\\DefaultIcon")), _T(""), lpszIco);

	CString strCmd; strCmd.Format(_T("\"%s\" %s \"%%1\""), lpszProg, lpszParme);
	CRegistry().SetString(hKey, _T("Software\\Classes\\") +CString(lpszPord) + CString(lpszExt) +CString(_T("\\shell\\open\\command")), _T(""), strCmd);

	CRegistry().SetString(hKey, CString(_T("Software\\")) + CString(lpszExt), _T(""), CString(lpszPord) + CString(lpszExt));
	CRegistry().SetString(hKey, CString(_T("Software\\Classes\\"))+ CString(lpszExt), _T(""), CString(lpszPord) + CString(lpszExt));
	return true;
}

bool CWinApi::SHNotifyAssocChanged()
{
	::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_DWORD | SHCNF_FLUSH, NULL, NULL);
	return true;
}

#include <userguid/userguid.h>
#include <msapi/msapp.h>
std::basic_string<TCHAR> CWinApi::GetUserGuid()
{
	return msdk::msapi::CApp::GetUserGuid();
}

std::string CWinApi::GetMainBoardSerialNumber()
{
	char szMainBoardSerialNumber[MAX_PATH] = { 0 };
	CGetMainBoardInfo::GetMainBoardInfo(szMainBoardSerialNumber, false);
	return szMainBoardSerialNumber;
}

std::string CWinApi::GetNetAdapter()
{
	TCHAR szNetAdapter[MAX_PATH] = { 0 };
	CGetNetAdapterInfo::GetMac(szNetAdapter);
	USES_CONVERSION;
	return W2A(szNetAdapter);
}

std::basic_string<TCHAR> CWinApi::GetDiskSerialNumber()
{
	return msdk::msapi::CApp::GetDiskSerial();
}



/////////////////////////////////////////////////////////////////////////////
bool CWinApi::DisableWow64FsRedirection_1(PVOID* pOldValue)
{
	BOOL bReturn = FALSE;
	BOOL (WINAPI* pWow64DisableWow64FsRedirection)(PVOID*) = NULL;
	HMODULE hKernel = ::GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel)
	{
		(FARPROC&)pWow64DisableWow64FsRedirection = GetProcAddress(hKernel, "Wow64DisableWow64FsRedirection");
		if (pWow64DisableWow64FsRedirection)
		{
			bReturn = pWow64DisableWow64FsRedirection(pOldValue);
			RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Wow64DisableWow64FsRedirection Return: %d\n"), bReturn);
		}
		else
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("GetProcAddress(Wow64DisableWow64FsRedirection) Failed!\n"));
		}
	}
	else
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("GetModuleHandle Failed!\n"));
	}

	return BOOL2bool(bReturn);
}

/////////////////////////////////////////////////////////////////////////////
bool CWinApi::RevertWow64FsRedirection_2(PVOID OldValue)
{
	BOOL bReturn = FALSE;
	BOOL (WINAPI* pWow64RevertWow64FsRedirection)(PVOID) = NULL;
	HMODULE hKernel = ::GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel)
	{
		(FARPROC&)pWow64RevertWow64FsRedirection = GetProcAddress(hKernel, "Wow64RevertWow64FsRedirection");
		if (pWow64RevertWow64FsRedirection)
		{
			bReturn = pWow64RevertWow64FsRedirection(OldValue);
			RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Wow64RevertWow64FsRedirection Return: %d\n"), bReturn);
		}
		else
		{
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("GetProcAddress(Wow64RevertWow64FsRedirection) Failed!\n"));
		}
	}
	else
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("GetModuleHandle Failed!\n"));
	}

	return BOOL2bool(bReturn);
}

bool CWinApi::RevertWow64FsRedirectionLua(DWORD dwRet)
{
	return RevertWow64FsRedirection_2((PVOID)dwRet);
}

//注意：把指针当32位了
DWORD CWinApi::DisableWow64FsRedirection()
{
	PVOID OldValue = NULL;
	DisableWow64FsRedirection_1(&OldValue);
	return (DWORD)OldValue;
}