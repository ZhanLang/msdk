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
#include <database/sqlite/sqlite3.h>
#include "sqliteconn.h"
#include "sqliterst.h"
//#include <boost/bind.hpp>


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
		GrpWarning( MODULE_NAME, DEFAULT_LEV, _T("BusyHandler retried SQLITE_BUSY_RETRY %d times.\n"), SQLITE_BUSY_RETRY );
		return 0;
	}
	
}

/********************************************************************
	Constructor/Destructor
*********************************************************************/
CSqliteConn::CSqliteConn( VOID )
{
	m_pConn = NULL;
	m_bInTrans = FALSE;
}

CSqliteConn::~CSqliteConn()
{
	Close();
}

/********************************************************************
	Connect
*********************************************************************/
HRESULT CSqliteConn::Connect( LPCTSTR szConn, LONG lOptions )
{
	if( !szConn || _tcslen(szConn)==0 )
		return E_INVALIDARG;
	
	int nRet = -1;

	mspath::CPath::CreateDirectoryEx(szConn);
#ifdef _UNICODE
	nRet = sqlite3_open16( szConn, &m_pConn );
#else
	nRet = sqlite3_open( lpszDataBase, &m_pConn );
#endif 

	if (nRet != SQLITE_OK)
	{
		const char * pError = sqlite3_errstr(nRet);
		USES_CONVERSION;
		GrpError( MODULE_NAME, DEFAULT_LEV, _T(" sqlite3_open Error: %d, %s\n"), nRet , A2W(pError));
		return E_FAIL;
	}


	RFAILED( ExecuteDML( _T("PRAGMA journal_mode = TRUNCATE") ) );

	// Set busy handler
	nRet = sqlite3_busy_handler( m_pConn, &BusyHandler, this );
	//nRet = sqlite3_busy_timeout( m_pConn, 10000 );

	if( nRet != SQLITE_OK )
	{
		GrpError( MODULE_NAME, DEFAULT_LEV, _T(" sqlite3_busy_handler Error: %d\n"), nRet );
		return E_FAIL;
	}

	return S_OK;
}

int CSqliteConn::RetryStep( sqlite3_stmt* pStmt, LONG* pAffected/* =NULL */ )
{
	int nRet;
	//for( int i=0; i<SQLITE_BUSY_RETRY; i++ )
	//{
	//	nRet = sqlite3_step( pStmt );

	//	if( nRet == SQLITE_DONE || nRet == SQLITE_ROW )
	//	{
	//		if( pAffected )
	//			*pAffected = sqlite3_changes( m_pConn );

	//		return nRet;
	//	}
	//	else if( (nRet==SQLITE_BUSY || nRet==SQLITE_LOCKED)  && !IsInTrans() )
	//	{
	//		
	//		Sleep( SQLITE_BUSY_DELAY );
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}

	nRet = sqlite3_step( pStmt );

	if( nRet == SQLITE_DONE || nRet == SQLITE_ROW )
	{
		if( pAffected )
			*pAffected = sqlite3_changes( m_pConn );
	}

	return nRet;
}

/********************************************************************
	Close
*********************************************************************/
HRESULT CSqliteConn::Close( void )
{
	if( m_pConn )
	{
		sqlite3_close( m_pConn );
		m_pConn = NULL;
	}

	//UnLoad();
	return S_OK;
}

/********************************************************************
	IsTableExists
*********************************************************************/
BOOL CSqliteConn::IsTableExists( LPCTSTR szTable )
{
	RASSERT( m_pConn, E_FAIL );

	tstring strSQL;
	strSQL = _T("SELECT COUNT(*) FROM sqlite_master WHERE name='");
	strSQL += szTable;
	strSQL += _T("'");

	LONG nCount=0;

	HRESULT hr = ExecuteScaler( strSQL.c_str(), nCount );
	if( hr != S_OK )
	{
		GrpError( MODULE_NAME, DEFAULT_LEV, _T(" IsTableExists Error: %s\n"), szTable );
		return FALSE;
	}

	return nCount>0;
}

