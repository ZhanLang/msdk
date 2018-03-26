#include "StdAfx.h"
#include "DiskMonitor.h"
#include "WinIoCtl.h "
#include <Dbt.h>

/*
MS_DEFINE_GUID( GUID_IO_VOLUME_DISMOUNT, 0xd16a55e8L, 0x1059, 0x11d2, 0x8f, 0xfd, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_DISMOUNT_FAILED, 0xe3c5b178L, 0x105d, 0x11d2, 0x8f, 0xfd, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_MOUNT, 0xb5804878L, 0x1a96, 0x11d2, 0x8f, 0xfd, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_LOCK, 0x50708874L, 0xc9af, 0x11d1, 0x8f, 0xef, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_LOCK_FAILED, 0xae2eed10L, 0x0ba8, 0x11d2, 0x8f, 0xfb, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_UNLOCK, 0x9a8c3d68L, 0xd0cb, 0x11d1, 0x8f, 0xef, 0x00, 0xa0, 0xc9, 0xa0, 0x6d, 0x32 );
MS_DEFINE_GUID( GUID_IO_VOLUME_NAME_CHANGE, 0x2de97f83, 0x4c06, 0x11d2, 0xa5, 0x32, 0x0, 0x60, 0x97, 0x13, 0x5, 0x5a);
MS_DEFINE_GUID( GUID_IO_VOLUME_PHYSICAL_CONFIGURATION_CHANGE, 0x2de97f84, 0x4c06, 0x11d2, 0xa5, 0x32, 0x0, 0x60, 0x97, 0x13, 0x5, 0x5a);
MS_DEFINE_GUID( GUID_IO_VOLUME_DEVICE_INTERFACE, 0x53f5630d, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
*/
GUID g_diskGuids[] = 
{
	GUID_DEVINTERFACE_DISK,
	GUID_DEVINTERFACE_CDROM,
	GUID_DEVINTERFACE_FLOPPY,
	GUID_DEVINTERFACE_TAPE,
};


CDiskMonitor::CDiskMonitor(void)
{
	m_hService = NULL;
	m_hNotifyWnd = NULL;
	m_hReadyEvent = NULL;
}

CDiskMonitor::~CDiskMonitor(void)
{
}


STDMETHODIMP CDiskMonitor::Init(void*)
{
	HWND hWnd = CWindowWnd::Create(NULL, _T("filemonitor"), WS_OVERLAPPEDWINDOW, UI_WNDSTYLE_EX_FRAME);

	if ( hWnd )
		GrpMsg(GroupName, MsgLevel_Msg, _T("创建窗口成功"));
	else
		GrpMsg(GroupName, MsgLevel_Msg, _T("创建窗口失败"));

// 	m_pRot->GetObject(ClSID_CTrayWnd,re_uuidof(ITrayWnd),(void**)&m_pTrayWnd);
// 
// 	if (m_pTrayWnd)
// 	{
// 		m_MessageConnectPoint.Connect(m_pTrayWnd,UTIL::com_ptr<IMSBase>(this));
// 		GrpMsg(GroupName, MsgLevel_Msg, _T("挂载到托盘链接点"));
// 	}
// 	
// 	m_pRot->GetObject(CLSID_MsEnv, re_uuidof(IMsEnv), (void**)&m_pEnv);
// 	if (m_pEnv)
// 	{
// 		m_ServiceCtrlConnectPoint.Connect(m_pEnv,UTIL::com_ptr<IMSBase>(this));
// 		GrpMsg(GroupName, MsgLevel_Msg, _T("挂载到服务链接点"));
// 	}

	m_hReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	StartThread();
	AddConnectionPoint(re_uuidof(IDiskChangeConnectPoint),m_DiskChangePoint);
	return S_OK;
}

