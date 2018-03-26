/********************************************************************
	created:	2009/09/08
	created:	8:9:2009   15:45
	filename: 	d:\C++Work\approduct2010\app\managedb\sqliterst.cpp
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	sqliterst
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <database/sqlite/sqlite3.h>
#include "sqliterst.h"
#include "propertyset.h"
#include "sqliteconn.h"

/********************************************************************
	Constructor/Destructor
*********************************************************************/
CSqliteRST::CSqliteRST( CSqliteConn* pConn, sqlite3_stmt* pStmt, LPCTSTR szSQL/* =NULL */ , BOOL bOhterStmt/* = FALSE*/)
{
	_ASSERT( pConn && pStmt );

	m_pParent = pConn;
	m_pConn = pConn->GetConn();
	m_pStmt = pStmt;
	m_bEOF = FALSE;
	m_pFieldsMap = NULL;

	if( szSQL )
		m_strSQL = szSQL;

	m_bOhterStmt = bOhterStmt;

	m_nCol = sqlite3_column_count( pStmt );
}

CSqliteRST::~CSqliteRST()
{
	SAFE_DELETE_BUFFER( m_pFieldsMap );

	if( m_pStmt )
	{
		if(m_bOhterStmt)	//不是自己产生的m_pStmt，不能释放
			sqlite3_reset(m_pStmt);
		else
			sqlite3_finalize( m_pStmt );

		m_pStmt = NULL;
	}
}

//********************************************************************
//	IGenericMS members
//********************************************************************
HRESULT CSqliteRST::MoveNext( VOID )
{
	_ASSERT( m_pStmt );

	if( m_bEOF )
		return E_FAIL;

	int nRet = m_pParent->RetryStep( m_pStmt );

	if( nRet == SQLITE_ROW )
		return S_OK;

	if( nRet == SQLITE_DONE )
	{
		m_bEOF = TRUE;
		return S_OK;
	}

	GrpWarning( MODULE_NAME, 1, _T("CSqliteRST::MoveNext failed. sql=%s, error=%d, %s\n"), 
		m_strSQL.c_str(), nRet, sqlite3_errmsg16(m_pConn) );
	return E_UNEXPECTED;
}

HRESULT CSqliteRST::MoveFirst( VOID )
{
	_ASSERT( m_pStmt );

	if( /*m_bEOF*/ TRUE )
	{
		sqlite3_reset( m_pStmt );
		m_bEOF = FALSE;
	}

	int nRet = m_pParent->RetryStep( m_pStmt );

	if( nRet == SQLITE_ROW )
		return S_OK;

	if( nRet == SQLITE_DONE )
	{
		m_bEOF = TRUE;
		return S_OK;
	}

	GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" CSqliteRST::MoveFirst,%d\n"), nRet );

	GrpWarning( MODULE_NAME, 1, _T("CSqliteRST::MoveFirst failed. sql=%s, error=%d, %s\n"), 
		m_strSQL.c_str(), nRet, sqlite3_errmsg16(m_pConn) );
	return E_UNEXPECTED;
}

BOOL CSqliteRST::IsEOF( VOID )
{
	return m_bEOF;
}

HRESULT CSqliteRST::GetRecord( IProperty2** ppRecord )
{
	if( m_bEOF )
	{
		*ppRecord = NULL;
		return E_FAIL;
	}

	UTIL::com_ptr<IProperty2> record;
	RFAILED( CPropertySet::CreateProp( &record.m_p ) );

	for( int i=0; i<m_nCol; i++ )
	{
		CPropVar prop;

		if( FAILED( GetField( i, &prop ) ) )
		{
			GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" GetField failed，%d\n"), i );
		}

		if( prop.vt == VT_NULL )
		{
			continue;
		}

		DWORD dwID;
		if( !m_pFieldsMap )
			dwID = i;
		else
			dwID = m_pFieldsMap[i];

		RFAILED( record->SetProperty( dwID, &prop ) );
	}

	*ppRecord = record.m_p;
	record->AddRef();
	
	return S_OK;
}

STDMETHODIMP CSqliteRST::GetRecord( IPropertyStr** ppRecord )
{
	if( m_bEOF )
	{
		*ppRecord = NULL;
		return E_FAIL;
	}

	UTIL::com_ptr<IPropertyStr> record;
	RFAILED( CPropertySet::CreateProp( &record.m_p ) );

	for( int i=0; i<m_nCol; i++ )
	{
		CPropVar prop;

		if( FAILED( GetField( i, &prop ) ) )
		{
			GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" GetField failed，%d\n"), i );
		}

		if( prop.vt == VT_NULL )
		{
			continue;
		}

		DWORD dwID;
		if( !m_pFieldsMap )
			dwID = i;
		else
			dwID = m_pFieldsMap[i];

		RFAILED( record->SetProperty( GetFieldName(i), &prop ) );
	}

	*ppRecord = record.m_p;
	record->AddRef();

	return S_OK;
}

