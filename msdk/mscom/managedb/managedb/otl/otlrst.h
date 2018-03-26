/********************************************************************
	created:	2009/12/18
	created:	18:12:2009   17:07
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\otl\otlrst.h
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb\otl
	file base:	otlrst
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/
#pragma once

#include <database/genericdb.h>
#include "../../common/cdt.h"
#include <map>
#include <string>
#include <vector>

class otl_connect;
class otl_stream;
class otl_column_desc;
class IOtlFieldHandler;
class CPropVar;

typedef std::map<DWORD, IOtlFieldHandler*>	MapHandlers;
typedef MapHandlers::iterator				IterHandlers;


class COtlRST : public IGenericRS, IGenericStmt, CUnknownImp
{
public:

	class OtlHandlerTable : public CDisTable
	{
	public:
		OtlHandlerTable( VOID );

		DWORD GetHandlerType( DWORD otlType );
	};


public:
	COtlRST( otl_connect* pConn, LPCTSTR szSQL );
	virtual ~COtlRST();

	UNKNOWN_IMP2_( IGenericRS, IGenericStmt );

public:
	// IGenericRS members
	STDMETHOD( GetRecord )( ISrvProperty2** ppRecord );
	STDMETHOD( GetScale )( PROPVARIANT* pScale );
	STDMETHOD( MoveNext )( VOID );
	STDMETHOD( MoveFirst )( VOID );
	STDMETHOD_( BOOL, IsEOF )( VOID );
	STDMETHOD( SetLobMode )( VOID );

	// IGenericDB members
	STDMETHOD( BindParam )( INT nIndex, PROPVARIANT* value );
	STDMETHOD( Reset )( VOID );
	STDMETHOD( ExecuteDML )( LPLONG pAffected=NULL );
	STDMETHOD( Execute )( IGenericRS** ppRST, DWORD* pFields=NULL, INT nFields=0 );
	STDMETHOD( SetLobParam )( INT index );
	STDMETHOD( BindParams )( ISrvProperty2* params );

	HRESULT Open( VOID );
	BOOL	PrepareColumn( VOID );
	VOID	SetFieldsMap( DWORD* pFields=NULL, INT nFields=0 );

	HRESULT ParseStmt( VOID );

private:
	
	IOtlFieldHandler*	CreateFieldHandler( DWORD handlerType );

	BOOL	GetOtlVarString( const PROPVARIANT* prop, char* szVar, BOOL bLob=FALSE );
	LPVOID	GetBindHandler( VARTYPE vt, BOOL bLob=FALSE );

	BOOL	PrepareVars( VOID );
	BOOL	SetParams( VOID );

private:
	otl_connect*		m_pConn;
	otl_stream*			m_pStream;
	otl_column_desc*	m_pColDesc;
	std::string			m_strSQL;

	INT					m_nCol;
	DWORD*				m_pFieldsMap;	
	IOtlFieldHandler**	m_ppHandlers;
	MapHandlers			m_handlers;
	BOOL				m_bStreamMode;
	BOOL				m_bOpened;

	// Statement members
	std::vector<INT>	m_vecLob;
	BOOL				m_bExectuted;

	typedef std::vector<INT>::iterator	IterIndex;

	INT					m_nVars;
	std::map<DWORD, PROPVARIANT>	m_Params;
	typedef std::map<DWORD, PROPVARIANT>::iterator IterParam;

	LPVOID*				m_bindHandlers;

	static OtlHandlerTable	ms_otlTypes;
};