STDMETHODIMP CDiskMonitor::Uninit()
{
	DelConnectionPoint(re_uuidof(IDiskChangeConnectPoint),m_DiskChangePoint);
	//m_MessageConnectPoint.DisConnect();
	//m_ServiceCtrlConnectPoint.DisConnect();

	CSubThread::StopThread(TRUE);
	m_volumeReadySet.clear();

	SAFE_RELEASE(m_pEnv);
	SAFE_RELEASE(m_pRot);
	//SAFE_RELEASE(m_pTrayWnd);
	SAFE_CLOSEHANDLE(m_hReadyEvent);
	
//	CWindowWnd::Close();
	return S_OK;
}

STDMETHODIMP CDiskMonitor::Start()
{	
// 	m_hService = m_pEnv->GetServiceStatusHandle();
// 	if (m_pTrayWnd)
// 	{
// 		m_hNotifyWnd = m_pTrayWnd->GetWndHwnd();
// 	}
	
	m_hNotifyWnd = CWindowWnd::GetHWND();

	GrpMsg(GroupName, MsgLevel_Msg, _T("SetServiceStatusHandle:%d"), m_hService);



	for (int nIndex = 0 ; nIndex < _countof(g_diskGuids) ; nIndex++)
	{
		DEV_BROADCAST_DEVICEINTERFACE   NotificationFilter = {0};
		NotificationFilter.dbcc_size  = sizeof(NotificationFilter);
		NotificationFilter.dbcc_devicetype  =  DBT_DEVTYP_DEVICEINTERFACE ;
		NotificationFilter.dbcc_classguid = g_diskGuids[nIndex];
		HDEVNOTIFY hNotify = RegisterDeviceNotification(
			m_hNotifyWnd ? (HANDLE)m_hNotifyWnd :(HANDLE)m_hService,
			&NotificationFilter,
			m_hNotifyWnd? DEVICE_NOTIFY_WINDOW_HANDLE : DEVICE_NOTIFY_SERVICE_HANDLE
		);
		
		if (!hNotify)
		{
			GrpMsg(GroupName, MsgLevel_Msg, _T("RegisterDeviceNotification(%s, %d) Failed.") ,m_hNotifyWnd? _T("DEVICE_NOTIFY_WINDOW_HANDLE") : _T("DEVICE_NOTIFY_SERVICE_HANDLE"), m_hService);
			continue;
		}

		m_DevNofityVector.insert(m_DevNofityVector.end(),hNotify);
	}

	TCHAR lpDriveString[MAX_PATH] = {0};
	GetLogicalDriveStrings(MAX_PATH,lpDriveString);
	int nCount  = 0;  
	TCHAR * pDrive = lpDriveString;  
	for(int nlen =_tcslen(pDrive); nlen == 3 ;nCount++)  
	{   
		if (_tcsicmp(pDrive, _T("A:\\")) != 0)
		{
			WCHAR cDocName = pDrive[0];
			if(cDocName>=L'a') cDocName-=32;

			if (FAILED(CreateVolumeMonitor(cDocName)))
			{
				AUTOLOCK_CS(m_volumeReadySet);
				m_volumeReadySet.insert(cDocName);
				SetEvent(m_hReadyEvent);
			}
		}

		pDrive +=4;  
		nlen = _tcslen(pDrive); 
	}

	return S_OK;
}

STDMETHODIMP CDiskMonitor::Stop()
{
	CDevNofityVector::iterator itv = m_DevNofityVector.end();
	for (; itv != m_DevNofityVector.end() ; itv++)
	{
		UnregisterDeviceNotification(*itv);
	}

	CVolumeMap::iterator it = m_hDevNotifyMap.begin();
	for (;it != m_hDevNotifyMap.end() ; ++it)
	{
		RemoveVolumeMonitor(it->first,FALSE);
	}

	m_hDevNotifyMap.clear();

	//CWindowWnd::Close();
	::DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	return S_OK;
}

