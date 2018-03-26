#include "StdAfx.h"
#include "DeviceManage.h"
#include "Device.h"
#include "DiskDevice.h"
#include "NetCardDevice.h"
#include "CpuDevice.h"
#include "MemoryDevice.h"
#include "MBDevice.h"
#include <objbase.h>
#include "UsbStorageDevice.h"
#include "CDROMDevice.h"

CDeviceManage::CDeviceManage(IDeviceManageNotify* pNotify)
{
	m_pDeviceManageNotify = pNotify;
	m_bCancel = FALSE;
}

CDeviceManage::~CDeviceManage(void)
{
	Clear();
}

BOOL CDeviceManage::ScanControl(DeviceScanControl dm, DeviceTypeEnum dt /*= DT_All*/, BOOL bSync /*= FALSE*/)
{
	switch(dm)
	{
	case DEVICE_SCAN_CONTROL_START:
		{
			m_ScanDeviceType = dt;
			if (bSync)
			{
				Clear();
				OnScanControlNotify(DEVICE_SCAN_RESULT_BEGIN);
				CoInitializeEx(0, COINIT_MULTITHREADED );
				BOOL bRet = Scan();
				OnScanControlNotify(m_bCancel ? DEVICE_SCAN_RESULT_CANCEL : (bRet ? DEVICE_SCAN_RESULT_BUCCESS : DEVICE_SCAN_RESULT_FAILURE));
				CoUninitialize();
				return bRet;
			}
			else if (m_pDeviceManageNotify && !Win32Thread::IsActived())
			{
				m_bCancel = FALSE;
				Win32Thread::Close();
				Win32Thread::ResetStopEvent();
				return Win32Thread::Run(false) == true;
			}
			
			return FALSE;
		}
	case DEVICE_SCAN_CONTROL_STOP:
		{

			Close();
			/*
			if (Win32Thread::IsActived())
			{
				return FALSE;
			}

			m_bCancel = TRUE;
			Win32Thread::SetStopEvent();
			*/
			return TRUE;
		}
	}

	return FALSE;
}


void CDeviceManage::threadBody()
{
	OnScanControlNotify(DEVICE_SCAN_RESULT_BEGIN);
	Clear();
	CoInitializeEx(0, COINIT_MULTITHREADED );
	OnScanControlNotify(m_bCancel ? DEVICE_SCAN_RESULT_CANCEL : (Scan() ? DEVICE_SCAN_RESULT_BUCCESS : DEVICE_SCAN_RESULT_FAILURE));
	CoUninitialize();
}

BOOL CDeviceManage::Scan()
{
	if (m_ScanDeviceType & DEVICE_TYPE_USBSTORAGE)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_USBSTORAGE, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_USBSTORAGE, TRUE, CUsbStorageDevice::Scan(this));
	}
	if (m_ScanDeviceType & DEVICE_TYPE_DISKDRIVE)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_DISKDRIVE, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_DISKDRIVE, TRUE, CDiskDevice::Scan(this));
	}
	if (m_ScanDeviceType & DEVICE_TYPE_PROCESSOR)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_PROCESSOR, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_PROCESSOR, TRUE, CCpuDevice::Scan(this));
	}
	if(m_ScanDeviceType & DEVICE_TYPE_NETCARD)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_NETCARD, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_NETCARD, TRUE, CNetCardDevice::Scan(this));
	}
	if (m_ScanDeviceType & DEVICE_TYPE_CDROM)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_CDROM, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_CDROM, TRUE, CCDROMDevice::Scan(this));
	}
	if(m_ScanDeviceType & DEVICE_TYPE_BASEBOARD)
	{
		OnDeviceTypeScanNotify(DEVICE_TYPE_BASEBOARD, FALSE, TRUE);
		OnDeviceTypeScanNotify(DEVICE_TYPE_BASEBOARD, TRUE, CMBDevice::Scan(this));
	}
	return TRUE;
}

VOID CDeviceManage::OnDeviceScanNotify(CDevice* pDevice)
{
	RASSERT(pDevice,);
	InsertDevice(static_cast<IDevice*>(pDevice));
}

void CDeviceManage::onThreadDead()
{
	
}

DWORD CDeviceManage::GetDeviceConut()
{
	CStackAutoCSLock lock(m_DeviceLock);

	return m_DeviceArray.size();
}

