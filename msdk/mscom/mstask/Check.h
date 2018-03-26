#pragma once
#include <tlhelp32.h>


typedef BOOL (WINAPI * PFNENUMPROCESSES)(
	DWORD * lpidProcess,
	DWORD   cb,
	DWORD * cbNeeded
	);

typedef BOOL (WINAPI * PFNENUMPROCESSMODULES)(
	HANDLE hProcess,
	HMODULE *lphModule,
	DWORD cb,
	LPDWORD lpcbNeeded
	);

typedef DWORD (WINAPI * PFNGETMODULEFILENAMEEX)(
	HANDLE hProcess,
	HMODULE hModule,
	LPTSTR lpFilename,
	DWORD nSize
	);

typedef HANDLE (WINAPI *xCreateToolhelp32Snapshot) ( DWORD dwFlags, DWORD th32ProcessID );
typedef  BOOL (WINAPI  *xProcess32First) (HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef  BOOL (WINAPI *xProcess32Next)(HANDLE hSnapshot,LPPROCESSENTRY32 lppe);


class CCheck
{
public:
	CCheck(void);
	~CCheck(void);
	DWORD CheckExplorer(DWORD dwSessionID=0xFFFFFFFF);

	DWORD GetActiveExplorer(DWORD& dwSessionID);

	BOOL GetDllFunc();
private:
	BOOL GetDllFunc9x();
	BOOL GetDllFuncNT();

	DWORD CheckExplorer9x(DWORD dwSessionID=0xFFFFFFFF);
	DWORD CheckExplorerNT(DWORD dwSessionID=0xFFFFFFFF);

	HMODULE m_hModPSAPI;
	HMODULE  m_hKernel;

	PFNENUMPROCESSES        m_pfnEnumProcesses;
	PFNENUMPROCESSMODULES   m_pfnEnumProcessModules;
	PFNGETMODULEFILENAMEEX m_pfnGetModuleFileNameEx;

	xCreateToolhelp32Snapshot m_pCreateToolhelp32Snapshot;
	xProcess32First m_pProcess32First;
	xProcess32Next m_pProcess32Next;


	DWORD GetCurrentSession(DWORD dwPid);

	TCHAR m_szPath[MAX_PATH];
	TCHAR m_szFileName[MAX_PATH];

	BOOL m_bXpAbove ;
};
