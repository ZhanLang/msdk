

#pragma once

#include <stdio.h>

LPSTR	MyW2A( LPCWSTR szW );
LPWSTR	MyA2W( LPCSTR szA );


#ifdef _UNICODE 
#define DeleteW(szW) delete[] szW;
#else
#define DeleteW(szW)
#endif


int EncodeUTF8( LPCSTR pSource, LPSTR* pDest );

int DecodeUTF8( LPCSTR pSource, LPSTR* pDest );

void EncodeFileUTF8( FILE* pFile );