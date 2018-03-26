#include "StdAfx.h"
#include "MscomEnv.h"
#include "ObjectLoader.h"
#include "RunningObjectTable.h"
#include "xml/rapidtree_s.h"
#include "ServiceOpt.h"
#include "MainRun.h"
#include "mscom/prophelpers.h"

#include <AccCtrl.h>
#include <Aclapi.h>
#include <msapi/msapp.h>


#define MESSAGE_ID  WM_USER + 100
#define SINGLE_APP_MSG 1000


CMscomEnv* CMscomEnv::g_mscomEnv = NULL;
static LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2) throw()
{
	while (p1 != NULL && *p1 != NULL)
	{
		LPCTSTR p = p2;
		while (p != NULL && *p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1 = CharNext(p1);
	}
	return NULL;
}


#pragma warning(push)
#pragma warning(disable : 4302)	// 'type cast' : truncation from 'LPSTR' to 'TCHAR'

static int WordCmpI(LPCTSTR psz1, LPCTSTR psz2) throw()
{
	TCHAR c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
	TCHAR c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
	while (c1 != NULL && c1 == c2 && c1 != ' ' && c1 != '\t')
	{
		psz1 = CharNext(psz1);
		psz2 = CharNext(psz2);
		c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
		c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
	}
	if ((c1 == NULL || c1 == ' ' || c1 == '\t') && (c2 == NULL || c2 == ' ' || c2 == '\t'))
		return 0;

	return (c1 < c2) ? -1 : 1;
}
#pragma warning (pop)

CMscomEnv::CMscomEnv(void) : 
	m_bExit(FALSE) ,
	m_runMode(RM_NULL),
	m_hParentInstance(NULL),
	m_hServiceStatus(NULL)
{
	m_envParam.AddRef();
}

CMscomEnv::~CMscomEnv(void)
{

}


HRESULT CMscomEnv::InstallServices()
{
	CServiceOpt sopt;
	
	if (m_SrvInfo.strName.IsEmpty())
	{
		return E_INVALIDARG;
	}
	
	if (m_SrvInfo.strDesc.IsEmpty())
	{
		m_SrvInfo.strDesc = m_SrvInfo.strName;
	}

	sopt.SetServiceName(m_SrvInfo.strName);
	RFAILED(sopt.Init());
	
	TCHAR szFilePath[MAX_PATH] = {0};
	GetModuleFileName(NULL , szFilePath , MAX_PATH);
	CString strCmd;
	strCmd.Format(_T("\"%s\" %s") , szFilePath , m_SrvInfo.strCmd);

	RFAILED(sopt.Install(strCmd , m_SrvInfo.strName ,SERVICE_WIN32_OWN_PROCESS, m_SrvInfo.nStarttype , m_SrvInfo.strDesc));
	return S_OK;
}

HRESULT CMscomEnv::UnstallServices()
{
	CServiceOpt sopt;
	sopt.SetServiceName(m_SrvInfo.strName);
	RFAILED(sopt.Init());

	RFAILED(sopt.UnInstall(FALSE));
	return S_OK;
}

HRESULT CMscomEnv::CheckCommandLine()
{
	LPTSTR lpCmdLine = GetCommandLine();
	TCHAR szTokens[] = _T("-/");
	LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);

	HRESULT hr = S_OK;
	while (lpszToken != NULL)
	{
		if ( WordCmpI(lpszToken, _T("install")) == 0 )
		{
			//安装
			hr = InstallServices();
			if(SUCCEEDED(hr))
				hr = E_INVALIDARG; //故意返回失败的

			break;
		}
		else if (WordCmpI(lpszToken, _T("uninstall"))==0)
		{
			//卸载
			hr = UnstallServices();
			if(SUCCEEDED(hr))
				hr = E_INVALIDARG; //故意返回失败的

			break;
		}
		else if (WordCmpI(lpszToken, _T("app"))==0)
		{
			//app启动
			m_runMode = RM_APPLICATION;

			hr = S_OK;
			break;
		}
		else if (WordCmpI(lpszToken, _T("srv"))==0)
		{
			//服务启动
			m_runMode = RM_SERVICE;

			hr = S_OK;
			break;
		}

		lpszToken = FindOneOf(lpszToken, szTokens);
	}

	return hr;
}

