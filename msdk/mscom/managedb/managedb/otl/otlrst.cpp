/********************************************************************
	created:	2009/12/19
	created:	19:12:2009   10:08
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\otl\otlrst.cpp
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb\otl
	file base:	otlrst
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "otlinc.h"
#include "otlfieldhandlers.h"
#include "otlrst.h"
#include "../propertyset.h"
#include <algorithm>

COtlRST::OtlHandlerTable COtlRST::ms_otlTypes;

#define OTL_GET_ITEM(type, handler) Insert( DisItem(type, (LPVOID)handler) );

COtlRST::OtlHandlerTable::OtlHandlerTable()
{
	OTL_GET_ITEM( otl_var_bigint,		otl_handler_numeric )
	OTL_GET_ITEM( otl_var_float,		otl_handler_numeric )
	OTL_GET_ITEM( otl_var_int,			otl_handler_numeric )
	OTL_GET_ITEM( otl_var_long_int,		otl_handler_numeric )
	OTL_GET_ITEM( otl_var_short,		otl_handler_numeric )
	OTL_GET_ITEM( otl_var_unsigned_int,	otl_handler_numeric )

	OTL_GET_ITEM( otl_var_blob,			otl_handler_blob	)
	OTL_GET_ITEM( otl_var_char,			otl_handler_char	)
	OTL_GET_ITEM( otl_var_raw,			otl_handler_binary	)
	OTL_GET_ITEM( otl_var_raw_long,		otl_handler_blob	)
	OTL_GET_ITEM( otl_var_clob,			otl_handler_blob	)
	OTL_GET_ITEM( otl_var_timestamp,	otl_handler_datetime )
	OTL_GET_ITEM( otl_var_varchar_long,	otl_handler_varchar_long )
	
	Sort();
}

DWORD COtlRST::OtlHandlerTable::GetHandlerType( DWORD otlType )
{
	return (DWORD)FindHandler( otlType );
}

//--------------------------------------------------------------------
//	Constructor/Destructor
//--------------------------------------------------------------------
COtlRST::COtlRST( otl_connect* pConn, LPCTSTR szSQL )
{
	m_pConn = pConn;
	m_pStream = NULL;
	m_pFieldsMap = NULL;
	m_ppHandlers = NULL;

	m_bStreamMode = FALSE;
	m_bOpened = FALSE;
	m_bExectuted = FALSE;

	m_nVars = 0;
	m_bindHandlers = NULL;

#ifdef _UNICODE
	LPSTR szA = MyW2A( szSQL );
	m_strSQL = szA;
	delete[] szA;
#else
	m_strSQL = szSQL;
#endif
}

COtlRST::~COtlRST()
{
	if( m_pStream )
	{
		delete		m_pStream;
	}
	
	delete[]	m_ppHandlers;
	delete[]	m_bindHandlers;
};

//--------------------------------------------------------------------
//	Open
//--------------------------------------------------------------------
HRESULT COtlRST::Open()
{
	if( m_bOpened )
		return S_OK;

	if( m_strSQL.empty() )
		return E_FAIL;

	HRESULT hr = S_OK;

	try
	{
		if( !m_bStreamMode )
		{
			m_pStream = new otl_stream( OTL_BUFF_SIZE, m_strSQL.c_str(), *m_pConn );
		}
		else
		{
			m_pStream = new otl_stream;
			m_pStream->set_lob_stream_mode( true );
			m_pStream->open( 1, m_strSQL.c_str(), *m_pConn );
			m_pStream->set_commit( 0 );
		}

	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, _T("####数据库Adapter COtlRST::Open %s"), e.msg );
		hr = E_FAIL;
	}

	RFAILED( hr );

	m_bOpened = TRUE;

	return hr;
}

HRESULT COtlRST::SetLobMode( VOID )
{
	m_bStreamMode = TRUE;
	return S_OK;
}

//--------------------------------------------------------------------
//	PrepareColumn
//	Config stream options according to column types
//--------------------------------------------------------------------
BOOL COtlRST::PrepareColumn()
{
	m_pColDesc = m_pStream->describe_select( m_nCol );
	if( m_nCol == 0 )
		return TRUE;
	
	m_ppHandlers = new IOtlFieldHandler*[m_nCol];
	memset( m_ppHandlers, 0, sizeof(IOtlFieldHandler*)*m_nCol );

	for( INT i=0; i<m_nCol; i++ )
	{
		otl_column_desc& column = m_pColDesc[i];

		DWORD handlerType = (DWORD)ms_otlTypes.FindHandler( column.otl_var_dbtype );
		IOtlFieldHandler* pHandler = CreateFieldHandler( handlerType );
		
		if( !pHandler )
			return FALSE;

		RASSERT( pHandler->PrepareColumn( &column ), FALSE );
		m_ppHandlers[i] = pHandler;
	}

	IterHandlers iter = m_handlers.begin();
	for( ; iter!=m_handlers.end(); ++iter )
	{
		IOtlFieldHandler* pHandler = iter->second;
		RASSERT( pHandler && pHandler->PrepareIO(m_bStreamMode), FALSE );
	}

	return TRUE;
}

#define NEW_OTL_HANDLER( type ) case type: { pHandler = new concrete_handler<type>(this->m_pStream); } break;

IOtlFieldHandler* COtlRST::CreateFieldHandler( DWORD handlerType )
{
	IOtlFieldHandler* pHandler = NULL;

	IterHandlers iter = m_handlers.find( handlerType );
	if( iter == m_handlers.end() )
	{
		switch( handlerType )
		{
		NEW_OTL_HANDLER( otl_handler_numeric );
		NEW_OTL_HANDLER( otl_handler_blob );
		NEW_OTL_HANDLER( otl_handler_char );
		NEW_OTL_HANDLER( otl_handler_binary );
		NEW_OTL_HANDLER( otl_handler_varchar_long );
		NEW_OTL_HANDLER( otl_handler_datetime );
		}

		m_handlers[handlerType] = pHandler;
	}
	else
	{
		pHandler = iter->second;
	}

	return pHandler;
}

HRESULT COtlRST::GetRecord( ISrvProperty2** ppRecord )
{
	RASSERT( m_bOpened, E_UNEXPECTED );

	if( m_pStream->eof() )
	{
		*ppRecord = NULL;
		return E_FAIL;
	}

	UTIL::com_ptr<ISrvProperty2> propSet;
	RFAILED( CPropertySet::CreateProp( &propSet.m_p ) );

	for( int i=0; i<m_nCol; i++ )
	{
		CPropVar* pProp = new CPropVar;
		IOtlFieldHandler* pHandler = m_ppHandlers[i];
		RASSERTP( pHandler, E_UNEXPECTED );

		pHandler->GetField( pProp, m_pColDesc[i].otl_var_dbtype );
		RASSERT( m_pStream->good(), E_FAIL );

		DWORD dwID;
		if( !m_pFieldsMap )
			dwID = i;
		else
			dwID = m_pFieldsMap[i];

		if( propSet->SetProperty( dwID, pProp ) != S_OK )
		{
			delete pProp;
			assert( false );
		}
	}

	*ppRecord = propSet.m_p;
	propSet->AddRef();

	return S_OK;
}

HRESULT COtlRST::GetScale( PROPVARIANT* pScale )
{
	RASSERTP( pScale, E_INVALIDARG );	
	RASSERT( m_bOpened && !m_pStream->eof(), E_UNEXPECTED );

	IOtlFieldHandler* pHandler = m_ppHandlers[0];
	RASSERTP( pHandler, E_UNEXPECTED );

	pHandler->GetField( (CPropVar*)pScale, m_pColDesc[0].otl_var_dbtype );
	RASSERT( m_pStream->good(), E_FAIL );

	if( m_nCol > 1 )
		m_pStream->skip_to_end_of_row();

	return S_OK;
}

HRESULT COtlRST::MoveNext( VOID )
{
	RASSERT( m_bOpened, E_UNEXPECTED );

	return S_OK;
}

HRESULT COtlRST::MoveFirst( VOID )
{
	RASSERT( m_bOpened, E_UNEXPECTED );

	try
	{
		m_pStream->rewind();
	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, _T("####数据库Adapter COtlRST::MoveFirst %s"), e.msg );
		return E_FAIL;
	}

	return S_OK;
}

BOOL COtlRST::IsEOF( VOID )
{
	if( !m_bOpened )
		return FALSE;

	return m_pStream->eof();
}

VOID COtlRST::SetFieldsMap( DWORD* pFields/* =NULL */, INT nFields/* =0 */ )
{
	RASSERTV( pFields );

	SAFEDELETEARRAY( m_pFieldsMap );

	m_pFieldsMap = new DWORD[m_nCol];
	memset( m_pFieldsMap, 0, sizeof(DWORD)*m_nCol );
	memcpy( m_pFieldsMap, pFields, sizeof(DWORD)*nFields );
}

