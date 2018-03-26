#include "StdAfx.h"
#include "RunningObjectTable.h"
#include <algorithm>
#include "ObjectLoader.h"
#include "MainRun.h"

CRunningObjectTable::CRunningObjectTable(void)
{
	m_mainGuid = CLSID_NULL;
	AddRef();
}

CRunningObjectTable::~CRunningObjectTable(void)
{
	Uninit();
}

STDMETHODIMP CRunningObjectTable::Init(void*)
{
	return S_OK;
}	

STDMETHODIMP CRunningObjectTable::Uninit()
{
	m_pMainPlugin = INULL;
	m_rotInfoVector.clear();
	m_rotInfoMap.clear();
	return S_OK;
}


STDMETHODIMP_(REFCLSID) CRunningObjectTable::GetMainPlugin()
{
	return m_mainGuid;
}

STDMETHODIMP CRunningObjectTable::EnumRunningObjectTable(IRunningObjectTableEnum* pEnum, IID iid )
{
	RASSERT(pEnum, E_FAIL);
	AUTOLOCK_CS(rotmap);



	CRotInfoVector::iterator it = m_rotInfoVector.begin();
	for (; it != m_rotInfoVector.end(); it++)
	{
		CLSID clsid = it->clsid;
		CRotInfoMap::iterator it_map = m_rotInfoMap.find(clsid);

		if (it_map == m_rotInfoMap.end())
		{
			USES_GUIDCONVERSION;
			GrpError(GroupName, MsgLevel_Error,_T("EnumRunningObjectTable[%s][%s]没有初始化"),GUID2S(clsid),it->strName.GetBuffer());
		}

		
		if (iid == __uuidof(IMSBase))
		{
			if (!pEnum->OnRunningObjectTableEnum(clsid,it_map->second.pObj))
			{
				break;
			}
		}
		else if(it_map->second.pObj)
		{
			IMSBase* pObj = it_map->second.pObj.m_p;
			UTIL::com_ptr<IMSBase> pTemp;

			if (SUCCEEDED(pObj->QueryInterface(iid, (void**)&pTemp.m_p)))
			{
				if (!pEnum->OnRunningObjectTableEnum(clsid,pTemp))
				{
					break;
				}
			}
		}
	}

	return S_OK;
}


STDMETHODIMP CRunningObjectTable::CreateInstance(const CLSID& rclsid, IMSBase *punkOuter, const IID& riid, void **ppv)
{
	UTIL::com_ptr<IMsObjectLoader> pLd;
	RFAILED(this->GetObject(CLSID_ObjectLoader , re_uuidof(IMsObjectLoader) , (void**)&pLd));
	RASSERT(pLd , E_FAIL);

	return pLd->CreateInstance(UTIL::com_ptr<IMscomRunningObjectTable>(this) , rclsid , punkOuter , riid , ppv);
}

STDMETHODIMP CRunningObjectTable::Register(const CLSID& rpid, IMSBase *punk)
{
	{
		AUTOLOCK_CS(rotmap);
		CRotInfoMap::iterator it = m_rotInfoMap.find(rpid);
		if (it != m_rotInfoMap.end())
		{
			USES_GUIDCONVERSION;
			GrpMsg(GroupName, MsgLevel_Msg, _T("ROT[%s]对象已注册"),GUID2S(rpid));
			return S_OK;
		}
	}
	
	{
		USES_GUIDCONVERSION;
		AddObject(GUID2S(rpid),NULL,-1,FALSE,FALSE,_T(""), punk);
	}
	
	return S_OK;
}


STDMETHODIMP CRunningObjectTable::Revoke(const CLSID& rpid)
{
	RASSERT(!IsExclude(rpid),S_OK);

	Stop(rpid);
	Uninit(rpid);

	m_rotInfoMap.erase(rpid);
	CRotInfoVector::iterator _it_sort = m_rotInfoVector.begin();
	for (; _it_sort != m_rotInfoVector.end() ; )
	{
		if (_it_sort->clsid == rpid)
		{
			_it_sort = m_rotInfoVector.erase(_it_sort);
		}
		else _it_sort ++;
	}
	
	return S_OK;
}

