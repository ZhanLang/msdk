#include "StdAfx.h"
#include "HttpAsyncRequest.h"


CHttpAsyncRequest::CHttpAsyncRequest(void)
{
}


CHttpAsyncRequest::~CHttpAsyncRequest(void)
{
}

STDMETHODIMP CHttpAsyncRequest::HttpAsyncRequest(LPCWSTR lpszUrl, DWORD dwCookie, IHttpAsyncRequestNotify* pNotify)
{
	UTIL::com_ptr<IHttpAsyncRequestNotify> pTest = pNotify;
	RASSERT(lpszUrl && wcslen(lpszUrl) && pTest, E_FAIL);


	//
	{
		AUTOLOCK_CS(m_asyncHttpObject);
		for (CAsyncHttpObject::iterator it = m_asyncHttpObject.begin() ; it != m_asyncHttpObject.end() ; it++)
		{
			if (it->second.dwCookie == dwCookie)
			{
				GrpError(GroupName, MsgLevel_Error, _T("CHttpAsyncRequest::HttpAsyncRequest(%s,%d,%x) 下载对象已存在"), lpszUrl, dwCookie, pNotify);
				return E_FAIL;
			}
		}
	}
	

	UTIL::com_ptr<IHttpSyncRequest>	m_httpSyncRequest;
	CreateInstance(CLSID_HttpSyncRequest, NULL, __uuidof(IHttpSyncRequest), (void**)&m_httpSyncRequest);
	RASSERT(m_httpSyncRequest, E_FAIL);

	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, &CHttpAsyncRequest::DownLoadThreadProc, this, CREATE_SUSPENDED, &dwThreadID);

	{
		
		AUTOLOCK_CS(m_asyncHttpObject);
		_DOWNLOAD_OBJ obj = {hThread,lpszUrl,dwCookie, m_httpSyncRequest, pNotify};
		m_asyncHttpObject[dwThreadID] = obj;
	}
	
	ResumeThread(hThread);

	return S_OK;
}

DWORD WINAPI CHttpAsyncRequest::DownLoadThreadProc(__in LPVOID lpParameter)
{
	CHttpAsyncRequest* pThis = static_cast<CHttpAsyncRequest*>(lpParameter);
	RASSERT(pThis, 0);

	return pThis->DownLoad();	
}

DWORD CHttpAsyncRequest::DownLoad()
{
	DWORD dwThreadId = GetCurrentThreadId();
	//HANDLE hThread = GetCurrentThread();
	_DOWNLOAD_OBJ obj;
	std::wstring strUrl;
	{
		AUTOLOCK_CS(m_asyncHttpObject);
		CAsyncHttpObject::iterator it_asyncHttpObject = m_asyncHttpObject.find(dwThreadId);
		RASSERT(it_asyncHttpObject != m_asyncHttpObject.end(), 0);
		obj = it_asyncHttpObject->second;
	}

	RASSERT(obj.pSyncRequest, -1);
	UTIL::com_ptr<IMsBuffer>	pBuffer;
	HRESULT hRet = obj.pSyncRequest->HttpRequest(obj.strUrl.c_str(), (IMSBase**)&pBuffer.m_p);
	obj.pNotify->OnHttpAsyncRequestNotify(obj.dwCookie, pBuffer, hRet);
	return hRet == S_OK ? 0:-1;
}

STDMETHODIMP CHttpAsyncRequest::Init()
{
	return S_OK;
}

STDMETHODIMP CHttpAsyncRequest::UnInit()
{
	AUTOLOCK_CS(m_asyncHttpObject);
	for (CAsyncHttpObject::iterator it = m_asyncHttpObject.begin() ; it != m_asyncHttpObject.end() ; it++)
	{
		_DOWNLOAD_OBJ obj = it->second;
		if (obj.hThread)
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(obj.hThread, 2000))
			{
				TerminateThread(obj.hThread, -1);
			}
		}
	}
	m_asyncHttpObject.clear();
	return S_OK;
}
