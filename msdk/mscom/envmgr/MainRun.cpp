#include "StdAfx.h"
#include "MainRun.h"

CMainRun::CMainRun(void)
{
	m_dwThreadID= 0;
}

CMainRun::~CMainRun(void)
{
}


STDMETHODIMP CMainRun::Init(void*)
{
	/*
	UTIL::com_ptr<IMsEnv> pEnv;
	m_pRot->GetObject(CLSID_MsEnv, re_uuidof(IMsEnv), (void**)&pEnv.m_p);
	RASSERT(pEnv, E_FAIL);


	if (pEnv->GetServiceStatusHandle())
	{
		//RFAILED(m_ServiceCtrlConnect.Connect(pEnv, UTIL::com_ptr<IMSBase>(this)));
	}
	*/
	return S_OK;
}

STDMETHODIMP CMainRun::Uninit()
{
	//m_ServiceCtrlConnect.DisConnect();
	return S_OK;
}

STDMETHODIMP CMainRun::Start()
{
	BOOL bRet;
	MSG msg = {0};
	m_dwThreadID = GetCurrentThreadId();
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
		{
			return S_OK;
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}

	return S_OK;
}

STDMETHODIMP CMainRun::Stop()
{
	return S_OK;
}

STDMETHODIMP CMainRun::NotifyExit(bool* bExit /*= NULL*/)
{
	GrpMsg(GroupName, MsgLevel_Msg, _T("½ø³ÌÍË³ö"));
	PostThreadMessage(m_dwThreadID, WM_QUIT,0,0);
	//PostQuitMessage(IDCANCEL);
	if (bExit)
	{
		bExit = FALSE;
	}

	return S_OK;
}

//STDMETHODIMP_(DWORD) CMainRun::OnServiceCtrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
//{
//	switch(dwControl)
//	{
//	case SERVICE_CONTROL_SHUTDOWN:
//	case SERVICE_CONTROL_STOP:
//		{
//			PostQuitMessage(IDCANCEL);
//			break;
//		}
//	}
//	return NO_ERROR;
//}