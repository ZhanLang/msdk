// call_mgr.h

#pragma once
#include "rscenter.h"

namespace msdk {

//进程内通讯中心：实现进程内的通讯
struct ICommInProcess : public IMSBase
{
public:
	virtual HRESULT SendMessage(ISendMessage* pSendMessage) = 0;
};

MS_DEFINE_IID(ICommInProcess, "{D6192496-29EC-4584-BDB7-DC1A45C67129}");
// {54CB5506-9256-4377-B862-81219ADED6EC}
MS_DEFINE_GUID(CLSID_CCommInProcess, 
			0x54cb5506, 0x9256, 0x4377, 0xb8, 0x62, 0x81, 0x21, 0x9a, 0xde, 0xd6, 0xec);

class CCommInProcess : public ICommInProcess
	, public CUnknownImp
{
public:
	UNKNOWN_IMP1(ICommInProcess);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	virtual HRESULT SendMessage(ISendMessage* pSendMessage);
};


//进程间通讯中心：实现进程间的通讯
struct ICommInHost : public IMSBase
{
public:
	virtual HRESULT SendMessage(ISendMessage* pSendMessage) = 0;
	// 判断RPC是否有效。S_FALSE表示无效
	virtual HRESULT CheckRPCState(OBJID objid, BOOL bTryConnected = FALSE) = 0;
	//
	virtual HRESULT SetCCenterNamesManager(ICCenterNamesManager *pCCenterNamesManager) = 0;
};

MS_DEFINE_IID(ICommInHost, "{160AF117-CFB5-42e0-809F-C9BC27565266}");
// {AA0BF2C1-FF3D-4f0e-8C80-E404204524D1}
MS_DEFINE_GUID(CLSID_CCommInHost, 
			0xaa0bf2c1, 0xff3d, 0x4f0e, 0x8c, 0x80, 0xe4, 0x4, 0x20, 0x45, 0x24, 0xd1);

class CCommInHost : public ICommInHost
	, public CUnknownImp
{
public:
	UNKNOWN_IMP1(ICommInHost);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	UTIL::com_ptr<ICCenterNamesManager> m_pCCenterNamesManager;
	CAutoCriticalSection m_lockConn;

	typedef struct _st_Conneted_handle
	{
		RPC_BINDING_HANDLE m_Binding;
		unsigned char *m_stringBinding;
		SERVERINFO m_si;
	}CONNECTED_HANDLE, * LPCONNECTED_HANDLE;	
	typedef map<OBJID, LPCONNECTED_HANDLE> CMapClientConnect;

	CMapClientConnect m_cc;
public:
	CCommInHost();
	~CCommInHost();
	HRESULT SetCCenterNamesManager(ICCenterNamesManager *pCCenterNamesManager);

	HRESULT RPC_SendMessage(int sourcedefid, int sourcemaskid, int sourceobjid, IServer *pServer, ISendMessage* pSendMessage);

public:
	virtual HRESULT SendMessage(ISendMessage* pSendMessage);

	// 判断RPC是否有效。
	virtual HRESULT	CheckRPCState(OBJID objid, BOOL bTryConnected = FALSE);
};


//通讯中心对象：负责数据的真正的通讯的实现
struct ICCenterObject : public IMSBase
{
public:
	virtual HRESULT Start(LPCTSTR szEndpoint, ICCenterNamesManager *pCCenterNamesManager, DWORD dwType) = 0;
	virtual HRESULT Stop() = 0;
	virtual HRESULT SendMessage(ISendMessage* pSendMessage) = 0;	
};

MS_DEFINE_IID(ICCenterObject, "{1A4CB627-2866-4ce9-88C6-E8D53B7C7413}");
// {A8D3A924-3863-430f-B67C-1412247F4C5D}
MS_DEFINE_GUID(CLSID_CCenterObject, 
			0xa8d3a924, 0x3863, 0x430f, 0xb6, 0x7c, 0x14, 0x12, 0x24, 0x7f, 0x4c, 0x5d);

//////////////////////////////////////////////////////////////////////////



#include <util/utility_ex.h>
typedef UTIL::sentry<HANDLE, msdk::UTILEX::handle_sentry> CRSHandle;

//通讯中心对象：负责数据的真正的通讯的实现
class CCCenterObject : public ICCenterObject
	, public CUnknownImp
{	
	static CSingleAutoCriticalSection m_lockref;
	UTIL::com_ptr<ICCenterNamesManager> m_pCCenterNamesManager;
	UTIL::com_ptr<ICommInHost> m_pCommInHost;
	UTIL::com_ptr<ICommInProcess> m_pCommInProcess;	

	static TCHAR m_szEndpoint[SERVER_RPC_NAME_MAX];
	//static HANDLE s_hThread;	
	static RPC_STATUS m_status;	
	static CRSHandle m_hNotify;
	static CAutoCriticalSection m_lockStart;

private:
	BOOL CheckListenOk(LPCTSTR szEndpoint, DWORD dwTimeout = 200);
	inline BOOL CheckStartOk(DWORD dwTimeout = 200);
protected:

	void Run();
public:	
	UNKNOWN_IMP1(ICCenterObject);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}
	//UNKNOWN_IMP_SPEC_EX( QIENTRY(ICCenterObject) )//全局变量，不会释放的

	CCCenterObject();

	virtual ~CCCenterObject();

	virtual HRESULT Start(LPCTSTR szEndpoint, ICCenterNamesManager *pCCenterNamesManager, DWORD dwType);
	virtual HRESULT Stop();

	virtual HRESULT SendMessage(ISendMessage* pSendMessage);
};

} //namespace msdk {
