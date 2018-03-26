
#pragma once
#include <string>
#include <vector>


/*
//编码
CBase64Encode Base64Encode;
Base64Encode.Encode((unsigned char*) L"/report/install?userid=maguojun" , sizeof(L"/report/install?userid=maguojun"));


//解码
CBase64Decode Base64Decode;
Base64Decode.Decode(Base64Encode.GetResult());
wchar_t* pResult = (wchar_t*)Base64Decode.GetResult();
*/

static char	g_szSafeBase64TAB[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static int	g_nSafeBase64Mask[]= { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

class CSafeBase64Encode
{
public:
	const char* GetResult()
	{
		return m_result.c_str();
	}

	int GetResultLength()
	{
		return m_result.size();
	}

	bool Encode(unsigned char* lpbytEncoding, int lInputLength)
	{
		int nDigit;
		int nNumBits = 6;
		int nIndex = 0;

		if( lpbytEncoding == NULL )
			return false;

		if( lInputLength == 0 )
			return false;

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
			nDigit = (int)(lScratch & g_nSafeBase64Mask[nNumBits]);
			nNumBits = nBits;
			if( nNumBits <=0 )
				break;

			m_result.push_back(g_szSafeBase64TAB[nDigit]);
			//pszEncoded[nRetLen] = g_szSafeBase64TAB[nDigit];
			nRetLen++;
		}
		// Pad with '=' as per RFC 1521
		while( nRetLen % 4 != 0 )
		{
			m_result.push_back('=');
			//pszEncoded[nRetLen] = '=';
			nRetLen++;
		}

		return true;
	}

private:
	std::string m_result;
};

class CSafeBase64Decode
{
public: 

	unsigned char* GetResult()
	{
		return &m_result[0];
	}

	int GetResultLength()
	{
		return m_result.size();
	}

	bool Decode(const char* lpszDecoding)
	{
		int nIndex =0;
		int nDigit;
		int nDecode[ 256 ];
		int nSize;
		int nNumBits = 6;
		long lOutLength = 0;

		if( lpszDecoding == NULL )
			return false;

		if( ( nSize = strlen(lpszDecoding) ) == 0 )
			return false;

		// Build Decode Table
		for( int i = 0; i < 256; i++ ) 
			nDecode[i] = -2; // Illegal digit
		for(int i=0; i < 64; i++ )
		{
			nDecode[ g_szSafeBase64TAB[ i ] ] = i;
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
					nScratch = lBitsStorage >> (nBitsRemaining - 8);
					//strDecoded += (nScratch & 0xFF);
					m_result.push_back(nScratch & 0xFF);
					//pbytDecoded[lOutLength] = (nScratch & 0xFF);
					lOutLength++;
					i++;
					nBitsRemaining -= 8;
				}
			}
		}	

		return true;
	}

private:
	std::vector<unsigned char> m_result;
};