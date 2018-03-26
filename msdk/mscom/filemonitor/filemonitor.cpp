#include "StdAfx.h"
#include <list>
#include "FileMonitor.h"

#define BUFFER_LENGTH 4096/*((sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH)*2)*/
CFileMonitor::CFileMonitor(void)
{
	//AddRef();
	m_appendMonitor = NULL;
	m_pOnVolumeMonitor = INULL;
	m_pFolder2 = NULL;
	m_pRecycleBin = NULL;
}

CFileMonitor::~CFileMonitor(void)
{
	CloseMonitor();
}

STDMETHODIMP CFileMonitor::OpenMonitor(LPCTSTR* szVolumes , DWORD dwCount, IOnFileMonitor* pOnVolumeMonitor)
{
	if (!pOnVolumeMonitor)
	{
		return E_FAIL;
	}

	m_pOnVolumeMonitor = pOnVolumeMonitor;
	m_appendMonitor = CreateEvent(NULL, FALSE, FALSE,NULL);
	if(!(m_appendMonitor && m_appendMonitor != INVALID_HANDLE_VALUE))
	{
		return E_FAIL;
	}

	
	BOOL bRet = CSubThread::StartThread();
	if (!bRet)
	{
		return E_FAIL;
	}
	
	if (szVolumes && dwCount)
	{
		for (DWORD dwIndex = 0 ; dwIndex < dwCount ; dwIndex++)
		{
			HRESULT hr = AppendMonitor(szVolumes[dwIndex]);
			if (hr != S_OK) 
				GrpMsg(GroupName,MsgLevel_Msg, _T("AppendMonitor:%s Failed"),szVolumes[dwIndex]);
		}
	}
	else
	{
		TCHAR lpDriveString[MAX_PATH] = {0};
		GetLogicalDriveStrings(MAX_PATH,lpDriveString);

		int nCount  = 0;  
		TCHAR * pDrive = lpDriveString;  
		for(int nlen =_tcslen(lpDriveString); nlen == 3 ;nCount++)  
		{    
			if (_tcsicmp(pDrive, _T("A:\\")) != 0)
			{
				AppendMonitor(pDrive);
			}
			
			pDrive +=4;  
			nlen = _tcslen(pDrive); 
		}   
		return nCount;  

	}

	return S_OK;
}

#define ALL_FILE_NOTIFY_MASK \
	FILE_NOTIFY_CHANGE_FILE_NAME|\
	FILE_NOTIFY_CHANGE_DIR_NAME|\
	FILE_NOTIFY_CHANGE_ATTRIBUTES|\
	FILE_NOTIFY_CHANGE_SIZE|\
	FILE_NOTIFY_CHANGE_LAST_WRITE|\
	FILE_NOTIFY_CHANGE_LAST_ACCESS|\
	FILE_NOTIFY_CHANGE_CREATION|\
	FILE_NOTIFY_CHANGE_SECURITY