/********************************************************************
	ExecuteDML
*********************************************************************/
HRESULT CSqliteConn::ExecuteDML( LPCTSTR szSQL, LONG* pAffected )
{
	RASSERT( m_pConn, E_FAIL );

	sqlite3_stmt* pStmt = Compile( szSQL );
	RASSERT( pStmt, E_FAIL );

	int nRet = RetryStep( pStmt, pAffected );

	if( nRet==SQLITE_DONE || nRet==SQLITE_ROW )
	{
		if( pAffected )
			*pAffected = sqlite3_changes(m_pConn);

		sqlite3_finalize( pStmt );
		return S_OK;
	}
	else
	{
		GrpError( MODULE_NAME, 1, _T("CSqliteConn::ExecuteDML failed. sql=%s, error=%d, %s\n"), 
			szSQL, nRet, sqlite3_errmsg16(m_pConn) );
		sqlite3_finalize( pStmt );
		return E_FAIL;
	}
}

HRESULT CSqliteConn::CompileStmt( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	RASSERT( m_pConn, E_FAIL );

	char* szError = NULL;
	sqlite3_stmt* pStmt = Compile( szSQL );

	if( pStmt )
	{
		CSqliteRST* pRST = new CSqliteRST( this, pStmt, szSQL );
		return pRST->QueryInterface( __uuidof(IGenericStmt), (void**)ppStmt );
	}
	else
	{
		return E_FAIL;
	}
}

/********************************************************************
	Execute
*********************************************************************/
HRESULT CSqliteConn::Execute( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields )
{
	RASSERT( m_pConn, E_FAIL );

	sqlite3_stmt* pStmt = Compile( szSQL );

	if( pStmt )
	{
		CSqliteRST* pRST = new CSqliteRST( this, pStmt, szSQL );
		if( FAILED(pRST->MoveFirst()) )
		{
			delete pRST;
			return E_FAIL;
		}

		pRST->SetFieldsMap( pFields );
		//pRST->SetFieldsMap( )
		return pRST->QueryInterface( __uuidof(IGenericMS), (void**)ppRecordset );
	}
	else
	{
		return E_FAIL;
	}
}

/********************************************************************
	ExecuteScaler
*********************************************************************/
HRESULT CSqliteConn::ExecuteScaler( LPCTSTR szSQL, LONG& lResult )
{
	RASSERT( m_pConn, E_FAIL );

	sqlite3_stmt* pStmt = Compile( szSQL );

	RASSERT( pStmt, E_FAIL );

	if( sqlite3_step(pStmt)==SQLITE_DONE || sqlite3_column_count(pStmt)==0 )
	{
		sqlite3_finalize( pStmt );
		return E_FAIL;
	}

	lResult = sqlite3_column_int( pStmt, 0 );
	sqlite3_finalize( pStmt );
	return S_OK;
}

/********************************************************************
	Compile
*********************************************************************/
sqlite3_stmt* CSqliteConn::Compile( LPCTSTR szSQL )
{
	sqlite3_stmt* pStmt;

	// This v2 compile function is recommended
	// It stores a sql statement and handles
	// SQLITE_SCHEMA automatically
#ifdef _UNICODE
	int nRet = sqlite3_prepare16_v2( m_pConn, szSQL, -1, &pStmt, NULL );
#else
	int nRet = sqlite3_prepare_v2( m_pConn, szSQL, -1, &pStmt, NULL );
#endif
	
	if (nRet == SQLITE_OK)
	{
		return pStmt;
	}
	else
	{
		GrpError( MODULE_NAME, 1, _T("CSqliteConn::Compile failed. sql=%s, error=%d, %s\n"), 
			szSQL, nRet, sqlite3_errmsg16(m_pConn) );
		return NULL;
	}
}

