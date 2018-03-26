#pragma once

#define FileActionAdded				0x00000001
#define FileActionRemoved			0x00000002
#define FileActionModified			0x00000003
#define FileActionRenamedOldName	0x00000004
#define FileActionRenamedNewName	0x00000005

struct IOnFileMonitor :public IUnknown
{
	STDMETHOD(OnFileActionChange)(
		DWORD dwMask,//FileActionAdded
		LPCWSTR lpName, 
		DWORD dwAttribute
		) = 0;


	STDMETHOD(OnFileActionTimeOut)(DWORD dwTimer) = 0;
};

MS_DEFINE_IID(IOnFileMonitor, "{071963EF-23DC-4EE3-A4E1-32FE84A013B7}");

struct IFileMonitor  :public IUnknown
{
	STDMETHOD(OpenMonitor)(LPCWSTR* szVolumes, DWORD dwCount, IOnFileMonitor* pOnVolumeMonitor) = 0;
	STDMETHOD(CloseMonitor)() = 0;
	STDMETHOD(AppendMonitor)(LPCWSTR szVolumes) = 0;
	STDMETHOD(RemoveMonitor)(LPCWSTR szVolumes = NULL) = 0;
};
MS_DEFINE_IID(IFileMonitor, "{E27FA54D-A05E-414F-AB38-0B4BE808CF3A}");

// {DA88C118-4434-4c5e-A23E-DCA3A292C1AE}
MS_DEFINE_GUID(CLSID_FileMonitor, 
			0xda88c118, 0x4434, 0x4c5e, 0xa2, 0x3e, 0xdc, 0xa3, 0xa2, 0x92, 0xc1, 0xae);



struct IOnFileMonitorConnectPoint :public IUnknown
{
	STDMETHOD(OnFileChangeNotify)(DWORD dwMask,LPCWSTR lpName, DWORD dwAttribute) = 0;
	STDMETHOD(OnFileChangeTimeOut)(DWORD dwTimer) = 0;
	STDMETHOD(OnAppendMonitor)(LPCWSTR lpVolumes,BOOL succeed) = 0;
	STDMETHOD(OnRemoveMonitor)(LPCWSTR szVolumes,BOOL succeed) = 0;
};
MS_DEFINE_IID(IOnFileMonitorConnectPoint, "{0EC1D877-89FD-4155-870A-26467359FB53}");

/*
	ROT对象:监视本机所有盘符的文件
	实现接口 ：IMsPlugin;IMsPluginRun
*/
// {C05AFE0B-1CFB-461f-87B2-CBAA81FEDA37}
MS_DEFINE_GUID(CLSID_FileMonitorSrv, 
			0xc05afe0b, 0x1cfb, 0x461f, 0x87, 0xb2, 0xcb, 0xaa, 0x81, 0xfe, 0xda, 0x37);


enum DiskType
{
	DiskType_Unknown,
	DiskType_No_Root_Dir,
	DiskType_Removable	,
	DiskType_Fixed,
	DiskType_Remote,
	DiskType_CDROM,
	DiskType_RAMDISK,
};


/**
*/
enum DiskCtrl
{
	DiskCtrl_Mount,
	DiskCtrl_Ready,
	DiskCtrl_uMount,
	DiskCtrl_uMountQuery,
	DiskCtrl_uMountFailed,
};


struct IDiskChangeConnectPoint :public IUnknown
{
	STDMETHOD(OnDiskChange)(
		DWORD dwCtrlMask,   //盘符操作掩码:添加删除 DiskCtrl_Mount
		DWORD dwType,		//盘符类型，U盘，硬盘。。。DiskType_Unknown
		LPCWSTR lpName		//盘符名称:c:\\ ;
		) = 0;

};

MS_DEFINE_IID(IDiskChangeConnectPoint, "{FAA8713B-306D-41e3-81A3-766CCDA38291}");
// {14F50ED6-4F0F-42e0-BA27-98D3B30E8CD4}
MS_DEFINE_GUID(CLSID_DiskMonitor,
			0x14f50ed6, 0x4f0f, 0x42e0, 0xba, 0x27, 0x98, 0xd3, 0xb3, 0xe, 0x8c, 0xd4);

/*
	说明:CLSID_FileMonitorSrv 依赖ROT，traywnd
*/

/* msocm configure
<moudle guid='{72F26EDA-275E-4699-8F75-ECE41C297655}'  path='.\filemonitor.dll'>
	<class clsid='{DA88C118-4434-4c5e-A23E-DCA3A292C1AE}'  progid='CLSID_FileMonitor.1'  name='CLSID_FileMonitor'/>
	<class clsid='{C05AFE0B-1CFB-461f-87B2-CBAA81FEDA37}'  progid='CLSID_FileMonitorSrv.1'  name='CLSID_FileMonitorSrv'/>
	<class clsid='{14F50ED6-4F0F-42e0-BA27-98D3B30E8CD4}'  progid='CLSID_DiskMonitor.1'  name='CLSID_DiskMonitor'/>

</moudle>
*/

/*rot
<!--begin filemonitor-->
<class clsid='{C05AFE0B-1CFB-461f-87B2-CBAA81FEDA37}' name='CLSID_FileMonitorSrv'  mustinit='1' muststart='1'/>
 <class clsid='{14F50ED6-4F0F-42e0-BA27-98D3B30E8CD4}' name='CLSID_DiskMonitor'  mustinit='1' muststart='1'/>
<!--end filemonitor-->
*/