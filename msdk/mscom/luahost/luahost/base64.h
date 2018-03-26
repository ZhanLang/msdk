#ifndef	_BASE64_H__
#define	_BASE64_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 用于BASE64编码、解码的静态常量
const char	g_szBase64TAB[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const int	g_nBase64Mask[]= { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

//解密
std::vector<BYTE> Base64Decode(IN const char* lpszDecoding, IN int nDecodedSize)
{
	std::vector<BYTE> ret;
	int nIndex =0;
	int nDigit;
	int nDecode[ 256 ];
	int nSize;
	int nNumBits = 6;
	long lOutLength = 0;

	if( lpszDecoding == NULL )
		return ret;

	if( ( nSize = lstrlenA(lpszDecoding) ) == 0 )
		return ret;

	// Build Decode Table
	for( int i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for( int i=0; i < 64; i++ )
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
	int i;
	for( nIndex = 0, i = 0; nIndex < nSize; nIndex++ )
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
				if(nDecodedSize <= lOutLength)
					return ret;	//不能被赋值，返回失败

				nScratch = lBitsStorage >> (nBitsRemaining - 8);
				//strDecoded += (nScratch & 0xFF);
				ret.push_back(nScratch & 0xFF);
				lOutLength++;
				i++;
				nBitsRemaining -= 8;
			}
		}
	}	

	return ret;
}

//加密
std::string Base64Encode(IN byte* lpbytEncoding, IN long lInputLength)
{
	std::string ret;
	int nDigit;
	int nNumBits = 6;
	int nIndex = 0;

	if( lpbytEncoding == NULL )
		return ret;

	if( lInputLength == 0 )
		return ret;

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

		ret += g_szBase64TAB[nDigit];
		nRetLen++;
	}
	// Pad with '=' as per RFC 1521
	while( nRetLen % 4 != 0 )
	{
		ret += '=';
		nRetLen++;
	}

	return ret;
}

#endif	//_BASE64_H__