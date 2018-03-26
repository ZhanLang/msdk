#include "StdAfx.h"
#include "CDBSrv.h"


CDBFastSrv::CDBFastSrv(void)
{
	m_dwPortBase = DEFAULT_PORT;

}

CDBFastSrv::~CDBFastSrv(void)
{
	//Uninit();
}

HRESULT CDBFastSrv::init_class(IMSBase *prot, IMSBase *punkOuter)
{
	m_pRot = prot;
	return S_OK;
}

STDMETHODIMP CDBFastSrv::Init(void*)
{
	//OutputDebugStringW(L"CDBFastSrv::Init 1\n");
	//CDBFastSrv
	RASSERT(m_pRot, E_FAIL);

	m_theExitEvent.ResetEvent();

	HRESULT hr = DeleteAllFdb();
	FAILD_LOGONLY(SUCCEEDED(hr));
	hr = InitPortMap();
	FAILD_LOGONLY(SUCCEEDED(hr));

	/*
	以下为测试用代码.*/
//#ifdef _DEBUG
//	DWORD dwPort = 0;
//	CreateTask(L"test100",L"test100",dwPort);
//
//	static WCHAR g_CreateTable[] = L"create table table_TTT("
//		L"TaskID int4,RecordID int4,ParentID int4,HaveSub int4,HostType int4,"
//		L"Target wstring,TargetRoot wstring,"
//		L"Time int4, EngineID int4,VirusID int4,"
//		L"VirusName wstring,"
//		L"VirusType int4,TreatMethod int4,TreatStatus int4,Trojcrc int8,"
//		L"Win32Pid int4,Win32Handle int4,Suspicous int4,ScanReult int4,ID int4);";
//
//	ExecDML(L"test100",g_CreateTable);
//	
//	for(int i = 0; i < 10; ++i)
//	{
//		wstring strInsertSql = 
//		L"insert into table_TTT values(1,2,3,1,1,'c:\\23sdff.exe','c:\\',2,3,3,'asdf',0,32,23,4234234,2,6,45,34,";
//		wostringstream oo;
//		oo << strInsertSql;
//		oo << i <<L");";
//
//		OutputDebugString(oo.str().c_str());
//		ExecDML(L"test100",oo.str().c_str());
//	}
//
//#endif

	
		

	//////ExecDML(L"test100",L"create table table_bbb(id int4,name string);");
	//////ExecDML(L"test100",L"insert into table_ttt values(100,'aaa');");
	////////ExecDML(L"test100",L"insert into table_ttt values(101,'aaa');");
	////////ExecDML(L"test100",L"insert into table_ttt values(103,'aaa');");
	////////ExecDML(L"test100",L"insert into table_ttt values(104,'aaa');");

	//////UTIL::com_ptr<IGenericMS> pRs;
	//////Execute(L"test100",L"select * from table_ttt ;", &pRs.m_p);
	//////while(FALSE == pRs->IsEOF())
	//////{
	//////	//这里提取记录，得到xml
	//////	UTIL::com_ptr<IProperty2> pRecord = (IUnknown*)NULL;
	//////	if(FAILED(pRs->GetRecord(&pRecord.m_p)))
	//////		break;

	//////	CPropSet propSet( pRecord );
	//////	int iTemp = (DWORD)propSet[0];    


	//////	HRESULT hr  = pRs->MoveNext();
	//////	if(FAILED(hr))
	//////		break;
	//////}



	//OutputDebugStringW(L"CDBFastSrv::Init 2\n");

	return S_OK;
}

STDMETHODIMP CDBFastSrv::Uninit()
{
	//CloseTask(L"test100");
	//OutputDebugStringW(L"CDBFastSrv::Uninit 1\n");
	
	CancleRunAsynFunc;
	//m_theExitEvent.SetEvent();
	//最多等待1秒,等所有正在处理的线程消息退出.
	//一般很快.
	//m_theMsgSem.WaitAllCountFree(1000);			

	if(m_pRot)
		m_pRot = INULL;
	
	HRESULT hr = DeleteAllFdb();
	FAILD_LOGONLY(SUCCEEDED(hr));

	m_mapDBConnSet.clear();
	m_mapDBPortGot.clear();
	m_mapDBPort.clear();


	CExprNodeAllocatorTlsMgr::Uninit();
		
	//OutputDebugStringW(L"CDBFastSrv::Uninit 2\n");


	return S_OK;
}


