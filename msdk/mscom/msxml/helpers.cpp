/********************************************************************
	created:	2009/11/20
	created:	20:11:2009   10:56
	filename: 	d:\C++Work\approduct2010\utm\src\pubcomm\rsxml3\helpers.cpp
	file path:	d:\C++Work\approduct2010\utm\src\pubcomm\rsxml3
	file base:	helpers
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "helpers.h"
#include <io.h>
//===============================================
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

int EncodeUTF8( LPCSTR pSource, LPSTR* pDest )
{
	if( !pSource || !pSource[0] ) 
		return 0;

	LPWSTR wBuffer = NULL;
	int len = MultiByteToWideChar( CP_ACP, 0, pSource, -1, NULL, 0 );
	wBuffer = new WCHAR[len];
	MultiByteToWideChar( CP_ACP, 0, pSource, -1, wBuffer, len );

	len = WideCharToMultiByte( CP_UTF8, 0, wBuffer, -1, NULL, 0, NULL, NULL );
	*pDest = new CHAR[len];
	len = WideCharToMultiByte( CP_UTF8, 0, wBuffer, -1, *pDest, len, NULL, NULL );

	delete[] wBuffer;
	return len;
}

int DecodeUTF8( LPCSTR pSource, LPSTR* pDest )
{
	if( !pSource || !pSource[0] ) 
		return 0;

	LPWSTR wBuffer = NULL;
	int len = MultiByteToWideChar( CP_UTF8, 0, pSource, -1, NULL, 0 );
	wBuffer = new WCHAR[len];
	MultiByteToWideChar( CP_UTF8, 0, pSource, -1, wBuffer, len );

	len = WideCharToMultiByte( CP_ACP, 0, wBuffer, -1, NULL, 0, NULL, NULL );
	*pDest = new CHAR[len];
	len = WideCharToMultiByte( CP_ACP, 0, wBuffer, -1, *pDest, len, NULL, NULL );

	delete[] wBuffer;
	return len;
}

void EncodeFileUTF8( FILE* pFile )
{
	fflush( pFile );
	fseek( pFile, 0, SEEK_SET );
	int fDesc = _fileno( pFile );
	int fileLen = _filelength( fDesc );	

	LPSTR buffer = new CHAR[fileLen+1];

	int nread = (int)fread( buffer, 1, fileLen, pFile );
	buffer[nread] = 0;

	LPSTR strEncode = NULL;
	int len = EncodeUTF8( buffer, &strEncode );

	// empty the file and write
	_chsize( fDesc, 0 );
	fseek( pFile, 0, SEEK_SET );
	fwrite( strEncode, 1, len-1, pFile );

	delete[] buffer;
	delete[] strEncode;
}