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


class CSqliteConn : public IGenericDB, CUnknownImp
{
public:
	CSqliteConn( VOID );
	~CSqliteConn();

	UNKNOWN_IMP1( IGenericDB );

	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		RASSERT(pRot, E_FAIL);
		m_pRot = pRot;

		return S_OK;
	}

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


private:
	

	UTIL::com_ptr<IMscomRunningObjectTable>	m_pRot;
	UTIL::com_ptr<IGenericDB>				m_pDb;
};