STDMETHODIMP CFileMonitor::AppendMonitor(LPCTSTR volume)
{
	if (!(volume && _tcslen(volume))) return E_FAIL;
	{
		AUTOLOCK_CS(VolumeMap);
		if (m_VolumeHandleMap.find(volume) != m_VolumeHandleMap.end())
		{
			return S_OK;
		}
	}

	UINT driveType = GetDriveType(volume);
	RASSERT(!(DRIVE_FIXED != driveType && DRIVE_REMOVABLE != driveType) ,E_FAIL);

	DWORD dwMaxComLen = 0,dwFileSysFlag = 0;
	TCHAR szVolumeName[MAX_PATH] = {0};
	TCHAR fileSysBuf[MAX_PATH] = {0};

	RASSERT(GetVolumeInformation(volume,szVolumeName,MAX_PATH,NULL,&dwMaxComLen,&dwFileSysFlag,fileSysBuf,MAX_PATH),E_FAIL);
	 
	HVOLUME hVolume = CreateFile(volume,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		 NULL,
		 OPEN_EXISTING,
		 FILE_FLAG_BACKUP_SEMANTICS | // normal file
		 FILE_FLAG_OVERLAPPED,   // overlapped operation
		 NULL);	
		 	
	if (!(hVolume && hVolume != INVALID_HANDLE_VALUE))
	{
		GrpMsg(GroupName,MsgLevel_Msg, _T(""));
		return E_FAIL;
	}

	HEVENT hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if (!(hEvent && hEvent != INVALID_HANDLE_VALUE))
	{
		CloseHandle(hVolume);
		hVolume = NULL;
		return E_FAIL;
	}
	
	{
		AUTOLOCK_CS(VolumeMap);
		CFileOverlappedPoint volumeOverlapped;
		volumeOverlapped.lpPoint = malloc(BUFFER_LENGTH);
		volumeOverlapped.overlapped.hEvent = hEvent;

		m_VolumeEventPointMap.insert(m_VolumeEventPointMap.end(),CVolumeEventPointMap::value_type(hEvent,volumeOverlapped));
		m_VolumeEventMap.insert(m_VolumeEventMap.end(),CVolumeEventMap::value_type(hEvent,hVolume));
		m_VolumeHandleMap.insert(m_VolumeHandleMap.end(),CVolumeHandleMap::value_type(std::wstring(volume),hEvent));
		
	
		DWORD dwRead = 0;
		ZeroMemory(volumeOverlapped.lpPoint,BUFFER_LENGTH);
		BOOL bRet = ReadDirectoryChangesW(hVolume,volumeOverlapped.lpPoint,
			BUFFER_LENGTH,TRUE,ALL_FILE_NOTIFY_MASK,
			&dwRead,&volumeOverlapped.overlapped,NULL);
		if (!bRet)
		{
			TCHAR strErr[MAX_PATH] = {0};
			GrpMsg(GroupName,MsgLevel_Msg,_T("AppendMonitor:%s->%s"),volume,GetLastErrorText(strErr,MAX_PATH));
		}
	}
	
	SetEvent(m_appendMonitor);
	
	return S_OK;
}



STDMETHODIMP CFileMonitor::RemoveMonitor(LPCTSTR volume)
{
	if (volume == NULL)
	{
		//递归删除所有目录
		std::list<std::wstring> volumeList;
		{
			AUTOLOCK_CS(VolumeMap);
			for (CVolumeHandleMap::iterator it = m_VolumeHandleMap.begin();
				it != m_VolumeHandleMap.end();
				it++)
			{
				volumeList.insert(volumeList.end(),it->first.c_str());
			}
		}
		

		for (std::list<std::wstring>::iterator itList = volumeList.begin();
			itList != volumeList.end();
			itList ++)
		{
			RemoveMonitor((*itList).c_str());
		}
	}
	
	if (volume)
	{
		AUTOLOCK_CS(VolumeMap);
		CVolumeHandleMap::iterator it = m_VolumeHandleMap.find(volume);
		if (it == m_VolumeHandleMap.end())
		{
			return S_FALSE;
		}

		HEVENT hEvent = it->second;
		CVolumeEventMap::iterator itEvent = m_VolumeEventMap.find(hEvent);
		if (itEvent != m_VolumeEventMap.end())
		{
			HVOLUME hVolume = itEvent->second;
			if (hVolume)
			{
				CloseHandle(hVolume);
				hVolume = NULL;
			}
			m_VolumeEventMap.erase(itEvent);

		}

		CVolumeEventPointMap::iterator itPoint = m_VolumeEventPointMap.find(hEvent);
		if (itPoint != m_VolumeEventPointMap.end())
		{
			LPVOID lpPoint = itPoint->second.lpPoint;
			if (lpPoint)
			{
				free(lpPoint);
			}
			m_VolumeEventPointMap.erase(itPoint);
		}


		hEvent = NULL;
		m_VolumeHandleMap.erase(it);

		SetEvent(m_appendMonitor);
		CloseHandle(hEvent);
	}
	
	return S_OK;
}

STDMETHODIMP CFileMonitor::CloseMonitor()
{
	
	CSubThread::StopThread(TRUE);
	RemoveMonitor();
	if(m_pOnVolumeMonitor)
	{
		m_pOnVolumeMonitor = INULL;
	}

	if (m_appendMonitor)
	{
		CloseHandle(m_appendMonitor);
		m_appendMonitor = NULL;
	}

	return S_OK;
}


