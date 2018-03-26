#pragma once
#include "devicecontrol\idevicecontrol.h"
class CDeviceManageFactory :
	public IDeviceManageFactory,
	public CUnknownImp
{
public:
	UNKNOWN_IMP1(IDeviceManageFactory);

	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		m_pRot = pRot;
		return S_OK;
	}

	CDeviceManageFactory(void);
	~CDeviceManageFactory(void);

	IDeviceManage* CreateDeviceManage(IDeviceManageNotify* pNotify)
	{
		return ::CreateDeviceManage(pNotify);
	}

	VOID DestoryDeviceManage(IDeviceManage* P)
	{
		::DestoryDeviceManage(P);
	}

private:
	UTIL::com_ptr<IMscomRunningObjectTable>	m_pRot;
};

