#include "StdAfx.h"
#include "SoftReportPerson.h"
#include <network/HttpImplement.h>
#include <msapi/msapp.h>
#include <userguid/GetNetAdapterInfo.h>


CSoftReportPerson::CSoftReportPerson(void):m_softScan(NULL)
{
}


CSoftReportPerson::~CSoftReportPerson(void)
{
}

STDMETHODIMP CSoftReportPerson::Init(void*)
{
	
	
	return S_OK;
}

STDMETHODIMP CSoftReportPerson::Uninit()
{
	
	return S_OK;
}

STDMETHODIMP CSoftReportPerson::Start()
{
	if ( !m_softScan.Init(/*'C', NULL*/) )
		return E_FAIL;

	if ( !m_worker.Do([this]{Do();}) )
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CSoftReportPerson::Stop()
{
	m_worker.WaitForExit();
	return S_OK;
}

VOID CSoftReportPerson::Do()
{
	CString strSoftID;
	SOFTID softid = m_softScan.FindSoft(TRUE,SOFTLIB_DEFAULE | SOFTLIB_MERGE); //只检查库中存在的软件即可
	while(INVALID_SOFTID != softid)
	{
		strSoftID.AppendFormat(_T("%d;"), softid);
		softid = m_softScan.FindNextSoft();
	}

	//执行上报
	msdk::network::CUrlParamValueMap ParamValueMap;
	ParamValueMap["hdd"]	= msdk::msapi::CApp::GetDiskSerial();
	ParamValueMap["mac"]	= CGetNetAdapterInfo::GetMac();
	ParamValueMap["osver"]	= msapi::GetMicroVersion();
	ParamValueMap["wow64"]	= msapi::IsWindowsX64();
	ParamValueMap["softids"]  = strSoftID.GetBuffer();

	msdk::network::CHttpImplement HttpImpl;
	HttpImpl.GetRequest("update.kyan.com.cn", 80, "SoftReportPerson" , ParamValueMap);
}
