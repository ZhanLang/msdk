/********************************************************************
	created:	2009/09/04
	created:	4:9:2009   17:00
	filename: 	d:\C++Work\approduct2010\app\managedb\adorecordset.cpp
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adorecordset
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "adorecordset.h"
#include "../propertyset.h"
#include "varcast.h"
#include "../virtualconn.h"
#include "adoconn.h"


/********************************************************************
	Constructor/Destructor
*********************************************************************/
CAdoRecordset::CAdoRecordset( CAdoConn* pConn, _RecordsetPtr pRecordset )
:m_pRecordset( pRecordset )
{
	m_pRealConn = pConn;
	m_pFieldsMap = NULL;
	m_nCol = pRecordset->Fields->Count;
	m_pVirConn = NULL;
}

CAdoRecordset::~CAdoRecordset()
{
	SAFEDELETEARRAY( m_pFieldsMap );

	if( m_pVirConn && m_pRealConn )
		m_pVirConn->FreeConn( m_pRealConn );
}

HRESULT CAdoRecordset::GetRecord( IProperty2** ppRecord )
{
	bool bEnd = m_pRecordset->EOFile?true:false;
	if( bEnd )
	{
		*ppRecord = NULL;
		return E_FAIL;
	}

	UTIL::com_ptr<IProperty2> record;
	RFAILED( CPropertySet::CreateProp( &record.m_p ) );

	for( int i=0; i<m_nCol; i++ )
	{
		CPropVar prop;

		_variant_t vValue = m_pRecordset->Fields->GetItem( (short)i )->GetValue();
		Var2Prop( vValue, prop );

		DWORD dwID;
		if( !m_pFieldsMap )
			dwID = i;
		else
			dwID = m_pFieldsMap[i];

		HRESULT hr = record->SetProperty( dwID, &prop );

		RFAILED( hr );
	}

	*ppRecord = record.m_p;
	record->AddRef();

	return S_OK;
}

HRESULT CAdoRecordset::GetScale( PROPVARIANT* pScale )
{
	bool bEnd = m_pRecordset->EOFile?true:false;
	if( bEnd )
	{
		pScale->vt = VT_NULL;
		return E_FAIL;
	}

	CPropVar var;
	RFAILED( GetField( 0, &var ) );

	*pScale = var;

	while( !IsEOF() )
		MoveNext();

	return S_OK;
}


HRESULT CAdoRecordset::MoveNext( VOID )
{
	HRESULT hr = S_OK;

	try
	{
		if( m_pRecordset->EOFile )
			return E_FAIL;

		if( m_vBookmark.vt != VT_EMPTY )
			m_pRecordset->Bookmark = m_vBookmark;

		RFAILED( m_pRecordset->MoveNext() );
	}
	catch( _com_error e )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("AdoRecordset::MoveNext, error:%s\n"), e.ErrorMessage() );
		m_pRealConn->HandleError( e );
		hr = e.Error();
	}
	catch( ... )
	{
		// // RSLOG( RSLOG_ALERT, "CAdoRecordset::MoveNext, Unknown exception" );
		hr = E_FAIL;
	}

	return hr;
}


HRESULT CAdoRecordset::MoveFirst( VOID )
{
	HRESULT hr = S_OK;

	try
	{
		RFAILED( m_pRecordset->MoveFirst() );
		m_vBookmark = m_pRecordset->Bookmark;
	}
	catch( _com_error e )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("AdoRecordset::MoveFirst, error:%s\n"), e.ErrorMessage() );
		m_pRealConn->HandleError( e );
		hr = e.Error();
	}
	catch( ... )
	{
		// // RSLOG( RSLOG_ALERT, "CAdoRecordset::MoveFirst, Unknown exception" );
		hr = E_FAIL;
	}

	return hr;
}

BOOL CAdoRecordset::IsEOF( VOID )
{
	//assert( m_pRecordset );

	_variant_t vResult = m_pRecordset->EOFile;
	return vResult;
}

HRESULT CAdoRecordset::GetField( int nField, CPropVar* pProp )
{
	RASSERT( !m_pRecordset->EOFile, E_UNEXPECTED );

	_variant_t vValue = m_pRecordset->Fields->GetItem( (short)nField )->GetValue();
	Var2Prop( vValue, *pProp );

	return S_OK;
}


VOID CAdoRecordset::SetFieldsMap( DWORD* pFields/* =NULL */ )
{
	RASSERTV( pFields );
	SAFEDELETEARRAY( m_pFieldsMap );

	m_pFieldsMap = new DWORD[m_nCol];
	memcpy( m_pFieldsMap, pFields, sizeof(DWORD)*m_nCol );
}