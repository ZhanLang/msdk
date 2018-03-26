#pragma once
namespace msdk{;
namespace UTIL{;

#if !defined mputInt64 
#define mputInt64(memPtr,data) \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 56 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 48 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 40 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 32 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 24 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 16 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 8 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( data ) & 0xFF )
#endif


#if !defined mputLong
#define mputLong(memPtr,data) \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 24 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 16 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 8 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( data ) & 0xFF )
#endif

#if !defined mputWord
#define mputWord(memPtr,data) \
	*memPtr++ = ( unsigned char ) ( ( ( data ) >> 8 ) & 0xFF ), \
	*memPtr++ = ( unsigned char ) ( ( data ) & 0xFF )
#endif


#if !defined mputByte
#define mputByte(memPtr,data)	\
	*memPtr++ = ( unsigned char ) data
#endif


#if !defined mputBytes
#define mputBytes(memPtr,data,len)  \
	memcpy (memPtr,data,len); \
	memPtr += len;
#endif


#if !defined mgetInt64
#define mgetInt64(memPtr) 		\
	( memPtr += 8, ( ( unsigned __int64 ) memPtr[ -8 ] << 56 ) | ( ( unsigned __int64 ) memPtr[ -7 ] << 48 ) | \
	( ( unsigned __int64 ) memPtr[ -6 ] << 40 ) | ( ( unsigned __int64 ) memPtr[ -5 ] << 32 ) | \
	( ( unsigned __int64 ) memPtr[ -4 ] << 24 ) | ( ( unsigned __int64 ) memPtr[ -3 ] << 16 ) | \
	( ( unsigned __int64 ) memPtr[ -2 ] << 8 ) | ( unsigned __int64 ) memPtr[ -1 ] )
#endif


#if !defined mgetLong
#define mgetLong(memPtr) 		\
	( memPtr += 4, ( ( unsigned __int32 ) memPtr[ -4 ] << 24 ) | ( ( unsigned __int32 ) memPtr[ -3 ] << 16 ) | \
	( ( unsigned __int32 ) memPtr[ -2 ] << 8 ) | ( unsigned __int32 ) memPtr[ -1 ] )
#endif


#if !defined mgetWord
#define mgetWord(memPtr) 		\
	( memPtr += 2, ( unsigned short ) memPtr[ -2 ] << 8 ) | ( ( unsigned short ) memPtr[ -1 ] ) 
#endif


#if !defined mgetByte
#define mgetByte(memPtr)		\
	( ( unsigned char ) *memPtr++ )
#endif


inline unsigned __int16 MirrorBytes16 (unsigned __int16 x)
{
	return (x << 8) | (x >> 8);
}

inline unsigned __int32 MirrorBytes32 (unsigned __int32 x)
{
	unsigned __int32 n = (unsigned __int8) x;
	n <<= 8; n |= (unsigned __int8) (x >> 8);
	n <<= 8; n |= (unsigned __int8) (x >> 16);
	return (n << 8) | (unsigned __int8) (x >> 24);
}

inline unsigned __int64 MirrorBytes64 (unsigned __int64 x)
{
	unsigned __int64 n = (unsigned __int8) x;
	n <<= 8; n |= (unsigned __int8) (x >> 8);
	n <<= 8; n |= (unsigned __int8) (x >> 16);
	n <<= 8; n |= (unsigned __int8) (x >> 24);
	n <<= 8; n |= (unsigned __int8) (x >> 32);
	n <<= 8; n |= (unsigned __int8) (x >> 40);
	n <<= 8; n |= (unsigned __int8) (x >> 48);
	return (n << 8) | (unsigned __int8) (x >> 56);
}

#ifdef _WIN32
#define burn(mem,size) do { volatile char *burnm = (volatile char *)(mem); int burnc = size; RtlSecureZeroMemory (mem, size); while (burnc--) *burnm++ = 0; } while (0)
#else
#define burn(mem,size) do { volatile char *burnm = (volatile char *)(mem); int burnc = size; while (burnc--) *burnm++ = 0; } while (0)
#endif

};};