#pragma once

//设置和获取IE相关的API均放到这个头文件中

namespace msdk{;
namespace msapi{;



TCHAR tcSub[] = L"Software\\Microsoft\\Internet Explorer\\Main";

static BOOL CALLBACK EnumIEWindowsProc(HWND hwnd, LPARAM lParam )
{
	HWND* g_hAttrWnd = (HWND*)lParam;
	TCHAR tcName[MAX_PATH] = {0};
	GetWindowText( hwnd, tcName, MAX_PATH);
	if ( _tcscmp(tcName, L"Internet 属性") == 0 || _tcscmp(tcName, L"Internet Options") == 0)
	{
		*g_hAttrWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

static BOOL SetIEStartPage( LPCTSTR tcPage)
{
	HKEY hKey;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, tcSub, 0, KEY_READ|KEY_WRITE, &hKey))
		return FALSE;

	std::wstring wstr(tcPage);
	int len = wstr.length();
	if( ERROR_SUCCESS != RegSetValueEx(hKey, L"Start Page", NULL, REG_SZ, (BYTE*)tcPage, (len+1)*sizeof(TCHAR)))
		return FALSE;

	RegCloseKey(hKey);

	HDESK hDesk = CreateDesktop(L"IESet", NULL, NULL, 0, GENERIC_ALL, NULL);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	STARTUPINFO si;     
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = L"IESet";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	TCHAR tcSysDir[MAX_PATH] = {0};
	GetSystemDirectory(tcSysDir, MAX_PATH);
	_tcscat(tcSysDir, L"\\rundll32.exe" );
	BOOL b = CreateProcess( tcSysDir, L" shell32.dll, Control_RunDLL Inetcpl.cpl,0", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi );
	if ( !b )
		return FALSE;
	
	Sleep(2000);

	HWND g_hAttrWnd = NULL;

	EnumDesktopWindows(hDesk, EnumIEWindowsProc, (LPARAM)&g_hAttrWnd);
	HWND hMainWnd = g_hAttrWnd;
	if ( g_hAttrWnd )
	{
		g_hAttrWnd = FindWindowEx( hMainWnd, NULL, NULL, L"常规" );
		if ( g_hAttrWnd == NULL )
			g_hAttrWnd = FindWindowEx( hMainWnd, NULL, NULL, L"General" );

		if ( g_hAttrWnd )
		{
			g_hAttrWnd = FindWindowEx( g_hAttrWnd, NULL, L"Edit", NULL);
			::SendMessage( g_hAttrWnd, WM_SETTEXT, NULL, (LPARAM)tcPage );
			PostMessage(g_hAttrWnd, WM_KEYDOWN, VK_SPACE, 0);  
			PostMessage(g_hAttrWnd, WM_KEYUP, VK_SPACE, 0); 
			PostMessage( g_hAttrWnd, WM_KEYDOWN, VK_BACK, 0);
			PostMessage( g_hAttrWnd, WM_KEYUP, VK_BACK, 0);

		}
		HWND hIEWnd = FindWindowEx( hMainWnd, NULL, NULL, L"确定");
		if ( hIEWnd == NULL)
			hIEWnd = FindWindowEx( hMainWnd, NULL, NULL, L"OK");

		if ( hIEWnd )
		{
			Sleep(100);
			SendMessage(hIEWnd, WM_SETFOCUS, 0, 0);  
			SendMessage(hIEWnd, WM_LBUTTONDOWN, 0, 0);  
			SendMessage(hIEWnd, WM_LBUTTONUP, 0, 0);  
		}
	}
	CloseDesktop(hDesk);
	return TRUE;
}


};};