TCHAR FirstDriveFromMask (ULONG unitmask)  
{  
	TCHAR i;  

	for (i = 0; i < 26; ++i)  
	{  
		if (unitmask & 0x1)  
			break;  
		unitmask = unitmask >> 1;  
	}  
	return (i + 'A');  
}  

STDMETHODIMP_(DWORD) CDiskMonitor::OnServiceCtrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	if (dwControl == SERVICE_CONTROL_DEVICEEVENT)
	{
		BOOL bHandle = FALSE;
		OnWndMessage(WM_DEVICECHANGE, dwEventType, (LPARAM)lpEventData, bHandle);
	}
	return NO_ERROR;
}

STDMETHODIMP_(LRESULT)CDiskMonitor::OnWndMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	if (msg == WM_DEVICECHANGE)
	{
		switch(wParam)
		{
		case DBT_DEVICEARRIVAL:
			{	
				PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam; 
				switch(lpdb->dbch_devicetype)
				{
					case DBT_DEVTYP_VOLUME:
					{
						PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;  
						TCHAR cVolume = FirstDriveFromMask(lpdbv ->dbcv_unitmask);
						if(cVolume != 'A')
						{
							GrpMsg(GroupName,MsgLevel_Msg,_T("盘符触发消息%c(DBT_DEVICEARRIVAL)"),cVolume);
							CreateVolumeMonitor(cVolume);
							TCHAR strVolume[MAX_PATH] = {0};
							_stprintf_s(strVolume,MAX_PATH,_T("%c:\\"),cVolume);

							CallConnectPointFunc(m_DiskChangePoint,
								IDiskChangeConnectPoint,
								OnDiskChange(DiskCtrl_Mount,GetDriveType(strVolume),strVolume));

							if (IsVolumeReady(cVolume))
							{
								CallConnectPointFunc(m_DiskChangePoint,
									IDiskChangeConnectPoint,
									OnDiskChange(DiskCtrl_Ready,GetDriveType(strVolume),strVolume));
							}
							else
							{
								AUTOLOCK_CS(m_volumeReadySet);
								m_volumeReadySet.insert(cVolume);
								SetEvent(m_hReadyEvent);
							}
							break;
						}
					}
				}
				
				
				break;
			}
		case DBT_DEVICEQUERYREMOVE   :
			{
				PDEV_BROADCAST_HDR pDBHdr = (PDEV_BROADCAST_HDR) lParam;
				switch (pDBHdr->dbch_devicetype)
				{
				case DBT_DEVTYP_HANDLE:
					{
						PDEV_BROADCAST_HANDLE pDBHandle = (PDEV_BROADCAST_HANDLE) pDBHdr;
						WCHAR cDosName = GetVolumeDosName(pDBHandle->dbch_hdevnotify);
						if (cDosName)
						{
							GrpMsg(GroupName,MsgLevel_Msg,_T("盘符触发消息%c(DBT_DEVICEQUERYREMOVE)"),cDosName);
							TCHAR strVolume[MAX_PATH] = {0};
							_stprintf_s(strVolume,MAX_PATH,_T("%c:\\"),cDosName);
							RemoveVolumeMonitor(cDosName,FALSE);
							CallConnectPointFunc(m_DiskChangePoint,
								IDiskChangeConnectPoint,
								OnDiskChange(DiskCtrl_uMountQuery,GetDriveType(strVolume),strVolume));
							break;
						}
					}
				}
				break;
			}
		case DBT_DEVICEREMOVECOMPLETE :
			{
				PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;  
				switch(lpdb->dbch_devicetype)
				{
				case DBT_DEVTYP_VOLUME:
					{
						PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;  
						TCHAR cVolume = FirstDriveFromMask(lpdbv ->dbcv_unitmask);
						if(cVolume != 'A')
						{
							GrpMsg(GroupName,MsgLevel_Msg,_T("盘符触发消息%c(DBT_DEVICEREMOVECOMPLETE)"),cVolume);
							RemoveVolumeMonitor(cVolume,TRUE); //通知和句柄都移除

							TCHAR strVolume[MAX_PATH] = {0};
							_stprintf_s(strVolume,MAX_PATH,_T("%c:\\"),cVolume);


							CallConnectPointFunc(m_DiskChangePoint,
								IDiskChangeConnectPoint,
								OnDiskChange(DiskCtrl_uMount,GetDriveType(strVolume),strVolume));
							{
								AUTOLOCK_CS(m_volumeReadySet);
								m_volumeReadySet.erase(cVolume);
							}
							break;
						}
					}
				}
				break;
			}
		case DBT_DEVICEQUERYREMOVEFAILED:
			{
				PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;  
				switch(lpdb->dbch_devicetype)
				{
				case DBT_DEVTYP_HANDLE: //能到这一步说明盘符句柄已经被释放了
					{
						PDEV_BROADCAST_HANDLE pDBHandle = (PDEV_BROADCAST_HANDLE) lpdb;
						WCHAR cDosName = GetVolumeDosName(pDBHandle->dbch_hdevnotify);
						if(cDosName)
						{
							GrpMsg(GroupName,MsgLevel_Msg,_T("盘符触发消息%c(DBT_DEVICEQUERYREMOVEFAILED)"),cDosName);
							

							TCHAR strVolume[MAX_PATH] = {0};
							_stprintf_s(strVolume,MAX_PATH,_T("%c:\\"),cDosName);
							RemoveVolumeMonitor(cDosName,TRUE); //全部移除
							CreateVolumeMonitor(cDosName);	//还要重新建立一份
							CallConnectPointFunc(m_DiskChangePoint,
								IDiskChangeConnectPoint,
								OnDiskChange(DiskCtrl_uMountFailed,GetDriveType(strVolume),strVolume));
						}
					}
					break;
				}
				break;
			}
		}
	}
	return TRUE;
}

