
#pragma once

#include <database/genericdb.h>

using namespace mscom;
class CConnPool;
class CONN_IMPL;

class CVirtualConn : public IGenericDB, CUnknownImp
{
public:
	CVirtualConn( CConnPool* pPool );
	~CVirtualConn();

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

	STDMETHOD( AttachRealConn )( VOID );
	STDMETHOD( DetachRealConn )( VOID );
	STDMETHOD( GetTableFieldsCount) ( LPCTSTR szTable,DWORD& dwCount );
	STDMETHOD(GetTableFieldInfo) (LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize );

	STDMETHOD_( BOOL, IsOpen )(){return TRUE;};

public:
	CONN_IMPL* GetConn();
	VOID FreeConn( CONN_IMPL* pConn );

private:
	CConnPool*		m_pPool;
	CONN_IMPL*		m_pConn;	
	BOOL			m_bInTrans;		// 是否在事务处理中
	BOOL			m_bAttached;	// 是否绑定到了物理数据库连接
};

class CAutoConn
{
public:
	CAutoConn( CVirtualConn* pVirConn, CONN_IMPL* pConn=NULL )
	{
		m_pVirConn = pVirConn;
		m_pConn = pConn;

		if( m_pConn )
			return;

		if( pVirConn )
			m_pConn = pVirConn->GetConn();
	}

	~CAutoConn()
	{
		if( m_pVirConn && m_pConn )
			m_pVirConn->FreeConn( m_pConn );
	}

	BOOL IsValid() 
	{ 
		return m_pConn!=NULL; 
	}

	VOID Detach() 
	{
		m_pVirConn = NULL;
		m_pConn = NULL;
	}

	operator CONN_IMPL*()
	{
		return m_pConn;
	}

	CONN_IMPL* operator->()
	{
		return m_pConn;
	}

private:
	CVirtualConn*	m_pVirConn;
	CONN_IMPL*		m_pConn;
};