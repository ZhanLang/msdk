#include "StdAfx.h"
#include "FileMonitorSrv.h"

CFileMonitorSrv::CFileMonitorSrv(void)
{
}

CFileMonitorSrv::~CFileMonitorSrv(void)
{
}

STDMETHODIMP CFileMonitorSrv::Init(void*)
{	
	//Sleep(9000);
	RFAILED(DllQuickCreateInstance(CLSID_FileMonitor,re_uuidof(IFileMonitor),m_pFileMonitor,NULL));
	RASSERT(m_pFileMonitor,E_FAIL);

	RASSERT(AddConnectionPoint(re_uuidof(IOnFileMonitorConnectPoint), m_FileChangeConnectPoint),E_FAIL);
	return S_OK;
}

STDMETHODIMP CFileMonitorSrv::Uninit()
{
	if(m_pFileMonitor)
	{
		m_pFileMonitor->CloseMonitor();
		//m_pFileMonitor->RemoveMonitor(NULL);
		
		m_pFileMonitor = INULL;
	}

	DelConnectionPoint(re_uuidof(IOnFileMonitorConnectPoint), m_FileChangeConnectPoint);
	m_FileChangeConnectPoint.EmptyConnection();
	
	if (m_pRot)
	{
		m_pRot = INULL;
	}

	return S_OK;
}

STDMETHODIMP CFileMonitorSrv::Start()
{
	UTIL::com_ptr<IMSBase> m_diskMonitor;
	RFAILED(m_pRot->GetObject(CLSID_DiskMonitor,re_uuidof(IMSBase),(void**)&m_diskMonitor));
	RASSERT(m_diskMonitor,E_FAIL);
	RFAILED(m_useDiskChangePoint.Connect(m_diskMonitor,UTIL::com_ptr<IMSBase>(this)));
	if (m_pFileMonitor)
	{
		return m_pFileMonitor->OpenMonitor(NULL,0,this);
	}
	
	return E_FAIL;
}

STDMETHODIMP CFileMonitorSrv::Stop()
{
	m_useDiskChangePoint.DisConnect();
	if (m_pFileMonitor)
	{
		return m_pFileMonitor->RemoveMonitor(NULL);
	}

	return S_OK;
}


STDMETHODIMP CFileMonitorSrv::OnFileActionChange(DWORD dwMask,LPCWSTR lpName, DWORD dwAttribute)
{
	//FUNC_TIME(_T("CFileMonitorSrv::OnFileActionChange{%s}"), lpName);
	CallConnectPointFunc(m_FileChangeConnectPoint,
		IOnFileMonitorConnectPoint,
		OnFileChangeNotify(dwMask,lpName,dwAttribute));

	return S_OK;
}
STDMETHODIMP CFileMonitorSrv::OnFileActionTimeOut(DWORD dwTimer)
{
	CallConnectPointFunc(m_FileChangeConnectPoint,
		IOnFileMonitorConnectPoint,
		OnFileChangeTimeOut(dwTimer));
	return S_OK;
}

STDMETHODIMP CFileMonitorSrv::OnDiskChange(
						DWORD dwCtrlMask,   //盘符操作掩码:添加删除
						DWORD dwType,		//盘符类型，U盘，硬盘。。。DiskType_Unknown
						LPCWSTR lpName		//盘符名称:c:\\ ;
						)
{
	if (dwCtrlMask == DiskCtrl_Ready/*DiskCtrl_Mount*/)
	{
		HRESULT hr = m_pFileMonitor->AppendMonitor(lpName);
		GrpMsg(GroupName,MsgLevel_Msg,_T("磁盘(%s)插入。创建文件监视对象(%s)"),lpName,hr == S_OK ? _T("成功"):_T("失败"));

		CallConnectPointFunc(m_FileChangeConnectPoint,
			IOnFileMonitorConnectPoint,
			OnAppendMonitor(lpName,hr == S_OK));	
	}
	if (dwCtrlMask == DiskCtrl_uMountQuery)
	{
		HRESULT hr = m_pFileMonitor->RemoveMonitor(lpName);
		GrpMsg(GroupName,MsgLevel_Msg,_T("磁盘(%s)移除询问。删除文件监视对象(%s)"),lpName,hr == S_OK ? _T("成功"):_T("失败"));

		CallConnectPointFunc(m_FileChangeConnectPoint,
			IOnFileMonitorConnectPoint,
			OnRemoveMonitor(lpName,hr == S_OK));
		
	}
	if (dwCtrlMask == DiskCtrl_uMountFailed)
	{
		HRESULT hr = m_pFileMonitor->AppendMonitor(lpName);
		GrpMsg(GroupName,MsgLevel_Msg,_T("磁盘(%s)移除失败。重新建立文件监视对象(%s)"),lpName,hr == S_OK ? _T("成功"):_T("失败"));
	}
	if (dwCtrlMask == DiskCtrl_uMount)
	{
		HRESULT hr = m_pFileMonitor->RemoveMonitor(lpName);
		GrpMsg(GroupName,MsgLevel_Msg,_T("磁盘(%s)卸载。卸载监视(%s)"),lpName,hr == S_OK ? _T("成功"):_T("失败"));
	}
	return S_OK;
}