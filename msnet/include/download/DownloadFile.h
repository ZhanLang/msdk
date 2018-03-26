#ifndef _downloadFile
#define _downloadFile

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Urlmon.h>
#include <wininet.h>
#pragma comment(lib, "urlmon.lib")
#include <algorithm>

#ifndef fstring
#	ifdef UNICODE
#		define fstring std::wstring
#	else
#		define fstring std::string
#	endif//ifdef UNICODE
#endif//#ifndef fstring

struct stuDownloadStatus
{
	enum{wait=0, succeeded, failed, doing, notExist};
};

class IDownStatus
{上
public:
	virtual void OnDownloadStatus(fstring strUrl, int state, int percent) = NULL;
	virtual void OnAllStatus(int percent) = NULL;
	virtual void OnAllFinish(bool succeed) = NULL;
};

class CDownloadFile : public IBindStatusCallback
{
public:
	struct stuDownloadInfo
	{
		fstring strUrl;
		fstring strLocalFileName;
		int nStatue;
		int nReiteration;
	};

	class url_find
	{
	public:
		url_find(const fstring& str):
		  m_str(str)
		  {
		  }

		  bool operator ()(const vector<pair<fstring, stuDownloadInfo> >::value_type &pair) 
		  { 
			  return pair.second.strUrl == m_str; 
		  } 

		  fstring m_str;
	};


	class DownloadList
	{
	private:
		vector<pair<fstring, stuDownloadInfo> > vt;
		int m_currentDown;
		CRITICAL_SECTION m_section;

	public:
		DownloadList()
		{
			m_currentDown = 0;
			InitializeCriticalSection(&m_section);
		}

		~DownloadList()
		{
			DeleteCriticalSection(&m_section); 
		}

		void DownloadNext(CDownloadFile* pDown)
		{
			/*DWORD code = 0;
			if (!InternetGetConnectedState(&code, 0))
				return;*/

			EnterCriticalSection(&m_section);
			if (vt.size() <= 0)
			{
				pDown->OnProgress(0, 0, BINDSTATUS_SESSION_COOKIES_ALLOWED + 1, NULL);
				LeaveCriticalSection(&m_section);
				return;
			}

			if (vt[m_currentDown].second.nStatue == stuDownloadStatus::wait)
			{
					vt[m_currentDown].second.nStatue = stuDownloadStatus::doing;
					if (URLDownloadToFile(NULL, vt[m_currentDown].second.strUrl.c_str(), vt[m_currentDown].second.strLocalFileName.c_str(), 0, pDown) == S_OK)
						vt[m_currentDown].second.nStatue = stuDownloadStatus::succeeded;
					else
					{
						vt[m_currentDown].second.nStatue = stuDownloadStatus::failed;
						vt[m_currentDown].second.nReiteration += 1;
					}
					pDown->OnProgress(0, 0, BINDSTATUS_SESSION_COOKIES_ALLOWED + 1, NULL);
			}
			else if(vt[m_currentDown].second.nStatue == stuDownloadStatus::failed && vt[m_currentDown].second.nReiteration <= 3)
			{
				vt[m_currentDown].second.nReiteration += 1;
				vt[m_currentDown].second.nStatue = stuDownloadStatus::doing;
				if (URLDownloadToFile(NULL, vt[m_currentDown].second.strUrl.c_str(),vt[m_currentDown].second.strLocalFileName.c_str(), 0, pDown) == S_OK)
					vt[m_currentDown].second.nStatue = stuDownloadStatus::succeeded;
				else
				{
					vt[m_currentDown].second.nStatue = stuDownloadStatus::failed;
					vt[m_currentDown].second.nReiteration += 1;
				}

				pDown->OnProgress(0, 0, BINDSTATUS_SESSION_COOKIES_ALLOWED + 1, NULL);
			}

			if (m_currentDown == vt.size() - 1)
				m_currentDown = 0;
			else
				++m_currentDown;

			LeaveCriticalSection(&m_section);
		}

		
		int GetDownloadStatus(fstring url)
		{
			vector<pair<fstring, stuDownloadInfo> >::iterator it = find_if(vt.begin(), vt.end(), url_find(url));
			if (it == vt.end())
			{
				return true;
			}
			else
			{
				return it->second.nStatue;
			}	
		}

		int GetCurrentDownload()
		{
			return m_currentDown;
		}

		stuDownloadInfo& GetCurrentDownloadDate()
		{
			return vt[m_currentDown].second;
		}

		void AddDownload(fstring url, fstring localFileName)
		{
			stuDownloadInfo info = {url, localFileName, stuDownloadStatus::wait, 0};
			pair<fstring, stuDownloadInfo> apair = make_pair(url, info);
			EnterCriticalSection(&m_section);
			vt.push_back(apair);
			LeaveCriticalSection(&m_section);
		}

		UINT GetSize()
		{
			return static_cast<UINT>(vt.size());
		}

		UINT GetSucceededSize()
		{
			UINT count = 0;
			for(UINT i = 0; i < vt.size(); i++)
			{
				if (vt[i].second.nStatue == stuDownloadStatus::succeeded)
				{
					count ++;
				}
			}

			return count;
		}

		stuDownloadInfo& GetDownloadDate(int nIndex)
		{
			return vt[nIndex].second;
		}

		void CleanDate()
		{
			//EnterCriticalSection(&m_section);
			vt.erase(vt.begin(), vt.end());
			m_currentDown = 0;
			//LeaveCriticalSection(&m_section);
		}

