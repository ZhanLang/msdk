/********************************************************************
	created:	2009/12/17
	created:	17:12:2009   16:45
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\otlconn.h
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb
	file base:	otlconn
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

#include <managedb/genericdb.h>

class otl_connect;
class otl_stream;

class COtlConn :  public IGenericDB, CUnknownImp
{
public:
	COtlConn();
	~COtlConn() {}

	UNKNOWN_IMP1( IGenericDB );

public:
	// IGenericDB members
	STDMETHOD( Connect )( LPCTSTR szConn, LONG lOptions=0 );
	STDMETHOD( Close )( void );
	STDMETHOD_( BOOL, IsTableExists )( LPCTSTR szTable );
	STDMETHOD( CompileStmt )( LPCTSTR szSQL, IGenericStmt** ppStmt );
	STDMETHOD( ExecuteDML )( LPCTSTR szSQL, LONG* pAffected=NULL );
	STDMETHOD( Execute )( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields=NULL, INT nFields=0 );
	STDMETHOD( GetTypeDecl )( VARTYPE vt, LPTSTR szBuf, INT nBufLen );

	STDMETHOD( BeginTrans )( VOID );
	STDMETHOD( CommitTrans )( VOID );
	STDMETHOD( RollbackTrans )( VOID );

	STDMETHOD( GetTypeDecl )( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen ) { return S_OK; }
	STDMETHOD( GetAutoIDDecl )( LPTSTR szBuf, INT nBufLen ) { return S_OK; }

private:
	BOOL		IsConnected( VOID );

public:
	otl_connect*	m_pConn;
};