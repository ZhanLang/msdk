#include "StdAfx.h"
#include "DBConn.h"
#include "Helper.h"
#include "DBRecordSet.h"

CFastDBConn::CFastDBConn(void)
{
	m_iInitSize = 0;
	m_iPort = 0;
	m_pDBServer = NULL;
	m_pDB = NULL;
}

CFastDBConn::CFastDBConn(IN LPCTSTR lpDBName,IN LPCTSTR lpTableName,IN INT iInitSize,IN INT iIndexSize,IN BOOL bAcceptLink,IN INT iPort)
{
	m_iInitSize = 0;
	m_iPort = 0;
	m_pDBServer = NULL;
	m_pDB = NULL;
	return ;
}

CFastDBConn::~CFastDBConn(void)
{
}

HRESULT CFastDBConn::Init(IN LPCTSTR lpDBName,IN LPCTSTR lpTableName,IN INT iInitSize,IN INT iIndexSize,IN BOOL bAcceptLink,IN INT iPort)
{
	FAILEXIT_FAIL(lpDBName);
	FAILEXIT_FAIL(iInitSize);

	m_strDBName = lpDBName;

	//建立允许远程访问侦听.
	HRESULT hr = CreateServer(bAcceptLink,iInitSize,iIndexSize,iPort);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	

	return S_OK;
}
HRESULT CFastDBConn::Uninit()
{
	if(m_pDBServer )
	{
		m_pDBServer->stop();
		delete m_pDBServer;
		m_pDBServer = NULL;
	}

	if(m_pDB)
	{
		m_pDB->close();
		delete m_pDB;
		m_pDB = NULL;
	}
#ifdef DISKLESS
	wstring strDllName = L"FastDBDllDiskLess.dll";
#else
	wstring strDllName = L"FastDBDll.dll";
#endif

	//wstring strDllName = L"FastDBDll.dll";
	wstring strExePath;
	Helper::GetDllPath(strDllName,strExePath);

	wstring strFileName = strExePath;
	strFileName += m_strDBName;
	strFileName += _T(".fdb");

	HRESULT hr = DeleteFdb(m_strDBName.c_str());
	FAILEXIT_FAIL(SUCCEEDED(hr));
	
	return S_OK;
}

