#pragma once

#include <filemonitor/IFileMonitor.h>
#include <mscomhelper/connectionpointhelper.h>
#include "mscomhelper/UseConnectionPoint.h"
//监视所有盘符
class CFileMonitorSrv
	:public IMsPlugin
	,public IMsPluginRun
	,public IOnFileMonitor
	,public IDiskChangeConnectPoint
	,public CConnectionPointContainerHelper<CFileMonitorSrv> //实现连接点
	,private CUnknownImp
{
public:
	CFileMonitorSrv(void);
	~CFileMonitorSrv(void);
	UNKNOWN_IMP5_(IMsPlugin,IMsPluginRun,IOnFileMonitor,IMsConnectionPointContainer,IDiskChangeConnectPoint);
	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		RASSERT(pRot,E_INVALIDARG);
		m_pRot = pRot;
		return S_OK;
	}

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	STDMETHOD(OnFileActionChange)(DWORD dwMask,LPCWSTR lpName, DWORD dwAttribute);
	STDMETHOD(OnFileActionTimeOut)(DWORD dwTimer);
	STDMETHOD(OnDiskChange)(
		DWORD dwCtrlMask,   //盘符操作掩码:添加删除
		DWORD dwType,		//盘符类型，U盘，硬盘。。。DiskType_Unknown
		LPCWSTR lpName		//盘符名称:c:\\ ;
		);

private:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
	UTIL::com_ptr<IFileMonitor>	m_pFileMonitor;
	UTIL::com_ptr<IMSBase>		m_pDiskMonitor;
	CConnectionPointHelper m_FileChangeConnectPoint;
	UseConnectPoint<IDiskChangeConnectPoint> m_useDiskChangePoint;
};