HRESULT CFileMonitor::Run()
{
	HEVENT waitEvents[MAX_PATH] = {m_hExit,m_appendMonitor};
	int nEventCount = 2;

	while(1)
	{
		//等待三秒时间吧，这个时间应该是差不多够用的
		DWORD dwWait = WaitForMultipleObjects(nEventCount,waitEvents,FALSE,1000);
		if (dwWait == WAIT_OBJECT_0 + 0) //m_hExit
		{
			break;
		}
		else if (dwWait == 0xFFFFFFFF || (dwWait == WAIT_OBJECT_0+1 && (nEventCount - 2) != m_VolumeEventMap.size()))//m_appendMonitor
		{
			AUTOLOCK_CS(VolumeMap);
			//判断是否添加或删除事件
			nEventCount = 2;
			for (CVolumeEventMap::iterator it = m_VolumeEventMap.begin();it!=m_VolumeEventMap.end();it++)
			{
				waitEvents[nEventCount] = it->first;
				nEventCount++;
			}
			
			continue;
		}
		else if (dwWait == WAIT_TIMEOUT)
		{
			m_pOnVolumeMonitor->OnFileActionTimeOut(1000);
			continue;
		}
		else if((WAIT_OBJECT_0 + 1) < dwWait && dwWait< (WAIT_OBJECT_0+nEventCount) )
		{
			HEVENT hEvent = NULL;
			HVOLUME hVolume = NULL;

			if (!((hEvent = waitEvents[dwWait]) != NULL && ((hVolume = GetVolume(hEvent)) != NULL)))
			{
				GrpMsg(GroupName,MsgLevel_Msg,_T("Run:!(hEvent && hVolume)"));
				continue;
			}
			
			AUTOLOCK_CS(VolumeMap);
			LPVOID lpPoint = NULL;
			std::wstring strDir;
			CVolumeEventPointMap::iterator itPoint = m_VolumeEventPointMap.find(hEvent);
			if(!(itPoint != m_VolumeEventPointMap.end() && 
				(lpPoint = itPoint->second.lpPoint) != NULL
				))
			{
				TCHAR strErr[MAX_PATH] = {0};
				GrpMsg(GroupName,MsgLevel_Msg,_T("AppendMonitor:%c->%s"),strDir.c_str(),GetLastErrorText(strErr,MAX_PATH));
				
				continue;
			}
			
			strDir = GetVolumeName(hEvent);
			FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION*)lpPoint;
			for (FILE_NOTIFY_INFORMATION*p = pNotify ;p; p = (FILE_NOTIFY_INFORMATION *)(((LPBYTE)p)+p->NextEntryOffset))
			{
				TCHAR strName[MAX_PATH] = {0};;
				int nFileNameLen = p->FileNameLength / sizeof(TCHAR);
				p->FileName[nFileNameLen] = 0;
			
				_tcscpy_s(strName,MAX_PATH,strDir.c_str());
				_tcsncat_s(strName,MAX_PATH,p->FileName,p->FileNameLength/sizeof(WCHAR));

				if(m_pOnVolumeMonitor)
				{
					
					//Sleep(1000);
					//文件修改
					
					/*
					if (TRUE)
					{
						//_T("$Recycle.Bin")
						TCHAR strlowName[MAX_PATH] = {0};
						if(_tcsstr(_tcslwr(_tcscpy(strlowName, strName)), _T("$recycle.bin")) != NULL)
						{
							TCHAR szFileName[MAX_PATH] = {0};
							if (GetRecycleFileDisplayName(strName, szFileName,MAX_PATH))
							{
								ZM1_GrpDbgOutput(GroupName,_T("-----删除{%s}"), szFileName);
								m_pOnVolumeMonitor->OnFileActionChange(FILE_ACTION_REMOVED,szFileName,GetFileAttributes(strName));
								if (!p->NextEntryOffset)
								{
									break;
								};
								continue;
							}
						}
					}
					*/
					m_pOnVolumeMonitor->OnFileActionChange(p->Action,strName,GetFileAttributes(strName));
				}

				if (!p->NextEntryOffset)
				{
					break;
				}
			}
			
	
			DWORD dwRead = 0;
			ZeroMemory(lpPoint,BUFFER_LENGTH);
			BOOL bRet = ReadDirectoryChangesW(hVolume,lpPoint,
				BUFFER_LENGTH,TRUE,ALL_FILE_NOTIFY_MASK,
				&dwRead,&itPoint->second.overlapped,NULL);

			if (!bRet)
			{
				TCHAR strErr[MAX_PATH] = {0};
				GrpMsg(GroupName,MsgLevel_Msg,_T("AppendMonitor:Run%s->%s"),strDir,GetLastErrorText(strErr,MAX_PATH));
			}
		}
		
	}
	return S_OK;
}




