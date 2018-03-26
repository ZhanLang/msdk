/********************************************************************
	created:	2009/09/04
	created:	4:9:2009   15:30
	filename: 	d:\C++Work\approduct2010\app\managedb\adoconn.cpp
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adoconn
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "adoconn.h"
#include <tchar.h>
#include <iostream>
#include <sstream>
#include "adorecordset.h"
#include "adostmt.h"

//#include <mslog/syslay/dbg.h>

#undef Error

#define RECONNECT_DELAY		2000
#define RECONNECT_LIMIT		10

//struct ComEnv
//{
//	ComEnv()
//	{
//		/*if( FAILED(::CoInitialize(NULL) ) )
//		{
//			ZM1_GrpWarn( MODULE_NAME, 1, _T("CoInitialize failed\n") );
//		}*/
//	}
//
//	~ComEnv()
//	{
//		//::CoUninitialize();
//	}
//} g_ComEnv;


CAdoConn::CAdoConn( VOID )
{
	if( FAILED(::CoInitialize(NULL) ) )//这个有问题哦，线程相关的东西放到这里是有问题的！！！ onlyu
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CoInitialize failed\n") );
	}

	m_pConn = NULL;
	m_bInTrans = FALSE;
	m_strConn = _T("");
	m_lOptions = 0;
}

CAdoConn::CAdoConn( IUnknown* pRot )
{
	if( FAILED(::CoInitialize(NULL) ) )//这个有问题哦，线程相关的东西放到这里是有问题的！！！ onlyu
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CoInitialize failed\n") );
	}

	m_pConn = NULL;
	m_bInTrans = FALSE;
	m_strConn = _T("");
	m_lOptions = 0;
	
	m_pRot = pRot;

}

CAdoConn::~CAdoConn( VOID )
{
	::CoUninitialize();
}


HRESULT CAdoConn::Connect(LPCTSTR szConn, LONG lOptions )
{
	if( !szConn || _tcslen(szConn)==0 )
		return E_INVALIDARG;

	m_strConn = szConn;
	m_lOptions = lOptions;

	HRESULT hr = E_FAIL;

	try
	{
		hr = m_pConn.CreateInstance( __uuidof(Connection) );
		if(SUCCEEDED(hr))
		{
			m_pConn->ConnectionTimeout = 10;//10

			hr = m_pConn->Open( _bstr_t(szConn), _T(""), _T(""), lOptions );
		}
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Connect, error:%x, %s\n"), hr, e.ErrorMessage() );
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Connect, Unknown exception\n"));
	}

	return hr;
}

HRESULT CAdoConn::Close( VOID )
{
	try
	{
		if( IsOpen() )
		{
			m_pConn->Close();			
		}

		m_pConn = INULL;
	}
	catch( _com_error e )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Close, error:%x, %s\n"), e.Error(), e.ErrorMessage() );
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Close, Unknown exception\n"));
	}

	return S_OK;
}

BOOL CAdoConn::IsOpen()
{
	if(m_pConn == NULL)
		return FALSE;
	BOOL bOpen = FALSE;
	try
	{
		bOpen = m_pConn->State & adStateOpen;
	}
	catch( _com_error e )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::IsOpen, error:%x, %s\n"), e.Error(), e.ErrorMessage() );
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::IsOpen, Unknown exception\n"));
	}
	return bOpen;
}

BOOL CAdoConn::IsTableExists( LPCTSTR szTable )
{
	RASSERT( IsOpen(), E_FAIL );

	tostringstream oss;
	oss << _T("SELECT id FROM sysobjects WHERE name='") << szTable << _T("'");

	CAdoRecordset* pRS = NULL;
	RFAILEDP( InnerExecute( oss.str().c_str(), &pRS ), FALSE );
	RASSERTP( pRS, FALSE );

	BOOL bRet = !pRS->IsEOF();

	delete pRS;
	return bRet;
}

