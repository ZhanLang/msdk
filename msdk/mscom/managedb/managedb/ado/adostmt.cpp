/********************************************************************
	created:	2009/09/05
	created:	5:9:2009   16:53
	filename: 	d:\C++Work\approduct2010\app\managedb\adostmt.cpp
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adostmt
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "adostmt.h"
#include "varcast.h"
#include "adorecordset.h"
#include "adoconn.h"
#include "../virtualconn.h"
//#include <rscom/prophelpers.h>
#include "adoconn.h"

CAdoStmt::CAdoStmt()
{
	m_numParam = 0;
	m_pVirConn = NULL;
	m_bBound = FALSE;
	m_pComm = NULL;
	m_pConn = NULL;
}

CAdoStmt::~CAdoStmt()
{
	if( m_bBound )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Reset, still bound.\n") );
		UnbindReadlConn();
	}
}

HRESULT CAdoStmt::Compile( _ConnectionPtr pConn, LPCTSTR szSQL )
{
	HRESULT hr = S_OK;

	try
	{
		m_pComm.CreateInstance( __uuidof(Command) );
		//m_pComm->ActiveConnection = pConn;

		_bstr_t bstr( szSQL );
		m_pComm->CommandText = bstr;
		m_pComm->CommandType = adCmdText;

		//m_pComm->PutPrepared(true);
	}
	catch( _com_error e )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Compile, sql=%s, error:%s\n"), szText, e.ErrorMessage() );
		hr = E_FAIL;
	}
	catch( ... )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Compile, sql=%s, error:Unknown.\n"), szText );
		hr = E_FAIL;
	}
	
	return hr;
}

HRESULT CAdoStmt::BindRealConn()
{
	if( m_bBound || !m_pVirConn )
		return S_OK;

	HRESULT hr = S_OK;

	for (int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			CAdoConn* pRealConn = m_pVirConn->GetConn();
			RASSERTP( pRealConn, E_FAIL );
			m_pConn = pRealConn;

			hr = SetRealConn( pRealConn );
			if( SUCCEEDED(hr) )
			{
				m_pComm->Parameters->Refresh();
				m_numParam = m_pComm->Parameters->Count;

				m_bBound = TRUE;
			}		

			break;
		}
		catch( _com_error e )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindRealConn, sql=%s, error: %s\n"), szText, e.ErrorMessage() );

			if( m_pConn->IsInTransaction() || FAILED( m_pConn->HandleError( e ) ) )
			{
				hr = e.Error();
				break;
			}
		}
		catch( ... )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindRealConn, sql=%s, error:Unknown.\n"), szText );
			hr = E_FAIL;
			break;
		}
	}

	

	if( FAILED(hr) )
	{
		UnbindReadlConn();
	}

	return hr;
}

VOID CAdoStmt::UnbindReadlConn( BOOL bFree/* =TRUE */ )
{
	m_pComm->ActiveConnection = NULL;

	if( bFree && m_pVirConn && m_pConn )
	{
		m_pVirConn->FreeConn( m_pConn );
	}

	m_pConn = NULL;
	m_bBound = FALSE;
}

HRESULT CAdoStmt::CutStringByConfig(DWORD dwLen,wstring& strValue)
{
	DWORD dwSize = dwLen;
	if(0 == dwSize)
		return S_OK;
	if(true == strValue.empty())
		return S_OK;


	if(strValue.length() <= dwSize)
		return S_OK;

	wstring strTempValue = strValue.substr(0,dwSize - 1 - 3);
	strValue = strTempValue;
	strValue += L"...";

	return S_OK;
}

HRESULT CAdoStmt::BindParam( INT nIndex, PROPVARIANT* value )
{
	HRESULT hr = S_OK;
	RFAILED( BindRealConn() );

	try
	{
		if( nIndex >= (INT)m_numParam )
			return E_INVALIDARG;

		VARIANT var;
		VariantInit( &var );

		PROPVARIANT& prop = *value;
		SAFEARRAY* pArray = NULL;

		switch( prop.vt )
		{
		case VT_LPSTR:
			{
				_bstr_t bstr( prop.pszVal );
				var.vt = VT_BSTR;
				var.bstrVal = bstr;

				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
			}
			break;
		case VT_LPWSTR:
			{
				LONG lSize = m_pComm->Parameters->GetItem((short)nIndex )->GetSize();
				wstring strTempVal = prop.pwszVal;
				CutStringByConfig(lSize,strTempVal);

				_bstr_t bstr( strTempVal.c_str());
				var.vt = VT_BSTR;
				var.bstrVal = bstr;

				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
			}
			break;
		case VT_CLSID:
			{
				SAFEARRAY* pArray = new SAFEARRAY;
				memset( pArray, 0, sizeof(SAFEARRAY) );

				pArray->cDims = 1;
				pArray->fFeatures = FADF_STATIC;
				pArray->cbElements = 1;
				pArray->pvData = prop.puuid;
				pArray->rgsabound[0].lLbound = 0;
				pArray->rgsabound[0].cElements = sizeof(GUID);

				var.vt = VT_ARRAY|VT_UI1;
				var.parray = pArray;

				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
				delete pArray;
			}	
			break;
		case VT_BUFFER:
			{
				SAFEARRAY* pArray = new SAFEARRAY;
				memset( pArray, 0, sizeof(SAFEARRAY) );

				pArray->cDims = 1;
				pArray->fFeatures = FADF_STATIC;
				pArray->cbElements = 1;
				pArray->pvData = prop.cac.pElems;
				pArray->rgsabound[0].lLbound = 0;
				pArray->rgsabound[0].cElements = prop.cac.cElems;

				var.vt = VT_ARRAY|VT_UI1;
				var.parray = pArray;

				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
				delete pArray;
			}
			break;
		case VT_DATETIME:
			{
				TCHAR szTime[128] = {0};
				ULONG wordNum = value->cac.cElems;

				if( wordNum == sizeof(SYSTEMTIME)/sizeof(WORD) )
				{
					SYSTEMTIME* pSystem = (SYSTEMTIME*)value->cac.pElems;

					_stprintf_s( szTime, _countof(szTime), _T("%04d-%02d-%02d %02d:%02d:%02d"),
						pSystem->wYear, pSystem->wMonth, pSystem->wDay,
						pSystem->wHour, pSystem->wMinute, pSystem->wSecond );

					_bstr_t bstr( szTime );
					var.vt = VT_BSTR;
					var.bstrVal = bstr;
				}

				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
			}
			break;
		default:
			{
				memcpy( &var, &prop, sizeof(VARIANT) );
				m_pComm->Parameters->GetItem( (short)nIndex )->Value = var;
			}
			break;
		}
	}
	catch( _com_error e )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindParam, sql=%s, error:%s\n"), szText, e.ErrorMessage() );
		
		m_pConn->HandleError( e );
		hr = e.Error();
	}
	catch( ... )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindParam, sql=%s, error:Unknown.\n"), szText );
		hr = E_FAIL;
	}

	if( FAILED(hr) )
		UnbindReadlConn();

	return hr;
}