CFileMonitor::HVOLUME CFileMonitor::GetVolume(HEVENT hEvent)
{
	HVOLUME hVolume = NULL;
	{
		AUTOLOCK_CS(VolumeMap);
		CVolumeEventMap::iterator it = m_VolumeEventMap.find(hEvent);
		if (it != m_VolumeEventMap.end())
		{
			hVolume = it->second;
		}
	}
	
	return hVolume;
}

LPCTSTR CFileMonitor::GetVolumeName(HEVENT hEvent)
{
	AUTOLOCK_CS(VolumeMap);
	CVolumeHandleMap::iterator it = m_VolumeHandleMap.begin();
	for (; it != m_VolumeHandleMap.end() ; it++)
	{
		if (it->second == hEvent)
		{
			return it->first.c_str();
		}
	}

	return 0;
}


HRESULT BindToCsidl(int csidl, REFIID riid, void **ppv)
{
	HRESULT hr;
	PIDLIST_ABSOLUTE pidl;
	hr = SHGetSpecialFolderLocation(0, csidl, &pidl);
	if (SUCCEEDED(hr)) {
		IShellFolder *psfDesktop;
		hr = SHGetDesktopFolder(&psfDesktop);
		if (SUCCEEDED(hr)) {
			if (pidl->mkid.cb) {
				hr = psfDesktop->BindToObject(pidl, NULL, riid, ppv);
			} else {
				hr = psfDesktop->QueryInterface(riid, ppv);
			}
			psfDesktop->Release();
		}
		CoTaskMemFree(pidl);
	}
	return hr;
}


