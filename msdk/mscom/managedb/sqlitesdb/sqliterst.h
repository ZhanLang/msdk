/********************************************************************
	created:	2009/09/08
	created:	8:9:2009   15:39
	filename: 	d:\C++Work\approduct2010\app\managedb\sqliterst.h
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	sqliterst
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once
#include "database\genericdb.h"

//#include "../../common/imanagedb.h"

using namespace std;

struct sqlite3;
struct sqlite3_stmt;
class CSqliteConn;

class CSqliteRST : public IGenericMS, IGenericStmt, CUnknownImp
{
public:
	CSqliteRST( CSqliteConn* pConn, sqlite3_stmt* pStmt, LPCTSTR szSQL=NULL, BOOL bOhterStmt = FALSE );
	~CSqliteRST();

	UNKNOWN_IMP2_( IGenericMS, IGenericStmt );

public:
	// IGenericMS members
	STDMETHOD( GetRecord )( IProperty2** ppRecord );
	STDMETHOD( GetRecord )( IPropertyStr** ppRecord );
	STDMETHOD( GetScale )( PROPVARIANT* pScale );
	STDMETHOD( MoveNext )( VOID );
	STDMETHOD( MoveFirst )( VOID );
	STDMETHOD_( BOOL, IsEOF )( VOID );
	STDMETHOD( SetLobMode )( VOID ){ return S_OK; }

	// IGenericStmt members
	STDMETHOD( BindParam )( INT nIndex, PROPVARIANT* value );
	STDMETHOD( Reset )( VOID );
	STDMETHOD( ExecuteDML )( LPLONG pAffected=NULL );
	STDMETHOD( Execute )( IGenericMS** ppRST, DWORD* pFields=NULL, INT nFields=0 );
	STDMETHOD( SetLobParam )( INT index ) { return S_OK; }
	STDMETHOD( BindParams )( IProperty2* params );

public:
	HRESULT GetField( int nField, CPropVar* pProp );
	LPCSTR	GetFieldName(int nField);
	VOID	SetFieldsMap( DWORD* pFields=NULL, INT nFields=0 );

private:
	CSqliteConn*	m_pParent;
	sqlite3*		m_pConn;
	sqlite3_stmt*	m_pStmt;
	BOOL			m_bEOF;
	INT				m_nCol;
	DWORD*			m_pFieldsMap;
	tstring			m_strSQL;
	BOOL			m_bOhterStmt;
};