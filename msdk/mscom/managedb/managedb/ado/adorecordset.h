/********************************************************************
	created:	2009/09/04
	created:	4:9:2009   16:44
	filename: 	d:\C++Work\approduct2010\app\managedb\adorecordset.h
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adorecordset
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once
//#include "../common/imanagedb.h"
//#include "adoint.h"
#include "msado15.tlh"
#include "database\genericdb.h"

class CAdoConn;
class CVirtualConn;

class CAdoRecordset : public IGenericMS, CUnknownImp
{
public:
	CAdoRecordset( CAdoConn* pConn, _RecordsetPtr pRecordset );
	~CAdoRecordset();

	UNKNOWN_IMP1( IGenericMS );

public:
	// IGenericMS members
	STDMETHOD( GetRecord )( IProperty2** ppRecord );		// Êä³öIProperty2£¨Ö»¶Á£©
	STDMETHOD( GetScale )( PROPVARIANT* pScale );
	STDMETHOD( MoveNext )( VOID );
	STDMETHOD( MoveFirst )( VOID ) ;
	STDMETHOD_( BOOL, IsEOF )( VOID );
	STDMETHOD( SetLobMode )( VOID ){ return S_OK; }

public:
	HRESULT GetField( int nField, CPropVar* pProp );
	VOID	SetFieldsMap( DWORD* pFields=NULL );
	VOID	SetVirtualConn( CVirtualConn* pVirConn ) { m_pVirConn = pVirConn; }

public:
	_RecordsetPtr		m_pRecordset;
	_variant_t			m_vBookmark;
	INT					m_nCol;
	DWORD*				m_pFieldsMap;
	CAdoConn*			m_pRealConn;
	CVirtualConn*		m_pVirConn;
};