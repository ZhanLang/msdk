/********************************************************************
	created:	2009/09/04
	created:	4:9:2009   15:14
	filename: 	d:\C++Work\approduct2010\app\managedb\adoconn.h
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adoconn
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once
#include "msado15.tlh"
#include <database/genericdb.h>
#include <xstring>
//#include "adoint.h"


using namespace std;

class CRecordset;
class CAdoRecordset;
class CAdoStmt;

class CAdoConn : public IGenericDB, CUnknownImp
{
public:
	CAdoConn( IUnknown* pRot );
	CAdoConn( VOID );
	~CAdoConn( VOID );

	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		RASSERT(pRot, E_FAIL);
		m_pRot = pRot;
		return S_OK;
	}
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
	STDMETHOD(GetTableFieldInfo) ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType,DWORD& dwSize  );

	STDMETHOD_(BOOL, IsOpen)( VOID );

public:
	HRESULT	InnerExecute( LPCTSTR szSQL, CAdoRecordset** ppAdoRS, DWORD* pFields=NULL, INT nFields=0 );
	HRESULT InnerCompile( LPCTSTR szSQL, CAdoStmt** ppStmt );

	_ConnectionPtr GetConnPtr() { return m_pConn; }
	BOOL	IsInTransaction() { return m_bInTrans; }

	HRESULT HandleError( _com_error& e );

private:
	HRESULT CAdoConn::ResetService();
private:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;

public:
	_ConnectionPtr	m_pConn;
	tstring			m_strConn;
	LONG			m_lOptions;
	BOOL			m_bInTrans;
};