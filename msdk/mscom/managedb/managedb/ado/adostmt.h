/********************************************************************
	created:	2009/09/05
	created:	5:9:2009   16:27
	filename: 	d:\C++Work\approduct2010\app\managedb\adostmt.h
	file path:	d:\C++Work\approduct2010\app\managedb
	file base:	adostmt
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

#include <database/genericdb.h>
//#include "adoint.h"
#include "msado15.tlh"

using namespace std;

class CAdoConn;
class CVirtualConn;

class CAdoStmt : public IGenericStmt, CUnknownImp
{
public:
	CAdoStmt();
	virtual ~CAdoStmt();

	UNKNOWN_IMP1( IGenericStmt );

public:
	STDMETHOD( BindParam )( INT nIndex, PROPVARIANT* value );
	STDMETHOD( Reset )( VOID );
	STDMETHOD( ExecuteDML )( LPLONG pAffected=NULL );
	STDMETHOD( Execute )( IGenericMS** ppRST, DWORD* pFields=NULL, INT nFields=0 );
	STDMETHOD( SetLobParam )( INT index ) { return S_OK; }
	STDMETHOD( BindParams )( IProperty2* params );

public:
	HRESULT Compile( _ConnectionPtr pConn, LPCTSTR szSQL );
	VOID SetVirtualConn( CVirtualConn* pVirConn ) { m_pVirConn = pVirConn; }

	VOID SetSQL( LPCTSTR szSQL ) { m_strSQL = szSQL; }
	LPCTSTR GetSQL( VOID ) { return m_strSQL.c_str(); }

private:
	HRESULT BindRealConn( VOID );
	HRESULT SetRealConn( CAdoConn* pConn );
	VOID UnbindReadlConn( BOOL bFree=TRUE );

	HRESULT CutStringByConfig(DWORD dwLen,wstring& strValue);
	

public:
	_CommandPtr		m_pComm;
	tstring			m_strSQL;
	DWORD			m_numParam;
	CVirtualConn*	m_pVirConn;
	BOOL			m_bBound;		// 是否有绑定的参数
	CAdoConn*		m_pConn;		// 绑定到的实连接
};