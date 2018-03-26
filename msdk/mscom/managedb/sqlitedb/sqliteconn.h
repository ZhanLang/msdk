/********************************************************************
	created:	2009/09/08
	created:	8:9:2009   13:14
	filename: 	d:\C++Work\approduct2010\app\managedb\sqliteconn.h
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	sqliteconn
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once
#include "database\genericdb.h"


#define SQLITE_BUSY_DELAY	1
#define SQLITE_BUSY_RETRY	4000

struct sqlite3;
struct sqlite3_stmt;

class CSqliteConn : public IGenericDB, CUnknownImp
{
public:
	CSqliteConn( VOID );
	~CSqliteConn();

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

	STDMETHOD( GetTypeDecl )( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen );
	STDMETHOD( GetAutoIDDecl )( LPTSTR szBuf, INT nBufLen );

	STDMETHOD( AttachRealConn )( VOID ){ return E_UNEXPECTED; }
	STDMETHOD( DetachRealConn )( VOID ){ return E_UNEXPECTED; }
	STDMETHOD( GetTableFieldsCount) ( LPCTSTR szTable,DWORD&  dwCount );
	STDMETHOD(GetTableFieldInfo) (LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  );

	STDMETHOD_(BOOL ,IsOpen)(){return TRUE;};

public:
	STDMETHOD( ExecuteScaler )( LPCTSTR szSQL, LONG& lResult );
	sqlite3* GetConn( VOID ) { return m_pConn; }
	BOOL IsInTrans( VOID ) { return m_bInTrans; }
	int RetryStep( sqlite3_stmt* pStmt, LONG* pAffected=NULL );

private:
	sqlite3_stmt*	Compile( LPCTSTR szSQL );

private:
	sqlite3*	m_pConn;
	BOOL		m_bInTrans;


public:
	//sqlite_safe m_sqlites;
};
