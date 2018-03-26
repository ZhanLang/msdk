
//下载公共基类

#pragma once

#include "combase/stream_i.h"
#include "download/download_i.h"
class CDownloadImp
	: public IDownLoader
	, private CSubThread
	, private CUnknownImp
{
public:
	CDownloadImp();
	virtual ~CDownloadImp();
	
	UNKNOWN_IMP1(IDownLoader);
	
	STDMETHOD(Open)(IDownLoadEventNotify * pEvent, 
		LPCWSTR lpstrUrl,
		LPCWSTR lpstrSavePath,
		DWORD dwStartPos= 0,
		DWORD dwEndPos = -1);

	STDMETHOD(Close)();
	STDMETHOD(DeleteConfigFile)();
	STDMETHOD(Abort)();


	STDMETHOD(Download)();
	STDMETHOD_(UINT64,GetFileSize)();
	STDMETHOD_(UINT64,GetDownloadSize)();	

	STDMETHOD_(DownloadType,GetDownloadType)();
	STDMETHOD_(BOOL,IsAbort)();


	STDMETHOD(SetOption)(DWORD option, LPVOID lpData, DWORD dwLen, BOOL bSave = FALSE);
	STDMETHOD(GetOption)(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen);

	virtual HRESULT Run();
public:
	virtual HRESULT WriteBuffer(LPBYTE lpData, DWORD dwLen, BOOL bPush = FALSE);

	//派生类实现
	virtual HRESULT ReadBuffer(LPBYTE lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	virtual UINT64	GetRemoteFileSize() = 0;	//获取远程文件大小
	virtual HRESULT OnAfterOpen();
	virtual HRESULT OnBeforeClose();
protected:

	UTIL::com_ptr<IMsFileStream> m_pFileStream;
	std::map<DWORD,XBuffer>		 m_OptionMap;
	IDownLoadEventNotify*		 m_pEventNotify;

	CApiString					 m_strUrl;
	CApiString					 m_strSavePath;
	XBuffer						 m_SaveBuffer;
	DWORD						 m_dwStartPos;
	DWORD						 m_dwEndPos;
	
	DWORD						 m_dwDownloadSize; //已下载大小
	BOOL						 m_bAbove;
private:
	DECLARE_AUTOLOCK_CS(objectoption);
};


CDownloadImp::CDownloadImp()
{
	m_pEventNotify = NULL;
	m_dwDownloadSize = 0;
	m_bAbove =FALSE;
}

CDownloadImp::~CDownloadImp()
{
	Close();
}

STDMETHODIMP CDownloadImp::Open(IDownLoadEventNotify* pEvent , 
								LPCTSTR lpstrUrl,
								LPCTSTR lpstrSavePath,
								DWORD dwStartPos/*= 0 */, 
								DWORD dwEndPos /*= -1*/)
{
	RASSERT(pEvent&&lpstrUrl&&lpstrSavePath,E_INVALIDARG);
	RASSERT(!m_pFileStream,E_FAIL);

	m_pEventNotify = pEvent;
	m_strUrl = lpstrUrl;
	m_strSavePath = lpstrSavePath;

	m_dwStartPos = dwStartPos;
	m_dwEndPos = dwEndPos;
	
	mspath::CPath::CreateDirectoryEx(lpstrSavePath);
	DllQuickCreateInstance(CLSID_MsFileStream,re_uuidof(IMsFileStream),m_pFileStream,NULL);
	RASSERT(m_pFileStream,E_FAIL);
	RFAILED(m_pFileStream->Open(m_strSavePath,OPEN_ALWAYS));
	
	/* 这个偏移不能放到这个地方吧
	LARGE_INTEGER largeSeek;
	largeSeek.QuadPart = m_dwStartPos;
	m_pFileStream->Seek(largeSeek,S_SEEK_BEGIN,NULL);
	*/

	return OnAfterOpen();
}

HRESULT CDownloadImp::OnAfterOpen()
{
	return E_NOTIMPL;
}

HRESULT CDownloadImp::OnBeforeClose()
{
	return E_NOTIMPL;
}

STDMETHODIMP CDownloadImp::Close()
{
	OnBeforeClose();
	if (m_pFileStream)
	{
		m_pFileStream->Close();
		m_pFileStream = INULL;
	}

	StopThread(TRUE);

	return S_OK;
}

STDMETHODIMP CDownloadImp::Abort()
{
	m_bAbove = TRUE;
	StopThread(TRUE);
	return S_OK ;
}


STDMETHODIMP_(BOOL) CDownloadImp::IsAbort()
{
	return m_bAbove;
}


HRESULT CDownloadImp::WriteBuffer(LPBYTE lpData, DWORD dwLen,BOOL bPush /*= FALSE*/)
{
	RASSERT(lpData && dwLen , E_INVALIDARG);
	RASSERT(m_pFileStream,E_FAIL);

	RASSERT(m_SaveBuffer.AddTail(lpData,dwLen),E_FAIL);
	if (m_SaveBuffer.GetDataSize() >= Max_TempBufSize || bPush)
	{
		LPBYTE pTemBuf = 0;
		DWORD dwTempBufSize = 0;
		DWORD dwWrite = 0;
		m_SaveBuffer.GetRawBuffer(&pTemBuf, &dwTempBufSize);
		RFAILED(m_pFileStream->Write(pTemBuf, dwTempBufSize, &dwWrite));
		m_SaveBuffer.Clear();
		m_pFileStream->FlushFileBuffers();
	}

	m_dwDownloadSize += dwLen;
	return S_OK;
}

HRESULT CDownloadImp::ReadBuffer(LPBYTE lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	if (lpNumberOfBytesRead)
	{
		*lpNumberOfBytesRead = 0;
	}

	return S_OK;
}

STDMETHODIMP CDownloadImp::SetOption(DWORD option, LPVOID lpData, DWORD dwLen, BOOL bSave /*= FALSE*/)
{
	RASSERT(lpData && dwLen , E_INVALIDARG);
	AUTOLOCK_CS(objectoption);
	XBuffer xBuf(static_cast<unsigned char *>(lpData), dwLen);
	m_OptionMap[option] = xBuf;
	return S_OK;
}

STDMETHODIMP CDownloadImp::GetOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen)
{
	RASSERT(lpData , E_FAIL);
	AUTOLOCK_CS(objectoption);

	std::map<DWORD,XBuffer>::const_iterator it = m_OptionMap.find(option);
	RASSERT(it != m_OptionMap.end() , E_FAIL);
	return  it->second.GetRawBuffer((UCHAR **)(lpData), &dwLen) ? S_OK : E_FAIL;
}

