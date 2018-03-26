/********************************************************************
	created:	2009/12/17
	created:	17:12:2009   16:50
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\otlconn.cpp
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb
	file base:	otlconn
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "otlinc.h"
#include "otlconn.h"
#include "otlrst.h"

/********************************************************************
	Constructor/Destructor
*********************************************************************/
COtlConn::COtlConn()
{
	// initialize the OTL environment
	otl_connect::otl_initialize();

	// create connection object
	m_pConn = new otl_connect();
}

/********************************************************************
	Connect
*********************************************************************/
HRESULT COtlConn::Connect( LPCTSTR szConn, LONG lOptions )
{
	if( IsConnected() )
		RFAILED( Close() );

	try
	{
#ifdef _UNICODE
		LPSTR szA = MyW2A( szConn );
		m_pConn->rlogon( szA, 1 );
		delete[] szA;
#else
		m_pConn->rlogon( szConn, 1 );
#endif
	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, _T("####数据库Adapter COtlRST::Open %s"), e.msg );
		return E_FAIL;
	}
	
	return m_pConn->connected ? S_OK : E_FAIL;
}

/********************************************************************
	Close
*********************************************************************/
HRESULT COtlConn::Close()
{
	if( !IsConnected() )
		return S_OK;

	m_pConn->logoff();
	m_pConn->connected = FALSE;

	return S_OK;
}

BOOL COtlConn::IsConnected()
{
	if( m_pConn && m_pConn->connected )
		return TRUE;
	else
		return FALSE;
}

/********************************************************************
	IsTableExists
*********************************************************************/
BOOL COtlConn::IsTableExists( LPCTSTR szTable )
{
	if( !IsConnected() )
		return FALSE;

	char szSchema[MAX_PATH];

#ifdef _UNICODE
	LPSTR szA = MyW2A( szTable );
	//sprintf( szSchema, "$SQLTables $1:'' $2:'' $3:'%s'", szA );
	sprintf_s( szSchema, MAX_PATH, "$SQLTables $3:'%s'", szA );
	delete[] szA;
#else
	sprintf( szSchema, "$SQLTables $3:'%s'", szTable );
#endif

	otl_stream sSchema;

	try
	{
		sSchema.open( 1, szSchema, *m_pConn );

		return sSchema.eof() ? FALSE : TRUE;
	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, _T("####数据库Adapter COtlRST::Open %s"), e.msg );
		return FALSE;
	}
}

/********************************************************************
	ExecuteDML
*********************************************************************/
HRESULT COtlConn::ExecuteDML( LPCTSTR szSQL, LONG* pAffected )
{
	RASSERT( IsConnected(), E_FAIL );

	long affected;

	HRESULT hr = S_OK;
	try
	{
#ifdef _UNICODE
		LPSTR szA = MyW2A( szSQL );
		affected = m_pConn->direct_exec( szA );
		delete[] szA;
#else
		affected = m_pConn->direct_exec( szSQL );
#endif
	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, (LPSTR)e.msg );

		hr = E_FAIL;
	}

	RFAILED( hr );

	if( pAffected )
		*pAffected = affected;

	return hr;
}

/********************************************************************
	CompileStmt
*********************************************************************/
HRESULT COtlConn::CompileStmt( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	COtlRST* pRST = new COtlRST( this->m_pConn, szSQL );

	return pRST->QueryInterface( __uuidof(IGenericStmt), (void**)ppStmt );
}

/********************************************************************
	Execute
*********************************************************************/
HRESULT COtlConn::Execute( LPCTSTR szSQL, IGenericRS** ppRecordset, DWORD* pFields, INT nFields )
{
	COtlRST* pRST = new COtlRST( this->m_pConn, szSQL );
	
	if( FAILED( pRST->Open() ) || FAILED( pRST->PrepareColumn() )  )
	{
		delete pRST;
		return E_FAIL;
	}

	return pRST->QueryInterface( __uuidof(IGenericRS), (void**)ppRecordset );
}

/********************************************************************
	GetTypeDecl
*********************************************************************/
HRESULT COtlConn::GetTypeDecl( VARTYPE vt, LPTSTR szBuf, INT nBufLen )
{
	return S_OK;
}

HRESULT COtlConn::BeginTrans()
{
	return S_OK;
}

HRESULT COtlConn::CommitTrans()
{
	return S_OK;
}

HRESULT COtlConn::RollbackTrans()
{
	return S_OK;
}