IDevice* CDeviceManage::GetDevice(DWORD dwIndex)
{
	CStackAutoCSLock lock(m_DeviceLock);

	if (dwIndex < m_DeviceArray.size())
	{
		return m_DeviceArray[dwIndex];
	}

	return NULL;
}

IDevice* CDeviceManage::GetDeviceFromID(DWORD	id)
{
	CStackAutoCSLock lock(m_DeviceLock);

	CDeviceMap::iterator it = m_DeviceMap.find(id);
	if (it != m_DeviceMap.end())
	{
		return it->second;
	}

	return NULL;
}

VOID CDeviceManage::Clear()
{
	CStackAutoCSLock lock(m_DeviceLock);
	
	for (CDeviceArray::iterator it = m_DeviceArray.begin() ; it != m_DeviceArray.end() ; ++it)
	{
		CDevice* pDevice = static_cast<CDevice*>(*it);
		if (pDevice)
		{
			pDevice->DelayDelete();
		}
	}

	m_DeviceMap.clear();
	m_DeviceArray.clear();
}

BOOL CDeviceManage::InsertDevice(IDevice* pDevice)
{
	RASSERT(pDevice, FALSE);

	CStackAutoCSLock lock(m_DeviceLock);

	DWORD dwId = pDevice->GetDeviceID();
	CDeviceMap::iterator it = m_DeviceMap.find(dwId);
	if (it != m_DeviceMap.end())
	{
		CDevice* p = static_cast<CDevice*>(pDevice);
		delete p;
		return FALSE;
	}

	m_DeviceArray.push_back(pDevice);
	m_DeviceMap.insert(m_DeviceMap.end(), CDeviceMap::value_type(dwId, pDevice));
	OnDevieceFind(pDevice);
	return TRUE;
}

BOOL CDeviceManage::IsCancel()
{
	return Win32Thread::TestStopEvent() == true;
}

BOOL CDeviceManage::IsVM()
{
	IDeviceManage* pDeviceMgr = ::CreateDeviceManage(NULL);
	RASSERT(pDeviceMgr, FALSE);
	RASSERT(pDeviceMgr->ScanControl(DEVICE_SCAN_CONTROL_START, DEVICE_TYPE_DISKDRIVE, TRUE),FALSE);

	BOOL bVM = FALSE;
	DWORD dwCount = pDeviceMgr->GetDeviceConut();
	for (DWORD dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
	{
		IDevice* pDevice = pDeviceMgr->GetDevice(dwLoop);
		if (pDevice)
		{
			switch(pDevice->GetDeviceType())
			{
			case DEVICE_TYPE_DISKDRIVE:
				bVM |= CheckVmFromDiskDevice(pDevice);
			}
		}
	}

	::DestoryDeviceManage(pDeviceMgr);
	//ZM1_GrpDbgOutput(GroupName, _T("IsVM:%s"), bVM ? _T("TRUE"):_T("FALSE"));
	return bVM;
}

VOID CDeviceManage::OnDeviceTypeScanNotify(DeviceTypeEnum dte, BOOL isEnded, BOOL isSuccessed)
{
	if (m_pDeviceManageNotify)
	{
		m_pDeviceManageNotify->OnDeviceTypeScanNotify(dte, isEnded, isSuccessed);
	}
}

VOID CDeviceManage::OnScanControlNotify(DeviceScanResult dmr)
{
	if (m_pDeviceManageNotify)
	{
		m_pDeviceManageNotify->OnScanControlNotify(dmr);
	}
}

VOID CDeviceManage::OnDevieceFind(IDevice* pDevice)
{
	if (m_pDeviceManageNotify)
	{
		m_pDeviceManageNotify->OnDevieceFind(pDevice);
	}
}

BOOL CDeviceManage::CheckVmFromDiskDevice(IDevice* pDevice)
{
	CString lpszFName = pDevice->GetDeviceModel();
	CString lpszNumber = pDevice->GetSerialNumber();

	if(lpszNumber.Find(_T("VMWARE")) != -1)
	{
		return TRUE;
	}

	if (lpszFName.Find(_T("VMWARE")) != -1)
	{
		return TRUE;
	}

	if(lpszNumber.Find(_T("VBOX")) != -1)
	{
		return TRUE;
	}

	if (lpszFName.Find(_T("VBOX")) != -1)
	{
		return TRUE;
	}

	return FALSE;
}




