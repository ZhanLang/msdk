// stdafx.cpp : source file that includes just the standard includes
// managedb.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
//#include <mslog/syslay/dbg.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#define DEBUG_MESSAGE_BUFLEN_MAX 1024

LPSTR MyW2A( LPCWSTR szW )
{
	if( !szW || !szW[0] ) 
		return NULL;

	LPSTR szA = NULL;
	int len = WideCharToMultiByte( CP_ACP, 0, szW, -1, NULL, 0, NULL, NULL );
	szA = new CHAR[len];
	len = WideCharToMultiByte( CP_ACP, 0, szW, -1, szA, len, NULL, NULL );

	return szA;
}

LPWSTR MyA2W( LPCSTR szA )
{
	if( !szA || !szA[0] ) 
		return NULL;

	LPWSTR szW = NULL;
	int len = MultiByteToWideChar( CP_UTF8, 0, szA, -1, NULL, 0 );
	szW = new WCHAR[len];
	MultiByteToWideChar( CP_UTF8, 0, szA, -1, szW, len );

	return szW;
}

VOID ZM1_GrpMsg( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX];

	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[MSG][%-8s][%-2d] : "), pGroupName, level );

	va_list args;
	va_start( args, pMsg );

	_vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );

	//GrpMsg( pGroupName, level, pTempBuffer );
	//Msg(pTempBuffer);
	OutputDebugString( pTempBuffer );

	va_end( args );
}

VOID ZM1_GrpError( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX];

	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[ERR][%-8s][%-2d] : "), pGroupName, level );

	va_list args;
	va_start( args, pMsg );

	_vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );

	//GrpError( pGroupName, level, pTempBuffer );
	//Msg(pTempBuffer);
	OutputDebugString( pTempBuffer );

	va_end( args );
}

VOID ZM1_GrpWarn( TCHAR const *pGroupName, int level, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX];

	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[WAR][%-8s][%-2d] : "), pGroupName, level );

	va_list args;
	va_start( args, pMsg );

	_vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );

	//GrpWarning( pGroupName, level, pTempBuffer );
//	Msg(pTempBuffer);
	OutputDebugString( pTempBuffer );

	va_end( args );
}