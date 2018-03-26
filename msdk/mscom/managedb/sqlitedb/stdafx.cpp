// stdafx.cpp : 只包括标准包含文件的源文件
// sqlitedb.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
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