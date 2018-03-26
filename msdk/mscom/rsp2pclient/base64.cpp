#include "stdafx.h"
#include "Base64.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// 用于BASE64编码、解码的静态常量
char	g_szBase64TAB[] = "ABCDEFGHIJKLMNOPQMSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int		g_nBase64Mask[]= { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

//////////////////////////////////////////////////////////////////////////////////
//	函数名：long Base64Decode(
//				IN const char* lpszDecoding,
//				OUT unsigned char* pbytDecoded ) 
//	用  途：BASE64解码
//	对全局变量的影响：无
//	参  数：
//		lpszDecoding : 待解码字符串
//		pbytDecoded   : 解码结果
//	返回值：long，解码后的长度
////////////////////////////////////////////////////////////////////////////////
long Base64Decode(IN const char* lpszDecoding, OUT unsigned char* pbytDecoded)
{
	int nIndex =0;
	int nDigit;
    int nDecode[ 256 ];
	int nSize;
	int nNumBits = 6;
	long lOutLength = 0;

	if( lpszDecoding == NULL )
		return 0;
	
	if( ( nSize = strlen(lpszDecoding) ) == 0 )
		return 0;

	// Build Decode Table
	for( int i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for(int i=0; i < 64; i++ )
	{
		nDecode[ g_szBase64TAB[ i ] ] = i;
		nDecode[ '=' ] = -1; 
    }

	// Clear the output buffer
	long lBitsStorage  =0;
	int nBitsRemaining = 0;
	int nScratch = 0;
	UCHAR c;
	
	// Decode the Input
	int i = 0;
	for( nIndex = 0; nIndex < nSize; nIndex++ )
	{
		c = lpszDecoding[ nIndex ];

		// 忽略所有不合法的字符
		if( c> 0x7F)
			continue;

		nDigit = nDecode[c];
		if( nDigit >= 0 ) 
		{
			lBitsStorage = (lBitsStorage << nNumBits) | (nDigit & 0x3F);
			nBitsRemaining += nNumBits;
			while( nBitsRemaining > 7 ) 
			{
				nScratch = lBitsStorage >> (nBitsRemaining - 8);
				//strDecoded += (nScratch & 0xFF);
				pbytDecoded[lOutLength] = (nScratch & 0xFF);
				lOutLength++;
				i++;
				nBitsRemaining -= 8;
			}
		}
    }	

	return lOutLength;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：long Base64Encode(
//				IN unsigned char* lpbytEncoding,
//				IN long lInputLength, 
//				OUT CString &strEncoded)
//	用  途：BASE64编码
//	对全局变量的影响：无
//	参  数：
//		lpbytEncoding : 待编码的内存串
//		lInputLength  : 待编码的内存串长度
//		strEncoded   : 编码结果
//	返回值：long，编码后的长度
////////////////////////////////////////////////////////////////////////////////
long Base64Encode(IN unsigned char* lpbytEncoding, IN long lInputLength, OUT char* pszEncoded)
{
	int nDigit;
	int nNumBits = 6;
	int nIndex = 0;

	if( lpbytEncoding == NULL )
		return 0;

	if( lInputLength == 0 )
		return 0;

	int nBitsRemaining = 0;
	long lBitsStorage	=0;
	long lScratch		=0;
	int nBits;
	UCHAR c;

	int nRetLen = 0;
	while( nNumBits > 0 )
	{
		while( ( nBitsRemaining < nNumBits ) &&  ( nIndex < lInputLength ) ) 
		{
			c = lpbytEncoding[ nIndex++ ];
			lBitsStorage <<= 8;
			lBitsStorage |= (c & 0xff);
			nBitsRemaining += 8;
		}
		if( nBitsRemaining < nNumBits ) 
		{
			lScratch = lBitsStorage << ( nNumBits - nBitsRemaining );
			nBits    = nBitsRemaining;
			nBitsRemaining = 0;
		}	 
		else 
		{
			lScratch = lBitsStorage >> ( nBitsRemaining - nNumBits );
			nBits	 = nNumBits;
			nBitsRemaining -= nNumBits;
		}
		nDigit = (int)(lScratch & g_nBase64Mask[nNumBits]);
		nNumBits = nBits;
		if( nNumBits <=0 )
			break;
		
		pszEncoded[nRetLen] = g_szBase64TAB[nDigit];
		nRetLen++;
	}
	// Pad with '=' as per RFC 1521
	while( nRetLen % 4 != 0 )
	{
		pszEncoded[nRetLen] = '=';
		nRetLen++;
	}

	return nRetLen;
}
