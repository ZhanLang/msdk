#pragma once


//#include <list>
#include <vector>

#include <luahost/ILuaHost.h>
#include "luahost/luahost.h"

namespace winfunc
{
	typedef std::basic_string<TCHAR> TString;
	typedef std::vector<TString> TStringS;

	class CWinService
	{
	public:
		CWinService();
		~CWinService();

		static BOOL BindToLua(ILuaVM* pLuaVM);

		static bool InstallService(LPCTSTR lpszServiceName, LPCTSTR lpszDisplayName, DWORD dwServiceType, DWORD dwStartType, LPCTSTR lpszBinaryPathName/*, LPCTSTR lpLoadOrderGroup = NULL, LPCTSTR lpDependencies = NULL, DWORD dwErrorControl = SERVICE_ERROR_NORMAL, LPCTSTR lpServiceStartName = NULL, LPCTSTR lpPassword = NULL*/);
		static bool UninstallService(LPCTSTR lpszServiceName);
		static bool SetServiceFailureAction(LPCTSTR lpszServiceName, DWORD dwResetPeriod, LPTSTR lpCommand, DWORD dwFirstAction, DWORD dwFirstDelay, DWORD dwSecondAction, DWORD dwSecondDelay, DWORD dwAfterAction, DWORD dwAfterDelay);
		static bool StartService(LPCTSTR lpszServiceName, DWORD dwNumServiceArgs = 0, LPCTSTR* lpServiceArgVectors = NULL);
		static bool StopService(LPCTSTR lpszServiceName, DWORD dwTimeOut = 10000, bool bStopDependencies = false);

		static BOOL InternalStopService (SC_HANDLE schService, SERVICE_STATUS* lpssStatus, LPCTSTR lpszServiceName);
		
		static bool IsExistedService(LPCTSTR lpszServiceName);
		static bool IsSrvRunning(LPCTSTR lpszServiceName);
		static bool SetServiceAutoStart(LPCTSTR lpszServiceName);
		////for lua
		static bool StartServiceForLua(LPCTSTR lpszServiceName, LPTSTR lpszServiceArgs);

		static bool VerifyServiceInfomations(LPCTSTR lpszServiceName, DWORD dwStartType, DWORD dwCurrentState);
	};
};//namespace winfunc