STDMETHODIMP CMscomEnv::Init(LPCTSTR lpszComPath, 
							 LPCTSTR lpszComXml,
							 LPCTSTR lpszRotPath,
							 LPCTSTR lpszRotXml,
							 HINSTANCE hInstance/* = NULL*/,
							 LPCWSTR  lpszWorkPath)
{

	
	if (lpszWorkPath)
	{
		if (!(FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(lpszWorkPath)))
		{
			GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init work path is invalid."));
			return E_FAIL;
		}
		m_strWorkPath = lpszWorkPath;
		SetDllDirectory(m_strWorkPath);
		GrpMsg(GroupName, MsgLevel_Msg, _T("SetDllDirectory::%s"), m_strWorkPath.GetBuffer());
	}
	else{
		msapi::GetModulePath(m_hParentInstance, m_strWorkPath.GetBufferSetLength(MAX_PATH) , MAX_PATH );
		m_strWorkPath.ReleaseBuffer();
	}

	//char* strKey = "maguojun";
//	m_encryptFile.Init(m_strWorkPath, (PBYTE)strKey, strlen(strKey)); 
	if ( !lpszComPath && !lpszComXml)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init (!lpszComPath && !lpszComXml)."));
		return E_INVALIDARG;
	}

	if (!lpszRotPath && !lpszRotXml)
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init (!lpszRotPath && !lpszRotXml)."));
		return E_INVALIDARG;
	}


	if (FAILED(m_ObjectLoader.Init(NULL)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::m_ObjectLoader::Init failed."));
		return E_FAIL;
	}
	
	if (FAILED(m_RunningObjectTable.Init(NULL)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::m_RunningObjectTable::Init failed."));
		return E_FAIL;
	}
	

	USES_GUIDCONVERSION;
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(g_hinstance,szPath,MAX_PATH);
	m_RunningObjectTable.AddObject(GUID2S(CLSID_MsEnv), /*szPath*/_T("CLSID_MsEnv"),  -1 , FALSE , FALSE, _T(""), UTIL::com_ptr<IMSBase>(this));
	m_RunningObjectTable.AddObject(GUID2S(CLSID_ObjectLoader), _T("CLSID_ObjectLoader"),  -1 , FALSE , FALSE, _T(""),UTIL::com_ptr<IMSBase>(&m_ObjectLoader));
	m_ObjectLoader.AddClsid(szPath,GUID2S(CLSID_MainRun),NULL,NULL);

	m_hParentInstance = hInstance;
	g_mscomEnv = this;
	

	//初始化用户ID

	/*
	{
		CPropStrSet proSet(&m_envParam);
		LPCWSTR lpszUserID = GUID2S(CRsUserGuid::GetGuid(USERGUID_MAKE_ESM));
		proSet["userguid"] = lpszUserID;

		//给软件分配一个临时的ID，才用CRC16和crc8组合是为了避免ID重复，导致不能正确扫描
		SHORT h = std_crc8((char*)lpszUserID, wcslen(lpszUserID)* sizeof(TCHAR));
		INT   l = std_crc16((char*)lpszUserID, wcslen(lpszUserID)* sizeof(TCHAR));
		proSet["userid"] = MAKELONG(l, h);
	}
	*/


	CoInitializeEx(NULL , COINIT_APARTMENTTHREADED );

	if (!ReadProcessInfo(lpszRotPath , lpszRotXml))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::ReadProcessInfo failed."));
		return E_FAIL;
	}
	
	if (FAILED(CheckCommandLine()))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::CheckCommandLine failed."));
		return E_FAIL;
	}
	
	
	if (m_runMode == RM_SERVICE)
	{
		return StartServices();
	}

	if ( m_runMode == RM_SYSTEM || m_hServiceStatus)
	{
		AddConnectionPoint(re_uuidof(IServiceCtrlConnectPoint), m_ServiceCtrlConnectPoint);
	}

	if (FAILED(InitObjLoader(lpszComPath, lpszComXml)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::InitObjLoader failed."));
		return E_FAIL;
	}
	
	if (FAILED(InitRot(lpszRotPath , lpszRotXml)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::InitRot failed."));
		return E_FAIL;
	}

	HRESULT hr = m_RunningObjectTable.InitAll();
	if (FAILED(hr))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CMscomEnv::Init::m_RunningObjectTable::InitAll failed."));
		return hr;
	}

	return S_OK;
}