STDMETHODIMP_(UINT64) CDownloadImp::GetDownloadSize()
{
	return m_dwDownloadSize;
}

STDMETHODIMP_(DownloadType) CDownloadImp::GetDownloadType()
{
	return DT_UNKNOWN;
}

STDMETHODIMP CDownloadImp::Download()
{
	return StartThread() ? S_OK : E_FAIL;
}

HRESULT CDownloadImp::Run()
{
	
	BYTE buffer[1024] = {0};
	HRESULT hRet = S_OK;
	for (;m_bAbove == FALSE;)
	{
		DWORD dwRead = 0;
		memset(buffer,0,sizeof(buffer));
		HRESULT hRet = ReadBuffer(buffer,sizeof(buffer),&dwRead);
		RFAILED(hRet);

		if(dwRead == 0) break;

		hRet = WriteBuffer(buffer,dwRead,FALSE);
		RFAILED(hRet);

		m_pEventNotify->OnDownLoadEventNotify(DLEvent_Downloading,(WPARAM)dwRead,(LPARAM)buffer);
	}
	
	
	m_pEventNotify->OnDownLoadEventNotify(m_bAbove ? DLEvent_Abort:(SUCCEEDED(hRet) ? DLEvent_Done : DLEvent_Error), NULL, NULL);
	return S_OK;
}


STDMETHODIMP CDownloadImp::DeleteConfigFile()
{
	return S_OK;
}

STDMETHODIMP_(UINT64) CDownloadImp::GetFileSize()
{
	return 0;
}

