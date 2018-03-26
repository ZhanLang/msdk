// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define  _WTL_NO_CSTRING		//不使用WTL的CString
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "atlres.h"
#include <mscom/mscominc.h>
using namespace mscom;



#define GroupName	_T("softeng")
#define MODULE_NAME GroupName
#include <mslog/msdkoutput.h>

#include "SuperEdit.h"
extern	CSuperEditMgr* g_pEditMgr;


class CPERegLoader
{
public:
	CPERegLoader(LPCTSTR szFilePath, LPCTSTR szName)
	{
		m_bLoadResult = FALSE;
		LONG lRet = RegLoadKey(HKEY_LOCAL_MACHINE, szName, szFilePath);
		if( ERROR_SUCCESS == lRet ) 
		{
			m_strName = szName;
			m_bLoadResult = TRUE;
		}
	}

	~CPERegLoader()
	{
		if( m_bLoadResult ) 
		{
			RegUnLoadKey(HKEY_LOCAL_MACHINE, m_strName);
		}
	}


	BOOL GetLoadStatus() {return m_bLoadResult;}
private:
	CString m_strName;
	BOOL m_bLoadResult;
};