HRESULT CMscomEnv::ParseMsComFile(LPCTSTR lpszFile)
{
	RASSERT(lpszFile && _tcslen(lpszFile), S_OK);
	CRapidTreeS xmlDoc;
	RASSERT(xmlDoc.Load(lpszFile) , E_FAIL);
	int nLen = xmlDoc.GetXML(xmlDoc.GetRoot() , NULL , 0 , 0);
	RASSERT(nLen , E_FAIL);

	CString strXml;
	int nRead = 0;
	xmlDoc.GetXML(xmlDoc.GetRoot(),strXml.GetBufferSetLength(nLen+1) , nLen+1,&nRead);
	strXml.ReleaseBuffer();
	return ParseMsComXml(strXml.GetBuffer());
}

HRESULT CMscomEnv::ParseRunningObjectFile(LPCTSTR lpszFile)
{
	RASSERT(lpszFile && _tcslen(lpszFile), S_OK);
	CRapidTreeS xmlDoc;
	RASSERT(xmlDoc.Load(lpszFile) , E_FAIL);
	int nLen = xmlDoc.GetXML(xmlDoc.GetRoot() , NULL , 0 , 0);
	RASSERT(nLen , E_FAIL);

	CString strXml;
	int nRead = 0;
	xmlDoc.GetXML(xmlDoc.GetRoot(),strXml.GetBufferSetLength(nLen+1) , nLen+1,&nRead);
	strXml.ReleaseBuffer();
	return ParseRunningObjectXml(strXml.GetBuffer());
}


HRESULT init_env_param(CRapidTreeS& xml)
{
	return S_OK;
}


HRESULT CMscomEnv::InitRot(LPCTSTR lpszRotPath,LPCTSTR lpszRotXml)
{
	if (FAILED(ParseRunningObjectFile(lpszRotPath)))
	{
		GrpError(GroupName, MsgLevel_Error,_T("解析ROT文件失败[%s]"),lpszRotPath);
	}
	
	if (FAILED(ParseRunningObjectXml(lpszRotXml)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("解析ROT XML 失败"));
	}
	

	CString strExt;
	GetModuleFileName(m_hParentInstance , strExt.GetBufferSetLength(MAX_PATH ),MAX_PATH);
	strExt.ReleaseBuffer();

	strExt = strExt.Mid(strExt.ReverseFind('\\') + 1);
	strExt = strExt.Left(strExt.Find('.'));

	CString strCurPath = GetWorkPath();
	

	CString strFind = strCurPath;
	strFind.AppendFormat(_T("\\*.%s") , strExt);


	WIN32_FIND_DATA findData = {0};
	HANDLE hFind = FindFirstFile(strFind , &findData);
	while(hFind != INVALID_HANDLE_VALUE )
	{
		CString strPath = strCurPath + _T("\\") + findData.cFileName;
		if (FAILED(ParseRunningObjectFile(strPath)))
		{
			GrpError(GroupName, MsgLevel_Error, _T("解析ROT文件失败[%s].") , strPath.GetBuffer());
		}
		if (!FindNextFile(hFind , &findData))
		{
			FindClose(hFind);
			break;
		}
	}

	if(FAILED(ParseRunningObjectXml(OnMakeOwnerRuningObjConfig())))
	{
		GrpError(GroupName, MsgLevel_Error, _T("解析 OWNER ROT文件失败."));
	}
	return S_OK;
}