STDMETHODIMP CDBFastSrv:: CreateTask (IN LPCWSTR szDBName,IN LPWSTR szTableName,OUT DWORD& dwPort) 
{
	//Helper::CTimeUsed theTimer(L"CDBFastSrv:: CreateTask");

	FAILEXIT_FAIL(szDBName);
	FAILEXIT_FAIL(szTableName);

	CanRunAsynFunc;
// 	BOOL bRet = m_theExitEvent.Wait(1);
//  	if(TRUE == bRet)
//  	{//事件有信号,要退出.
//  		return E_FAIL;
//  	}
	//为当前消息线程申请线号.
	//Helper::CGetSem theGetSem(&m_theMsgSem);

	wstring strName = szDBName;
	HRESULT hr = AllocOnePort(dwPort,strName);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	//先找,找不到就新建
	CFastDBConn* pNewDB = new CFastDBConn(szDBName,szTableName,DEFAULT_INITDBSIZE,DEFAULT_INITDBSIZE - 8,TRUE,dwPort);
	FAILEXIT_FAIL(pNewDB);

	hr = pNewDB->Init(szDBName,szTableName,DEFAULT_INITDBSIZE,DEFAULT_INITDBSIZE - 8,TRUE,dwPort);
	if(FAILED(hr))
	{
		delete pNewDB;
		return E_FAIL;
	}

	{
		Helper::CritLock theLock(m_theLock.GetLock());
		this->m_mapDBConnSet[strName] = pNewDB;
	}

	//OutputDebugString(L"CDBFastSrv:: CreateTask 2\n");
	return S_OK;
}

STDMETHODIMP CDBFastSrv:: ExecDML (IN LPCWSTR szDBName,LPCWSTR szSql) 
{
	//Helper::CTimeUsed theTimer(L"CDBFastSrv:: ExecDML");
	//OutputDebugString(szSql);
	//OutputDebugString(L"\n");
	/*OutputDebugString(L"CDBFastSrv:: ExecDML 1\n");
#ifdef _DEBUG
	OutputDebugString(L"CDBFastSrv:: ExecDML\n");
	OutputDebugString(szDBName);
	OutputDebugString(L"\n");
	OutputDebugString(szSql);
	OutputDebugString(L"\n");
#endif*/
	//先找,找不到就新建

// 	BOOL bRet = m_theExitEvent.Wait(1);
// 	if(TRUE == bRet)
// 	{//事件有信号,要退出.
// 		return E_FAIL;
// 	}
	CanRunAsynFunc;
	//为当前消息线程申请线号.
	//Helper::CGetSem theGetSem(&m_theMsgSem);

	UTIL::com_ptr<IGenericDB> pConn;
	HRESULT hr = GetConn(szDBName,&pConn.m_p);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	//不论有盘无盘，都可能申请内存失败。扩展内存失败，所以这里都要catch
	try{
		hr = pConn->ExecuteDML(szSql,NULL);
	}catch(dbException e)
	{
		long lErrorCode = e.getErrCode();
		if(dbDatabase::FileLimitExeeded == lErrorCode)
		{//捕捉到内存申请失败的异常，然后返一个特定的错误值。
			OutputDebugStringW(L"CDBFastSrv::Execute:    dbDatabase::FileLimitExeeded  \n");
			return E_FileLimitExeeded;
		}
	}

	FAILEXIT_FAIL(SUCCEEDED(hr));

	//OutputDebugString(L"CDBFastSrv:: ExecDML 2\n");
	return S_OK;
}

STDMETHODIMP CDBFastSrv:: Execute (IN LPCWSTR szDBName,LPCTSTR szSql,IGenericMS** ppRS ) 
{
	//Helper::CTimeUsed theTimer(L"CDBFastSrv:: Execute");
	//OutputDebugString(szSql);
	//OutputDebugString(L"\n");
	/*OutputDebugString(L"CDBFastSrv:: Execute 1\n");
#ifdef _DEBUG
	OutputDebugString(L"CDBFastSrv:: Execute\n");
	OutputDebugString(szDBName);
	OutputDebugString(L"\n");
	OutputDebugString(szSql);
	OutputDebugString(L"\n");
#endif*/

	CanRunAsynFunc;
// 	BOOL bRet = m_theExitEvent.Wait(1);
// 	if(TRUE == bRet)
// 	{//事件有信号,要退出.
// 		return E_FAIL;
// 	}
	//为当前消息线程申请线号.
	//Helper::CGetSem theGetSem(&m_theMsgSem);

	//先找,找不到就新建
	UTIL::com_ptr<IGenericDB> pConn;
	HRESULT hr = GetConn(szDBName,&pConn.m_p);
	FAILEXIT_FAIL(SUCCEEDED(hr));


	//不论有盘无盘，都可能申请内存失败。扩展内存失败，所以这里都要catch
	try{
		hr = pConn->Execute(szSql,ppRS);
	//FAILEXIT_FAIL(SUCCEEDED(hr));

	}catch(dbException e)
	{
		long lErrorCode = e.getErrCode();
		if(dbDatabase::FileLimitExeeded == lErrorCode)
		{//捕捉到内存申请失败的异常，然后返一个特定的错误值。
			OutputDebugStringW(L"CDBFastSrv::Execute:    dbDatabase::FileLimitExeeded  \n");
			return E_FileLimitExeeded;
		}
	}

	//OutputDebugString(L"CDBFastSrv:: Execute 2\n");
	return hr;
}

