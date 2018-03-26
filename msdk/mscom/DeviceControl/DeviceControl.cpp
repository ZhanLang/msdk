// DeviceControl.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DeviceControl.h"
#include "DeviceManage.h"



EXTERN_C
{
	DEVICECONTROL_API IDeviceManage* WINAPI CreateDeviceManage(IDeviceManageNotify* pNotify)
	{
		//RASSERT(pNotify, NULL);
		CDeviceManage* pManage = new CDeviceManage(pNotify);
		return pManage;
	}

	DEVICECONTROL_API VOID WINAPI DestoryDeviceManage(IDeviceManage* p)
	{
		CDeviceManage* pManage = static_cast<CDeviceManage*>(p);
		
		if (pManage)
		{
			delete pManage;
		}
	}

};