STDMETHODIMP_(BOOL) CRunningObjectTable::IsRunning(const CLSID& rpid)
{
 	//没有被注册
	CRotInfoMap::iterator it = m_rotInfoMap.end();
	RASSERT(it != m_rotInfoMap.end(), FALSE);

	return it->second.pObj ? TRUE : FALSE;
}


STDMETHODIMP CRunningObjectTable::GetObject(const CLSID& rpid, const IID& iid, void **ppunk)
{
	RASSERTP(ppunk , E_INVALIDARG);

	if (rpid == m_mainGuid)
	{
		if (!m_pMainPlugin)
		{
			CreateInstance(rpid,INULL,re_uuidof(IMSBase),(void**)&m_pMainPlugin);
			RASSERTP(m_pMainPlugin, E_FAIL);
			m_rotInfoMap[rpid].pObj = m_pMainPlugin;
		}
		return m_pMainPlugin->QueryInterface(iid,ppunk);
	}

	UTIL::com_ptr<IMSBase> pObj;
	{
		CRotInfoMap::iterator it = m_rotInfoMap.find(rpid);
		if ( it == m_rotInfoMap.end() )
		{
			USES_GUIDCONVERSION;
			GrpMsg(GroupName, MsgLevel_Msg, _T("未找到组件:%s"), GUID2S(rpid));
			return E_FAIL;
		}
		
		pObj = it->second.pObj;
	}
	
	if (!pObj)
	{
		HRESULT hr = CreateInstance(rpid,NULL,re_uuidof(IMSBase),(void**)&pObj);
		if ( !pObj )
		{
			USES_GUIDCONVERSION;
			GrpMsg(GroupName, MsgLevel_Msg, _T("CreateInstance:%s"), (LPCWSTR)GUID2S(rpid));
			return hr;
		}
		
		
		m_rotInfoMap[rpid].pObj = pObj;
		
	}
	
	return pObj->QueryInterface(iid,ppunk);
}

STDMETHODIMP CRunningObjectTable::RevokeAll()
{
	AUTOLOCK_CS(rotmap);
	CRotInfoMap::iterator it = m_rotInfoMap.begin();
	for (; it != m_rotInfoMap.end() ;it++)
	{
		Revoke(it->first);
	}
	
	m_rotInfoMap.clear();
	return S_OK;
}

HRESULT CRunningObjectTable::SetMainPlugin(REFCLSID clsid)
{
	m_mainGuid = clsid;
	return S_OK;
}

bool sort_rotinfo(const RotInfo& l , const RotInfo& r)
{
	return l.nStart < r.nStart;
}

HRESULT CRunningObjectTable::AddObject(LPCTSTR lpszClsid,LPCTSTR lpszName, INT nStart, BOOL bMustinit, BOOL bMuststart, LPCTSTR lpszParam, IMSBase* pObj)
{
	RASSERT(lpszClsid&&lpszName , E_INVALIDARG);
	AUTOLOCK_CS(rotmap);

	CString strClsid = lpszClsid;
	strClsid.Replace(_T("\""),_T(""));
	CLSID clsid = S2GUID(strClsid.GetBuffer(),TRUE);

	RASSERT(m_rotInfoMap.find(clsid) == m_rotInfoMap.end(),S_OK);
	RotInfo rotInfo = {clsid,lpszName ? lpszName : _T(""),bMustinit,bMuststart,nStart,pObj,lpszParam ? lpszParam : _T("")};
	m_rotInfoMap.insert(CRotInfoMap::value_type(clsid,rotInfo));
	m_rotInfoVector.insert(m_rotInfoVector.end(),rotInfo);
	std::sort(m_rotInfoVector.begin(),m_rotInfoVector.end(),sort_rotinfo);
	
	return S_OK;
}

