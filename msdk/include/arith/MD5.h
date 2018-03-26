#pragma once

//Magic initialization constants
#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89
#define MD5_INIT_STATE_2 0x98badcfe
#define MD5_INIT_STATE_3 0x10325476

//Constants for Transform routine.
#define MD5_S11  7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21  5
#define MD5_S22  9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31  4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41  6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

//Transformation Constants - Round 1
#define MD5_T01  0xd76aa478 //Transformation Constant 1 
#define MD5_T02  0xe8c7b756 // 2
#define MD5_T03  0x242070db // 3
#define MD5_T04  0xc1bdceee // 4
#define MD5_T05  0xf57c0faf // 5
#define MD5_T06  0x4787c62a // 6
#define MD5_T07  0xa8304613 // 7
#define MD5_T08  0xfd469501 // 8
#define MD5_T09  0x698098d8 // 9
#define MD5_T10  0x8b44f7af // 10
#define MD5_T11  0xffff5bb1 // 11
#define MD5_T12  0x895cd7be // 12
#define MD5_T13  0x6b901122 // 13
#define MD5_T14  0xfd987193 // 14
#define MD5_T15  0xa679438e // 15
#define MD5_T16  0x49b40821 // 16

//s - Round 2
#define MD5_T17  0xf61e2562 // 17
#define MD5_T18  0xc040b340 // 18
#define MD5_T19  0x265e5a51 // 19
#define MD5_T20  0xe9b6c7aa // 20
#define MD5_T21  0xd62f105d // 21
#define MD5_T22  0x02441453 // 22
#define MD5_T23  0xd8a1e681 // 23
#define MD5_T24  0xe7d3fbc8 // 24
#define MD5_T25  0x21e1cde6 // 25
#define MD5_T26  0xc33707d6 // 26
#define MD5_T27  0xf4d50d87 // 27
#define MD5_T28  0x455a14ed // 28
#define MD5_T29  0xa9e3e905 // 29
#define MD5_T30  0xfcefa3f8 // 30
#define MD5_T31  0x676f02d9 // 31
#define MD5_T32  0x8d2a4c8a // 32

//s - Round 3
#define MD5_T33  0xfffa3942 // 33
#define MD5_T34  0x8771f681 // 34
#define MD5_T35  0x6d9d6122 // 35
#define MD5_T36  0xfde5380c // 36
#define MD5_T37  0xa4beea44 // 37
#define MD5_T38  0x4bdecfa9 // 38
#define MD5_T39  0xf6bb4b60 // 39
#define MD5_T40  0xbebfbc70 // 40
#define MD5_T41  0x289b7ec6 // 41
#define MD5_T42  0xeaa127fa // 42
#define MD5_T43  0xd4ef3085 // 43
#define MD5_T44  0x04881d05 // 44
#define MD5_T45  0xd9d4d039 // 45
#define MD5_T46  0xe6db99e5 // 46
#define MD5_T47  0x1fa27cf8 // 47
#define MD5_T48  0xc4ac5665 // 48

//s - Round 4
#define MD5_T49  0xf4292244 // 49
#define MD5_T50  0x432aff97 // 50
#define MD5_T51  0xab9423a7 // 51
#define MD5_T52  0xfc93a039 // 52
#define MD5_T53  0x655b59c3 // 53
#define MD5_T54  0x8f0ccc92 // 54
#define MD5_T55  0xffeff47d // 55
#define MD5_T56  0x85845dd1 // 56
#define MD5_T57  0x6fa87e4f // 57
#define MD5_T58  0xfe2ce6e0 // 58
#define MD5_T59  0xa3014314 // 59
#define MD5_T60  0x4e0811a1 // 60
#define MD5_T61  0xf7537e82 // 61
#define MD5_T62  0xbd3af235 // 62
#define MD5_T63  0x2ad7d2bb // 63
#define MD5_T64  0xeb86d391 // 64

namespace msdk{;
namespace arith{;
//Null data (except for first BYTE) used to finalise the checksum calculation
static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

class CMD5  
{
public:
	//interface functions for the RSA MD5 calculation	
	static VOID GetMD5(BYTE* pBuf, UINT nLength, char* pOutBuf)
	{
		CMD5 MD5_VC;
		MD5_VC.Update( pBuf, nLength );
		return MD5_VC.FinalByte(pOutBuf);
	}
	

protected:
	//constructor/destructor
	CMD5()
	{
			memset( m_lpszBuffer, 0, 64 );
			m_nCount[0] = m_nCount[1] = 0;
	
			// Load magic state initialization constants
			m_lMD5[0] = MD5_INIT_STATE_0;
			m_lMD5[1] = MD5_INIT_STATE_1;
			m_lMD5[2] = MD5_INIT_STATE_2;
			m_lMD5[3] = MD5_INIT_STATE_3;	
	}
	virtual ~CMD5() {};
	