//--------------------------------------------------------------------
//	IGenericStmt members
//--------------------------------------------------------------------
HRESULT COtlRST::BindParam( INT nIndex, PROPVARIANT* value )
{
	m_Params[nIndex] = *value;

	return S_OK;
}

HRESULT COtlRST::Reset( VOID )
{
	return S_OK;
}

HRESULT COtlRST::ExecuteDML( LPLONG pAffected )
{
	if( !m_bExectuted )
	{
		RASSERT( PrepareVars(), E_FAIL );

		// Open the stream
		m_bStreamMode = m_vecLob.empty() ? FALSE : TRUE;
		RFAILED( Open() );

		m_bExectuted = TRUE;
	}

	RASSERT( SetParams(), E_FAIL );
	m_pConn->commit();

	return S_OK;
}

HRESULT COtlRST::Execute( IGenericRS** ppRST, DWORD* pFields, INT nFields )
{
	if( !m_bExectuted )
	{
		RASSERT( PrepareVars(), E_FAIL );

		// Open the stream
		m_bStreamMode = m_vecLob.empty() ? FALSE : TRUE;
		RFAILED( Open() );
		RFAILED( PrepareColumn() );

		m_bExectuted = TRUE;
	}

	RASSERT( SetParams(), E_FAIL );

	SetFieldsMap( pFields, nFields );
	RFAILED( QueryInterface( __uuidof(IGenericRS), (void**)ppRST ) );

	return S_OK;
}

