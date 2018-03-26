#pragma once

#ifdef DEVICECONTROL_DLL
#	define DEVICECONTROL_API _declspec(dllexport)
#else
#	define DEVICECONTROL_API	
#endif // DEVICECONTROL_DLL

enum  DeviceTypeEnum
{
	DEVICE_TYPE_UNKNOWN			= 0x00000,			// 未知设备
	DEVICE_TYPE_CDROM			= 0x00002,			// 光驱
	DEVICE_TYPE_DISKDRIVE		= 0x00004,			//	磁盘设备
	DEVICE_TYPE_USBSTORAGE		= 0x00008,			//  USB存储设备
	DEVICE_TYPE_PROCESSOR		= 0x00010,			//	处理器
	DEVICE_TYPE_NETCARD			= 0x00020,			//	网卡
	DEVICE_TYPE_BASEBOARD		= 0x00040,			//	主板
	DEVICE_TYPE_ALL				= 0xFFFFF			// 多媒体设备
};

struct IDevice
{
	virtual DWORD			GetDeviceID()	= 0;		//id
	virtual DeviceTypeEnum	GetDeviceType() = 0;		//设备类型
	
	//有些设备不支持
	virtual BOOL			StartDevice()	= 0;		//启用设备
	virtual BOOL			StopDevice()	= 0;		//停用设备

	//获取属性
	virtual LPCWSTR			GetSerialNumber() = 0;		//获取设备序列号
	virtual	LPCWSTR			GetManufacturer() = 0;		//获取制造商
	virtual LPCWSTR			GetDescription()  = 0;		//获取设备描述
	virtual LPCWSTR			GetSpecification()= 0;		//获取设备规格
	virtual LPCWSTR			GetDeviceModel()  = 0;		//获取设备型号

	//获取扩展属性，当该设备的某个属性具有特殊性的时候，使用该接口
	virtual LPCWSTR			GetProperty(DWORD dwProperty) = 0;

};

//硬件扫描控制码
enum DeviceScanControl
{
	DEVICE_SCAN_CONTROL_START =0	,  //开始扫描
	DEVICE_SCAN_CONTROL_STOP		,  //停止扫描
};

//硬件扫描结果码
enum DeviceScanResult
{
	DEVICE_SCAN_RESULT_BEGIN		,	//开始下载
	DEVICE_SCAN_RESULT_BUCCESS		,	//成功
	DEVICE_SCAN_RESULT_FAILURE		,	//失败
	DEVICE_SCAN_RESULT_CANCEL		,	//用户取消
};

struct IDeviceManage
{
	virtual BOOL     ScanControl(DeviceScanControl dm, DeviceTypeEnum dt = DEVICE_TYPE_ALL, BOOL bSync = FALSE/*是否采用同步扫描*/) = 0;

	virtual DWORD    GetDeviceConut() = 0;
	virtual IDevice* GetDevice(DWORD dwIndex) = 0;
	virtual IDevice* GetDeviceFromID(DWORD	dwID) = 0;
	virtual	VOID	 Clear() = 0;

	//判断是否虚拟机
	virtual BOOL	IsVM() = 0;
};


struct IDeviceManageNotify
{
	virtual VOID OnDeviceTypeScanNotify(DeviceTypeEnum dte, BOOL isEnded, BOOL isSuccessed) = 0;
	virtual VOID OnScanControlNotify(DeviceScanResult dmr) = 0;
	virtual VOID OnDevieceFind(IDevice* pDevice) = 0;
	//virtual VOID OnDeviceChangeNotify() = 0
};

struct IDeviceManageFactory : public IMSBase
{
	virtual IDeviceManage* CreateDeviceManage(IDeviceManageNotify* pNotify) = 0;
	virtual VOID DestoryDeviceManage(IDeviceManage*) = 0;
};
MS_DEFINE_IID(IDeviceManageFactory, "{D9024199-CD82-4cbf-87AD-FCECA00CB6C7}");

// {5AE385A1-901D-4b49-947D-F5F6E87435C6}
MS_DEFINE_GUID(CLSID_DeviceManageFactory,
	0x5ae385a1, 0x901d, 0x4b49, 0x94, 0x7d, 0xf5, 0xf6, 0xe8, 0x74, 0x35, 0xc6);



//lua扩展
// {594F28B5-F7B8-4cb5-8715-E25AAD2EBD3B}
MS_DEFINE_GUID(CLSID_OsInfoLuaEx, 
	0x594f28b5, 0xf7b8, 0x4cb5, 0x87, 0x15, 0xe2, 0x5a, 0xad, 0x2e, 0xbd, 0x3b);



extern "C"
{
	DEVICECONTROL_API IDeviceManage* WINAPI CreateDeviceManage(IDeviceManageNotify* pNotify);
	DEVICECONTROL_API VOID WINAPI DestoryDeviceManage(IDeviceManage*);
};