HRESULT CMscomEnv::InitObjLoader(LPCTSTR lpszComPath,LPCTSTR lpszComXml)
{
	if (FAILED(ParseMsComFile(lpszComPath)))
	{
		GrpError(GroupName, MsgLevel_Error,_T("解析MSCOM文件失败[%s]"),lpszComPath);
	}
	if (FAILED(ParseMsComFile(lpszComXml)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("解析MSCOM XML 失败"));
	}
	

	CString strCurPath = GetWorkPath();
	
	
	CString strFind = strCurPath +_T("\\*.mscom");
	WIN32_FIND_DATA findData = {0};
	HANDLE hFind = FindFirstFile(strFind , &findData);
	while(hFind != INVALID_HANDLE_VALUE )
	{
		CString strPath = strCurPath + _T("\\") + findData.cFileName;
		if (FAILED(ParseMsComFile(strPath)))
		{
			GrpError(GroupName, MsgLevel_Error,_T("解析MSCOM文件失败[%s]"),strPath.GetBuffer());
		}
		if (!FindNextFile(hFind , &findData))
		{
			FindClose(hFind);
			break;
		}
	}

	if (FAILED(ParseMsComXml(OnMakeOwnerLoaderConfig())))
	{
		GrpError(GroupName, MsgLevel_Error,_T("解析OWNER MSCOM文件失败"));
	}
	
	return S_OK;
}



STDMETHODIMP CMscomEnv::Uninit()
{
//	m_ObjectRun.m_Rot.Revoke(CLSID_MsEnv);
	DelConnectionPoint(re_uuidof(IServiceCtrlConnectPoint), m_ServiceCtrlConnectPoint);
	m_ServiceCtrlConnectPoint.EmptyConnection();
	m_RunningObjectTable.UninitAll();
	m_RunningObjectTable.Uninit();
	m_ObjectLoader.Uninit();

	m_envParam.Clear();
	CoUninitialize();

//	m_encryptFile.UnInit();
	return S_OK;
}

HRESULT CMscomEnv::StartWithApp()
{
	return OnSrvRun();
}

HRESULT CMscomEnv::StartWithSrv()
{
	RASSERT(m_ntSrv.Init(m_SrvInfo.strName) , E_FAIL);
	RASSERT(m_ntSrv.StartService(this) , E_FAIL);
	m_hServiceStatus = m_ntSrv.GetServiceStatusHandle();
	return S_OK;
}
//启动服务
HRESULT CMscomEnv::StartServices()
{
	CServiceOpt srvOpt;

	if (m_SrvInfo.strName.IsEmpty())
	{
		return E_FAIL;
	}

	srvOpt.SetServiceName(m_SrvInfo.strName);
	RFAILED(srvOpt.Init());
	RFAILED(srvOpt.Start(FALSE))
	return S_OK;
}

//需要做启动方式判断
STDMETHODIMP CMscomEnv::Start()
{
	//判断是否已服务方式启动
	
	switch(m_runMode)
	{
	case RM_APPLICATION:
		return StartWithApp();
	case RM_SERVICE:
		return StartServices();
	case RM_SYSTEM:
		return StartWithSrv();
	default:
		return StartWithApp();
	}
	
	return S_OK;
}


STDMETHODIMP CMscomEnv::Stop()
{
	m_RunningObjectTable.Revoke(CLSID_MsEnv);
	m_RunningObjectTable.Revoke(CLSID_ObjectLoader);

	return m_RunningObjectTable.StopAll();
}




STDMETHODIMP_(HINSTANCE) CMscomEnv::GetInstance()
{
	return m_hParentInstance;
}

STDMETHODIMP_(SERVICE_STATUS_HANDLE)CMscomEnv::GetServiceStatusHandle()
{
	return m_hServiceStatus;
	//return m_ntSrv.GetServiceStatusHandle();
}

STDMETHODIMP_(LPCWSTR) CMscomEnv::GetWorkPath()
{
	return m_strWorkPath;
}

STDMETHODIMP CMscomEnv::GetEnvParam(IMSBase** ppIPropertyStr)
{
	return m_envParam.QueryInterface(re_uuidof(IPropertyStr), (void**)ppIPropertyStr);
}