HRESULT CSqliteRST::GetScale( PROPVARIANT* pScale )
{
	RASSERTP( pScale, E_INVALIDARG );
	if( m_bEOF )
	{
		pScale->vt = VT_NULL;
		return E_FAIL;
	}

	CPropVar var;
	RFAILED( GetField( 0, &var ) );

	*pScale = var;
	
	return S_OK;
}


HRESULT CSqliteRST::GetField( int nField, CPropVar* pProp )
{
	if( nField<0 || nField>=m_nCol )
	{
		return E_INVALIDARG;
	}

	if( m_bEOF )
	{
		GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" GetField EOF\n") );
		return E_UNEXPECTED;
	}

	int nType = sqlite3_column_type( m_pStmt, nField );

	switch( nType )
	{
	case SQLITE_INTEGER:
		{
			// All integers all fetched as 64 bit values
			sqlite3_int64 nLarge = sqlite3_column_int64( m_pStmt, nField );
			*pProp = (LONGLONG)nLarge;
		}	
		break;
	case SQLITE_TEXT:
		{
#ifdef _UNICODE
			const wchar_t* szValue = (const wchar_t*)sqlite3_column_text16( m_pStmt, nField );
#else
			const char* szValue = (const char*)sqlite3_column_text( m_pStmt, nField );
#endif
			*pProp = szValue;
		}
		break;
	case SQLITE_BLOB:
		{
			int nLen = sqlite3_column_bytes( m_pStmt, nField );
			void* pBuffer = (void*)sqlite3_column_blob( m_pStmt, nField );

			pProp->vt = VT_BUFFER;
			pProp->SetVector( pBuffer, nLen );
		}
		break;
	case SQLITE_NULL:
		{
			pProp->vt = VT_NULL;
		}
		break;
	default:
		{
			GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" GetField unexpected type\n") );
			return E_UNEXPECTED;
		}
		break;
	}

	return S_OK;
}

VOID CSqliteRST::SetFieldsMap( DWORD* pFields/* =NULL */, INT nFields/* =0 */ )
{
	RASSERTV( pFields );

	SAFE_DELETE_BUFFER( m_pFieldsMap );

	m_pFieldsMap = new DWORD[m_nCol];
	memset( m_pFieldsMap, 0, sizeof(DWORD)*m_nCol );
	memcpy( m_pFieldsMap, pFields, sizeof(DWORD)*nFields );
}

/********************************************************************
	IGenericStmt members
*********************************************************************/
HRESULT CSqliteRST::BindParam( INT nIndex, PROPVARIANT* value )
{
	VARTYPE vt = value->vt;
	int nRet;

	int tempIndex = nIndex+1;

	switch( vt )
	{
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI4:
		{
			nRet = sqlite3_bind_int( m_pStmt, tempIndex, value->ulVal );
		}
		break;
	case VT_I8:
		{
			nRet = sqlite3_bind_int64( m_pStmt, tempIndex, (sqlite3_int64)value->hVal.QuadPart );
		}
		break;
	case VT_LPSTR:
		{
			nRet = sqlite3_bind_text( m_pStmt, tempIndex, value->pszVal, -1, SQLITE_TRANSIENT );
		}
		break;
	case VT_LPWSTR:
		{
			nRet = sqlite3_bind_text16( m_pStmt, tempIndex, value->pwszVal, -1, SQLITE_TRANSIENT );
		}
		break;
//	case VT_DATE:
//		{
//#ifdef _UNICODE
//			nRet = m_pParent->sqlite3_bind_text16( m_pStmt, tempIndex, value->pwszVal, -1, SQLITE_TRANSIENT );
//#else
//			nRet = m_pParent->sqlite3_bind_text( m_pStmt, tempIndex, value->pszVal, -1, SQLITE_TRANSIENT );
//#endif
//		}
//		break;
	case VT_BUFFER:
		{
			CAC& ca = value->cac;
			nRet = sqlite3_bind_blob( m_pStmt, tempIndex, ca.pElems, ca.cElems, SQLITE_TRANSIENT );
		}
		break;
	case VT_CLSID:
		{
			nRet = sqlite3_bind_blob( m_pStmt, tempIndex, value->puuid, sizeof(CLSID), SQLITE_TRANSIENT );
		}
		break;
	case VT_NULL:
		{
			nRet = sqlite3_bind_null( m_pStmt, tempIndex );
		}
		break;
	case VT_DATETIME:
		{
			ULONG wordNum = value->cac.cElems;

			if( wordNum == sizeof(SYSTEMTIME)/sizeof(WORD) )
			{
				SYSTEMTIME* pSystem = (SYSTEMTIME*)value->cac.pElems;

				TCHAR szTime[128] = {0};

				_stprintf_s( szTime, _countof(szTime), _T("%04d-%02d-%02d %02d:%02d:%02d"),
					pSystem->wYear, pSystem->wMonth, pSystem->wDay,
					pSystem->wHour, pSystem->wMinute, pSystem->wSecond );

#ifdef _UNICODE
				nRet = sqlite3_bind_text16( m_pStmt, tempIndex, szTime, -1, SQLITE_TRANSIENT );
#else
				nRet = sqlite3_bind_text( m_pStmt, tempIndex, szTime, -1, SQLITE_TRANSIENT );
#endif
			}
		}	
		break;
	default:
		{
			nRet = SQLITE_MISMATCH;
		}
		break;
	}

	if( nRet == SQLITE_OK )
	{
		return S_OK;
	}
	else
	{
		// // // RSLOG( RSLOG_ALERT, "CSqliteRST::BindParam, error:%d", nRet );
		return E_FAIL;
	}
}

