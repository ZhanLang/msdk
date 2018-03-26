
#include "stdafx.h"
#include "virtualconn.h"
#include "connpool.h"
#include "ado/adorecordset.h"
#include "ado/adoconn.h"
#include "ado/adostmt.h"
//#include <mslog/syslay/dbg.h>
//#include <rscom/prophelpers.h>

CVirtualConn::CVirtualConn( CConnPool* pPool )
:m_pPool( pPool )
{
	m_bInTrans = FALSE;
	m_bAttached = FALSE;
	m_pConn = NULL;
}

CVirtualConn::~CVirtualConn()
{

}

//--------------------------------------------------------------------
//	Connect & Close are now trivially implemented
//--------------------------------------------------------------------
HRESULT CVirtualConn::Connect( LPCTSTR szConn, LONG lOptions )
{
	return S_OK;
}

HRESULT CVirtualConn::Close()
{
	return S_OK;
}

CONN_IMPL* CVirtualConn::GetConn()
{
	if( ( m_bInTrans || m_bAttached ) && m_pConn )
		return m_pConn;

	return m_pPool->GetConn();
}

VOID CVirtualConn::FreeConn( CONN_IMPL* pConn )
{
	if( m_bInTrans || m_bAttached )
		return;

	m_pPool->FreeConn( pConn );
}

//--------------------------------------------------------------------
//	ExecuteDML
//--------------------------------------------------------------------
HRESULT CVirtualConn::ExecuteDML( LPCTSTR szSQL, LONG* pAffected )
{
	//RASSERTP( m_pPool, E_UNEXPECTED );

	// temporarily seize a real connection
	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	// execute in the real connection
	HRESULT hr = pConn->ExecuteDML( szSQL, pAffected );
	if(E_FAIL == hr)
	{
		//严重错误。数据库断了。这里要关连接。
		//为了重连作准备。
		pConn->Close();
	}

	// free the real connection and return
	FreeConn( pConn );
	return hr;
}

BOOL CVirtualConn::IsTableExists( LPCTSTR szTable )
{
	//RASSERTP( m_pPool, E_UNEXPECTED );

	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	BOOL bExist = pConn->IsTableExists( szTable );

	FreeConn( pConn );
	return bExist;
}

HRESULT CVirtualConn::Execute( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields )
{
	//RASSERTP( m_pPool, E_UNEXPECTED );

	// temporarily seize a real connection
	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	RS_IMPL* pRS = NULL;
	HRESULT hr = pConn->InnerExecute( szSQL, &pRS, pFields, nFields );
	if( FAILED(hr) )
	{
		FreeConn( pConn );
		*ppRecordset = NULL;
		return hr;
	}

	pRS->SetVirtualConn( this );
	if(*ppRecordset)
	{
		(*ppRecordset)->Release();
	}
	return pRS->QueryInterface( __uuidof(IGenericMS), (void**)ppRecordset );
}

//--------------------------------------------------------------------
//	Transaction operations
//--------------------------------------------------------------------
HRESULT CVirtualConn::BeginTrans()
{
	if( m_bInTrans )
	{
		ZM1_GrpError( MODULE_NAME, DEFAULT_LEV, _T("CVirtualConn::BeginTrans already in transaction.\n") );
		return E_FAIL;
	}

	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	HRESULT hr = pConn->BeginTrans();
	if( FAILED(hr) )
	{
		m_pPool->FreeConn( pConn );
		return hr;
	}

	m_pConn = pConn;
	m_bInTrans = TRUE;

	return S_OK;
}

HRESULT CVirtualConn::CommitTrans()
{
	if( !m_bInTrans )
		return S_OK;

	RASSERTP( m_pConn, E_UNEXPECTED );
	HRESULT hr = m_pConn->CommitTrans();

	m_bInTrans = FALSE;
	FreeConn( m_pConn );

	if( !m_bAttached )
		m_pConn = NULL;

	return hr;
}

HRESULT CVirtualConn::RollbackTrans()
{
	if( !m_bInTrans )
		return S_OK;
	
	RASSERTP( m_pConn, E_UNEXPECTED );
	HRESULT hr = m_pConn->RollbackTrans();

	m_bInTrans = FALSE;
	FreeConn( m_pConn );
	
	if( !m_bAttached )
		m_pConn = NULL;

	return hr;
}

//--------------------------------------------------------------------
//	Real connection binding operations
//--------------------------------------------------------------------
HRESULT CVirtualConn::AttachRealConn()
{
	if( m_bAttached )
	{
		ZM1_GrpWarn( MODULE_NAME, DEFAULT_LEV, _T("CVirtualConn::AttachRealConn already attached.\n") );
		return E_FAIL;
	}

	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	m_pConn = pConn;
	m_bAttached = TRUE;

	return S_OK;
}

HRESULT CVirtualConn::DetachRealConn()
{
	if( !m_bAttached )
	{
		ZM1_GrpWarn( MODULE_NAME, DEFAULT_LEV, _T("CVirtualConn::DetachRealConn not attached.\n") );
		return E_FAIL;
	}

	m_bAttached = FALSE;
	FreeConn( m_pConn );

	if( !m_bInTrans )
		m_pConn = NULL;

	return S_OK;
}

//--------------------------------------------------------------------
//	CompileStmt
//--------------------------------------------------------------------
HRESULT CVirtualConn::CompileStmt( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	//RASSERTP( m_pPool, E_UNEXPECTED );

	// temporarily seize a real connection
	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	STMT_IMPL* pStmt = NULL;
	HRESULT hr = pConn->InnerCompile( szSQL, &pStmt );
	FreeConn( pConn );

	RFAILED( hr );

	pStmt->SetVirtualConn( this );
	return pStmt->QueryInterface( __uuidof(IGenericStmt), (void**)ppStmt );
}

