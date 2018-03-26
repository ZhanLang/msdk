/********************************************************************
	created:	2009/09/08
	created:	8:9:2009   13:16
	filename: 	d:\C++Work\approduct2010\app\managedb\sqliteconn.cpp
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	sqliteconn
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "sqliteconn.h"
#include "ParseCommand.h"

using namespace std;

// nCalled: the number of times that the busy handler has been invoked for this locking event
int BusyHandler( void* pContext, int nCalled )
{
	CSqliteConn* pConn = (CSqliteConn*)pContext;

	if( nCalled <= SQLITE_BUSY_RETRY )
	{
		Sleep( SQLITE_BUSY_DELAY );
		return 1;
	}
	else
	{
		ZM1_GrpWarn( MODULE_NAME, DEFAULT_LEV, _T("BusyHandler retried SQLITE_BUSY_RETRY %d times.\n"), SQLITE_BUSY_RETRY );
		return 0;
	}
	
}

/********************************************************************
	Constructor/Destructor
*********************************************************************/
CSqliteConn::CSqliteConn( VOID )
{

}

CSqliteConn::~CSqliteConn()
{
	Close();
	m_pRot = INULL;
}

/********************************************************************
	Connect
*********************************************************************/
HRESULT CSqliteConn::Connect( LPCTSTR szConn, LONG lOptions )
{
	RASSERT(m_pRot, E_FAIL);

	if( !m_pDb || _tcslen(szConn)==0 )
		return E_INVALIDARG;
	
	//Database=c:\\db ; Pwd=maguojun

	CParseCommand parseCmd;
	parseCmd.ParseCmd(szConn);


	BOOL bUseSafe = FALSE;
	LPCTSTR lpszPwd = parseCmd.GetArg(_T("Pwd"));
	if (_tcslen( lpszPwd) > 0)
	{
		bUseSafe = TRUE;
	}

	
	m_pRot->CreateInstance(bUseSafe ? CLSID_DbSqliteSafeDB : CLSID_DbSqliteDB, NULL, re_uuidof(IGenericDB), (VOID**)&m_pDb);
	RASSERT(m_pDb, E_FAIL);

	if (FAILED(m_pDb->Connect(szConn, lOptions)))
	{
		m_pDb->Close();
		m_pDb = INULL;
		return E_FAIL;
	}

	return S_OK;
}


/********************************************************************
	Close
*********************************************************************/
HRESULT CSqliteConn::Close( void )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->Close();
}

/********************************************************************
	IsTableExists
*********************************************************************/
BOOL CSqliteConn::IsTableExists( LPCTSTR szTable )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->IsTableExists(szTable);
}

/********************************************************************
	ExecuteDML
*********************************************************************/
HRESULT CSqliteConn::ExecuteDML( LPCTSTR szSQL, LONG* pAffected )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->ExecuteDML(szSQL, pAffected);
}

HRESULT CSqliteConn::CompileStmt( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->CompileStmt(szSQL, ppStmt);
}

/********************************************************************
	Execute
*********************************************************************/
HRESULT CSqliteConn::Execute( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->Execute(szSQL, ppRecordset, pFields, nFields);
}

/********************************************************************
	ExecuteScaler
*********************************************************************/


/********************************************************************
	GetTypeDecl
*********************************************************************/
HRESULT CSqliteConn::GetTypeDecl( VARTYPE vt, LPTSTR szBuf, INT nBufLen )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->GetTypeDecl(vt, szBuf, nBufLen);
}

HRESULT CSqliteConn::BeginTrans()
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->BeginTrans();
}

HRESULT CSqliteConn::CommitTrans()
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->CommitTrans();
}

HRESULT CSqliteConn::RollbackTrans()
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->RollbackTrans();
}

HRESULT CSqliteConn::GetTypeDecl( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->GetTypeDecl(fieldType, dwSize, szBuf, nBufLen);
}

HRESULT CSqliteConn::GetAutoIDDecl( LPTSTR szBuf, INT nBufLen )
{
	RASSERT(m_pDb, E_FAIL);
	
	return m_pDb->GetAutoIDDecl(szBuf, nBufLen);
}

HRESULT CSqliteConn::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount )
{
	RASSERT(m_pDb, E_FAIL);
	return m_pDb->GetTableFieldsCount(szTable, dwCount);

}

HRESULT CSqliteConn::GetTableFieldInfo ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize )
{
	RASSERT(m_pDb, E_FAIL);

	return m_pDb->GetTableFieldInfo(szTable, index, FieldName, FieldType, dwSize);
}