HRESULT CRunningObjectTable::Init(const CLSID& clsid)
{

	HRESULT hr = S_OK;
	do 
	{

		//在没有指定主插件的情况下
		if (clsid == CLSID_MainRun)
		{
			USES_GUIDCONVERSION;
			AddObject(GUID2S(CLSID_MainRun), _T("CLSID_MainRun"), TRUE, TRUE, TRUE, NULL, INULL);
		}

		CRotInfoMap::iterator itRotInfo = m_rotInfoMap.find(clsid);
		if (itRotInfo == m_rotInfoMap.end())
		{
			hr = E_FAIL;
			break;
		}
	

		UTIL::com_ptr<IMSBase> pBase;
		if (FAILED(GetObject(clsid,re_uuidof(IMSBase),(void**)&pBase)) && !pBase)
		{

			hr = E_FAIL;
			break;
		}
		
		UTIL::com_ptr<IMsPlugin> pPlugin = pBase;
		if (!pPlugin){
			break;
		}

		
		{
			USES_GUIDCONVERSION;
			//CFuncTime funcTime(_T("初始化组件："), GUID2S(clsid));
			GrpMsg(GroupName, MsgLevel_Msg,_T("初始化ROT插件[%s,%s]."),GUID2S(clsid),itRotInfo->second.strName.GetBuffer());
			hr = pPlugin->Init(itRotInfo->second.strParam.GetBuffer());
		}
		

		if (FAILED(hr))
		{
			BOOL bMustInit = (itRotInfo != m_rotInfoMap.end()) ? itRotInfo->second.bMustInit : TRUE;
			if (!bMustInit)
			{
				
				USES_GUIDCONVERSION;
				if (itRotInfo !=m_rotInfoMap.end())
				{
					GrpError(GroupName, MsgLevel_Error,_T("ROT 插件[%s,%s]初始化失败."),GUID2S(clsid),itRotInfo->second.strName.GetBuffer());
				}

				hr = S_OK;
				break;
			}
		}

	} while (FALSE);
	

	

	if (FAILED(hr))
	{
		CRotInfoMap::iterator it = m_rotInfoMap.find(clsid);
		USES_GUIDCONVERSION;
		//排除 CLSID_SignalApp
		if ( it !=m_rotInfoMap.end() )
		{
			GrpError(GroupName, MsgLevel_Error,_T("ROT 插件[%s,%s]初始化失败."),GUID2S(clsid),it->second.strName.GetBuffer());
		}
	}

	return hr;
}

HRESULT CRunningObjectTable::Uninit(const CLSID& clsid)
{
	AUTOLOCK_CS(rotmap);
	
	UTIL::com_ptr<IMsPlugin> p;
	GetObject(clsid,re_uuidof(IMsPlugin),(void**)&p);
	RASSERT(p, S_FALSE);

	HRESULT hr = E_FAIL;
	CRotInfoMap::iterator it = m_rotInfoMap.find(clsid);
	{
		USES_GUIDCONVERSION;
		//CFuncTime funcTime(_T("反初始化组件："), GUID2S(clsid));
		GrpMsg(GroupName, MsgLevel_Msg,_T("反初始化ROT插件[%s,%s]."),GUID2S(clsid),it->second.strName.GetBuffer());
		hr = p->Uninit();
	}
	
	if (FAILED(hr))
	{
		CRotInfoMap::iterator it = m_rotInfoMap.find(clsid);
		USES_GUIDCONVERSION;
		GrpError(GroupName, MsgLevel_Error,_T("ROT 插件[%s,%s]反初始化失败."),GUID2S(clsid),it->second.strName.GetBuffer());
	}
	
	return S_OK;
}

HRESULT CRunningObjectTable::InitAll()
{
	AUTOLOCK_CS(rotmap);

	if (m_mainGuid != GUID_NULL)
	{
		RFAILED(Init(m_mainGuid));
	}

	
	CRotInfoVector::iterator it = m_rotInfoVector.begin();
	for (; it != m_rotInfoVector.end() ; it++)
	{
		CLSID clsid = it->clsid;
		if (IsExclude(clsid)) continue;
		RFAILED(Init(clsid));
	}

	return S_OK;
}