	//RSA MD5 implementation
	void Transform(BYTE Block[64])
	{
    //initialise local data with current checksum
		ULONG a = m_lMD5[0];
		ULONG b = m_lMD5[1];
		ULONG c = m_lMD5[2];
		ULONG d = m_lMD5[3];
		
		//copy BYTES from input 'Block' to an array of ULONGS 'X'
		ULONG X[16];
		ByteToDWord( X, Block, 64 );
		
		//Perform Round 1 of the transformation
		FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01); 
		FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02); 
		FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03); 
		FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04); 
		FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05); 
		FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06); 
		FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07); 
		FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08); 
		FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09); 
		FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10); 
		FF (c, d, a, b, X[10], MD5_S13, MD5_T11); 
		FF (b, c, d, a, X[11], MD5_S14, MD5_T12); 
		FF (a, b, c, d, X[12], MD5_S11, MD5_T13); 
		FF (d, a, b, c, X[13], MD5_S12, MD5_T14); 
		FF (c, d, a, b, X[14], MD5_S13, MD5_T15); 
		FF (b, c, d, a, X[15], MD5_S14, MD5_T16); 
		
		//Perform Round 2 of the transformation
		GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17); 
		GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18); 
		GG (c, d, a, b, X[11], MD5_S23, MD5_T19); 
		GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20); 
		GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21); 
		GG (d, a, b, c, X[10], MD5_S22, MD5_T22); 
		GG (c, d, a, b, X[15], MD5_S23, MD5_T23); 
		GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24); 
		GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25); 
		GG (d, a, b, c, X[14], MD5_S22, MD5_T26); 
		GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27); 
		GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28); 
		GG (a, b, c, d, X[13], MD5_S21, MD5_T29); 
		GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30); 
		GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31); 
		GG (b, c, d, a, X[12], MD5_S24, MD5_T32); 
		
		//Perform Round 3 of the transformation
		HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33); 
		HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34); 
		HH (c, d, a, b, X[11], MD5_S33, MD5_T35); 
		HH (b, c, d, a, X[14], MD5_S34, MD5_T36); 
		HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37); 
		HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38); 
		HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39); 
		HH (b, c, d, a, X[10], MD5_S34, MD5_T40); 
		HH (a, b, c, d, X[13], MD5_S31, MD5_T41); 
		HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42); 
		HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43); 
		HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44); 
		HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45); 
		HH (d, a, b, c, X[12], MD5_S32, MD5_T46); 
		HH (c, d, a, b, X[15], MD5_S33, MD5_T47); 
		HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48); 
		
		//Perform Round 4 of the transformation
		II (a, b, c, d, X[ 0], MD5_S41, MD5_T49); 
		II (d, a, b, c, X[ 7], MD5_S42, MD5_T50); 
		II (c, d, a, b, X[14], MD5_S43, MD5_T51); 
		II (b, c, d, a, X[ 5], MD5_S44, MD5_T52); 
		II (a, b, c, d, X[12], MD5_S41, MD5_T53); 
		II (d, a, b, c, X[ 3], MD5_S42, MD5_T54); 
		II (c, d, a, b, X[10], MD5_S43, MD5_T55); 
		II (b, c, d, a, X[ 1], MD5_S44, MD5_T56); 
		II (a, b, c, d, X[ 8], MD5_S41, MD5_T57); 
		II (d, a, b, c, X[15], MD5_S42, MD5_T58); 
		II (c, d, a, b, X[ 6], MD5_S43, MD5_T59); 
		II (b, c, d, a, X[13], MD5_S44, MD5_T60); 
		II (a, b, c, d, X[ 4], MD5_S41, MD5_T61); 
		II (d, a, b, c, X[11], MD5_S42, MD5_T62); 
		II (c, d, a, b, X[ 2], MD5_S43, MD5_T63); 
		II (b, c, d, a, X[ 9], MD5_S44, MD5_T64); 
		
		//add the transformed values to the current checksum
		m_lMD5[0] += a;
		m_lMD5[1] += b;
		m_lMD5[2] += c;
		m_lMD5[3] += d;
	}
	
	void Update(BYTE* Input, ULONG nInputLen)
	{
      //Compute number of bytes mod 64
			UINT nIndex = (UINT)((m_nCount[0] >> 3) & 0x3F);
			
			//Update number of bits
			if ( ( m_nCount[0] += nInputLen << 3 )  <  ( nInputLen << 3) )
			{
				m_nCount[1]++;
			}
			m_nCount[1] += (nInputLen >> 29);
			
			//Transform as many times as possible.
			UINT i=0;  
			UINT nPartLen = 64 - nIndex;
			if (nInputLen >= nPartLen)  
			{
				memcpy( &m_lpszBuffer[nIndex], Input, nPartLen );
				Transform( m_lpszBuffer );
				for (i = nPartLen; i + 63 < nInputLen; i += 64) 
				{
					Transform( &Input[i] );
				}
				nIndex = 0;
			} 
			else 
			{
				i = 0;
			}
			
			// Buffer remaining input
			memcpy( &m_lpszBuffer[nIndex], &Input[i], nInputLen-i);	
	}