BOOL CFileMonitor::GetGetRecycleFileDisplayName2(LPCTSTR lpszRecycleFile, LPTSTR lpszDisplayName, DWORD dwDisplayNameLen)
{
	LPMALLOC		pMalloc = NULL;

	IShellFolder *pDeskFolder = NULL ;
	IShellFolder2 *pParentFolder = NULL;
	LPITEMIDLIST pidlParent = NULL, pidl = NULL;

	std::wstring strFullPath = lpszRecycleFile;
	
	size_t nPos = strFullPath.rfind('\\') + 1;

	std::wstring strPath = strFullPath.substr(0,nPos);
	std::wstring strName = strFullPath.substr(nPos);
	HRESULT hr = S_OK;

	BOOL isFind = FALSE;
	CoInitializeEx(NULL, COINIT_MULTITHREADED );

	do 
	{
		PIDLIST_ABSOLUTE pidl;
		hr = SHGetSpecialFolderLocation(0, CSIDL_BITBUCKET, &pidl);


		RASSERT2(SUCCEEDED(SHGetDesktopFolder(&pDeskFolder)), break);
		
		

		RASSERT2(SUCCEEDED(pDeskFolder->ParseDisplayName(NULL,NULL,(LPWSTR)strPath.c_str(),NULL,&pidlParent,NULL)),break);

		RASSERT2(SUCCEEDED(pDeskFolder->BindToObject(pidlParent,NULL,IID_IShellFolder2,(PVOID*)&pParentFolder)),break);

		pParentFolder->BindToObject(pidl, NULL,IID_PPV_ARGS(&pParentFolder));

		//RASSERT2(SUCCEEDED(pParentFolder->ParseDisplayName(NULL,NULL,(LPWSTR)lpszRecycleFile,NULL,&pidl,NULL)),break);
		//RASSERT2(SUCCEEDED(pDeskFolder->BindToObject(pidl,NULL,IID_IShellFolder2,(PVOID*)&pFile)),break);

		

		STRRET sr;
		hr = pParentFolder->GetDisplayNameOf(pidl, SHGDN_FOREDITING, &sr);

		SHELLDETAILS	sd;
		hr = S_OK;
		int iSubItem = 0;
		while (SUCCEEDED (hr))
		{
			hr = pParentFolder->GetDetailsOf (/*pidlParent*/pidl , iSubItem, &sd);
			if (SUCCEEDED (hr))
			{
				switch (sd.str.uType)
				{
				case STRRET_CSTR:
					//_tcscpy (szTemp, sd.str.cStr);
					break;
				case STRRET_OFFSET:
					break;
				case STRRET_WSTR:
					//WideCharToMultiByte (CP_ACP, 0, sd.str.pOleStr, -1, szTemp, sizeof (szTemp), NULL, NULL);
					//pMalloc->Free (sd.str.pOleStr);
					break;
				}
				//m_List.SetItemText (iItem, iSubItem , szTemp);
				iSubItem ++;
			}
		}


	} while (FALSE);
	

	do 
	{
		//TCHAR			szTemp[MAX_PATH];
		SHGetMalloc(&pMalloc);
		RASSERT2(SUCCEEDED(m_pFolder2->ParseDisplayName(NULL,NULL,(LPWSTR)/*strPath.c_str()*/lpszRecycleFile,NULL,&pidlParent,NULL)),break);

			SHELLDETAILS	sd;
		//HRESULT hr = m_pFolder2->GetDetailsOf (pidlParent , 1, &sd);

		HRESULT hr = S_OK;
		int iSubItem = 0;
		while (SUCCEEDED (hr))
		{
			hr = m_pFolder2->GetDetailsOf (pidlParent , iSubItem, &sd);
			if (SUCCEEDED (hr))
			{
				switch (sd.str.uType)
				{
				case STRRET_CSTR:
					//_tcscpy (szTemp, sd.str.cStr);
					break;
				case STRRET_OFFSET:
					break;
				case STRRET_WSTR:
					//WideCharToMultiByte (CP_ACP, 0, sd.str.pOleStr, -1, szTemp, sizeof (szTemp), NULL, NULL);
					//pMalloc->Free (sd.str.pOleStr);
					break;
				}
				//m_List.SetItemText (iItem, iSubItem , szTemp);
				iSubItem ++;
			}
		}

		STRRET sr;
		hr = m_pFolder2->GetDisplayNameOf(pidlParent, SHGDN_NORMAL, &sr);
		//RASSERT2(SUCCEEDED(m_pFolder2->BindToObject(pidlParent,NULL,IID_IShellFolder,(PVOID*)&pParentFolder)),break);
		RASSERT2(SUCCEEDED(m_pFolder2->ParseDisplayName(NULL,NULL,(LPWSTR)strName.c_str(),NULL,&pidl,NULL)),break);

		//SHELLDETAILS	sd;
		hr = m_pFolder2->GetDetailsOf (pidl , 1, &sd);
		//pParentFolder->GetDisplayNameOf(pidl, SHGDN_NORMAL, &sr);
		IEnumIDList *peidl;
		hr = pParentFolder->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INIT_ON_FIRST_NEXT, &peidl);
		
		if (hr == S_OK) 
		{
			PITEMID_CHILD pidlItem;
			while (peidl->Next(0, &pidlItem, NULL) == S_OK) 
			{
				STRRET sr;
				HRESULT hr = pParentFolder->GetDisplayNameOf(pidlItem, SHGDN_INFOLDER, &sr);
				PTSTR pszCiycleName;
				if (SUCCEEDED(StrRetToStr(&sr, pidlItem, &pszCiycleName)))
				{
					if (_tcsicmp(lpszRecycleFile, pszCiycleName) == 0)
					{

						HRESULT hr = pParentFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &sr);
						if (SUCCEEDED(hr)) 
						{
							PTSTR pszName;
							hr = StrRetToStr(&sr, pidlItem, &pszName);
							if (SUCCEEDED(hr)) 
							{
								_tcscpy_s(lpszDisplayName, MAX_PATH, pszName);
								CoTaskMemFree(pszName);
								CoTaskMemFree(pszCiycleName);
								isFind = TRUE;
								break;
							}
						}
					}

					CoTaskMemFree(pszCiycleName);
				}
			}
		}
		if (peidl)
		{
			peidl->Release();
		}

		//m_ShellMenu.ShowContextMenu(pNMHDR->hwndFrom,pParentFolder,pidl);

	} while (FALSE);

	if (pidl)
	{
		::CoTaskMemFree(pidl);
	}

	if (pParentFolder)
	{
		pParentFolder->Release();
		pParentFolder = NULL;
	}

	if (pidlParent)
	{
		::CoTaskMemFree(pidlParent);
	}

	if (pDeskFolder)
	{
		pDeskFolder->Release();
		pDeskFolder = NULL;
	}

	 CoUninitialize();

	return TRUE;
}

template< class E > 
std::basic_string<E> replace(const std::basic_string<E>& str, E _o, E _n)
{
	std::basic_string<E> strResult = str;
	std::basic_string<E>::size_type i = std::basic_string<E>::npos;

	while( ( i = strResult.find(_o) ) != std::basic_string<E>::npos )
		strResult[i] = _n;

	return strResult;
}