HRESULT CAdoConn::ExecuteDML( LPCTSTR szSQL, LONG* pAffected )
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	for( int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			_variant_t vAffected;
			hr = m_pConn->Execute( _bstr_t(szSQL), &vAffected, adExecuteNoRecords );

			if( pAffected )
				*pAffected = vAffected;

			break;
		}
		catch( _com_error e )
		{
			hr = e.Error();

			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::ExecuteDML, sql=%s, error:%x, %s\n"), szSQL, hr, e.ErrorMessage() );

			if( m_bInTrans || FAILED( HandleError( e ) ) )
			{
				break;
			}
		}
		catch( ... )
		{
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::ExecuteDML, Unknown exception\n"));
			break;
		}

		Sleep( SQL_RETRY_DELAY );
	}

	return hr;
}

HRESULT CAdoConn::Execute( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields, INT nFields )
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	for( int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			_variant_t vAffected;
			_RecordsetPtr pRST = m_pConn->Execute( _bstr_t(szSQL), &vAffected, adOptionUnspecified );

			CAdoRecordset* pOutRST = new CAdoRecordset( this, pRST );
			pOutRST->SetFieldsMap( pFields );

			hr = pOutRST->QueryInterface( __uuidof(IGenericMS), (void**)ppRecordset );

			break;
		}
		catch( _com_error e )
		{
			hr = e.Error();

			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Execute, sql=%s, error:%x, %s\n"), szSQL, hr, e.ErrorMessage() );

			if( m_bInTrans || FAILED( HandleError( e ) ) )
			{
				break;
			}
		}
		catch( ... )
		{
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Execute, Unknown exception\n"));			
			break;
		}

		Sleep( SQL_RETRY_DELAY );
	}

	return hr;
}

HRESULT CAdoConn::InnerExecute( LPCTSTR szSQL, CAdoRecordset** ppAdoRS, DWORD* pFields/* =NULL */, INT nFields/* =0 */ )
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	for( int i=0; i<SQL_RETRY_NUM; i++ )
	{
		try
		{
			_variant_t vAffected;
			_RecordsetPtr pRST = m_pConn->Execute( _bstr_t(szSQL), &vAffected, adOptionUnspecified );

			CAdoRecordset* pOutRST = new CAdoRecordset( this, pRST );
			pOutRST->SetFieldsMap( pFields );

			*ppAdoRS = pOutRST;
			hr = S_OK;
			break;
		}
		catch( _com_error e )
		{
			hr = e.Error();

			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::InnerExecute,%s,-- error:%x, %s\n"), szSQL, hr, e.ErrorMessage() );

			if( m_bInTrans || FAILED( HandleError( e ) ) )
			{
				break;
			}
		}
		catch( ... )
		{
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::InnerExecute, Unknown exception\n"));
			break;
		}


		Sleep( SQL_RETRY_DELAY );
	}

	return hr;
}

HRESULT CAdoConn::CompileStmt( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	RASSERT( IsOpen(), E_FAIL );

	CAdoStmt* pStmt = new CAdoStmt();

	HRESULT hr = E_FAIL;
	try
	{
		hr = pStmt->Compile( m_pConn, szSQL );
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::CompileStmt,%s,-- error:%x, %s\n"), szSQL, hr, e.ErrorMessage() );
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::CompileStmt, Unknown exception\n"));
	}

	if( FAILED(hr) )
	{
		delete pStmt;
		return hr;
	}

	return pStmt->QueryInterface( __uuidof(IGenericStmt), (void**)ppStmt );
}

HRESULT CAdoConn::InnerCompile( LPCTSTR szSQL, CAdoStmt** ppStmt )
{
	RASSERT( IsOpen(), E_FAIL );

	CAdoStmt* pStmt = new CAdoStmt();

	HRESULT hr = E_FAIL;
	try
	{
		hr = pStmt->Compile( m_pConn, szSQL );
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::InnerCompile,%s,-- error:%x, %s\n"), szSQL, hr, e.ErrorMessage() );
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::InnerCompile, Unknown exception\n"));
	}

	if( FAILED(hr) )
	{
		delete pStmt;
		return hr;
	}

	*ppStmt = pStmt;
	return S_OK;
}

HRESULT CAdoConn::GetTypeDecl( VARTYPE vt, LPTSTR szBuf, INT nBufLen )
{
	RASSERT( IsOpen(), E_FAIL );

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

HRESULT CAdoConn::BeginTrans()
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	if( m_bInTrans )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::BeginTrans, already in transaction\n") );
		return hr;
	}

	try
	{
		hr = m_pConn->BeginTrans();
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::BeginTrans, error:%x, %s\n"), hr, e.ErrorMessage() );
		
		HandleError( e );		
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::BeginTrans, Unknown exception\n"));
	}

	if( SUCCEEDED(hr) )
		m_bInTrans = TRUE;

	return hr;
}