// 	
// 	CString Final()
// 	{
// 		//Save number of bits
// 		BYTE Bits[8];
// 		DWordToByte( Bits, m_nCount, 8 );
// 
// 		//Pad out to 56 mod 64.
// 		UINT nIndex = (UINT)((m_nCount[0] >> 3) & 0x3f);
// 		UINT nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
// 		Update( PADDING, nPadLen );
// 
// 		//Append length (before padding)
// 		Update( Bits, 8 );
// 
// 		//Store final state in 'lpszMD5'
// 		const int nMD5Size = 16;
// 		unsigned char lpszMD5[ nMD5Size ];
// 		DWordToByte( lpszMD5, m_lMD5, nMD5Size );
// 
// 		//Convert the hexadecimal checksum to a CString
// 		CString strMD5;
// 		for ( int i=0; i < nMD5Size; i++) 
// 		{
// 			CString Str;
// 			if (lpszMD5[i] == 0) {
// 				Str = CString("00");
// 			}
// 			else if (lpszMD5[i] <= 15)  {
// 				Str.Format(_T("0%X"),lpszMD5[i]);
// 			}
// 			else {
// 				Str.Format(_T("%X"),lpszMD5[i]);
// 			}
// 
// 			ATLASSERT( Str.GetLength() == 2 );
// 			strMD5 += Str;
// 		}
// 		ATLASSERT( strMD5.GetLength() == 32 );
// 		return strMD5;	
// 	}

	VOID FinalByte(char *pOutBuf)
	{
		//Save number of bits
		BYTE Bits[8];
		DWordToByte( Bits, m_nCount, 8 );

		//Pad out to 56 mod 64.
		UINT nIndex = (UINT)((m_nCount[0] >> 3) & 0x3f);
		UINT nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
		Update( PADDING, nPadLen );

		//Append length (before padding)
		Update( Bits, 8 );

		//Store final state in 'lpszMD5'
		const int nMD5Size = 16;
		unsigned char lpszMD5[ nMD5Size ];
		DWordToByte( lpszMD5, m_lMD5, nMD5Size );

		//Convert the hexadecimal checksum to a CString

		for ( int i=0; i < nMD5Size; i++) 
		{
			char szMd5[5] = {0};
			if (lpszMD5[i] == 0) {
				strcpy(szMd5,"00");
			}
			else if (lpszMD5[i] <= 15)  {
				sprintf(szMd5,"0%X",lpszMD5[i]);
			}
			else {
				sprintf(szMd5,"%X",lpszMD5[i]);
			}
			strcat(pOutBuf,szMd5);
		}
		return ;	
	}

	inline DWORD RotateLeft(DWORD x, int n)
	{
		//rotate and return x
		return (x << n) | (x >> (32-n));				
	}

	inline void FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
	{
		DWORD F = (B & C) | (~B & D);
		A += F + X + T;
		A = RotateLeft(A, S);
		A += B;
	}

	inline void GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
	{
		DWORD G = (B & D) | (C & ~D);
		A += G + X + T;
		A = RotateLeft(A, S);
		A += B;
	}

	inline void HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
	{
		DWORD H = (B ^ C ^ D);
		A += H + X + T;
		A = RotateLeft(A, S);
		A += B;		
	}

	inline void II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
	{
		DWORD I = (C ^ (B | ~D));
		A += I + X + T;
		A = RotateLeft(A, S);
		A += B;
	}

	//utility functions
	void DWordToByte(BYTE* Output, DWORD* Input, UINT nLength)
	{
		//transfer the data by shifting and copying
		UINT i = 0;
		UINT j = 0;
		for ( ; j < nLength; i++, j += 4) 
		{
			Output[j] =   (UCHAR)(Input[i] & 0xff);
			Output[j+1] = (UCHAR)((Input[i] >> 8) & 0xff);
			Output[j+2] = (UCHAR)((Input[i] >> 16) & 0xff);
			Output[j+3] = (UCHAR)((Input[i] >> 24) & 0xff);
		}   
	}

	void ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength)
	{

		//init
		UINT i=0; //index to Output array
		UINT j=0; //index to Input array

		//transfer the data by shifting and copying
		for ( ; j < nLength; i++, j += 4)
		{
			Output[i] = (ULONG)Input[j]   | 
				(ULONG)Input[j+1] << 8 | 
				(ULONG)Input[j+2] << 16 | 
				(ULONG)Input[j+3] << 24;
		}		
	}
	
private:
	BYTE  m_lpszBuffer[64];  //input buffer
	ULONG m_nCount[2];   //number of bits, modulo 2^64 (lsb first)
	ULONG m_lMD5[4];   //MD5 checksum
	
	
};
};};