BOOL CMscomEnv::ReadProcessInfo(LPCTSTR lpCfgFile , LPCTSTR lpCfgXml)
{
	if (!lpCfgFile && !lpCfgXml)
		return FALSE;

	CRapidTreeS rapidDoc;
	if (lpCfgFile)
	{
		RASSERT(rapidDoc.Load(lpCfgFile),FALSE);
	}
	else if (lpCfgXml)
	{
		RASSERT(rapidDoc.LoadBuff(lpCfgXml), FALSE);
	}

	HXMLTREEITEM pRoot = rapidDoc.GetRoot();
	RASSERTP(pRoot, FALSE);

	

	HXMLTREEITEM pSrv = rapidDoc.LocalItem(pRoot , _T("mscomcfg\\process\\mssrv") , FALSE);

	if (pSrv)
	{
		m_runMode = RM_SYSTEM;
		m_SrvInfo.strName    = rapidDoc.GetAttribute(pSrv ,_T("name"));
		m_SrvInfo.strDesc    = rapidDoc.GetAttribute(pSrv ,_T("desc"));
		m_SrvInfo.nControl   = rapidDoc.GetAttributeInt(pSrv ,0 ,_T("control"));
		m_SrvInfo.nStarttype = rapidDoc.GetAttributeInt(pSrv , 0,_T("starttype"));
		m_SrvInfo.strCmd     = rapidDoc.GetAttribute(pSrv , _T("cmd"));
	}
	else
		m_runMode = RM_APPLICATION;


	
	return TRUE;
}


HRESULT CMscomEnv::OnSrvRun()
{
	return m_RunningObjectTable.StartAll();
}

STDMETHODIMP CMscomEnv::NotifyExit(bool* bExit/* = NULL*/)
{
	UTIL::com_ptr<IExit> pExit;

	{
		AUTOLOCK_CS(MainPlugin);
		m_bExit = TRUE;	//标明都已经要退出了，主要考虑没启动完就要退出
		CLSID clsid = m_RunningObjectTable.GetMainPlugin();

		RFAILED(m_RunningObjectTable.GetObject(clsid, re_uuidof(IExit), (void**)&pExit));
	}

	//MainPlugin锁不包括下面处理，是防止NotifyExit里面长时间不放，影响外面的锁
	RASSERT(pExit, E_INVALIDARG);
	return pExit->NotifyExit(bExit);

}

DWORD CMscomEnv::SrvHandle(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, LPVOID lpContext)
{
	return OnUserControl(dwControl, dwEventType, lpEventData, lpContext);
}

VOID CMscomEnv::SetServiceStatusHandle(SERVICE_STATUS_HANDLE hSrv)
{
	m_hServiceStatus = hSrv;
}

HRESULT CMscomEnv::OnSrvInit()
{
	m_hServiceStatus = m_ntSrv.GetServiceStatusHandle();
	GrpMsg(GroupName, MsgLevel_Msg, _T("OnSrvInit m_hServiceStatus=%d"), m_hServiceStatus);
	return S_OK;
}

HRESULT CMscomEnv::OnUserControl(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, LPVOID lpContext)
{
	UTIL::com_ptr<IMsEnumConnections> pEnum;
	if(SUCCEEDED(m_ServiceCtrlConnectPoint.EnumConnections((IMsEnumConnections**)&pEnum)) && pEnum)
	{
		while(1)
		{
			MSCONNECTDATA data[1];
			/*一个一个遍历的，S_FALSE不行	*/
			if(S_OK == pEnum->Next(1, data, NULL))
			{
				UTIL::com_ptr<IServiceCtrlConnectPoint> p = data[0].pUnk;
				if(p)
				{
					p->OnServiceCtrl(dwControl, dwEventType, lpEventData, lpContext);
				}
			}
			else
				break;
		}
	}	

	switch(dwControl)		
	{		
	case SERVICE_CONTROL_STOP:		
	case SERVICE_CONTROL_SHUTDOWN:
		NotifyExit();
		return 0;			
	default:		
		break;		
	}
	return NO_ERROR;
}