HRESULT CFastDBConn::CreateServer( IN BOOL bAcceptLink,IN INT iInitSize,IN INT iIndexSize,IN INT& iPort) 
{
	FAILEXIT_FAIL(bAcceptLink);
	FAILEXIT_FAIL(iPort);

	//如果没有指定端口, 用默认端口作为起始.
	m_pDB = new CMyDataBase(iInitSize,iIndexSize);
	FAILEXIT_FAIL(m_pDB);

	Helper::CTransStr theTrans;
	string strDBName = theTrans.MYW2A(m_strDBName.c_str());

	//string strDllName = "FastDBDll.dll";
#ifdef DISKLESS
	string strDllName = "FastDBDllDiskLess.dll";
#else
	string strDllName = "FastDBDll.dll";
#endif

	string strExePath;
	Helper::GetDllPath(strDllName,strExePath);
	strExePath += strDBName;
	strExePath += ".fdb";

	string strSqlOpen = "open ";
	strSqlOpen += " '";
	strSqlOpen += strDBName;
	strSqlOpen += "'  '";
	strSqlOpen += strExePath;
	strSqlOpen += "'  ;";

	//注意:这里的括号表示可先,而不是实际的语句成分.
	// open 'database-name' ( 'database-file-name' ) ';'

	m_pDB->ExecDML(strSqlOpen.c_str());

	 //autocommit (on|off)
	string strAutoCommitOff = "autocommit off;";
	m_pDB->ExecDML(strAutoCommitOff.c_str());

	stringstream oo;
	oo<<"localhost:"<<iPort;
	string strLocalHost = oo.str();

	m_pDBServer = (CMyDBServer*)new CMyDBServer(m_pDB ,strLocalHost.c_str());
	FAILEXIT_FAIL(m_pDBServer);	
	if(m_pDBServer->IsCreateLocalSockSuccessed())
	{
		m_pDBServer->start();
		return S_OK;
	}

	/*for(INT i = 1; i < MAX_PORT_COUNT; ++i)
	{
		stringstream oo;
		oo<<"localhost:"<<iDefaultPort + i;
		string strLocalHost = oo.str();
		BOOL bRet = m_pDBServer->ReInitSocket(strLocalHost);
		if(m_pDBServer->IsCreateGlobalSockSuccessed()  && m_pDBServer->IsCreateLocalSockSuccessed())
		{
			m_pDBServer->start();
			iPort = iDefaultPort + i;
			return S_OK;
		}			
	}
	delete m_pDBServer;
	m_pDBServer = NULL;*/
	return E_FAIL;
}

 HRESULT CFastDBConn::Connect ( LPCTSTR szConn, LONG lOptions )
 {
	 return E_NOTIMPL;
 }
 HRESULT CFastDBConn::Close ( void ) 
 {
	 return E_NOTIMPL;
 }

 BOOL  CFastDBConn::IsTableExists ( LPCTSTR szTable )
 {
	 FAILEXIT_FAIL(szTable);
	 FAILEXIT_FAIL(m_pDB);
	 Helper::CTransStr theTrans;
	 string strTableName = theTrans.MYW2A(szTable);
	 dbTableDescriptor* pDesc = m_pDB->findTableByName(strTableName.c_str());
	 FAILEXIT_FALSE(pDesc);
	 return TRUE;
 }

 HRESULT CFastDBConn::CompileStmt ( LPCTSTR szSQL, IGenericStmt** ppStmt )
 {
	

	 return E_NOTIMPL;
 }

 HRESULT CFastDBConn::ExecuteDML ( LPCTSTR szSQL, LONG* pAffected )
 {
	 //Helper::CritLock  theLock(m_theLockSubSql.GetLock());
	 FAILEXIT_FAIL(szSQL);
	 FAILEXIT_FAIL(m_pDB);

	 Helper::CTransStr theTrans;
     string strSql = theTrans.MYW2A(szSQL);

	 m_pDB->attach();
	
	 BOOL bRet = TRUE;


	 bRet = m_pDB->ExecDML(strSql.c_str() );


	 FAILEXIT_FAIL(bRet);

	 //这里显示调用commit,这样外面忘调了也无所谓.
	 m_pDB->ExecDML("commit;");

	 return S_OK;
 }

 HRESULT CFastDBConn::Execute( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields ) 
 {
	 //Helper::CritLock  theLock(m_theLockSubSql.GetLock());
	 //注:这里可以考虑自己提取Fields列表.
	 Helper::CTransStr theTrans;

	 //取得表名,对于
	 wstring wstrTableName ;
	 BOOL bFinded = theTrans.FindNextWord(TEXT("from"),szSQL,wstrTableName);
	 FAILEXIT_FAIL(bFinded);

	 string strTableName = theTrans.MYW2A(wstrTableName.c_str() );
	
	 //构建一个记录集对象
	 m_pDB->attach();
	 CDBRecordSet* pRecordSet = new CDBRecordSet(strTableName ,szSQL ,m_pDB	 );
	 FAILEXIT_FAIL(pRecordSet);
	
	 HRESULT hr = pRecordSet->Init(strTableName,szSQL,m_pDB);
	 if(FAILED(hr))
	 {
		 delete pRecordSet;
		 return E_FAIL;
	 }

	 hr = pRecordSet->QueryInterface(re_uuidof(IGenericMS),(VOID**)ppRecordset);
	 if(FAILED(hr))
	 {
		 delete pRecordSet;
		 return E_FAIL;
	 }


	 return S_OK;
 }

 HRESULT CFastDBConn::GetTypeDecl ( VARTYPE vt, LPTSTR szBuf, INT nBufLen ) 
 {

	 return E_NOTIMPL;
 }


 HRESULT CFastDBConn::BeginTrans ( VOID ) 
 {

	 return E_NOTIMPL;
 }
 HRESULT CFastDBConn::CommitTrans ( VOID ) 
 {

	 return E_NOTIMPL;
 }
 HRESULT CFastDBConn::RollbackTrans( VOID ) 
 {

	 return E_NOTIMPL;
 }

 HRESULT CFastDBConn::GetTypeDecl( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen ) 
 {
 
	 return E_NOTIMPL;
 }

 HRESULT CFastDBConn::GetAutoIDDecl ( LPTSTR szBuf, INT nBufLen ) 
 {

	 return E_NOTIMPL;
 }

 HRESULT CFastDBConn::AttachRealConn ( VOID ) 
 {

	 return E_NOTIMPL;
 }

 HRESULT CFastDBConn::DetachRealConn ( VOID ) 
 {

	 return E_NOTIMPL;
 }