HRESULT CAdoConn::ResetService()
{
	/*
	UTIL::com_ptr<IMscomRunningObjectTable> pRot = (IMscomRunningObjectTable*)m_pRot;
	if(NULL == pRot.m_p)
		return E_FAIL;
	//FAILEXIT_FAIL(pRot.m_p);

	ZM1_GrpError( MODULE_NAME, 1, _T(" CAdoConn::ResetService is called !\n") );
	using namespace rsdk;
	using namespace rscom;

	UTIL::com_ptr<IExit2> pExit;
	HRESULT hr = pRot->GetObject(CLSID_RscomEnv, __uuidof(IExit2), (IUnknown**)&pExit);
	if(SUCCEEDED(hr) && pExit)
	{
		ZM1_GrpError( MODULE_NAME, 1, _T(" CAdoConn::ResetService CLSID_RscomEnv->NotifyReset\n") );
		pExit->NotifyReset();
	}
	*/
	return E_NOTIMPL;

}


HRESULT CAdoConn::CommitTrans()
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	if( !m_bInTrans )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::CommitTrans, not in transaction\n") );
		return hr;
	}

	try
	{
		hr = m_pConn->CommitTrans();
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::CommitTrans, error:%x, %s\n"), hr, e.ErrorMessage() );
		
		HandleError( e );
				
		if(0x8000ffff == hr)//灾难性故障。重启服务。
			ResetService();

	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::CommitTrans, Unknown exception\n"));
	}

	if( SUCCEEDED(hr) )
		m_bInTrans = FALSE;

	return hr;
}

HRESULT CAdoConn::RollbackTrans()
{
	RASSERT( IsOpen(), E_FAIL );

	HRESULT hr = E_FAIL;

	if( !m_bInTrans )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::RollbackTrans, not in transaction\n") );
		return hr;
	}

	try
	{
		hr = m_pConn->RollbackTrans();
	}
	catch( _com_error e )
	{
		hr = e.Error();

		ZM1_GrpMsg( MODULE_NAME, 1, _T("CAdoConn::RollbackTrans, error:%x, %s\n"), hr, e.ErrorMessage() );
		
		HandleError( e );
		
	}
	catch( ... )
	{
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::RollbackTrans, Unknown exception\n"));
	}

	if( SUCCEEDED(hr) )
		m_bInTrans = FALSE;

	return hr;
}

