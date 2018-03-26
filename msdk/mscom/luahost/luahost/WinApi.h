#pragma once
#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"
#include <vistafunc/vistafunc.h>
#include <msapi/mswinapi.h>
#include <msapi/EnumWnd.h>
#include <msapi/MonitorResolution.h>
class CWinApi
{
public:
	CWinApi(void);
	~CWinApi(void);

	static BOOL BindToLua(ILuaVM* pLuaVm)
	{
		RASSERT(pLuaVm, FALSE);

		GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("winapi")
			+mluabind::Function("GetCommandLine", &CWinApi::GetCommandLine)
			+mluabind::Function("GetCurrentPath", &CWinApi::GetCurrentPath)
			+mluabind::Function("GetCurrentFullPath", &CWinApi::GetCurrentFullPath)
			+mluabind::Function("Execute", &CWinApi::Execute)
			+mluabind::Function("Sleep", &CWinApi::Sleep)
			+mluabind::Function("ShellExecute", &CWinApi::_ShellExecute)
			+mluabind::Function("TerminateProcessAsName",&CWinApi::TerminateProcessAsName)
			+mluabind::Function("GetProcessIdAsName", &CWinApi::GetProcessIdAsName)
			+mluabind::Function("GetProcesssHandleByName", &GetProcesssHandleByName)
			+mluabind::Function("GetProcesssHandleByID", &GetProcesssHandleByID)
			+mluabind::Function("GetTickCount", &CWinApi::GetTickCount)
			+mluabind::Function("GetPeSize", &CWinApi::GetPeSize)
			+mluabind::Function("CheckOutFileFromPe", &CWinApi::CheckOutFileFromPe)
			+mluabind::Function("CheckOutFileFromeResource", &CWinApi::CheckOutFileFromeResource)
			+mluabind::Function("GetLastErrorText", &CWinApi::GetLastErrorText)
			+mluabind::Function("GetModuleHandle", &CWinApi::GetModuleHandle)
			+mluabind::Function("GetModuleFileName", &CWinApi::GetModuleFileName)
			+mluabind::Function("GetModuleFilePath", &CWinApi::GetModuleFilePath)
			+mluabind::Function("AppendStringToPeFile", &CWinApi::AppendStringToPeFile)
			+mluabind::Function("GetUserGuid", &CWinApi::GetUserGuid)
			+mluabind::Function("GetMainBoardSerialNumber", &CWinApi::GetMainBoardSerialNumber)
			+mluabind::Function("GetNetAdapter", &CWinApi::GetNetAdapter)
			+mluabind::Function("GetMicroVersion",&msapi::GetMicroVersion)
			+mluabind::Function("GetDiskSerialNumber", &CWinApi::GetDiskSerialNumber)
			+mluabind::Function("IsWindowsX64", msapi::IsWindowsX64)
			+mluabind::Function("PathFileName", CWinApi::PathFileName)
			+mluabind::Function("DllRegisterServer", &DllRegisterServer)
			+mluabind::Function("SetDefProgram", &_SetDefProgram)
			+mluabind::Function("SHNotifyAssocChanged", &SHNotifyAssocChanged)
			+mluabind::Function("FindMostActiveSession", &vistafunc::CVistaTools::FindMostActiveSession)
			+mluabind::Function("CreateProcess", &vistafunc::CVistaTools::CreateProcess)
			+mluabind::Function<DWORD, HANDLE, DWORD>("WaitForSingleObject", &WaitForSingleObject)
			+mluabind::Function("CloseHandle", &CloseHandle)


			//////////////////////////////////////////////////////////////////////////
			//WND
			+mluabind::Function("FindWindowEx", &FindWindowEx)
			+mluabind::Function("FindWindow", &FindWindow)
			+mluabind::Function("EnumWindow", &EnumWindow)
			+mluabind::Function("ShowWindow", &ShowWindow)
			+mluabind::Function("CenterWindow", &CenterWindow)


			//////////////////////////////////////////////////////////////////////////
			//Display
			+mluabind::Function("AdjustResolution", &CMonitorResolution::AdjustResolution)
			+mluabind::Function("ChangeResolution", &CMonitorResolution::ChangeResolution)


			//vistafunc
			+mluabind::Function("DisableWow64FsRedirection", &DisableWow64FsRedirection)
			+mluabind::Function("RevertWow64FsRedirection", &RevertWow64FsRedirectionLua)

			//file
			+mluabind::Function("DeleteFileForce", &msapi::DeleteFileForce)
			+mluabind::Function("DeleteFileEx", &msapi::DeleteFileEx)
		
			);