HRESULT CRunningObjectTable::UninitAll()
{
	AUTOLOCK_CS(rotmap);
	CRotInfoVector::reverse_iterator it = m_rotInfoVector.rbegin();
	for ( ; it != m_rotInfoVector.rend(); it++)
	{
		CLSID clsid = it->clsid;
		if (IsExclude(clsid)) continue;
		Uninit(clsid);
	}
	if (m_mainGuid != CLSID_NULL)
	{
		Uninit(m_mainGuid);
	}
	
	return S_OK;
}

HRESULT CRunningObjectTable::Start(const CLSID& clsid)
{
	UTIL::com_ptr<IMsPluginRun> p;
	GetObject(clsid,re_uuidof(IMsPluginRun),(void**)&p);
	RASSERT(p, S_OK);
	HRESULT hr = E_FAIL;
	CRotInfoMap::iterator it = m_rotInfoMap.find(clsid);
	{
		USES_GUIDCONVERSION;
		//CFuncTime funcTime(_T("启动组件："), GUID2S(clsid));
		GrpMsg(GroupName, MsgLevel_Msg,_T("启动ROT插件[%s,%s]."),GUID2S(clsid),it->second.strName.GetBuffer());
		hr = p->Start();
	}
	
	if (FAILED(hr))
	{
		
		USES_GUIDCONVERSION;
		GrpError(GroupName, MsgLevel_Error,_T("ROT 插件[%s,%s]启动失败."),GUID2S(clsid),it->second.strName.GetBuffer());
		
		BOOL bMustInit = (it != m_rotInfoMap.end()) ? it->second.bMustStart : TRUE;
		if (bMustInit)
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CRunningObjectTable::Stop(const CLSID& clsid)
{
	UTIL::com_ptr<IMsPluginRun> p;
	GetObject(clsid,re_uuidof(IMsPluginRun),(void**)&p);
	RASSERT(p, S_OK);
	HRESULT hr = E_FAIL;
	CRotInfoMap::iterator it = m_rotInfoMap.find(clsid);
	{
		USES_GUIDCONVERSION;
		//CFuncTime funcTime(_T("停止组件："), GUID2S(clsid));
		GrpMsg(GroupName, MsgLevel_Msg,_T("停止ROT插件[%s,%s]"),GUID2S(clsid),it->second.strName.GetBuffer());
		hr = p->Stop();
	}
	
	if (FAILED(hr))
	{
		USES_GUIDCONVERSION;
		GrpError(GroupName, MsgLevel_Error,_T("ROT 插件[%s,%s]停止失败."),GUID2S(clsid),it->second.strName.GetBuffer());
	}

	return S_OK;
}

HRESULT CRunningObjectTable::StartAll()
{
	CRotInfoVector::iterator it = m_rotInfoVector.begin();
	for (; it != m_rotInfoVector.end() ; it++)
	{
		CLSID clsid = it->clsid;
		if (IsExclude(clsid)) continue;
		RFAILED(Start(clsid));
	}

	if ( m_mainGuid != CLSID_NULL)
	{
		return Start(m_mainGuid);
	}

	return S_OK;
}

HRESULT CRunningObjectTable::StopAll()
{
	Stop(m_mainGuid);
	CRotInfoVector::reverse_iterator it = m_rotInfoVector.rbegin();
	for ( ; it != m_rotInfoVector.rend(); it++)
	{
		CLSID clsid = it->clsid;
		if (IsExclude(clsid)) continue;
		Stop(clsid);
	}

	return S_OK;
}


struct exclude_clsid
{
	CLSID clsid;
};

exclude_clsid g_exclude_clsid[] = {
	CLSID_ObjectLoader,
	CLSID_RunningObjectTable,
	CLSID_MsEnv,
	CLSID_MainRun,
};

BOOL CRunningObjectTable::IsExclude(const CLSID& clsid)
{
	for (INT nIndex = 0 ; nIndex < _countof(g_exclude_clsid) ; nIndex++)
	{
		if (g_exclude_clsid[nIndex].clsid == clsid)
		{
			return TRUE;
		}
	}
	
	return clsid == m_mainGuid;
}