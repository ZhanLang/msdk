// luahost.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "luahost.h"
#include "xml/ixmltree3.h"
#include "luahost/luahost.h"

STDMETHODIMP CLuaHost_S::Init( void* _init)
{
	DllQuickCreateInstance(CLSID_LuaVM, __uuidof(ILuaVM), m_pLuaVM.m_p, m_pRot);

	RASSERT(m_pLuaVM, E_FAIL);
	RFAILED(m_pLuaVM->OpenVM());



	if(!(_init && wcslen((LPCWSTR)_init)))
	{
		RSLOG(GroupName , MsgLevel_Error,_T("LuaHost init can use param eg:<lua><file>*.lua</file><script></script></lua>"));
		return S_OK;
	}

	UTIL::com_ptr<IXMLTree3> pXml;
	m_pRot->CreateInstance(CLSID_IXMLTreeW, NULL, __uuidof(IXMLTree3), (void**)&pXml.m_p);
	RASSERT(pXml, E_FAIL);

	RASSERT(pXml->LoadBuff((LPCWSTR)_init),E_FAIL);

	HXMLTREEITEM hFile = pXml->LocalItem(pXml->GetRoot(), _T("lua\\file"));
	if (hFile)
	{
		LPCTSTR lpszPath = pXml->GetText(hFile);
		//lpszPath += "\\";
		if (!PathFileExists(lpszPath))
		{
			GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaHost_S::Init::PathFileExists(%s) 尝试使用 workpath"), lpszPath);
			m_strFile = GetCurrentPath();
			m_strFile+="\\";
			m_strFile += lpszPath;
			if (!PathFileExists(m_strFile))
			{
				GrpError(GRP_NAME, MsgLevel_Error, _T("CLuaHost_S::Init::找不到文件(%s)，这回彻底失败了."), m_strFile.GetBuffer());
				return E_FAIL;
			}
		}
		else
		{
			m_strFile = lpszPath;
		}
	}

	HXMLTREEITEM hScript = pXml->LocalItem(pXml->GetRoot(), _T("lua\\script"));
	if (hScript)
	{
		m_strScript = pXml->GetText(hScript);
	}

	if (!hFile && !hScript)
	{
		RSLOG(GroupName, MsgLevel_Error,_T("LuaHost init can use param eg:<lua><file>*.lua</file><script></script></lua>"));
	}
	return S_OK;
}

STDMETHODIMP CLuaHost_S::Uninit()
{
	if (m_pLuaVM)
	{
		m_pLuaVM->ClosetVM();
		m_pLuaVM = INULL;
	}

	SAFE_RELEASE(m_pRot);
	m_UseServiceCtrlConnectPoint.DisConnect();
	return S_OK;
}

STDMETHODIMP CLuaHost_S::Start()
{

	UTIL::com_ptr<IMsEnv> pEnv;
	m_pRot->GetObject(CLSID_MsEnv, __uuidof(IMsEnv), (void**)&pEnv);
	RASSERT( pEnv , E_FAIL);
	//服务模式
	if ( pEnv->GetServiceStatusHandle() )
	{
		RFAILED(m_UseServiceCtrlConnectPoint.Connect(pEnv, UTIL::com_ptr<IServiceCtrlConnectPoint>(this)));
	}



	RASSERT(m_pLuaVM, E_FAIL);
	if (m_strScript.GetLength()){
		HRESULT hRet = m_pLuaVM->DoString(m_strScript);
		if (FAILED(hRet)){
			RSLOG(GroupName, MsgLevel_Error, _T("LuaHost DoString[%s] Failed."), m_strScript.GetBuffer());
			return hRet;
		}
	}else if (m_strFile.GetLength()){
		HRESULT hRet = m_pLuaVM->DoFile(m_strFile);
		if (FAILED(hRet)){
			RSLOG(GroupName,  MsgLevel_Error,_T("LuaHost DoFile[%s] Failed."), m_strFile.GetBuffer());
			return hRet;
		}
	}

	CSubThread::StartThread();
	return S_OK;
}

STDMETHODIMP CLuaHost_S::Stop()
{
	CSubThread::StopThread(TRUE);
	return S_OK;
}

STDMETHODIMP CLuaHost_S::GetLuaVM( ILuaVM** pVM )
{
	if (m_pLuaVM && pVM)
	{
		return m_pLuaVM->QueryInterface(__uuidof(ILuaVM), (void**)pVM);
	}
	return S_OK;
}

STDMETHODIMP_(DWORD) CLuaHost_S::OnServiceCtrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
//	GrpMsg( GroupName, MsgLevel_Msg, L"OnServiceCtrl(%x,%x,%x,%x)", dwControl, dwEventType, lpEventData, lpContext);
	return GET_LUAHOST(m_pLuaVM)->CallLuaFunctionRet<int, 0>("service_ctrl", dwControl, dwEventType, lpEventData, lpContext);
}

HRESULT CLuaHost_S::Run()
{
	RASSERT(m_pLuaVM, 0);
	return GET_LUAHOST(m_pLuaVM)->CallLuaFunctionRet<int, -1>("run");
}
