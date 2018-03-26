#pragma once
#include <database/genericdb.h>

#include "inc/fastdb.h"
#include "inc/exception.h"
#include "CNewDataBase.h"

#define MAX_PORT_COUNT 200

class CFastDBConn : public IGenericDB, CUnknownImp
{
public:
	CFastDBConn(void);
	CFastDBConn(IN LPCTSTR lpDBName,IN LPCTSTR lpTableName,IN INT iInitSize,IN INT iIndexSize,IN BOOL bAcceptLink,IN INT iPort = 0);
	~CFastDBConn(void);

	UNKNOWN_IMP1( IGenericDB );
	
	HRESULT Init(IN LPCTSTR lpDBName,IN LPCTSTR lpTableName,IN INT iInitSize,IN INT iIndexSize,IN BOOL bAcceptLink,IN INT iPort = 0);
	HRESULT Uninit();

public:

	STDMETHOD( Connect )( LPCTSTR szConn, LONG lOptions=0 );
	STDMETHOD( Close )( void ) ;

	STDMETHOD_( BOOL, IsTableExists )( LPCTSTR szTable );

	STDMETHOD( CompileStmt )( LPCTSTR szSQL, IGenericStmt** ppStmt );

	STDMETHOD( ExecuteDML )( LPCTSTR szSQL, LONG* pAffected=NULL );

	STDMETHOD( Execute )( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields=NULL, INT nFields=0 ) ;

	STDMETHOD( GetTypeDecl )( VARTYPE vt, LPTSTR szBuf, INT nBufLen ) ;


	STDMETHOD( BeginTrans )( VOID ) ;
	STDMETHOD( CommitTrans )( VOID ) ;
	STDMETHOD( RollbackTrans )( VOID ) ;


	STDMETHOD( GetTypeDecl )( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen ) ;

	STDMETHOD( GetAutoIDDecl )( LPTSTR szBuf, INT nBufLen ) ;


	STDMETHOD( AttachRealConn )( VOID ) ;


	STDMETHOD( DetachRealConn )( VOID ) ;


	STDMETHOD( GetTableFieldsCount )( LPCTSTR szTable,DWORD& dwCount ) ;


	STDMETHOD(GetTableFieldInfo) ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  ) ;

	//判断此连接是否可用。以方便重连
	STDMETHOD_(BOOL, IsOpen)( VOID ) ;

	//本地api接口.

	STDMETHOD( CreateServer )( IN BOOL bAcceptLink,IN INT iInitSize,IN INT iIndexSize,IN INT& iPort ) ;

	STDMETHOD( InsertIntoTable )( IN LPCTSTR szTable,VARIANT* pVarArray, DWORD dwArraySize) ;

	STDMETHOD( DeleteFromTable )( IN LPCTSTR szTable,IN LPCTSTR lpCondition) ;

	STDMETHOD( CreateNewTable )(IN LPCTSTR  szTable,IN tag_FieldInfo* pFieldsSet,IN DWORD dwSetSize);

	STDMETHOD(SelectFromTable)(IN LPCTSTR szTable,LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields=NULL, INT nFields=0); 

	STDMETHOD(UpdateTable)(IN LPCTSTR szTable,IN tag_FieldInfoValue* pFieldsValSet,IN LPCTSTR lpCondition = NULL); 


private:
	HRESULT DeleteFdb(IN LPCTSTR lpDBName);
	
	
	


	


private:

	wstring m_strDBName;	
	INT		m_iInitSize;
	INT		m_iPort;

private:
	CMyDBServer* m_pDBServer;
	

	CMyDataBase* m_pDB;
	//CDBSql*			m_pDB;

	Helper::CMyLockObj m_theLockSubSql;





};
