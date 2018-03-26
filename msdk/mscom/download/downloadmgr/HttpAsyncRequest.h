#pragma once

#include <map>
#include <vector>
#include "HttpSyncRequest.h"
#include "mscom\srvbase.h"
#include "SyncObject\criticalsection.h"

class CHttpAsyncRequest : 
	public IHttpAsyncRequest
	,public CMsComBase<CHttpAsyncRequest>
{
public:
	UNKNOWN_IMP1_(IHttpAsyncRequest);
	CHttpAsyncRequest(void);
	~CHttpAsyncRequest(void);

	STDMETHOD(Init)();
	STDMETHOD(UnInit)();
	STDMETHOD(HttpAsyncRequest)(LPCWSTR lpszUrl, DWORD dwCookie, IHttpAsyncRequestNotify* pNotify);



	static DWORD WINAPI DownLoadThreadProc(__in  LPVOID lpParameter);


	struct _DOWNLOAD_OBJ
	{
		HANDLE hThread;
		std::wstring strUrl;
		DWORD dwCookie;
		UTIL::com_ptr<IHttpSyncRequest> pSyncRequest;
		UTIL::com_ptr<IHttpAsyncRequestNotify> pNotify;
	};

	
	typedef std::map<DWORD, _DOWNLOAD_OBJ>	CAsyncHttpObject;

protected:
	DWORD DownLoad();

private:

	DECLARE_AUTOLOCK_CS(m_asyncHttpObject);
	CAsyncHttpObject	m_asyncHttpObject;
};