/********************************************************************
	GetTypeDecl
*********************************************************************/
HRESULT CSqliteConn::GetTypeDecl( VARTYPE vt, LPTSTR szBuf, INT nBufLen )
{
	RASSERT( m_pConn, E_FAIL );

	const TCHAR* szType;

	switch( vt )
	{
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI4:
		szType = _T("INTEGER");
		break;
	case VT_LPSTR:
	case VT_LPWSTR:
		szType = _T("TEXT");
		break;
	case VT_I8:
		szType = _T("BIGINT");
		break;
	case VT_CLSID:
	case VT_BUFFER:
	case VT_UNKNOWN:
		szType = _T("BLOB");
		break;
	default:
		{
			_ASSERT( FALSE );
			szType = NULL;
		}
		break;
	}

	if( szType )
	{
		_tcsncpy_s( szBuf, nBufLen, szType, nBufLen );
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CSqliteConn::BeginTrans()
{
	//return S_OK;
	RASSERT( m_pConn, E_FAIL );
	//if( m_bInTrans )
	//	return S_OK;

	sqlite3_stmt* pStmt = Compile( _T("BEGIN IMMEDIATE") );
	RASSERT( pStmt, E_FAIL );

	int nRet = RetryStep( pStmt );

	if( nRet == SQLITE_DONE )
	{
		sqlite3_finalize( pStmt );

		m_bInTrans = TRUE;
		return S_OK;
	}
	else
	{
		GrpError( MODULE_NAME, 1, _T("CSqliteConn::BeginTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
		sqlite3_finalize( pStmt );
		return E_FAIL;
	}

	//char* errmsg = NULL;
	//int nRet = sqlite3_exec( m_pConn, "BEGIN IMMEDIATE", NULL, NULL, &errmsg );

	//HRESULT hr = S_OK;

	//if( nRet==SQLITE_OK )
	//{
	//	m_bInTrans = TRUE;
	//}
	//else
	//{
	//	ZM1_GrpWarn( MODULE_NAME, 1, _T("CSqliteConn::BeginTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
	//	hr = E_FAIL;
	//}

	//sqlite3_free( errmsg );
	//return hr;
}

HRESULT CSqliteConn::CommitTrans()
{
	//return S_OK;
	RASSERT( m_pConn, E_FAIL );
	//if( !m_bInTrans )
	//	return S_OK;

	sqlite3_stmt* pStmt = Compile( _T("COMMIT") );
	RASSERT( pStmt, E_FAIL );

	int nRet = sqlite3_step( pStmt );

	if( nRet == SQLITE_DONE )
	{
		sqlite3_finalize( pStmt );
		return S_OK;
	}
	else
	{
		GrpError( MODULE_NAME, 1, _T("CSqliteConn::CommitTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
		sqlite3_finalize( pStmt );
		return E_FAIL;
	}

	//char* errmsg = NULL;
	//int nRet = sqlite3_exec( m_pConn, "COMMIT", NULL, NULL, &errmsg );

	//HRESULT hr = S_OK;

	//if( nRet==SQLITE_OK )
	//{
	//	m_bInTrans = FALSE;
	//}
	//else
	//{
	//	ZM1_GrpWarn( MODULE_NAME, 1, _T("CSqliteConn::CommitTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
	//	hr = E_FAIL;
	//}

	//sqlite3_free( errmsg );
	//return hr;
}

HRESULT CSqliteConn::RollbackTrans()
{
	//return S_OK;
	RASSERT( m_pConn, E_FAIL );
	//if( !m_bInTrans )
	//	return S_OK;

	sqlite3_stmt* pStmt = Compile( _T("ROLLBACK") );
	RASSERT( pStmt, E_FAIL );

	int nRet = RetryStep( pStmt );

	if( nRet == SQLITE_DONE )
	{
		sqlite3_finalize( pStmt );

		m_bInTrans = FALSE;
		return S_OK;
	}
	else
	{
		GrpError( MODULE_NAME, 1, _T("CSqliteConn::RollbackTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
		sqlite3_finalize( pStmt );
		return E_FAIL;
	}

	//char* errmsg = NULL;
	//int nRet = sqlite3_exec( m_pConn, "ROLLBACK", NULL, NULL, &errmsg );

	//HRESULT hr = S_OK;

	//if( nRet==SQLITE_OK )
	//{
	//	m_bInTrans = FALSE;
	//}
	//else
	//{
	//	ZM1_GrpWarn( MODULE_NAME, 1, _T("CSqliteConn::RollbackTrans failed. error=%d, %s\n"), nRet, sqlite3_errmsg16(m_pConn) );
	//	hr = E_FAIL;
	//}

	//sqlite3_free( errmsg );
	//return hr;
}

HRESULT CSqliteConn::GetTypeDecl( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen )
{
	RASSERT( m_pConn, E_FAIL );

	LPTSTR szType = NULL;

	switch( fieldType )
	{
	case field_bigint:
	case field_int:
	case field_smallint:
		{
			szType = _T("INTEGER");
		}
		break;
	case field_string:
	case field_datetime:
	case field_guid:
		{
			szType = _T("TEXT");
		}
		break;
	case field_binary:
		{
			szType = _T("BLOB");
		}
		break;
	}

	if( szType )
	{
		_tcsncpy_s( szBuf, nBufLen, szType, nBufLen );
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CSqliteConn::GetAutoIDDecl( LPTSTR szBuf, INT nBufLen )
{
	// Use AUTOINCREMENT to prevent ROWIDs from being recycled
	LPTSTR szAutoDecl = _T("INTEGER PRIMARY KEY AUTOINCREMENT");
	_tcsncpy_s( szBuf, nBufLen, szAutoDecl, nBufLen );

	return S_OK;
}

HRESULT CSqliteConn::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount )
{
	RASSERT( m_pConn, E_FAIL );

	tstring strSQL;
	strSQL = _T("pragma table_info('");
	strSQL += szTable;
	strSQL +=_T("')");
 //  LPCSTR sSql = MyW2A(strSQL.c_str());
	UTIL::com_ptr<IGenericMS> rec;
    Execute( strSQL.c_str(),  &rec.m_p );

	DWORD m_key = 0;
	while(!rec->IsEOF() )
	{
		m_key++;
		rec->MoveNext();
	}

	dwCount = m_key;
	if(dwCount == 0)
		return E_FAIL;
   
	return S_OK;

}

HRESULT CSqliteConn::GetTableFieldInfo ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize )
{
	RASSERT( m_pConn, E_FAIL );
	if(index <= 0 || FieldName == NULL || FieldType == NULL)
		return E_FAIL;

    LPTSTR sFieldName = NULL;
	LPTSTR sFieldType = NULL;
	INT nNameLen = 50;
	INT nTypeLen = 50;

	tstring strSQL;
	strSQL = _T("pragma table_info('");
	strSQL += szTable;
	strSQL +=_T("')");

	UTIL::com_ptr<IGenericMS> rec;
	Execute( strSQL.c_str(),  &rec.m_p );

    CPropSet props;
    //CPropVar prop;
	DWORD m_key = 0;
	while(!rec->IsEOF() )
	{
		m_key++;
		if (m_key == index)
		{
		  UTIL::com_ptr<IProperty2> record;
		  HRESULT hr = rec->GetRecord( &record.m_p );
		  RFAILEDP(hr,E_FAIL);

		  props=record ;
		  //prop = props[1].Val();
		  sFieldName = props[1];
           //prop = props[2].Val();
		  sFieldType = props[2];
	
		  break;
		}

		rec->MoveNext();
	}
		//_tcsncpy_s( szBuf, nBufLen, szType, nBufLen );
	if( sFieldName && sFieldType)
	{
		_tcsncpy_s( FieldName, nNameLen, sFieldName, nNameLen );
		_tcsncpy_s( FieldType, nTypeLen, sFieldType, nTypeLen );
	}
	else
       return E_FAIL;

	return S_OK;
}