HRESULT CMscomEnv::ParseMsComXml(LPCTSTR lpszXml)
{
	RASSERT(lpszXml && _tcslen(lpszXml), S_OK);
	
	CRapidTreeS mscomXml;
	RASSERT(mscomXml.LoadBuff(lpszXml),E_FAIL);
	HXMLTREEITEM hMsComCfg = mscomXml.LocalItem(mscomXml.GetRoot(), _T("mscomcfg"), FALSE);
	RASSERT(hMsComCfg, E_FAIL);
	
	for (HXMLTREEITEM hModule = mscomXml.GetChild(hMsComCfg);hModule; hModule = mscomXml.GetNextItem(hModule))
	{
		CString strPath;
		CString strModulePath = mscomXml.GetAttribute(hModule, sizeof(void*) == sizeof(int) ?_T("X86") : _T("X64"),_T(""));
		if(strModulePath.IsEmpty()) continue;

		if (m_strWorkPath.GetLength()){
			strPath.Format(_T("%s\\%s"), m_strWorkPath.GetBuffer(), strModulePath.GetBuffer());
		}else{
			GetFullPathName(strModulePath,MAX_PATH,strPath.GetBufferSetLength(MAX_PATH),NULL);
			strPath.ReleaseBuffer();
		}
		
		for (HXMLTREEITEM hClass = mscomXml.GetChild(hModule);hClass;hClass = mscomXml.GetNextItem(hClass))
		{
			CString strClsid = mscomXml.GetAttribute(hClass,_T("clsid"),_T(""));
			if(strClsid.IsEmpty()) continue;

			CString strProgid = mscomXml.GetAttribute(hClass,_T("progid"),_T(""));
			CString strName = mscomXml.GetAttribute(hClass,_T("name"),_T(""));
			m_ObjectLoader.AddClsid(strPath,strClsid,strProgid,strName);
		}
	}

	return S_OK;
}

CString CMscomEnv::OnMakeOwnerLoaderConfig()
{
	/*
	LPCTSTR lpXml = 
	_T("<?xml version="1.0" encoding="UTF-8"?>")
	_T("<mscomcfg>")
		_T("<moudle guid='{87773F2C-D0FB-4EB8-9E54-7BF23EDA8838}'  path='%s'>")
			_T("<class clsid="{35FD921E-B758-46d8-B0AA-FCD033B0E66D}"  progid="IXMLTreeW"  name="CLSID_IXMLTreeW"/>")
		_T("</moudle>")
	_T("</mscomcfg>");
	*/

	USES_GUIDCONVERSION;

	TCHAR strModulePath[MAX_PATH] = {0};
	GetModuleFileName(g_hinstance , strModulePath , MAX_PATH);
	CString xml;
	xml.Append(_T("<?xml version='1.0' encoding='UTF-8'?>"));
	xml.Append(_T("<mscomcfg>"));
	xml.AppendFormat(_T("<moudle guid='{87773F2C-D0FB-4EB8-9E54-7BF23EDA8838}'  path='%s'>") , strModulePath);

	//envmgr
		xml.AppendFormat(_T("<class clsid='%s'  progid='CLSID_MainRun.1'  name='CLSID_MainRun'/>"), GUID2S(CLSID_MainRun));
	xml.Append(_T("</moudle>"));
	xml.Append(_T("</mscomcfg>"));

	return xml;
}

bool EnumCallBack (void* param, const char* rpid, const PROPVARIANT *pv)
{
	return true;
}