HRESULT COtlRST::SetLobParam( INT index )
{
	m_vecLob.push_back( index );
	return S_OK;
}


BOOL COtlRST::GetOtlVarString( const PROPVARIANT* prop, char* szVar, BOOL bLob/* =FALSE */ )
{
	switch( prop->vt )
	{
	case VT_INT:
	case VT_UINT:
		{
			strcpy_s( szVar, MAX_PATH, "int" );
		}
		break;
	case VT_I4:
	case VT_UI4:
		{
			strcpy_s( szVar, MAX_PATH, "long" );
		}
		break;
	case VT_I8:
		{
			strcpy_s( szVar, MAX_PATH, "bigint" );
		}
		break;
	case VT_LPSTR:
	case VT_LPWSTR:
		{
			if( !bLob )
			{
				sprintf_s( szVar, MAX_PATH, "char(%d)", OTL_STR_VAR_LEN );
			}
			else
			{	
				strcpy_s( szVar, MAX_PATH, "varchar_long" );
			}
		}
		break;
	case VT_DATETIME:
		{
			strcpy_s( szVar, MAX_PATH, "timestamp" );
		}
		break;
	case VT_BUFFER:
		{
			strcpy_s( szVar, MAX_PATH, "raw_long" );
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

LPVOID COtlRST::GetBindHandler( VARTYPE vt, BOOL bLob/* =FALSE */ )
{
	switch( vt ) 
	{
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI4:
	case VT_I8:
		return concrete_handler<otl_handler_numeric>::PutVar;
	case VT_LPSTR:
	case VT_LPWSTR:
		{
			if( bLob )
			{
				return concrete_handler<otl_handler_varchar_long>::PutVar;
			}
			else
			{	
				return concrete_handler<otl_handler_char>::PutVar;
			}
		}
	case VT_DATETIME:
		return concrete_handler<otl_handler_datetime>::PutVar;
	case VT_BUFFER:
		{
			if( bLob )
			{
				return concrete_handler<otl_handler_blob>::PutVar;
			}
			else
			{	
				return concrete_handler<otl_handler_binary>::PutVar;
			}
		}
	}

	return NULL;
}

BOOL COtlRST::PrepareVars()
{
	DWORD dwVars = (DWORD)m_Params.size();
	if( dwVars==0 )
		return TRUE;

	m_bindHandlers = new LPVOID[dwVars];

	INT varMark;
	varMark = (INT)m_strSQL.find( '?' );

	char szType[MAX_PATH];
	char szParam[MAX_PATH];

	INT index = 0;

	while( varMark != string::npos )
	{
		IterParam iterParam = m_Params.find( index );
		if( iterParam == m_Params.end() )
			return FALSE;

		PROPVARIANT& param = iterParam->second;
		 
		std::pair<IterIndex, IterIndex> result = std::equal_range( m_vecLob.begin(), m_vecLob.end(), index );
		BOOL bLob = ( result.first == result.second ) ? FALSE : TRUE;

		if( !GetOtlVarString( &param, szType, bLob ) )
			return FALSE;

		sprintf_s( szParam, MAX_PATH, ":f%d<%s>", index+1, szType );
		m_strSQL.replace( varMark, 1, szParam );

		// Store bind handler
		LPVOID pHandler = GetBindHandler( param.vt, bLob );
		if( !pHandler )
			return FALSE;

		m_bindHandlers[index] = pHandler;

		index++;
		varMark = (INT)m_strSQL.find( '?', varMark );
	}

	return TRUE;
}


typedef VOID (* LPFN_BIND)( otl_stream& stream, CPropVar* prop );

BOOL COtlRST::SetParams( VOID )
{
	IterParam iter = m_Params.begin();

	try
	{
		for( ; iter!=m_Params.end(); ++iter )
		{
			PROPVARIANT& param = iter->second;

			LPFN_BIND bindFn = (LPFN_BIND)m_bindHandlers[iter->first];
			if( !bindFn )
				return FALSE;

			bindFn( *m_pStream, (CPropVar*)&param );
		}
	}
	catch( otl_exception& e )
	{
		GrpMsg( MODULE_NAME, DEFAULT_LEV, _T("####数据库Adapter COtlRST::Open %s"), e.msg );
	}

	return TRUE;
}

HRESULT COtlRST::BindParams( ISrvProperty2* params )
{
	RASSERTP( params, E_INVALIDARG );

	DWORD dwIndex = 0;

	while( TRUE )
	{
		const PROPVARIANT* prop;
		if( FAILED(params->GetProperty( dwIndex, &prop )) )
			break;

		RFAILED( BindParam( dwIndex, (PROPVARIANT*)prop ) );
		dwIndex++;
	}

	return S_OK;
}