HRESULT CFastDBConn::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount ) 
{

	return E_NOTIMPL;
}



HRESULT CFastDBConn::GetTableFieldInfo ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  ) 
{

	return E_NOTIMPL;
}


BOOL CFastDBConn::IsOpen( VOID ) 
{
	bool bRet = m_pDB->isOpen();
	FAILEXIT_FALSE(bRet);
	return TRUE;
}

//-------------------------------------
HRESULT CFastDBConn::InsertIntoTable( IN LPCTSTR szTable,VARIANT* pVarArray, DWORD dwArraySize) 
{
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(pVarArray);
	FAILEXIT_FAIL(dwArraySize);
	
	//dbList* pRoot = NULL;
	//dbList* pNode = NULL;
	//for(UINT i = 0;i < dwArraySize; ++i)
	//{
	//	VARIANT* pVar = &(pVarArray[i]);
	//	pNode = new dbList(pVar);
	//	FAILEXIT_FAIL(pNode);

	//	if(NULL == pRoot)
	//	{
	//		pRoot = pNode;
	//	}
	//	else
	//		pRoot->next = pNode;
	//}
	//
	//FAILEXIT_FAIL(pRoot);
	//
	//Helper::CTransStr theTrans;
	//string strTableName = theTrans.MYW2A(szTable);
	//
	//HRESULT hr = m_pDB->InsertIntoTable(strTableName,pRoot);
	//FAILEXIT_FAIL(SUCCEEDED(hr));

	return TRUE;
}

HRESULT CFastDBConn::DeleteFromTable( IN LPCTSTR szTable,IN LPCTSTR lpCondition) 
{
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(m_pDB);

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);
	
	dbQuery q;
	if(NULL == lpCondition)
	{
		q = "";
	}
	else
	{
		string strQuery = theTrans.MYW2A(lpCondition);
		q = strQuery.c_str();
	}

	bool bRet = m_pDB->DeleteFromTable(strTableName,q);
	FAILEXIT_FAIL(bRet);

	return S_OK;
}

HRESULT CFastDBConn::CreateNewTable(IN LPCTSTR  szTable,IN tag_FieldInfo* pFieldsSet,IN DWORD dwSetSize)
{
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(m_pDB);
	FAILEXIT_FAIL(pFieldsSet);
	FAILEXIT_FAIL(dwSetSize);

	std::vector<tag_FieldInfo> theFieldsSet;
	for(UINT i = 0;i < dwSetSize; ++i)
	{
		theFieldsSet.push_back(pFieldsSet[i]);
	}

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);
	bool bRet = m_pDB->CreateNewTable(strTableName,theFieldsSet);
	FAILEXIT_FAIL(bRet);

	return S_OK;
}

HRESULT CFastDBConn::SelectFromTable(IN LPCTSTR szTable,LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields)
{
	//从表中查询.
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(szSQL);
	FAILEXIT_FAIL(ppRecordset);

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);

	//构建一个记录集对象
	CDBRecordSet* pRecordSet = new CDBRecordSet(
		strTableName
		,szSQL
		,m_pDB			
		);
	FAILEXIT_FAIL(pRecordSet);

	HRESULT hr = pRecordSet->QueryInterface(re_uuidof(IGenericMS),(VOID**)ppRecordset);
	FAILEXIT_FAIL(SUCCEEDED(hr));





	return S_OK;
}

HRESULT CFastDBConn::UpdateTable(IN LPCTSTR szTable,IN tag_FieldInfoValue* pFieldsValSet,IN LPCTSTR lpCondition)
{
	

	return S_OK;
}

HRESULT CFastDBConn::DeleteFdb(IN LPCTSTR lpDBName)
{
	FAILEXIT_FAIL(lpDBName);

	wstring strDllName = L"FastDBDll.dll";
	wstring strExePath;
	Helper::GetDllPath(strDllName,strExePath);

	wstring strFielName = strExePath;
	strFielName += lpDBName;
	strFielName += TEXT(".fdb");
	BOOL bRet = ::DeleteFile(strFielName.c_str());
	FAILEXIT_FAIL(bRet);

	return S_OK;
}