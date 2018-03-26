#pragma once
//调整屏幕分辨率

class CMonitorResolution
{
public:
	struct DisplayInfo
	{
		DWORD dwW;
		DWORD dwH;
	};
public:

	//设置最佳分辨率
	static BOOL AdjustResolution()
	{
		std::vector<DisplayInfo> displayInfo;
		if ( !GetDisplayInfo( displayInfo ) )
			return FALSE;

		SIZE sz = GetCurResolution();
		for (UINT i = 0; i < displayInfo.size(); i++)
		{
			if ((DWORD)sz.cx == displayInfo[i].dwW && (DWORD)sz.cy == displayInfo[i].dwH)
				return TRUE;
			
			if (ChangeResolution(displayInfo[i].dwW, displayInfo[i].dwH))
				return TRUE;
		}

		return FALSE;
	}

	static BOOL ChangeResolution( DWORD dwX, DWORD dwY)
	{
		DEVMODE DevMd;
		DevMd.dmSize = sizeof(DevMd);
		DevMd.dmPelsWidth = dwX; //水平像素
		DevMd.dmPelsHeight = dwY; //垂直像素
		DevMd.dmBitsPerPel = 32; //32位真彩
		DevMd.dmDisplayFrequency = 60; //刷新率Hz
		DevMd.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		if(ChangeDisplaySettings(&DevMd,0) == DISP_CHANGE_SUCCESSFUL)
		{
			ChangeDisplaySettings(&DevMd,CDS_UPDATEREGISTRY); //永久的修改设置
			printf("ChangeDisplaySettings(%d,%d)\r\n", dwX, dwY);
			return TRUE;
		}
		else
		{
			ChangeDisplaySettings(NULL,0);
			printf("ChangeDisplaySettings faild. err:%d.\r\n",GetLastError());
			return FALSE;
		}

		return FALSE;
	}
	
	static  BOOL GetDisplayInfo( std::vector<DisplayInfo>& displayInfo )
	{
		CMonitorResolution monitorResolution;
		if ( monitorResolution.Init() )
			return monitorResolution.GetDisplay(displayInfo);
		
		return FALSE;
	}
	
	static SIZE GetCurResolution()
	{
		SIZE szRet = {GetSystemMetrics ( SM_CXSCREEN ),GetSystemMetrics ( SM_CYSCREEN )};
		printf("GetCurResolution: %d, %d\r\n", szRet.cx, szRet.cy);
		return szRet;
		/*
		HKEY hKey;
		LONG lRes = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SYSTEM\\ControlSet001\\Hardware Profiles\\UnitedVideo\\SERVICES\\BASICDISPLAY\\Mon00000000"), &hKey);
		if (lRes != ERROR_SUCCESS){
			return szRet;
		}

		DWORD dwType = REG_DWORD;
		DWORD dwVal = 0;
		DWORD dwSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("DefaultSettings.XResolution"), NULL, &dwType, (LPBYTE)&dwVal, &dwSize);

		szRet.cx = dwVal;

		dwVal = 0;
		dwSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("DefaultSettings.YResolution"), NULL, &dwType, (LPBYTE)&dwVal, &dwSize);
		szRet.cy = dwVal;
		return szRet;
		*/
	}

public:
	BOOL Init()
	{
		HKEY hKey;
		LONG lRes = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\"), &hKey);
		if (lRes != ERROR_SUCCESS)
			return FALSE;
		

		DWORD dwIndexs = 0;
		TCHAR keyName[MAX_PATH] = { 0 };
		DWORD charLength = 256;

		while (RegEnumKeyEx(hKey, dwIndexs, keyName, &charLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hTmp;
			lRes = RegOpenKey(hKey, keyName, &hTmp);
			if (lRes == ERROR_SUCCESS)
			{
				ReadDisplay(hTmp);
				RegCloseKey(hTmp);
			}

			++dwIndexs;
			charLength = 256;
			memset(keyName, 0, MAX_PATH*2);
		}

		RegCloseKey(hKey);

		return m_DisplayInfo.size() ? TRUE : FALSE;
	}

	BOOL GetDisplay( std::vector<DisplayInfo>& displayInfo )
	{
		displayInfo = m_DisplayInfo;
		return displayInfo.size() ? TRUE : FALSE;
	}

private:
	void ReadDisplay(HKEY hKey)
	{
		DWORD dwIndexs = 0;
		TCHAR keyName[MAX_PATH] = { 0 };
		DWORD charLength = 256;

		while (RegEnumKeyEx(hKey, dwIndexs, keyName, &charLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hTmp;
			LONG lRes = RegOpenKey(hKey, keyName, &hTmp);
			if (lRes == ERROR_SUCCESS)
			{
				ReadEDID(hTmp);
				RegCloseKey(hTmp);
			}

			++dwIndexs;
			charLength = 256;
			memset(keyName, 0, MAX_PATH*2);
		}
	}

	void ReadEDID(HKEY hKey)
	{
		HKEY hTmp;
		LONG lRes = RegOpenKey(hKey, _T("Device Parameters"), &hTmp);
		if (lRes != ERROR_SUCCESS){
			return;
		}

		BYTE buf[256];
		DWORD dwSize = 256;
		DWORD dwType;
		lRes = RegQueryValueEx(hTmp, _T("EDID"), NULL, &dwType, buf, &dwSize);
		if (lRes != ERROR_SUCCESS)
		{
			printf("err:%d\n", lRes);
			RegCloseKey(hTmp);
			return;
		}

		DisplayInfo info;
		info.dwW = ((buf[58]&0xF0)<<4) + buf[56];
		info.dwH =  ((buf[61]&0xF0)<<4) + buf[59];

		m_DisplayInfo.push_back(info);
		printf("wid:%u, hi:%u\n", info.dwW, info.dwH);

		RegCloseKey(hTmp);
	}
private:
	std::vector<DisplayInfo> m_DisplayInfo;
};