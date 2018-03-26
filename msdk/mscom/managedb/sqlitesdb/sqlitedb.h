
#include <database/genericdb.h>
#include <sqlite/CppSQLite3.h>

class CSQliteDB : public IGenericDB
{
public:
	// IGenericDB members
	HRESULT Connect( tstring strConn );

	VOID	Close( VOID );

	HRESULT	CompileStmt( tstring strSQL, IGenericStmt** ppStmt );

	INT		ExecuteDML( tstring strSQL );

	HRESULT BegINTrans( VOID );
	HRESULT CommitTrans( VOID );
	HRESULT RollbackTrans( VOID );

public:
	sqlite3*	m_pConn;
};


class CSQLiteStmt : public IGenericStmt
{
public:
	// IGenericStmt members
	HRESULT BindParam( INT nOrdinal, INT nValue );
	HRESULT BindParam( INT nOrdinal, CHAR* szValue );
	HRESULT BindParam( INT nOrdinal, LPBYTE pValue, INT nLen );

	HRESULT	Reset( VOID );
	INT		ExecuteDML( VOID );
	HRESULT	GetResult( IGenericMS** ppRS );

public:
	sqlite3*		m_pConn;
	sqlite3_stmt*	m_pStmt;
};


class CSQLiteRS : public IGenericMS
{
public:
	// IGenericMS members
	INT GetFieldsNum( VOID );

	DB_Type GetFieldType( INT index );

	INT		GetIntField( INT nField, INT nNullValue=0 );
	CHAR*	GettstringField( INT nField, CHAR* szNullValue="" );
	LPBYTE	GetBlobField( INT nField, INT& nLen );

	BOOL	FieldIsNull( INT nField );
	VOID	MoveNext();
	BOOL	EOF();

public:
	sqlite3*		m_pConn;
	sqlite3_stmt*	m_pStmt;
};