HRESULT CAdoConn::GetTypeDecl( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen )
{
	RASSERT( IsOpen(), E_FAIL );

	LPTSTR szType = NULL;
	TCHAR szBuffer[64];

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
				_stprintf_s( szBuffer, dwSize, _T("NVARCHAR(%d)"), dwSize );
#else
				_stprintf_s( szBuffer, dwSize, _T("VARCHAR(%d)"), dwSize );
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
				_stprintf_s( szBuffer, dwSize, _T("VARBINARY(%d)"), dwSize );
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

HRESULT CAdoConn::GetAutoIDDecl( LPTSTR szBuf, INT nBufLen )
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

//HRESULT CAdoConn::HandleError( _com_error& e )
//{
//	// 如果错误原因为网络断开或者数据库服务停止，错误为E_FAIL
//	// 如果错误原因为SQL逻辑错误，错误为DB_E_...，在oledberr.h中定义
//	HRESULT hErr = e.Error();
//	if( hErr == E_FAIL )
//	{
//		DWORD dwCounter = 0;
//		for (;;)
//		{
//			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn reconnect.\n") );
//
//			BOOL bConnected = FALSE;
//			try
//			{
//				//m_pConn->Close();
//				m_pConn = NULL;
//				m_pConn.CreateInstance( __uuidof(Connection) );
//	
//				m_pConn->Open( _bstr_t(m_strConn.c_str()), "", "", 0 );
//
//				bConnected = TRUE;
//			}
//			catch( ... )
//			{
//			}
//			
//			if( bConnected )
//				return S_OK;
//
//			Sleep( RECONNECT_DELAY );
//
//			if( RECONNECT_LIMIT!=0 && dwCounter++ > RECONNECT_LIMIT )
//				return E_FAIL;
//		}
//	}
//
//	return hErr;
//}

HRESULT CAdoConn::HandleError( _com_error& e )
{
	// 如果错误原因为网络断开或者数据库服务停止，错误为E_FAIL
	// 如果错误原因为SQL逻辑错误，错误为DB_E_...，在oledberr.h中定义
	HRESULT hr = e.Error();
	if( hr == E_FAIL )
	{		
		ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn reconnect.\n") );

		try
		{
			//m_pConn->Close();
			m_pConn = INULL;
			hr = m_pConn.CreateInstance( __uuidof(Connection) );
			if(SUCCEEDED(hr))
			{
				hr = m_pConn->Open( _bstr_t(m_strConn.c_str()), _T(""), _T(""), m_lOptions);
			}
		}
		catch( _com_error e )
		{
			hr = e.Error();

			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Connect, error:%x, %s\n"), hr, e.ErrorMessage() );
		}
		catch( ... )
		{
			ZM1_GrpWarn( MODULE_NAME, 1, _T("CAdoConn::Connect, Unknown exception\n"));
		}
	}

	return hr;
}

HRESULT CAdoConn::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount )
{
	RASSERT( IsOpen(), E_FAIL );

	tostringstream oss;
	oss << _T("SELECT Count(*) FROM SYSCOLUMNS WHERE ID=OBJECT_ID('") << szTable << _T("')");

	CAdoRecordset* pRS = NULL;
	RFAILEDP( InnerExecute( oss.str().c_str(), &pRS ), E_FAIL );
	RASSERTP( pRS, E_FAIL );

	if (!pRS->IsEOF())
	{
		CPropVar Procount;
		HRESULT hr = pRS->GetScale(&Procount);
		RFAILEDP(hr,E_FAIL);
		dwCount = Procount;
	}
	else
		dwCount = 0;

	delete pRS;

	if (dwCount == 0)
      return E_FAIL;

	return S_OK;

}

HRESULT CAdoConn::GetTableFieldInfo ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  )
{
	RASSERT( IsOpen(), E_FAIL );

	if(index <= 0 || FieldName == NULL || FieldType == NULL)
		return E_FAIL;

	LPTSTR sFieldName = NULL;
	LPTSTR sFieldType = NULL;
	INT nNameLen = 50;
	INT nTypeLen = 50;
	tostringstream oss;
	oss << _T("SELECT  SysColumns.Name as ColumnsName,SysTypes.Name as DateType,SysColumns.Length as DateLength ") ;
	oss << _T("FROM  SysObjects,SysTypes,SysColumns  ") ;
	oss << _T("WHERE  (Sysobjects.Xtype ='u'  OR  Sysobjects.Xtype     ='v')  ") ;
	oss << _T("AND   Sysobjects.Id =  Syscolumns.Id  ") ;
	oss << _T("AND   SysTypes.XType  =  Syscolumns.XType  ") ;
	oss << _T("AND   SysTypes.Name  <>  'sysname'  ") ;
	oss << _T("AND   SysObjects.name  = '") ;
	oss << szTable << _T("' ");
    oss << _T("AND   SysColumns.colid  = '") ;
	oss <<index<<_T("'");

	CAdoRecordset* pRS = NULL;
	RFAILEDP( InnerExecute( oss.str().c_str(), &pRS ),E_FAIL );
	RASSERTP( pRS, E_FAIL );
	CPropSet props;
	//CPropVar prop;

	if (!pRS->IsEOF())
	{
		UTIL::com_ptr<IProperty2> record;
		HRESULT hr = pRS->GetRecord( &record.m_p );
		RFAILEDP(hr,E_FAIL);

		props=record ;
		//prop = props[1].Val();
		sFieldName = props[0];
		//prop = props[2].Val();
		sFieldType = props[1];
		dwSize = props[2];

		_tcsncpy_s( FieldName, nNameLen, sFieldName, nNameLen );
		_tcsncpy_s( FieldType, nTypeLen, sFieldType, nTypeLen );
	}
	else
	{
		delete pRS;
		return E_FAIL;
	}

	delete pRS;
	return S_OK;
}