HRESULT CDiskMonitor::CreateVolumeMonitor(WCHAR cDosName)
{
	{
		AUTOLOCK_CS(m);
		RTEST(m_hDevNotifyMap.find(cDosName) != m_hDevNotifyMap.end(), S_OK);
	}

	TCHAR strDrivers[MAX_PATH] = {0};
	_stprintf_s(strDrivers,MAX_PATH,_T("%c://"),cDosName);
	UINT driveType = GetDriveType(strDrivers);
	RASSERT(!(DRIVE_FIXED != driveType && DRIVE_REMOVABLE != driveType) ,E_FAIL);

	DWORD dwMaxComLen = 0,dwFileSysFlag = 0;
	TCHAR szVolumeName[MAX_PATH] = {0};
	TCHAR fileSysBuf[MAX_PATH] = {0};

	RASSERT(GetVolumeInformation(strDrivers,szVolumeName,MAX_PATH,NULL,&dwMaxComLen,&dwFileSysFlag,fileSysBuf,MAX_PATH),E_FAIL);

	HANDLE  hU = CreateFile(strDrivers,
		GENERIC_READ,FILE_SHARE_READ | 
		FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL,
		0);

	RASSERT(hU && hU != INVALID_HANDLE_VALUE, E_FAIL);

	DEV_BROADCAST_HANDLE   NotificationFilter = {0};
	NotificationFilter.dbch_size  = sizeof(NotificationFilter);
	NotificationFilter.dbch_devicetype  =  DBT_DEVTYP_HANDLE ;
	NotificationFilter.dbch_handle = hU;

	HDEVNOTIFY hDevNotify = RegisterDeviceNotification(m_hNotifyWnd ? (HANDLE)m_hNotifyWnd :(HANDLE)m_hService,
		&NotificationFilter,
		m_hNotifyWnd? DEVICE_NOTIFY_WINDOW_HANDLE : DEVICE_NOTIFY_SERVICE_HANDLE);
	if (!hDevNotify)
	{
		CloseHandle(hU);
		return E_FAIL;
	}

	CVoumeNotify notify = {hU,hDevNotify};

	{
		AUTOLOCK_CS(m);
		m_hDevNotifyMap.insert(m_hDevNotifyMap.end() , CVolumeMap::value_type(cDosName,notify));
	}
	
	return S_OK;
}