		bool IsAllDone()
		{
			EnterCriticalSection(&m_section);
			int donecount = 0;
			for (int i = 0; i < static_cast<int>(vt.size()); i++)
			{
				if ( (GetDownloadDate(i).nStatue == stuDownloadStatus::succeeded) ||(GetDownloadDate(i).nStatue == stuDownloadStatus::failed && GetDownloadDate(i).nReiteration >= 3)) 
					donecount++;	
			}
			if (donecount == vt.size())
			{
				LeaveCriticalSection(&m_section);
				return true;
			}
			else
			{
				LeaveCriticalSection(&m_section);
				return false;
			}

		}

		bool IsEmpty()
		{
			bool ret = 0;
			EnterCriticalSection(&m_section);
			ret = static_cast<int>(vt.size()) <= 0 ? true : false; 
			LeaveCriticalSection(&m_section);
			return ret;
		}

		bool IsAllSucceed()
		{
			EnterCriticalSection(&m_section);
			for (int i = 0; i < static_cast<int>(vt.size()); i++)
			{
				if (GetDownloadDate(i).nStatue != stuDownloadStatus::succeeded)
				{
					LeaveCriticalSection(&m_section);
					return false;
				}
			}
			LeaveCriticalSection(&m_section);
			return true;
		}
	};

	CDownloadFile()
	{	
		m_hKillThreadEvent   =   ::CreateEvent(NULL,   TRUE,   FALSE,   NULL); 
		m_hThreadKilledEvent   =   ::CreateEvent(NULL,   TRUE,   FALSE,   NULL); 
	}

	~CDownloadFile()
	{
		Stop();
	}

	void Start(IDownStatus* pCallback = NULL)
	{
		m_pCall = pCallback;
		m_hThread = CreateThread(NULL, 0, FunProc, (LPVOID)this, 0, NULL);
	}

	void Stop()
	{
		::SetEvent(m_hKillThreadEvent);
		::WaitForSingleObject(m_hThreadKilledEvent,   INFINITE);
		SAFE_CLOSEHANDLE(m_hKillThreadEvent);
		SAFE_CLOSEHANDLE(m_hThreadKilledEvent); 
		CleanMission();
		SAFE_CLOSEHANDLE(m_hThread);
	}

	static DWORD WINAPI FunProc( LPVOID lpParameter)
	{
		CDownloadFile * pDown = reinterpret_cast<CDownloadFile *>(lpParameter);
		ATLASSERT(pDown);
		DownloadList* pList = &pDown->m_list;
		while(1)
		{
			DWORD dwEvent = WaitForSingleObject(pDown->m_hKillThreadEvent, 20); 
			if(dwEvent == WAIT_OBJECT_0) break; 
			pList->DownloadNext(pDown);
			::SetEvent(pDown->m_hThreadKilledEvent); 
		}

		return 1;
	}

private:
	DownloadList m_list;
	IDownStatus* m_pCall;
	HANDLE m_hThread;
	CRITICAL_SECTION m_section;
	HANDLE m_hKillThreadEvent;
	HANDLE m_hThreadKilledEvent; 
	

	STDMETHOD(OnStartBinding)(
		DWORD dwReserved,
		IBinding __RPC_FAR *pib)
	{ return E_NOTIMPL; }

	STDMETHOD(GetPriority)(
		LONG __RPC_FAR *pnPriority)
	{ return E_NOTIMPL; }

	STDMETHOD(OnLowResource)(
		DWORD reserved)
	{ return E_NOTIMPL; }

	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText)
	{ 
		if (m_pCall != NULL && (ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA || ulStatusCode == BINDSTATUS_DOWNLOADINGDATA) )
		{
			int percent = (WPARAM)(100.0 * ulProgress / ulProgressMax);
			m_pCall->OnDownloadStatus(m_list.GetCurrentDownloadDate().strUrl, stuDownloadStatus::succeeded, percent);
			float allPercent = 100 * m_list.GetSucceededSize() / m_list.GetSize() + percent * 1 / m_list.GetSize();
			m_pCall->OnAllStatus(static_cast<int>(allPercent + 0.5));			
		}
		if (ulStatusCode == BINDSTATUS_SESSION_COOKIES_ALLOWED + 1) // 自定义
		{
			if (m_list.IsEmpty())
			{
				m_pCall->OnAllFinish(false);
				return S_OK; 
			}

			if (m_pCall != NULL && m_list.IsAllDone())
			{
				if (m_list.IsAllSucceed())
					m_pCall->OnAllFinish(true);
				else
					m_pCall->OnAllFinish(false);
			}
		};
		
		return S_OK; 
	}

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError)
	{ 
		return E_NOTIMPL; 
	}

	STDMETHOD(GetBindInfo)(
		DWORD __RPC_FAR *grfBINDF,
		BINDINFO __RPC_FAR *pbindinfo)
	{ return E_NOTIMPL; }

	STDMETHOD(OnDataAvailable)(
		DWORD grfBSCF,
		DWORD dwSize,
		FORMATETC __RPC_FAR *pformatetc,
		STGMEDIUM __RPC_FAR *pstgmed)
	{ return E_NOTIMPL; }

	STDMETHOD(OnObjectAvailable)(
		REFIID riid,
		IUnknown __RPC_FAR *punk)
	{ return E_NOTIMPL; }

	STDMETHOD_(ULONG,AddRef)()
	{ return 0; }

	STDMETHOD_(ULONG,Release)()
	{ return 0; }

	STDMETHOD(QueryInterface)(
		REFIID riid,
		void __RPC_FAR *__RPC_FAR *ppvObject)
	{ return E_NOTIMPL; }

public:
	void AddDownload(fstring url, fstring localFileName)
	{
		m_list.AddDownload(url, localFileName);
	}

	DownloadList& GetDownloadList()
	{
		return m_list;
	}

	void CleanMission()
	{
		m_list.CleanDate();
	}

	void GetDownloadStatus(fstring url)
	{
		m_list.GetDownloadStatus(url);
	}

};

#endif