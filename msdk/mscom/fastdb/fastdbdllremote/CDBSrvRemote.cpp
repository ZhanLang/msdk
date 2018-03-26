#include "StdAfx.h"
#include "CDBSrvRemote.h"

#include "DBConnRemote.h"


CDBFastSrvRemote::CDBFastSrvRemote(void)
{
}

CDBFastSrvRemote::~CDBFastSrvRemote(void)
{
	Uninit();
}

HRESULT CDBFastSrvRemote::init_class(IRSBase *prot, IRSBase *punkOuter)
{
	m_pRot = prot;
	return S_OK;
}

STDMETHODIMP CDBFastSrvRemote::Init(void*)
{
	RASSERT(m_pRot, E_FAIL);

	//////以下是测试用代码
//#ifdef _DEBUG
//	DWORD dwPort = 2017;
//	CreateTask(L"test100",L"test100",dwPort);
//
//	
//
//	{
//
//	
//	UTIL::com_ptr<IGenericRS> pRs;
//	HRESULT hr = this->Execute(L"test100",L"select count ( * )  from table_TTT where ID>=1 and ID<=5 ",&pRs.m_p);//where ID>=nIndexBegin and ID<=nIndexEnd 
//	FAILEXIT_FAIL(SUCCEEDED(hr));
//	
//	while(FALSE == pRs->IsEOF())
//	{
//		UTIL::com_ptr<IProperty2> pRecord;
//		pRs->GetRecord(&pRecord.m_p);
//		CPropSet theProp(pRecord);	
//
//		INT iCount = (INT)theProp[0];
//		//LPCSTR lpValue = (LPCSTR)theProp[3];
//
//		OutputDebugStringA("values: \n");
//		OutputDebugStringA("\n");
//		
//		pRs->MoveNext();
//	}
//}
//	{
//
//
//		UTIL::com_ptr<IGenericRS> pRs;
//		HRESULT hr = this->Execute(L"test100",L"select ID  from table_TTT where ID>=1 and ID<=5 ",&pRs.m_p);//where ID>=nIndexBegin and ID<=nIndexEnd 
//		FAILEXIT_FAIL(SUCCEEDED(hr));
//
//		while(FALSE == pRs->IsEOF())
//		{
//			UTIL::com_ptr<IProperty2> pRecord;
//			pRs->GetRecord(&pRecord.m_p);
//			CPropSet theProp(pRecord);	
//
//			INT iCount = (INT)theProp[0];
//			//LPCSTR lpValue = (LPCSTR)theProp[3];
//
//			OutputDebugStringA("values: \n");
//			OutputDebugStringA("\n");
//
//			pRs->MoveNext();
//		}
//	}


//#endif



	return S_OK;
}

STDMETHODIMP CDBFastSrvRemote::Uninit()
{
	if(m_pRot)
		m_pRot = INULL;
	return S_OK;
}

//ITaskDBFast
STDMETHODIMP CDBFastSrvRemote:: CreateTask (IN LPCWSTR szDBName,IN LPWSTR szTableName,OUT DWORD& dwPort) 
{
	FAILEXIT_FAIL(szDBName);

	UTIL::com_ptr<IGenericDB> pDBRemote;
	HRESULT hr = GetConn(szDBName, &pDBRemote.m_p);
	if(SUCCEEDED(hr) && pDBRemote.m_p )
		return S_OK;

	//连接远程DB.
	CFastDBConnRemote* pNewConn = new CFastDBConnRemote(szDBName,_T("localhost"),dwPort);
	FAILEXIT_FAIL(pNewConn);
	hr = pNewConn->Init(szDBName,_T("localhost"),dwPort);
	if(FAILED(hr))
	{
		delete pNewConn;
		return E_FAIL;
	}

	Helper::CritLock theLock(m_theLock.GetLock());
	this->m_mapDBConnSet[szDBName] = pNewConn;

	return S_OK;
}

STDMETHODIMP CDBFastSrvRemote:: ExecDML (IN LPCWSTR szDBName,LPCWSTR szSql) 
{
	UTIL::com_ptr<IGenericDB> pDB;
	HRESULT hr = GetConn(szDBName,&pDB.m_p);
	FAILEXIT_FAIL(SUCCEEDED(hr));
	
	hr = pDB->ExecuteDML(szSql);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}
STDMETHODIMP CDBFastSrvRemote:: Execute (IN LPCWSTR szDBName,LPCTSTR szSql,IGenericRS** ppRS ) 
{
	UTIL::com_ptr<IGenericDB> pDB;
	HRESULT hr = GetConn(szDBName,&pDB.m_p);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	hr = pDB->Execute(szSql,ppRS);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}

 STDMETHODIMP CDBFastSrvRemote::CloseTask (IN LPCWSTR szDBName) 
 {
	 RASSERT(szDBName, E_INVALIDARG);

	 Helper::CritLock theLock(m_theLock.GetLock());
	 map<wstring,UTIL::com_ptr<IGenericDB> >::iterator it = m_mapDBConnSet.find(szDBName);
	 RASSERT(it != m_mapDBConnSet.end(), S_OK);
		
	 CFastDBConnRemote* pNewConn = (CFastDBConnRemote* )it->second.m_p;
	 pNewConn->Uninit();
	 it->second = INULL;
	 m_mapDBConnSet.erase(it);

	 return S_OK;
 }


HRESULT CDBFastSrvRemote::GetConn(IN LPCTSTR lpDBName,OUT IGenericDB** ppConn )
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
