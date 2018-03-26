#pragma once

#include <filemonitor/IFileMonitor.h>

#include <mscomhelper/connectionpointhelper.h>
#include "mscomhelper/UseConnectionPoint.h"
#include "tray/traywndplugin.h"
#include <map>
#include <set>
#include <winuser.h>
#include "SyncObject/criticalsection.h"
#include "thread/SubThreadImp.h"
#include "UIBase.h"
//磁盘监控
class CDiskMonitor
	:public IMsPlugin
	,public IMsPluginRun
	//,public IWndMessageConnectPoint
	//,public IServiceCtrlConnectPoint
	,public CConnectionPointContainerHelper<CDiskMonitor> //实现连接点
	,public CSubThread
	,public CWindowWnd
	,CUnknownImp
{
	struct CVoumeNotify
	{
		HANDLE	   hVolume;
		HDEVNOTIFY hNotify;
	};

	typedef std::map<WCHAR,CVoumeNotify>  CVolumeMap;
	typedef std::vector<HDEVNOTIFY>		  CDevNofityVector;
public:
	UNKNOWN_IMP3_(IMsPlugin,IMsPluginRun,IMsConnectionPointContainer/*,IWndMessageConnectPoint,IServiceCtrlConnectPoint,*/);
	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		RASSERT(pRot,E_INVALIDARG);
		m_pRot = pRot;
		return S_OK;
	}

	CDiskMonitor(void);
	~CDiskMonitor(void);

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	STDMETHOD_(LRESULT, OnWndMessage)(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);
	STDMETHOD_(DWORD, OnServiceCtrl)(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

protected:
	virtual HRESULT	Run();

protected:
	 virtual LPCTSTR GetWindowClassName() const;
	 virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	HRESULT CreateVolumeMonitor(WCHAR cDosName);
	HRESULT RemoveVolumeMonitor(WCHAR cDosName, BOOL bRemoveNotify = TRUE);
	WCHAR	GetVolumeDosName(HDEVNOTIFY hNotify);
	BOOL	IsVolumeReady(WCHAR cDosName);
private:
	CConnectionPointHelper m_DiskChangePoint;

	

	//UseConnectPoint<IWndMessageConnectPoint> m_MessageConnectPoint;
	//UseConnectPoint<IServiceCtrlConnectPoint> m_ServiceCtrlConnectPoint;
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
	//UTIL::com_ptr<ITrayWnd> m_pTrayWnd;
	UTIL::com_ptr<IMsEnv>	m_pEnv;
	CVolumeMap m_hDevNotifyMap;
	CDevNofityVector m_DevNofityVector;

	SERVICE_STATUS_HANDLE m_hService;
	HWND				  m_hNotifyWnd;
	HANDLE				  m_hReadyEvent;
	DECLARE_AUTOLOCK_CS(m);

	DECLARE_AUTOLOCK_CS(m_volumeReadySet);
	std::set<WCHAR>		m_volumeReadySet;
};
