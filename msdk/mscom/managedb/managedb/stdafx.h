// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WTypes.h>

#ifndef MS_DEFINE_IID
#define MS_DEFINE_IID(iface, uuid_tstring)	struct __declspec(uuid(uuid_tstring)) iface
#endif

#ifndef re_uuidof
#define re_uuidof(iface)	__uuidof(iface)
#endif


// TODO: reference additional headers your program requires here
//#include <Unknwn.h>
//#include <publib/rsnew.h>

#include "mscom\mscominc.h"


#include <map>
#include "mscom\tproperty.h"
#include "mscom\prophelpers.h"

using namespace mscom;

#include <assert.h>
#include <string>
#include <tchar.h>

#define _DEBUG_INTERLOCKED_VIEW
//#define  RAVDONOTHASDEBUGLOG

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif//_countof

#ifndef SAFEDELETE
#define SAFEDELETE(X)			if(X){ delete X; X = NULL; }
#define SAFEDELETEARRAY(X)		if(X){ delete[] X; X = NULL; }
#define SAFEDELETEOBJECT(X)		if(X){ 	::DeleteObject(X);  X = NULL; }
#define SAFERELEASE(X)			if(X){ 	X->Release();  X = NULL; }
#define SAFECLOSEHANDLE(X)		if(X){ 	::CloseHandle(X);  X = NULL; }
#define SAFEFREELIBRARY(X)		if(X){ 	::FreeLibrary(X);  X = NULL; }

#define SAFEDELETETYPE(TYPE, X)			if(X){ delete (TYPE)X; X = NULL; }
#define SAFEDELETEARRAYTYPE(TYPE,X)		if(X){ delete[] (TYPE)X; X = NULL; }
#define SAFERELEASETYPE(TYPE, X)		if(X){ 	((TYPE)X)->Release();  X = NULL; }
#define SAFEDELETEOBJECTTYPE(TYPE, X)	if(X){ 	::DeleteObject((TYPE)X);  X = NULL; }
#endif

#ifndef tstring
#ifdef _UNICODE
#define tstring std::wstring
#else	tstring std::string
#endif
#endif

#ifndef tostringstream
#ifdef _UNICODE
#define tostringstream wostringstream
#else	tostringstream ostringstream
#endif
#endif

LPSTR MyW2A( LPCWSTR szW );
LPWSTR MyA2W( LPCSTR szA );

#define MODULE_NAME L"managedb"
#define DEFAULT_LEV	2

// 服务器数据库接口实现定义
#define CONN_IMPL		CAdoConn
#define CONN_IMPL_PTR	CAdoConn*
#define RS_IMPL			CAdoRecordset
#define STMT_IMPL		CAdoStmt


VOID ZM1_GrpMsg( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... );
VOID ZM1_GrpError( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... );
VOID ZM1_GrpWarn( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... );


#define SQL_RETRY_NUM		5
#define SQL_RETRY_DELAY		1000