HRESULT CAdoStmt::Reset( VOID )
{
	if( m_bBound )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Reset, still bound.\n") );
		UnbindReadlConn();
	}

	return S_OK;
}

HRESULT CAdoStmt::ExecuteDML( LPLONG pAffected )
{
	HRESULT hr = S_OK;


	for (int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			if( !m_pConn )
			{
				m_pConn = m_pVirConn->GetConn();
				RASSERTP( m_pConn, E_FAIL );
				SetRealConn( m_pConn );
			}

			_variant_t vtCount;
			m_pComm->Execute( &vtCount, NULL, adCmdText|adExecuteNoRecords );

			if( pAffected )
				*pAffected = vtCount;

			break;
		}
		catch( _com_error e )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::ExecuteDML, sql=%s, error:%s\n"), szText, e.ErrorMessage() );

			if( m_pConn->IsInTransaction() || FAILED( m_pConn->HandleError( e ) ) )
			{
				hr = e.Error();
				break;
			}
		}
		catch( ... )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::ExecuteDML, sql=%s, error:Unknown.\n"), szText );
			hr = E_FAIL;
			break;
		}
	}
	

	UnbindReadlConn();

	return hr;
}

HRESULT CAdoStmt::SetRealConn( CAdoConn* pConn )
{
	HRESULT hr = S_OK;

	try
	{
		if( m_pComm && pConn )
			m_pComm->ActiveConnection = pConn->GetConnPtr();
	}
	catch( _com_error e )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::SetRealConn, sql=%s, error:%s\n"), szText, e.ErrorMessage() );
		
		m_pConn->HandleError( e );
		hr = e.Error();
	}
	catch( ... )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::SetRealConn, sql=%s, error:Unknown.\n"), szText );
		hr = E_FAIL;
	}

	return hr;
}

HRESULT CAdoStmt::Execute( IGenericMS** ppRST, DWORD* pFields, INT nFields )
{
	HRESULT hr = S_OK;

	for (int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			if( !m_pConn )
			{
				m_pConn = m_pVirConn->GetConn();
				RASSERTP( m_pConn, E_FAIL );

				hr = SetRealConn( m_pConn );
				if( FAILED(hr) )
				{
					UnbindReadlConn();
					return hr;
				}
			}

			_variant_t vtCount;
			_RecordsetPtr _pRST = m_pComm->Execute( &vtCount, NULL, adCmdText );

			CAdoRecordset* pRst = new CAdoRecordset( m_pConn, _pRST );
			pRst->SetFieldsMap( pFields );
			pRst->SetVirtualConn( m_pVirConn );

			hr = pRst->QueryInterface( __uuidof(IGenericMS), (void**)ppRST );

			break;
		}
		catch( _com_error e )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Execute, sql=%s, error:%s\n"), szText, e.ErrorMessage() );

			if( m_pConn->IsInTransaction() || FAILED( m_pConn->HandleError( e ) ) )
			{
				hr = e.Error();
				break;
			}
		}
		catch( ... )
		{
			LPCTSTR szText = m_pComm->CommandText;
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::Execute, sql=%s, error:Unknown.\n"), szText );
			hr = E_FAIL;

			break;
		}
	}

	

	BOOL bFree = FAILED(hr);
	UnbindReadlConn( bFree );

	return hr;
}

HRESULT CAdoStmt::BindParams( IProperty2* params )
{
	RASSERTP( params, E_INVALIDARG );
	RFAILED( BindRealConn() );

	HRESULT hr = S_OK;

	try
	{
		for( DWORD i=0; i<m_numParam; i++ )
		{
			const PROPVARIANT* prop;
			if( SUCCEEDED(params->GetProperty( i, &prop )) )
			{
				RFAILED( BindParam( i, (PROPVARIANT*)prop ) );
				continue;
			}

			// Bind null
			VARIANT varNull = {0};
			varNull.vt = VT_NULL;
			m_pComm->Parameters->GetItem( (short)i )->Value = varNull;
		}
	}
	catch( _com_error e )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindParams, sql=%s, error:%s\n"), szText, e.ErrorMessage() );
		
		m_pConn->HandleError( e );
		hr = e.Error();
	}
	catch( ... )
	{
		LPCTSTR szText = m_pComm->CommandText;
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoStmt::BindParams, sql=%s, error:Unknown.\n"), szText );
		hr = E_FAIL;
	}

	return hr;
}