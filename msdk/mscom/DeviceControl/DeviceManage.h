#pragma once
#include "win32-thread.h"
#include "SyncObject/criticalsection.h"
#include "Device.h"
#include <map>
#include <vector>


class CDeviceManage
	:public IDeviceManage
	,public DeviceScanNotify
	,public Win32Thread
	,private CUnknownImp
{
public:
	CDeviceManage(IDeviceManageNotify* pNotify);
	~CDeviceManage(void);

	virtual BOOL     ScanControl(DeviceScanControl dm, DeviceTypeEnum dt = DEVICE_TYPE_ALL, BOOL bSync = FALSE) ;
	virtual DWORD    GetDeviceConut() ;
	virtual IDevice* GetDevice(DWORD dwIndex);
	virtual IDevice* GetDeviceFromID(DWORD	id);
	virtual	VOID	 Clear();
	virtual BOOL	 IsVM();
protected:
	virtual void threadBody();
	virtual void onThreadDead();
	
	virtual VOID OnDeviceScanNotify(CDevice* pDevice);

private:
	BOOL IsCancel();
	BOOL Scan();
	BOOL InsertDevice(IDevice* pDevice);


	VOID OnDeviceTypeScanNotify(DeviceTypeEnum dte, BOOL isEnded, BOOL isSuccessed);
	VOID OnScanControlNotify(DeviceScanResult dmr);
	VOID OnDevieceFind(IDevice* pDevice);

	BOOL CheckVmFromDiskDevice(IDevice* pDevice);
private:
	typedef std::vector<IDevice*> CDeviceArray;
	typedef std::map<DWORD, IDevice*> CDeviceMap;


	IDeviceManageNotify* m_pDeviceManageNotify;
	DeviceTypeEnum		 m_ScanDeviceType;
	BOOL				 m_bCancel;

	

	CDeviceArray	m_DeviceArray;
	CDeviceMap		m_DeviceMap;

	CAutoCriticalSection	m_DeviceLock;

};
