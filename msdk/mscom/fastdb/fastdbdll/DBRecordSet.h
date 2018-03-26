#pragma once
#include <database/genericdb.h>
#include "CNewDataBase.h"

class CDBRecordSet : public IGenericMS,public CUnknownImp
{
public:
	CDBRecordSet(void);

	CDBRecordSet(
		string&			strTable
		,LPCTSTR		szSql
		, CMyDataBase*   pDB
		);


	~CDBRecordSet(void);

	UNKNOWN_IMP1(IGenericMS);

public:
	HRESULT Init(
		string&			strTable
		,LPCTSTR		szSql
		,CMyDataBase*   pDB
		);
	

	STDMETHOD( GetRecord )( IProperty2** ppRecord ) ;


	STDMETHOD( GetScale )( PROPVARIANT* pScale ) ;

	STDMETHOD( MoveFirst )( VOID ) ;


	STDMETHOD( MoveNext )( VOID ) ;
	
	STDMETHOD_( BOOL, IsEOF )( VOID ) ;

private:

	HRESULT Uninit();
	BOOL GetOneRecord(IProperty2** ppRecord );
	BOOL MakeRecord(byte* base, dbFieldDescriptor* first,IProperty2** ppRecord);

private:
	CMyDBAnyCursor*	m_pCursor	;
	dbQuery			m_theQuery	;
	CMyDataBase*	m_pDB		;

	INT				m_nCurIndex;

	BOOL			m_bIsGetCount;
	INT 			m_nCount;

};
