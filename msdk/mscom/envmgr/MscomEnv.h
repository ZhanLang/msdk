#pragma once

#include "NTService.h"
#include "util/globalevent.h"
#include <mscomhelper/connectionpointhelper.h>
#include <syncobject/criticalsection.h>
#include "ObjectRun.h"
#include "mscom/tproperty.h"
//初始化mscom及插件
class CMscomEnv : 
	public IExit,
	public IMsEnv,		  
	public IMsEnvMgr,
	public IOnSrvUserControl,
	public IEvnSrv,
	public CConnectionPointContainerHelper<CMscomEnv>, //实现连接点
	public CUnknownImp
{
	struct ServerInfo
	{
		ServerInfo()
		{
			nControl   = -1;
			nStarttype = -1;
		}

		CString strName;
		CString strDesc;
		INT     nControl;
		INT     nStarttype;
		CString strCmd;
	};

	enum RUN_MODE
	{
		RM_NULL,
		RM_SYSTEM,			//系统正常启动
		RM_SERVICE,			//服务方式
		RM_APPLICATION		//应用程序方式
	};

public:
	UNKNOWN_IMP5_(IMsEnv , IExit, IMsEnvMgr , IMsConnectionPointContainer,IEvnSrv)
public:
	HRESULT init_class(IMSBase* prot, IMSBase* punkOuter)
	{
		return S_OK;
	}
public:
	STDMETHOD(Init)( LPCTSTR lpszComPath, 
					 LPCTSTR lpszComXml,
					 LPCTSTR lpszRotPath,
					 LPCTSTR lpszRotXml,
					 HINSTANCE hInstance = NULL,
					 LPCWSTR  lpszWorkPath = NULL) ;
	STDMETHOD(Uninit)();
	STDMETHOD(GetEnvParam)(IMSBase** ppIPropertyStr);
	STDMETHOD_(HINSTANCE,GetInstance)();
	STDMETHOD_(SERVICE_STATUS_HANDLE, GetServiceStatusHandle)();
	STDMETHOD_(LPCWSTR, GetWorkPath)();

	STDMETHOD(Start)();
	STDMETHOD(Stop)();

	STDMETHOD(GetRunningObjectTable)(IMscomRunningObjectTable** pRot);

	//IOnSrvUserControl
	virtual HRESULT OnSrvRun();
	virtual HRESULT OnSrvInit();
	virtual HRESULT OnUserControl(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, LPVOID lpContext);


	STDMETHOD(NotifyExit)(bool* bExit = NULL);


	//IEvnSrvHandler
	virtual DWORD SrvHandle(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, LPVOID lpContext);
	virtual VOID SetServiceStatusHandle( SERVICE_STATUS_HANDLE hSrv);

private:
	HRESULT ParseMsComFile(LPCTSTR lpszFile);
	HRESULT ParseMsComXml(LPCTSTR lpszXml);
	HRESULT ParseRunningObjectFile(LPCTSTR lpszFile);
	HRESULT ParseRunningObjectXml(LPCTSTR lpszXml);
private:
	BOOL ReadProcessInfo(LPCTSTR lpCfg, LPCTSTR lpCfgXml);
	HRESULT InstallServices();
	HRESULT UnstallServices();
	HRESULT CheckCommandLine();
	HRESULT StartWithApp();
	HRESULT StartWithSrv();
	HRESULT StartServices();
	

	HRESULT InitObjLoader( LPCTSTR lpszComPath,LPCTSTR lpszComXml);
	HRESULT InitRot( LPCTSTR lpszRotPath,LPCTSTR lpszRotXml);

	CString OnMakeOwnerLoaderConfig();
	CString OnMakeOwnerRuningObjConfig();

	HRESULT init_env_param(CRapidTreeS& xml);


private:
	DECLARE_AUTOLOCK_CS(MainPlugin);
	BOOL		m_bExit;
public:

	CMscomEnv(void);
	virtual ~CMscomEnv(void);

	ServerInfo								m_SrvInfo;
	RUN_MODE								m_runMode;
	CNTService								m_ntSrv;

	CConnectionPointHelper m_ServiceCtrlConnectPoint;
	static CMscomEnv* g_mscomEnv;
	HINSTANCE m_hParentInstance;

	CRunningObjectTable m_RunningObjectTable;
	CObjectLoader       m_ObjectLoader;

	CPropertyStrThread	m_envParam;

	CString				m_strWorkPath;

	SERVICE_STATUS_HANDLE m_hServiceStatus;

	//CEncryptFile		m_encryptFile;
};
