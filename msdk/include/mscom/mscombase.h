#pragma once

namespace msdk{
namespace mscom{


//////////////////////////////////////////////////////////////////////////
//mscom底层基础，初始化MSCOM运行环境，相当于注册表
struct IMsObjectLoader:public IMSBase
{
	//用到rclsid所对应的DLL模块时该DLL才会被加载到内存中
	STDMETHOD(CreateInstance)(IMSBase* prot, const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv) = 0;
};
MS_DEFINE_IID(IMsObjectLoader , "{51F1439C-549F-49be-A556-ADEC01F329D2}");


interface IMsOLManager: public IMSBase
{
	STDMETHOD(Register)(const CLSID& rclsid, LPCWSTR filename) = 0;
	STDMETHOD(Revoke)(const CLSID& rclsid) = 0;
};
MS_DEFINE_IID(IMsOLManager , "{FA06B469-8884-4066-8395-019957AE3659}");


/*实现接口
public IMsObjectLoader ,
public IMsOLManager ,
public IMsOLManagerEx ,
*/
//{B37BFFCA-29FA-4b19-AE42-5FF0F533D7E6}
MS_DEFINE_GUID(CLSID_ObjectLoader,
			   0xb37bffca, 0x29fa, 0x4b19, 0xae, 0x42, 0x5f, 0xf0, 0xf5, 0x33, 0xd7, 0xe6);


//////////////////////////////////////////////////////////////////////////
//全局唯一实例。
interface IMscomRunningObjectTable : public IMSBase
{
	/************************************************************************/
	/*  快速创建对象，在内部实际上也是用IMsObjectLoader创建的对象
	 */
	STDMETHOD(CreateInstance)(const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv) = 0;

	//如果punk为空，则通过IMsObjectLoader运行时创建
	STDMETHOD(Register)(const CLSID& rpid, IMSBase *punk /*=NULL*/) = 0;
	STDMETHOD(Revoke)(const CLSID& rpid) = 0;
	STDMETHOD_(BOOL ,IsRunning)(const CLSID& rpid) = 0;
	STDMETHOD(GetObject)(const CLSID& rpid, const IID& iid, void **ppunk) = 0;
	STDMETHOD(RevokeAll)() = 0;
};
MS_DEFINE_IID(IMscomRunningObjectTable , "{14B578A4-11BC-48e5-8FCB-4E9974C5EC29}");


/*
	枚举Rot
*/
struct IRunningObjectTableEnum
{
	/*
		功能：ROT 枚举回调
		参数: 
			 clsid MSCOM 对象组件ID
			 pObject Rot 对象
		返回:FALSE 结束枚举
	*/
	virtual BOOL OnRunningObjectTableEnum(REFCLSID clsid , IMSBase* pObject) = 0;
};

interface IMscomRunningObjectTableEx : public IMSBase
{
	/*
		功能:枚举ROT
		参数:pEnum 枚举回调接口

	*/
	STDMETHOD(EnumRunningObjectTable)(IRunningObjectTableEnum* pEnum, IID iid = __uuidof(IMSBase)) = 0;
	STDMETHOD_(REFCLSID, GetMainPlugin)() = 0;
};

MS_DEFINE_IID(IMscomRunningObjectTableEx , "{CBC62BD1-000C-4329-80A9-1E7DEF71FA30}");

/*接口实现
public IMscomRunningObjectTable,
public IMscomRunningObjectTableEx,
public IMsCfgBase ,
*/
// {CEC19C43-D1D3-437c-B467-C77ED23BA5BC}
MS_DEFINE_GUID(CLSID_RunningObjectTable,
			0xcec19c43, 0xd1d3, 0x437c, 0xb4, 0x67, 0xc7, 0x7e, 0xd2, 0x3b, 0xa5, 0xbc);


//////////////////////////////////////////////////////////////////////////
//主插件需要实现的
struct IExit : public IMSBase
{
	//退出整个程序的运行，一般来说调用这个都是通过运行的另外一个线程来调用的
	STDMETHOD(NotifyExit)(bool* bExit = NULL) = 0;
};
MS_DEFINE_IID(IExit, "{AFFA4FB2-EFF6-4371-AADD-62C0B0766178}");


//////////////////////////////////////////////////////////////////////////
//env


struct IMsEnvMgr : public IMSBase
{
	STDMETHOD(Init)(
		LPCWSTR lpszComPath, 
		LPCWSTR lpszComXml,
		LPCWSTR lpszRotPath,
		LPCWSTR lpszRotXml,

		//主要考虑用在activex中，检索配置时用的都是父模块的相对路径及名称
		HINSTANCE hInstance = NULL,
		LPCWSTR lpszWorkPath = NULL //工作目录
		) = 0;

	STDMETHOD(Uninit)() = 0;

	STDMETHOD(Start)() = 0;
	STDMETHOD(Stop)() = 0;

	//当从插件建立一个独立的ROT时使用
	STDMETHOD(GetRunningObjectTable)(IMscomRunningObjectTable** pRot) = 0;
};
MS_DEFINE_IID(IMsEnvMgr , "{B253DFEA-E8E7-456A-9858-3A38B508C5BB}");


struct IMsEnv : public IMSBase
{
	
	//获取使用 envmgr 的句柄，可能是进程，也可能是DLL
	STDMETHOD_(HINSTANCE,GetInstance)() = 0;
	

	//获取服务句柄,在非服务模式下返回NULL
	STDMETHOD_(SERVICE_STATUS_HANDLE, GetServiceStatusHandle)() = 0;

	//获取当前工作目录
	STDMETHOD_(LPCWSTR, GetWorkPath)() = 0;

	/*
		获取环境变量：
		对应 **.xml 中的<envinfo> 节点
		IPropertyStr
	*/
	STDMETHOD(GetEnvParam)(IMSBase** ppIPropertyStr) = 0;
};



//MS_DEFINE_IID(IExtGrp ,"{EE2AA817-B521-43ed-AC17-4D7E0C9EDDB8}");

/*实现接口
IMsEnv
IExit 退出程序，在其内部调用的是主插件IExit
//IExtGrp
imspluginrun
IServiceCtrlConnectPoint 实现服务控制连接点
*/
MS_DEFINE_IID(IMsEnv , "{453D4064-779F-4771-9641-1D998138E2A4}");


/*如果外部的壳是服务的话，需要用到*/
struct IEvnSrv : public IMSBase
{
	//必须要在Init之前调用
	virtual VOID SetServiceStatusHandle( SERVICE_STATUS_HANDLE hSrv) = 0;

	virtual DWORD SrvHandle(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, LPVOID lpContext) = 0;
};
MS_DEFINE_IID(IEvnSrv , "{EF7C1999-96CF-4C08-AB92-B1D479C71C40}");

//响应服务控制事件连接点
//该连接点在服务模式下被触发

interface IServiceCtrlConnectPoint : public IMSBase
{
	//参见MSDN:RegisterServiceCtrlHandlerEx || HandlerEx
	STDMETHOD_(DWORD, OnServiceCtrl)(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) = 0;
};
MS_DEFINE_IID(IServiceCtrlConnectPoint, "{88E3C375-B6EA-4dd8-8C4E-FE00689AD25A}");


// {A3637D66-7A9F-4a7c-B7CA-FEA028137586}
MS_DEFINE_GUID(CLSID_MsEnv,
			   0xa3637d66, 0x7a9f, 0x4a7c, 0xb7, 0xca, 0xfe, 0xa0, 0x28, 0x13, 0x75, 0x86);


}
}