BOOL CFileMonitor::GetRecycleFileDisplayName(LPCTSTR lpszRecycleFile, LPTSTR lpszDisplayName, DWORD dwDisplayNameLen)
{
	
	//return GetGetRecycleFileDisplayName2(lpszRecycleFile, lpszDisplayName, dwDisplayNameLen);
	BOOL isFind = FALSE;
	IShellFolder2 *psfRecycleBin;
	HRESULT hr = BindToCsidl(CSIDL_BITBUCKET, IID_PPV_ARGS(&psfRecycleBin));
	if (SUCCEEDED(hr)) 
	{
		//psfRecycleBin->bi
		IEnumIDList *peidl;
		hr = psfRecycleBin->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS| SHCONTF_INCLUDEHIDDEN, &peidl);

		if (hr == S_OK) 
		{
			PITEMID_CHILD pidlItem;
			while (peidl->Next(1, &pidlItem, NULL) == S_OK) 
			{
				STRRET sr;
				HRESULT hr = psfRecycleBin->GetDisplayNameOf(pidlItem, SHGDN_FORPARSING, &sr);
				PTSTR pszCiycleName;
				if (SUCCEEDED(StrRetToStr(&sr, pidlItem, &pszCiycleName)))
				{
					//这么做是有问题的，但是想不出跟好的解决方法
					std::wstring strCiycleName = pszCiycleName;
					//strCiycleName.replace(strCiycleName.begin(), strCiycleName.end(),)
					if (_tcsicmp(lpszRecycleFile, pszCiycleName) == 0)
					{

						HRESULT hr = psfRecycleBin->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &sr);
						if (SUCCEEDED(hr)) 
						{
							PTSTR pszName;
							hr = StrRetToStr(&sr, pidlItem, &pszName);
							if (SUCCEEDED(hr)) 
							{
								_tcscpy_s(lpszDisplayName, MAX_PATH, pszName);
								CoTaskMemFree(pszName);
								CoTaskMemFree(pszCiycleName);
								isFind = TRUE;
								break;
							}
						}
					}

					CoTaskMemFree(pszCiycleName);
				}
			}
		}
		if (peidl)
		{
			peidl->Release();
		}
	}

	if (psfRecycleBin)
	{
		psfRecycleBin->Release();
	}
	
	return isFind;
}

BOOL CFileMonitor::GetFolder ()
{
	BOOL			bReturn			= FALSE;
	LPMALLOC		pMalloc			= NULL;
	LPSHELLFOLDER	pDesktop		= NULL;
	LPITEMIDLIST	pidlRecycleBin	= NULL;
	HRESULT			hr				= S_OK;

	SHGetMalloc(&pMalloc); // windows memory management pointer needed later
	hr = SHGetDesktopFolder(&pDesktop);

	hr = SHGetSpecialFolderLocation (NULL, CSIDL_BITBUCKET, &pidlRecycleBin);
	if (NULL != m_pRecycleBin)
	{
		m_pRecycleBin->Release ();
		m_pRecycleBin = NULL;
	}
	hr = pDesktop->BindToObject(pidlRecycleBin, NULL, IID_IShellFolder, (LPVOID *)&m_pRecycleBin);
	if (SUCCEEDED (hr))
	{
		bReturn = TRUE;
	}

	pMalloc->Free (pidlRecycleBin);
	pDesktop->Release();
	pMalloc->Release();

	return bReturn;
}

BOOL CFileMonitor::GetFolder2 ()
{
	BOOL			bReturn			= FALSE;
	LPMALLOC		pMalloc			= NULL;
	LPSHELLFOLDER	pDesktop		= NULL;
	LPITEMIDLIST	pidlRecycleBin	= NULL;
	HRESULT			hr				= S_OK;

	SHGetMalloc(&pMalloc); // windows memory management pointer needed later
	if (NULL != m_pFolder2)
	{
		m_pFolder2->Release ();
		m_pFolder2 = NULL;
	}

	if ((SUCCEEDED (SHGetDesktopFolder(&pDesktop))) &&
		(SUCCEEDED (SHGetSpecialFolderLocation (NULL, CSIDL_BITBUCKET, &pidlRecycleBin))))
	{
		if (SUCCEEDED (pDesktop->BindToObject(pidlRecycleBin, NULL, IID_IShellFolder2, (LPVOID *)&m_pFolder2)))
		{
			bReturn = TRUE;
		}
	}

	if (NULL != pidlRecycleBin)
	{
		pMalloc->Free (pidlRecycleBin);
	}
	if (NULL != pDesktop)
	{
		pDesktop->Release();
	}
	pMalloc->Release();

	return bReturn;
}