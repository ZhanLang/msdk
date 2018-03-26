#ifndef _HTTPDOWNLOADTASK_H_
#define _HTTPDOWNLOADTASK_H_
#include "downloadimp.h"
#include "use_inetfile.h"
class CHttpDwonloadTask
	:public CDownloadImp
{
public:
	CHttpDwonloadTask();
	virtual ~CHttpDwonloadTask();
	virtual HRESULT ReadBuffer(LPBYTE lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	virtual UINT64 GetRemoteFileSize();

	virtual HRESULT OnAfterOpen();
	virtual HRESULT OnBeforeClose();

private:
	HRESULT InitProxy();
private:
	fsHttpFile					m_internetFile;
	fsHttpConnection			m_InternetServerConnection;
	fsInternetSession			m_internetSession;
	UINT64						m_uRemoteFileSize;	//服务器端的文件大小

	//代理相关
	IAT_ProxyInfo				m_proxyInfo;
};






CHttpDwonloadTask::CHttpDwonloadTask()
{
	memset(&m_proxyInfo,0,sizeof(m_proxyInfo));
	m_proxyInfo.accessType = IAT_NO_PROXY;
}

CHttpDwonloadTask::~CHttpDwonloadTask()
{

}

HRESULT CHttpDwonloadTask::ReadBuffer(LPBYTE lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	return m_internetFile.Read(lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead) == IR_SUCCESS ? S_OK : E_FAIL;
}

UINT64 CHttpDwonloadTask::GetRemoteFileSize()
{
	return m_uRemoteFileSize;
}

HRESULT CHttpDwonloadTask::OnAfterOpen()
{
	InitProxy();
	
	

	return S_OK;
}

HRESULT CHttpDwonloadTask::OnBeforeClose()
{
	return S_OK;
}

HRESULT CHttpDwonloadTask::InitProxy()
{
	LPVOID	lpData = NULL;
	DWORD	dwLen = 0;
	HRESULT hRet = GetOption(ProxyInfoOption,&lpData,dwLen);
	if (SUCCEEDED(hRet) && !lpData && dwLen == sizeof(m_proxyInfo))
	{
		memcpy(&m_proxyInfo,lpData , sizeof(m_proxyInfo));
	}

	return S_OK;
}

#endif	//_HTTPDOWNLOADTASK_H_