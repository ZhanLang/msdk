#pragma once

namespace msnet{

//////////////////////////////////////////////////////////////////////////
//需要绑定的io对象

struct INetIoObj : public IMSBase
{
	//设置本io完成端口最大线程数 , 默认值为1
	STDMETHOD(SetThreadPoolCount)(DWORD dwTdPool) = 0;
	STDMETHOD(Init)(void) = 0;
	STDMETHOD(Uninit)() = 0;
};
MS_DEFINE_IID(INetIoObj , "{4E71628C-763D-45C2-B15D-DB94A2882E85}");


//////////////////////////////////////////////////////////////////////////
//TCP相关接口

//连接器
struct IOnNetTcpConnecter : public IMSBase
{
	STDMETHOD(OnReadStream)(IMsBuffer* lpBuf, int nErrorCode,size_t byteTransfered,const void * act = 0) = 0;
	STDMETHOD(OnWriteStream)(IMsBuffer* lpBuf, int nErrorCode,size_t byteTransfered,const void * act = 0) = 0;
	STDMETHOD(OnClose)(int nErrorCode) = 0;
	STDMETHOD(OnConnet)(LPCSTR lpAddr,WORD wPort,int nErrorCode) = 0;
	STDMETHOD(OnReport)(LPCSTR lpLocalIP) = 0; 
};
MS_DEFINE_IID(IOnNetTcpConnecter , "{969CECF6-E03F-4256-82C7-C22334907FEC}");

struct INetTcpConnecter : public IMSBase
{
	STDMETHOD(ReadStream)(IMsBuffer* lpBuf , const void * act = 0) = 0;
	STDMETHOD(WriteStream)(IMsBuffer* lpBuf, const void * act = 0) = 0;
	STDMETHOD(Close)() = 0;
	STDMETHOD(Cancel)() = 0;
	STDMETHOD(Connect)(LPCSTR lpAddr,WORD wPort) = 0;
	STDMETHOD(SetCallBack)(IOnNetTcpConnecter* pCallBack) = 0;
};
MS_DEFINE_IID(INetTcpConnecter , "{B2B82D5B-CF56-47A9-A80F-75BAF470524D}");

//监听器
struct IOnNetTcpAccepter : public IMSBase
{
	STDMETHOD(OnClose)(int nErrorCode) = 0;
	STDMETHOD(OnAccept)(INetTcpConnecter* lpAccept,LPCSTR lpAddr,WORD wPort,int nErrorCode) = 0;
	STDMETHOD(OnReport)(LPCSTR lpLocalIP) = 0; 
};
MS_DEFINE_IID(IOnNetTcpAccepter , "{DA455935-AB10-4A40-9224-62A50412ED3D}");

struct INetTcpAccepter : public IMSBase
{
	STDMETHOD(Close)() = 0;
	STDMETHOD(Cancel)() = 0;

	/*lpAddr 用于区分IPV6与IPV4,如果为空，则默认为IPV4
	返回S_OK 成功，否则返回错误码，应该严格判断返回值*/
	STDMETHOD(Accept)(LPCSTR lpAddr,WORD wListenPort) =0;

	STDMETHOD(SetCallBack)(IOnNetTcpAccepter* pCallBack) = 0;
};
MS_DEFINE_IID(INetTcpAccepter , "{A9B789A8-9E6F-4E52-823B-800A4698515A}");

//////////////////////////////////////////////////////////////////////////
//UDP相关
struct IOnNetUdpAdapter : public IMSBase
{
	STDMETHOD(OnReadDgram)(IMsBuffer * lpBuf, LPCSTR lpAddr,WORD wPort,int nErrorCode,const void * act = 0) = 0;
	STDMETHOD(OnWriteDgram)(IMsBuffer* lpBuf, int nErrorCode,const void * act = 0) = 0;
	STDMETHOD(OnBoradCastOk)(int nErrorCode,const void * act = 0) = 0;
	STDMETHOD(OnClose)(int nErrorCode) = 0;
};
MS_DEFINE_IID(IOnNetUdpAdapter , "{88A48C42-7077-45D3-9440-AEE7CB8D8990}");

struct INetUdpAdapter : public IMSBase
{
	STDMETHOD(SetCallBack)(IOnNetUdpAdapter* callBack) = 0;
	STDMETHOD(ReadDgram)(IMsBuffer* lpBuf,const void * act) = 0;
	STDMETHOD(WriteDgram)(IMsBuffer* lpBuf,LPCSTR lpAddr,WORD wPort,const void * act) = 0;
	STDMETHOD(Listen)(LPCSTR lpAddr,WORD wPort) = 0;
	STDMETHOD(Close)() = 0;
	STDMETHOD(Cancel)() = 0;
};
MS_DEFINE_IID(INetUdpAdapter , "{FE6935D2-7E2A-4BBE-982B-0D53D99E4985}");


//////////////////////////////////////////////////////////////////////////
//定时器相关
struct IOnNetTimer : public IMSBase
{
	STDMETHOD(OnNetTimer)(LPVOID lpVoid) = 0;
};
MS_DEFINE_IID(IOnNetTimer , "{B704A0A1-E8D0-4D25-BB60-B982BC18BFFC}");

struct INetTimer : public IMSBase
{
	//单位秒
	STDMETHOD(Schedule)(IOnNetTimer* pOnNetTimer,LPVOID lpVoid,DWORD dwSeconds=1) = 0;
	STDMETHOD(Cancel)() = 0;
};
MS_DEFINE_IID(INetTimer , "{01BC6844-BB6D-4B4A-9364-1196FE0ACD48}");

//////////////////////////////////////////////////////////////////////////
//netbase 代理接口

struct INetBaseProxy : public IMSBase
{
	//创建分离器
	STDMETHOD(CreateNetIoObj)(INetIoObj** pNetIoObj) = 0;

	//创建Tcp连接者
	STDMETHOD(CreateTcpConnecter)(INetIoObj* pNetIoObj , INetTcpConnecter**pConnecter) = 0;

	//创建监听者
	STDMETHOD(CreateTcpAccepter)(INetIoObj* pNetIoObj , INetTcpAccepter**pAccepter) = 0;

	//创建UDP适配器
	STDMETHOD(CreateUdpAdapter)(INetIoObj* pNetIoObj , INetUdpAdapter**pUdpAdapter) = 0;

	//创建定时器
	STDMETHOD(CreateNetTimer)(INetIoObj* pNetIoObj ,INetTimer** pNetTimer) = 0;
};
MS_DEFINE_IID(INetBaseProxy , "{3D71624F-5735-4EB3-932C-C676C5A00570}");

// {CC28CF4A-24AA-486D-9776-8AB47CFDCD97}
MS_DEFINE_GUID(CLSID_NetBaseProxy, 
			0xcc28cf4a, 0x24aa, 0x486d, 0x97, 0x76, 0x8a, 0xb4, 0x7c, 0xfd, 0xcd, 0x97);

};//namespace msnet