HRESULT CVirtualConn::GetTypeDecl( VARTYPE vt, LPTSTR szBuf, INT nBufLen )
{
	const TCHAR* szType;

	switch( vt )
	{
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI4:
		szType = _T("int");
		break;
	case VT_LPSTR:
		szType = _T("varchar(260)");
		break;
	case VT_LPWSTR:
		szType = _T("nvarchar(260)");
		break;
	case VT_I8:
		szType = _T("bigint");
		break;
	case VT_CLSID:
		szType = _T("binary(16)");
		break;
	case VT_BUFFER:
	case VT_UNKNOWN:
		szType = _T("image");
		break;
	default:
		{
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



	//if( m_pTypes == NULL )
	//{
	//	m_pTypes = m_pConn->OpenSchema( adSchemaProviderTypes );
	//}

	//int oleType;
	//switch( vt )
	//{
	//case VT_INT:
	//case VT_UINT:
	//case VT_I4:
	//case VT_UI4:
	//	oleType = adInteger;
	//	break;
	//case VT_I8:
	//	oleType = adBigInt;
	//	break;
	//case VT_LPSTR:
	//	oleType = adVarChar;
	//	break;
	//case VT_LPWSTR:
	//	oleType = adVarWChar;
	//	break;
	//case VT_CLSID:
	//	oleType = adGUID;
	//	break;
	//case VT_BUFFER:
	//case VT_UNKNOWN:
	//	oleType = adVarBinary;
	//	break;
	//}

	//tostringstream oss;
	//oss <<  _T("DATA_TYPE=") << oleType;

	//try
	//{
	//	m_pTypes->Filter = _bstr_t( oss.str().c_str() );

	//	while( !m_pTypes->EOFile )
	//	{
	//		int type = m_pTypes->Fields->GetItem( _T("DATA_TYPE") )->Value;
	//		_bstr_t strType = m_pTypes->Fields->GetItem( _T("TYPE_NAME") )->Value;
	//		_variant_t var = m_pTypes->Fields->GetItem( _T("CREATE_PARAMS") )->Value;
	//		
	//		_bstr_t strParam(  _T("") );
	//		if( var.vt==VT_BSTR )
	//			strParam = var.bstrVal;

	//		_tprintf( _T("%-5d%-15s%s\n"), type, (LPTSTR)strType, (LPCTSTR)strParam );

	//		m_pTypes->MoveNext();
	//	}

	//	if ((m_pTypes->EOFile || m_pTypes->BOF) )
	//	{
	//		return E_FAIL;
	//	}
	//	else
	//	{
	//		_bstr_t strType = m_pTypes->Fields->GetItem( _T("TYPE_NAME") )->Value;
	//		_tcsncpy( szBuf, strType, nBufLen );

	//		return S_OK;
	//	}
	//}
	//catch( _com_error e )
	//{
	//	wprintf( e.ErrorMessage() );
	//	hr = E_FAIL;
	//}
	//catch( ... )
	//{
	//	hr = E_FAIL;
	//}

	//return hr;
}

HRESULT CVirtualConn::GetTypeDecl( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen )
{
	LPTSTR szType = NULL;
	TCHAR szBuffer[64];
	DWORD dwBufLen = 64;

	switch( fieldType )
	{
	case field_bigint:
		{
			szType = _T("BIGINT");
		}
		break;
	case field_int:
		{
			szType = _T("INT");
		}
		break;
	case field_smallint:
		{
			szType = _T("SMALLINT");
		}
		break;
	case field_string:
		{
			if( dwSize!=0 )
			{
#ifdef _UNICODE
				_stprintf_s( szBuffer, dwBufLen, _T("NVARCHAR(%d)"), dwSize );
#else
				_stprintf_s( szBuffer, dwBufLen, _T("VARCHAR(%d)"), dwSize );
#endif

				szType = szBuffer;
			}
			else
			{
#ifdef _UNICODE
				szType = _T("NTEXT");
#else
				szType = _T("TEXT");
#endif
			}
		}
		break;
	case field_binary:
		{
			if( dwSize!=0 )
			{
				_stprintf_s( szBuffer, dwBufLen, _T("VARBINARY(%d)"), dwSize );
				szType = szBuffer;
			}
			else
			{
				szType = _T("IMAGE");
			}
		}
		break;
	case field_datetime:
		szType = _T("DATETIME");
		break;
	case field_guid:
		szType = _T("CHAR(36)");
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

HRESULT CVirtualConn::GetAutoIDDecl( LPTSTR szBuf, INT nBufLen )
{
	LPTSTR szAutoDecl = _T("INT IDENTITY");

	if( szAutoDecl )
	{
		_tcsncpy_s( szBuf, nBufLen, szAutoDecl, nBufLen );
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CVirtualConn::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount )
{
	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	HRESULT hr = pConn->GetTableFieldsCount( szTable,dwCount );
	RFAILEDP(hr,E_FAIL);

	FreeConn( pConn );
	return S_OK;

}

HRESULT CVirtualConn::GetTableFieldInfo (LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize )
{
	CONN_IMPL* pConn = GetConn();
	RASSERTP( pConn, E_FAIL );

	HRESULT hr = pConn->GetTableFieldInfo( szTable,index,FieldName,FieldType,dwSize );
	RFAILEDP(hr,E_FAIL);

	FreeConn( pConn );
	return S_OK;

}