HRESULT CDiskMonitor::RemoveVolumeMonitor(WCHAR cDosName,BOOL RemoveNotify)
{
	AUTOLOCK_CS(m);
	CVolumeMap::iterator it = m_hDevNotifyMap.find(cDosName);
	if (it != m_hDevNotifyMap.end())
	{
		if (it->second.hNotify && RemoveNotify)
		{
			UnregisterDeviceNotification(it->second.hNotify);
			it->second.hNotify = NULL;
		}
		if (it->second.hVolume)
		{
			CloseHandle(it->second.hVolume);
			it->second.hVolume = NULL;
		}
		if (RemoveNotify)
		{
			m_hDevNotifyMap.erase(it);
		}
	}
	return S_OK;
}

WCHAR CDiskMonitor::GetVolumeDosName(HDEVNOTIFY hNotify)
{
	AUTOLOCK_CS(m);
	CVolumeMap::iterator it = m_hDevNotifyMap.begin();
	for (; it != m_hDevNotifyMap.end() ; it++)
	{
		if (hNotify == it->second.hNotify)
		{
			return  it->first;
		}
	}

	return 0;
}

HRESULT CDiskMonitor::Run()
{
	HANDLE hHandle[2] = {m_hExit, m_hReadyEvent};
	while(TRUE)
	{
		DWORD dwWait = WaitForMultipleObjects(_countof(hHandle), hHandle, FALSE, 1000);
		if(WAIT_OBJECT_0 + 0 == dwWait)	//m_hExit
		{
			break;
		}
		else if (WAIT_OBJECT_0 + 1 == dwWait || WAIT_TIMEOUT == dwWait) //m_hReadyEvent
		{
			AUTOLOCK_CS(m_volumeReadySet);
			for (std::set<WCHAR>::iterator it = m_volumeReadySet.begin() ; it != m_volumeReadySet.end() ; )
			{
				WCHAR cDosName = *it;
				CreateVolumeMonitor(cDosName);
				if (IsVolumeReady(*it))
				{
					it = m_volumeReadySet.erase(it);

					TCHAR strVolume[MAX_PATH] = {0};
					_stprintf_s(strVolume,MAX_PATH,_T("%c:\\"),cDosName);

					CallConnectPointFunc(m_DiskChangePoint,
						IDiskChangeConnectPoint,
						OnDiskChange(DiskCtrl_Ready,GetDriveType(strVolume),strVolume));
				}
				else
					++it;
			}
		}
	}

	return 0;
}

LPCTSTR CDiskMonitor::GetWindowClassName() const
{
	return _T("__filemonitor__");
}

LRESULT CDiskMonitor::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle =FALSE;
	OnWndMessage(uMsg, wParam, lParam,bHandle);
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

BOOL CDiskMonitor::IsVolumeReady( WCHAR cDosName )
{
	TCHAR strDrivers[MAX_PATH] = {0};
	_stprintf_s(strDrivers,MAX_PATH,_T("%c:\\"),cDosName);
	UINT driveType = GetDriveType(strDrivers);
	DWORD dwMaxComLen = 0,dwFileSysFlag = 0;
	TCHAR szVolumeName[MAX_PATH] = {0};
	TCHAR fileSysBuf[MAX_PATH] = {0};
	RASSERT(GetVolumeInformation(strDrivers,szVolumeName,MAX_PATH,NULL,&dwMaxComLen,&dwFileSysFlag,fileSysBuf,MAX_PATH),FALSE);
	return TRUE;
}