HRESULT CSqliteRST::Reset( VOID )
{
	int nRet1 = sqlite3_reset( m_pStmt );
	int nRet2 = sqlite3_clear_bindings( m_pStmt );


	if( nRet1==SQLITE_OK && nRet2==S_OK )
	{
		if( m_bEOF )
			m_bEOF = FALSE;

		return S_OK;
	}
	else
	{
		GrpWarning( MODULE_NAME, 1, _T("CSqliteRST::Reset failed. sql=%s, error=%d, %s\n"), 
			m_strSQL.c_str(), nRet1, sqlite3_errmsg16(m_pConn) );
		return E_FAIL;
	}
}

HRESULT CSqliteRST::ExecuteDML( LPLONG pAffected )
{
	int nRet = m_pParent->RetryStep( m_pStmt, pAffected );

	if( nRet!=SQLITE_DONE && nRet!=SQLITE_ROW )
	{
		GrpWarning( MODULE_NAME, 1, _T("CSqliteRST::ExecuteDML failed. sql=%s, error=%d, %s\n"), 
			m_strSQL.c_str(), nRet, sqlite3_errmsg16(m_pConn) );

		return E_FAIL;
	}

	sqlite3_reset( m_pStmt );
	return S_OK;
}

HRESULT CSqliteRST::Execute( IGenericMS** ppRST, DWORD* pFields, INT nFields )
{
	CSqliteRST* pRST = new CSqliteRST( m_pParent, m_pStmt, m_strSQL.c_str(), TRUE );
	if( FAILED(pRST->MoveFirst()) )
	{
		delete pRST;
		return E_FAIL;
	}

	pRST->SetFieldsMap( pFields );
	return pRST->QueryInterface( __uuidof(IGenericMS), (void**)ppRST );

	//SetFieldsMap( pFields, nFields );
	//MoveFirst();

	//return QueryInterface( __uuidof(IGenericMS), (void**)ppRST );
}

HRESULT CSqliteRST::BindParams( IProperty2* params )
{
	RASSERTP( params, E_INVALIDARG );

	DWORD dwParam = sqlite3_bind_parameter_count( m_pStmt );
	for( DWORD i=0; i<dwParam; i++ )
	{
		const PROPVARIANT* prop;
		if( SUCCEEDED(params->GetProperty( i, &prop )) )
		{
			RFAILED( BindParam( i, (PROPVARIANT*)prop ) );
			continue;
		}

		//// Bind null
		//m_pParent->sqlite3_bind_null( m_pStmt, i+1 );
	}

	return S_OK;
}

LPCSTR CSqliteRST::GetFieldName( int nField )
{
	if( nField<0 || nField>=m_nCol )
	{
		return "";
	}

	if( m_bEOF )
	{
		GrpWarning( MODULE_NAME, DEFAULT_LEV, _T(" GetFieldName EOF\n") );
		return "";
	}

	return sqlite3_column_name( m_pStmt, nField );
}
