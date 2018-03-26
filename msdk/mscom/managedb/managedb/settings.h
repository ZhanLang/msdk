/********************************************************************
	created:	2009/11/19
	created:	19:11:2009   9:21
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\settings.h
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb
	file base:	settings
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

#include <managedb/genericdb.h>
#include <xml/ixmltree3.h>

using namespace std;

class CDBManager;

class CSetttings
{
public:
	CSetttings()
	{
		m_pManager = NULL;
	}

	struct _General
	{
		_General()
		{
			TCHAR path[MAX_PATH] = {0};
			GetModuleFileName(NULL,path,MAX_PATH);
			TCHAR *p = _tcsrchr(path,_T('\\'));
			*(p+1) = 0;

			_tcscpy_s( m_strModulePath, MAX_PATH, path );

		}
		TCHAR	m_strModulePath[MAX_PATH];
	} General;

	struct _Conn
	{
		_Conn()
		{
			dwDBType = DB_CLIENT;
			ulOption = 0;
		}

		MsDBType	dwDBType;
		tstring		strLocal;
		tstring		strCentor;
		ULONG		ulOption;
	} Conn;

	HRESULT Init( CDBManager* pManager );

	HRESULT Reload( IUnknown* pCfg, BOOL& bChanged );

private:
	CDBManager*		m_pManager;
};

extern CSetttings Settings;