HRESULT CMscomEnv::ParseRunningObjectXml(LPCTSTR lpszXml)
{
	RASSERT(lpszXml && _tcslen(lpszXml), S_OK);
	CRapidTreeS msrotXml;
	RASSERT(msrotXml.LoadBuff(lpszXml), E_FAIL);

	HXMLTREEITEM hMscomCfg = msrotXml.LocalItem(msrotXml.GetRoot(), _T("mscomcfg"),FALSE);
	RASSERT(hMscomCfg, E_FAIL);

	HXMLTREEITEM hMainPlugin = msrotXml.LocalItem(hMscomCfg,_T("mainplugin\\class"), FALSE);
	if (hMainPlugin)
	{
		CString strClass = msrotXml.GetAttribute(hMainPlugin,_T("clsid"),_T(""));

		if (!strClass.IsEmpty())
		{
			USES_GUIDCONVERSION;
			m_RunningObjectTable.SetMainPlugin(S2GUID(strClass,TRUE));
			CString strName = msrotXml.GetAttribute(hMainPlugin,_T("name"),NULL);
			CString strParam;
			HXMLTREEITEM hParam = msrotXml.GetChild(hMainPlugin, _T("param"));
			if (hParam)
			{
				hParam = msrotXml.GetChild(hParam);
				if (hParam)
				{
					int nSize = 0;
					nSize = msrotXml.GetXML(hParam, NULL, 0, NULL);
					if (nSize)
					{
						nSize = msrotXml.GetXML(hParam, strParam.GetBufferSetLength(nSize), nSize, NULL);
						strParam.ReleaseBuffer();
					}
				}
			}
			m_RunningObjectTable.AddObject(strClass,strName,1,1,1,strParam,INULL);
		}
	}

	HXMLTREEITEM hPlugins  = msrotXml.LocalItem(hMscomCfg,_T("plugins"));
	RASSERT(hPlugins, S_OK);

	for (HXMLTREEITEM hClass = msrotXml.GetChild(hPlugins); hClass ; hClass = msrotXml.GetNextItem(hClass))
	{
		CString strClass = msrotXml.GetAttribute(hClass,_T("clsid"),_T(""));
		if(strClass.IsEmpty()) continue;
		
		CString strName = msrotXml.GetAttribute(hClass,_T("name"),NULL);
		INT  nStart = msrotXml.GetAttributeInt(hClass, -1 , _T("start"));
		BOOL bMustInit = msrotXml.GetAttributeInt(hClass, -1 , _T("mustinit"));
		INT  bMustStart = msrotXml.GetAttributeInt(hClass, -1 , _T("muststart"));

		CString strParam;
		HXMLTREEITEM hParam = msrotXml.GetChild(hClass, _T("param"));
		if (hParam)
		{
			hParam = msrotXml.GetChild(hParam);
			if (hParam)
			{
				int nSize = 0;
				nSize = msrotXml.GetXML(hParam, NULL, 0, NULL);
				if (nSize)
				{
					nSize = msrotXml.GetXML(hParam, strParam.GetBufferSetLength(nSize), nSize, NULL);
					strParam.ReleaseBuffer();
				}
			}
		}

		m_RunningObjectTable.AddObject(strClass,strName,nStart,bMustInit,bMustStart,strParam,INULL);
	}
	

	//解析环境变量
	HXMLTREEITEM hEnvInfo = msrotXml.LocalItem(hMscomCfg , _T("envinfo"));
	if (hEnvInfo)
	{
		for(HXMLTREEITEM h= msrotXml.GetChild(hEnvInfo) ; h ; h = msrotXml.GetNextItem(h))
		{
			LPCTSTR lpName = msrotXml.GetName(h);
			CString strText;

			HXMLTREEITEM hChild = msrotXml.GetChild(h);
			if (hChild)
			{
				int nLen = msrotXml.GetXML(hChild , NULL , 0 , NULL);
				msrotXml.GetXML(hChild , strText.GetBufferSetLength(nLen) , nLen , NULL);
				strText.ReleaseBuffer();
			}
			else
			{
				strText = msrotXml.GetText(h);
			}

			if ( lpName)
			{
				USES_CONVERSION;
				CPropStrSet propSet(&m_envParam);
				propSet[W2A(lpName)] = strText.GetBuffer();
			}
		}
	}
	return S_OK;
}

CString CMscomEnv::OnMakeOwnerRuningObjConfig()
{
	/*
	<?xml version="1.0" encoding="utf-8"?>
	<mscomcfg>
		<mscomcfg>
			<class clsid="{53F64940-23FB-4170-9894-C38D17E019C0}" name="CLSID_DlUI"/>
		</plugins>
	</mscomcfg>
	*/
	USES_GUIDCONVERSION;
	CString xml;
	xml.Append(_T("<?xml version='1.0' encoding='utf-8'?>"));
	xml.Append(_T("<mscomcfg><plugins>"));

	//在这里添加

	xml.Append(_T("</plugins></mscomcfg>"));
	return xml;
}

STDMETHODIMP CMscomEnv::GetRunningObjectTable(IMscomRunningObjectTable** pRot)
{
	return m_RunningObjectTable.QueryInterface(re_uuidof(IMscomRunningObjectTable), (void**)pRot);
}