		return TRUE;
	}

	static std::string GetCommandLine();
	static std::string GetCurrentPath();
	static std::string GetCurrentFullPath();
	static std::string GetLastErrorText();
	static std::string GetModuleFileName(VOID* hModule);
	static std::string GetModuleFilePath(VOID* hModule);
	static std::basic_string<TCHAR> GetUserGuid();
	static std::string GetMainBoardSerialNumber();
	static std::string GetNetAdapter();	
	static std::basic_string<TCHAR> GetDiskSerialNumber();
	static VOID* GetModuleHandle(LPCSTR lpszModule);

	static DWORD Execute(LPCTSTR appfile, LPCTSTR commandline, bool waitforexit, bool showconcle, int waittime);
	static DWORD _ShellExecute(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd,BOOL bWait);
	static void Sleep(int nTickCount);

	static DWORD TerminateProcessAsName(LPCSTR lpszName);
	static DWORD GetProcessIdAsName(LPCSTR lpszName);
	static HANDLE GetProcesssHandleByName( LPCSTR lpszName );
	static HANDLE GetProcesssHandleByID( DWORD dwPid);
	static DWORD GetTickCount();
	static LONG GetPeSize(LPCTSTR lpszPeFile);

	static bool CheckOutFileFromPe(LPCTSTR lpszFileFrome, LPCTSTR lpszFileTo);
	static bool CheckOutFileFromeResource(LPCTSTR lpszModuleFileName, LPCTSTR lpszResType, int nResID, LPCTSTR lpszFileTo);
	static bool AppendStringToPeFile(LPCTSTR lpszFile, LPCSTR lpszString);

	static tstring PathFileName( LPCTSTR lpszFileName );


	static bool DllRegisterServer(LPCTSTR lpszDllPath, bool bInst = true);

	//
	static bool _SetDefProgram(LPCTSTR lpszPord, LPCTSTR lpszProg, LPCTSTR lpszExt, LPCTSTR lpszDes, LPCTSTR lpszParme , LPCTSTR lpszIco);

	static DWORD WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds );
	static VOID CloseHandle( __in HANDLE hObject );


	//////////////////////////////////////////////////////////////////////////
	//WND
	static VOID* FindWindowEx(VOID* hwndParent, VOID* hwndChildAfter, LPCSTR lpszClass, LPCSTR lpszWindow);
	static VOID* FindWindow(   LPCSTR lpClassName,   LPCSTR lpWindowName ); 
	static VOID* EnumWindow(int enumWnd, LPCTSTR lpClassName, LPCTSTR lpWindowName );
	static BOOL ShowWindow( VOID* hWnd, int nCmdShow ); 
	static BOOL CenterWindow(VOID* hWnd );

	//vistafunc
	static DWORD DisableWow64FsRedirection();
	static bool RevertWow64FsRedirectionLua(DWORD dwRet);

	static bool DisableWow64FsRedirection_1(PVOID* pOldValue);
	static bool RevertWow64FsRedirection_2(PVOID OldValue);

private:
	static bool SetDefProgram(HKEY hKey, LPCTSTR lpszPord, LPCTSTR lpszProg, LPCTSTR lpszExt, LPCTSTR lpszDes, LPCTSTR lpszParme , LPCTSTR lpszIco);
	static bool SHNotifyAssocChanged(  );
private:
};