STDMETHODIMP CDBFastSrv:: CloseTask (IN LPCWSTR szDBName) 
{
	/*OutputDebugString(L"CDBFastSrv:: CloseTask 1\n");
#ifdef _DEBUG
	OutputDebugString(L"CDBFastSrv:: CloseTask\n");
	OutputDebugString(szDBName);
	OutputDebugString(L"\n");
#endif*/


	CanRunAsynFunc;
// 	BOOL bRet = m_theExitEvent.Wait(1);
// 	if(TRUE == bRet)
// 	{//事件有信号,要退出.
// 		return E_FAIL;
// 	}
	//为当前消息线程申请线号.
	//Helper::CGetSem theGetSem(&m_theMsgSem);



	Helper::CritLock theLock(m_theLock.GetLock());

	map<wstring,UTIL::com_ptr<IGenericDB> >::iterator it ;

	wstring strName = szDBName;
	it = m_mapDBConnSet.find(strName);
	if(it == m_mapDBConnSet.end())
		return E_FAIL;

	CFastDBConn* pNewDB = (CFastDBConn*)it->second.m_p;
	pNewDB->Uninit();

	m_mapDBConnSet.erase(it);
	
	ReleaseOnePort(strName);

	//OutputDebugString(L"CDBFastSrv:: CloseTask 2\n");

	return S_OK;
}


//本地获取数据库连接,
HRESULT CDBFastSrv::GetConn(IN LPCTSTR lpDBName,OUT IGenericDB** ppConn )
{
	FAILEXIT_FAIL(lpDBName);

	Helper::CritLock theLock(m_theLock.GetLock());

	map<wstring,UTIL::com_ptr<IGenericDB> >::iterator it ;

	wstring strName = lpDBName;
	it = m_mapDBConnSet.find(strName);
	if(it == m_mapDBConnSet.end())
		return E_FAIL;

	
	HRESULT hr = it->second->QueryInterface(re_uuidof(IGenericDB),(VOID**)ppConn);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}


HRESULT CDBFastSrv::DeleteAllFdb()
{
#ifdef DISKLESS
	wstring strDllName = L"FastDBDllDiskLess.dll";
#else
	wstring strDllName = L"FastDBDll.dll";
#endif
	wstring strExePath;
	Helper::GetDllPath(strDllName,strExePath);

	wstring strFileName = strExePath;
	strFileName += _T("*.fdb");

	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData,sizeof(WIN32_FIND_DATA));
	HANDLE hFind = FindFirstFile(strFileName.c_str(),&FindFileData);

	while(hFind != INVALID_HANDLE_VALUE )
	{
		wstring strFileNameTemp;
		strFileNameTemp = strExePath + FindFileData.cFileName;

		BOOL bRet = ::DeleteFile(strFileNameTemp.c_str());	
		if(FALSE == bRet)
			break;

		FindNextFile(hFind,&FindFileData);
	}

	FindClose(hFind);

		
	return S_OK;
}

HRESULT CDBFastSrv::InitPortMap()
{
	Helper::CritLock theLock(m_theLock.GetLock());
	for(INT i = 0; i < MAX_PORT_COUNT; ++i)
	{
		wstring strDBName;
		DWORD dwPort = DEFAULT_PORT + i;
		m_mapDBPort[dwPort] = strDBName;
	}
	return S_OK;
}
HRESULT CDBFastSrv::AllocOnePort(DWORD& dwPort,wstring& strDBName)
{
	Helper::CritLock theLock(m_theLock.GetLock());	
	for(INT i = 0; i < MAX_PORT_COUNT; ++i)
	{
		DWORD dwTempPort = DEFAULT_PORT + i;
		BOOL bIsOccued = IsOccupanedPort(dwTempPort);
		if(TRUE == bIsOccued)
			continue;

		//申请到,就回去.
		m_mapDBPort[dwTempPort] = strDBName;
		m_mapDBPortGot[strDBName] = dwTempPort;
		dwPort = dwTempPort;
		return S_OK;	
	}
	
	return E_FAIL;
}
HRESULT CDBFastSrv::ReleaseOnePort(wstring& strDBName)
{
	Helper::CritLock theLock(m_theLock.GetLock());
	map<wstring,DWORD>::iterator it = m_mapDBPortGot.find(strDBName);
	if(it == m_mapDBPortGot.end())
		return S_OK;
	
	DWORD dwPort = it->second;
	m_mapDBPortGot.erase(it);

	wstring strNameTemp;
	m_mapDBPort[dwPort] = strNameTemp;

	return S_OK;
}
BOOL CDBFastSrv::IsOccupanedPort(DWORD& dwPort)
{
	Helper::CritLock theLock(m_theLock.GetLock());
	map<DWORD,wstring>::iterator it = m_mapDBPort.find(dwPort);
	if(it != m_mapDBPort.end() && true == it->second.empty())
	{
		return FALSE;
	}
	return TRUE;
}




