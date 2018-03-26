#pragma once

#include "windows.h"
#include "mscom\mscominc.h"
#include "mscom\prophelpers.h"

using namespace msdk;
using namespace mscom;
class CBasicStream
{
public:
	enum SeekType
	{
		ST_CURRENT,
		ST_BEGIN,
		ST_END,
	};

// Stream Flag bits defines
static const ULONG	STREAM_IO_COPY	= 1;		// not implemented

// StreamStatus bits defines
static const ULONG STREAM_ERROR		= 1;

static const DWORD END_MARK	= 0xAAAAFFFF;

public:
	CBasicStream( VOID )
	{
		Init();
	}

	CBasicStream( IUnknown* pBuffer )
		:m_pBuffer( pBuffer )
	{
		Init();
	}

	VOID Init()
	{
		m_dwPos = 0;
		m_ulFlags = 0;
		m_ulStatus = 0;
	}

	IMsBuffer* GetBuffer( VOID ) { return m_pBuffer; }

	VOID SetBuffer( IUnknown* pBuffer )
	{
		if( m_pBuffer )
			m_pBuffer->Release();

		m_pBuffer = pBuffer;
		m_dwPos = 0;
	}

public:
	// Read & Write based on current position
	// Current position is updated
	//
	HRESULT Read( LPVOID pData, DWORD dwSize, DWORD& dwRead )
	{
		if( m_ulStatus&STREAM_ERROR )
		{
			return E_FAIL;
		}

		RASSERTP( m_pBuffer.m_p, E_UNEXPECTED );
		
		RFAILED( m_pBuffer->GetBuffer( (LPBYTE)pData, dwSize, &dwRead, m_dwPos ) );
		m_dwPos += dwRead;

		return S_OK;
	}

	HRESULT Write( LPCVOID pData, DWORD dwSize )
	{
		if( m_ulStatus&STREAM_ERROR )
		{
			return E_FAIL;
		}

		RASSERTP( m_pBuffer.m_p, E_UNEXPECTED );
		if( dwSize == 0 )
			return S_OK;

		RFAILED( m_pBuffer->SetBuffer( (LPBYTE)pData, dwSize, m_dwPos ) );

		m_dwPos += dwSize;
		return S_OK;
	}

	// Seek functions
	HRESULT Tellp( LPBYTE* ppPos )
	{
		RASSERTP( m_pBuffer.m_p, E_UNEXPECTED );

		LPBYTE pStart;
		DWORD dwSize;
		RFAILED( m_pBuffer->GetRawBuffer(&pStart, &dwSize)==S_OK );
		
		if( m_dwPos < dwSize )
		{
			*ppPos = pStart + m_dwPos;
			return S_OK;
		}
		else if( m_dwPos == dwSize )
		{
			*ppPos = NULL;
			return S_OK;
		}

		*ppPos = NULL;
		return E_UNEXPECTED;
	}

	HRESULT Tellp( LPBYTE* ppPos, DWORD& dwDataLen )
	{
		RASSERTP( m_pBuffer.m_p, E_UNEXPECTED );

		LPBYTE pStart;
		DWORD dwSize;
		RFAILED( m_pBuffer->GetRawBuffer(&pStart, &dwSize)==S_OK );

		if( m_dwPos < dwSize )
		{
			*ppPos = pStart + m_dwPos;
			dwDataLen = dwSize - m_dwPos;
			return S_OK;
		}
		else if( m_dwPos == dwSize )
		{
			*ppPos = NULL;
			dwDataLen = 0;
			return S_OK;
		}

		*ppPos = NULL;
		dwDataLen = 0;
		return E_UNEXPECTED;
	}

	DWORD Tellp( VOID ) { return m_dwPos; }

	VOID Seekp( int nOffset, SeekType type=ST_CURRENT )
	{
		RASSERTPV( m_pBuffer.m_p );

		DWORD dwEnd = m_pBuffer->GetBufferSize();
		int nTempPos;

		if( type == ST_CURRENT )
			nTempPos = (int)m_dwPos + nOffset;
		else if( type == ST_BEGIN )
			nTempPos = nOffset;
		else if( type == ST_END )
			nTempPos = (int)dwEnd + nOffset;
		else
			nTempPos = (int)m_dwPos;

		if( nTempPos > (int)dwEnd )
			nTempPos = dwEnd;
		else if( nTempPos < 0 )
			nTempPos = 0;

		m_dwPos = nTempPos;
	}

	VOID ResetPos( VOID )
	{
		m_dwPos = 0;
	}

	DWORD GetUnreadSize( VOID )
	{
		RASSERTP( m_pBuffer.m_p, 0 );

		if( m_dwPos < m_pBuffer->GetBufferSize() )
			return m_pBuffer->GetBufferSize() - m_dwPos;
		else
			return 0;
	}

	BOOL End( VOID )
	{
		RASSERTP( m_pBuffer.m_p, TRUE );

		return m_dwPos >= m_pBuffer->GetBufferSize();
	}

	// Access internal flags
	ULONG GetIOFlags( VOID ) { return m_ulFlags; }

	ULONG SetIOFlags( ULONG ulFlag )
	{
		ULONG temp = m_ulFlags;
		m_ulFlags |= ulFlag;

		return temp;
	}

	ULONG UnsetIOFlags( ULONG ulFlag )
	{
		ULONG temp = m_ulFlags;
		m_ulFlags &= ~ulFlag;

		return temp;
	}

	// Access status
	ULONG GetStatus( VOID ) { return m_ulStatus; }

	ULONG SetStatus( ULONG ulStatus )
	{
		ULONG temp = m_ulStatus;
		m_ulStatus |= ulStatus;

		return temp;
	}

	ULONG UnsetIOStatus( ULONG ulStatus )
	{
		ULONG temp = m_ulStatus;
		m_ulStatus &= ~ulStatus;

		return temp;
	}

protected:
	UTIL::com_ptr<IMsBuffer>	m_pBuffer;
	DWORD						m_dwPos;
	ULONG						m_ulFlags;
	ULONG						m_ulStatus;
};


//--------------------------------------------------------------------
//	PROPVARIANT serialize templates
//--------------------------------------------------------------------
// dummy implement
//
template<class T>
struct prop_buffer
{
	T var;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<T>* __this = (prop_buffer<T>*)pThis;
		RFAILED( pStream->Write( &__this->var, sizeof(T) ) );
		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream ) 
	{
		prop_buffer<T>* __this = (prop_buffer<T>*)pThis;

		DWORD dwRead;
		RFAILED( pStream->Read( &__this->var, sizeof(T), dwRead ) );

		RASSERT( dwRead==sizeof(T), E_UNEXPECTED );
		return S_OK;
	}
};

// Specialization for zero terminated string
//
template<>
struct prop_buffer<LPSTR>
{
	LPSTR pszVal;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<LPSTR>* __this = (prop_buffer<LPSTR>*)pThis;
		if( !__this->pszVal )
		{
			size_t zero = 0;
			return pStream->Write( &zero, sizeof(size_t) );
		}

		size_t nLen = strlen(__this->pszVal) + 1;
		RFAILED( pStream->Write( &nLen, sizeof(size_t) ) );

		RFAILED( pStream->Write( __this->pszVal, (DWORD)nLen ) );
		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream )
	{
		size_t nLen;
		DWORD dwRead;
		HRESULT hr = pStream->Read( &nLen, sizeof(size_t), dwRead );
		RASSERT( hr==S_OK && dwRead==sizeof(size_t), E_UNEXPECTED );

		prop_buffer<LPSTR>* __this = (prop_buffer<LPSTR>*)pThis;

		__this->pszVal = NULL;
		if( nLen == 0 )
			return S_OK;

		__this->pszVal = new CHAR[nLen];
		RASSERT( __this->pszVal, E_OUTOFMEMORY );

		hr = pStream->Read( __this->pszVal, (DWORD)nLen, dwRead );
		RASSERT( hr==S_OK && dwRead==nLen, E_UNEXPECTED );
		

		return S_OK;
	}
};

template<>
struct prop_buffer<LPCSTR> : public prop_buffer<LPSTR>
{
};

template<>
struct prop_buffer<LPWSTR>
{
	LPWSTR pwszval;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<LPWSTR>* __this = (prop_buffer<LPWSTR>*)pThis;
		if( !__this->pwszval )
		{
			size_t zero = 0;
			return pStream->Write( &zero, sizeof(size_t) );
		}

		size_t nLen = wcslen(__this->pwszval)+1;
		RFAILED( pStream->Write( &nLen, sizeof(size_t) ) );

		RFAILED( pStream->Write( __this->pwszval, (DWORD)nLen*sizeof(WCHAR)) );
		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream )
	{
		size_t nLen;
		DWORD dwRead;
		HRESULT hr = pStream->Read( &nLen, sizeof(size_t), dwRead );
		RASSERT( hr==S_OK && dwRead==sizeof(size_t), E_UNEXPECTED );

		prop_buffer<LPWSTR>* __this = (prop_buffer<LPWSTR>*)pThis;

		__this->pwszval = NULL;
		if( nLen == 0 )
			return S_OK;

		__this->pwszval = new WCHAR[nLen];
		RASSERT( __this->pwszval, E_OUTOFMEMORY );

		hr = pStream->Read( __this->pwszval, (DWORD)nLen*sizeof(WCHAR), dwRead );
		RASSERT( hr==S_OK && dwRead==nLen*sizeof(WCHAR), E_UNEXPECTED );

		return S_OK;
	}
};

template<>
struct prop_buffer<LPCWSTR> : public prop_buffer<LPWSTR>
{
};

// Specialization for CLSID*
//
template<>
struct prop_buffer<CLSID*>
{
	CLSID* puuid;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<CLSID*>* __this = (prop_buffer<CLSID*>*)pThis;
		RFAILED( pStream->Write( __this->puuid, sizeof(CLSID)) );
		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<CLSID*>* __this = (prop_buffer<CLSID*>*)pThis;

		__this->puuid = NULL;
		__this->puuid = new CLSID;
		RASSERT( __this->puuid, E_UNEXPECTED );

		DWORD dwRead;
		HRESULT hr = pStream->Read( __this->puuid, sizeof(CLSID), dwRead );
		RASSERT( hr==S_OK && dwRead==sizeof(CLSID), E_UNEXPECTED );


		return S_OK;
	}
};

// Specialization for IUnknown
// Only IMsBuffer is expected
//
template<>
struct prop_buffer<IUnknown*>
{
	IUnknown* punkval;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<IUnknown*>* __this = (prop_buffer<IUnknown*>* )pThis;
		UTIL::com_ptr<IMsBuffer> pBuffer( __this->punkval );
		RASSERT( pBuffer, E_NOTIMPL );

		DWORD dwSize;
		LPBYTE pData;
		RFAILED( pBuffer->GetRawBuffer( &pData, &dwSize ) );
		RFAILED( pStream->Write( &dwSize, sizeof(DWORD ) ) );
		RFAILED( pStream->Write( pData, dwSize ) );

		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer<IUnknown*>* __this = (prop_buffer<IUnknown*>* )pThis;
		UTIL::com_ptr<IMsBuffer> pBuffer( __this->punkval );
		RASSERT( pBuffer, E_NOTIMPL );

		DWORD dwSize;
		DWORD dwRead;
		HRESULT hr = pStream->Read( &dwSize, sizeof(DWORD), dwRead );
		RASSERT( hr==S_OK && dwRead==sizeof(DWORD), E_UNEXPECTED );

		LPBYTE pData;
		RFAILED( pStream->Tellp(&pData) );
		RFAILED( pBuffer->SetBuffer( pData, dwSize ) );
		pStream->Seekp( dwSize );

		return S_OK;
	}
};

//--------------------------------------------------------------------
// type traits for variant vector
//
struct stream_prop_true {};
struct stream_prop_false {};

template<class T>
struct stream_prop_traits
{
	typedef stream_prop_true	prop_trival_serial;
	typedef stream_prop_true	prop_trival_unserial;
};

#define STREAM_PROP_NONTRIVAL_TRAITS(type)	\
	template<> struct stream_prop_traits<type>	\
{	\
	typedef stream_prop_false	prop_trival_serial;	\
	typedef stream_prop_false	prop_trival_unserial;	\
}

STREAM_PROP_NONTRIVAL_TRAITS(LPSTR);
STREAM_PROP_NONTRIVAL_TRAITS(LPWSTR);
STREAM_PROP_NONTRIVAL_TRAITS(IUnknown*);
STREAM_PROP_NONTRIVAL_TRAITS(CLSID*);

//--------------------------------------------------------------------
// variant vector implement
//
template<class T>
struct prop_buffer_vector
{
	ULONG cElems;
	T *pElems;
	static HRESULT serialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer_vector<T>* __this = (prop_buffer_vector<T>*)pThis;
		RFAILED( pStream->Write( &__this->cElems, sizeof(ULONG) ) );
		RFAILED( __this->__serialize_elements( pStream, stream_prop_traits<T>::prop_trival_serial() ) );

		return S_OK;
	}
	static HRESULT unserialize( void* pThis, CBasicStream* pStream )
	{
		prop_buffer_vector<T>* __this = (prop_buffer_vector<T>*)pThis;
		DWORD dwRead;
		HRESULT hr = pStream->Read( &__this->cElems, sizeof(ULONG), dwRead );
		RASSERT( hr==S_OK && dwRead==sizeof(ULONG), E_UNEXPECTED );

		if( __this->cElems ==0 )
			return S_OK;

		hr = __this->__unserialize_elements( pStream, stream_prop_traits<T>::prop_trival_unserial() );
		if( hr!=S_OK )
		{
			delete[] __this->pElems;
			__this->cElems = 0;
		}
		return hr;
	}

private:
	HRESULT __serialize_elements( CBasicStream* pStream, stream_prop_true )
	{
		RFAILED( pStream->Write( pElems, cElems*sizeof(T) ) );
		return S_OK;
	}
	HRESULT __serialize_elements( CBasicStream* pStream, stream_prop_false )
	{
		for( ULONG index=0; index<cElems; ++index )
		{
			RFAILED( prop_buffer<T>::serialize( pElems[index], pStream ) );
		}
		return S_OK;
	}

	HRESULT __unserialize_elements( CBasicStream* pStream, stream_prop_true )
	{
		RASSERTP( (pElems = new T[cElems]), E_OUTOFMEMORY );
		DWORD dwRead;
		HRESULT hr = pStream->Read( pElems, cElems*sizeof(T), dwRead );
		RASSERT( hr==S_OK && dwRead==cElems*sizeof(T), E_UNEXPECTED );

		return S_OK;
	}
	HRESULT __unserialize_elements( CBasicStream* pStream, stream_prop_false )
	{
		RASSERTP( (pElems = new T[cElems]), E_OUTOFMEMORY );
		for( ULONG index=0; index<cElems; ++index )
		{
			RFAILED( prop_buffer<T>::unserialize( pElems[index], pStream ) );
		}
		return S_OK;
	}
};

//--------------------------------------------------------------------
// variant entry table
// 
typedef HRESULT (*PFNSerialize)(void *pThis, CBasicStream* pStream);
typedef HRESULT (*PFNUnserialize)(void *pThis, CBasicStream* pStream);

struct PropBufEntryTable
{
	PFNSerialize	pfn_serialize;
	PFNUnserialize	pfn_unserialize;
	PFNSerialize	pfn_vector_serialize;
	PFNUnserialize	pfn_vector_unserialize;
};


#define PROPBUF_ENTRY_EMPTY	\
{ (PFNSerialize)0, (PFNUnserialize)0, (PFNSerialize)0, (PFNUnserialize)0 }

#define PROPBUF_ENTRY(type)	\
{	\
	&prop_buffer<type>::serialize, &prop_buffer<type>::unserialize,	\
	&prop_buffer_vector<type>::serialize, &prop_buffer_vector<type>::unserialize,	\
}

#define PROPBUF_ENTRY_ATOM_AS_PTR(type)	\
{	\
	&prop_buffer<type*>::serialize, &prop_buffer<type*>::unserialize,	\
	&prop_buffer_vector<type>::serialize, &prop_buffer_vector<type>::unserialize,	\
}

extern PropBufEntryTable g_tblPropBufEntry[];

const DWORD CUSTOME_TYPE		=	0xFFFFAAAA;
const DWORD CUSTOME_VECTORTYPE	=	0xAAAAFFFF;

#define CHECK_STREAM(hr) { HRESULT _h_r_=(hr); \
	if(FAILED(_h_r_)) {SetStatus(STREAM_ERROR); return _h_r_;} }

#define RCHECK_STREAM(hr) { CHECK_STREAM(hr) return S_OK; }
#define ASSERT_STREAM(x, hr) { if(!(x)){SetStatus(STREAM_ERROR); return hr;} }


static PropBufEntryTable g_tblPropBufEntry[] =
{
	// 	VT_EMPTY	= 0,	*  VT_EMPTY            [V]   [P]     nothing
	PROPBUF_ENTRY_EMPTY,
	// 	VT_NULL	= 1,	*  VT_NULL             [V]   [P]     SQL style Null
	PROPBUF_ENTRY_EMPTY,
	// 	VT_I2	= 2,	*  VT_I2               [V][T][P][S]  2 byte signed int
	PROPBUF_ENTRY(SHORT),
	// 	VT_I4	= 3,	*  VT_I4               [V][T][P][S]  4 byte signed int
	PROPBUF_ENTRY(LONG),
	// 	VT_R4	= 4,	*  VT_R4               [V][T][P][S]  4 byte real
	PROPBUF_ENTRY(FLOAT),
	// 	VT_R8	= 5,	*  VT_R8               [V][T][P][S]  8 byte real
	PROPBUF_ENTRY(DOUBLE),
	// 	VT_CY	= 6,	*  VT_CY               [V][T][P][S]  currency
	PROPBUF_ENTRY_EMPTY,
	// 	VT_DATE	= 7,	*  VT_DATE             [V][T][P][S]  date
	PROPBUF_ENTRY_EMPTY,
	// 	VT_BSTR	= 8,	*  VT_BSTR             [V][T][P][S]  OLE Automation string
	PROPBUF_ENTRY_EMPTY,
	// 	VT_DISPATCH	= 9,	*  VT_DISPATCH         [V][T]   [S]  IDispatch *
	PROPBUF_ENTRY_EMPTY,
	// 	VT_ERROR	= 10,	*  VT_ERROR            [V][T][P][S]  SCODE
	PROPBUF_ENTRY(SCODE),
	// 	VT_BOOL	= 11,	*  VT_BOOL             [V][T][P][S]  True=-1, False=0
	PROPBUF_ENTRY(VARIANT_BOOL),
	// 	VT_VARIANT	= 12,	*  VT_VARIANT          [V][T][P][S]  VARIANT *
	PROPBUF_ENTRY_EMPTY,
	// 	VT_UNKNOWN	= 13,	*  VT_UNKNOWN          [V][T]   [S]  IUnknown *
	PROPBUF_ENTRY(IUnknown*),
	// 	VT_DECIMAL	= 14,	*  VT_DECIMAL          [V][T]   [S]  16 byte fixed point
	PROPBUF_ENTRY_EMPTY,
	//	15
	PROPBUF_ENTRY_EMPTY,
	// 	VT_I1	= 16,	*  VT_I1               [V][T][P][s]  signed char
	PROPBUF_ENTRY(CHAR),
	// 	VT_UI1	= 17,	*  VT_UI1              [V][T][P][S]  unsigned char
	PROPBUF_ENTRY(UCHAR),
	// 	VT_UI2	= 18,	*  VT_UI2              [V][T][P][S]  unsigned short
	PROPBUF_ENTRY(USHORT),
	// 	VT_UI4	= 19,	*  VT_UI4              [V][T][P][S]  unsigned long
	PROPBUF_ENTRY(ULONG),
	// 	VT_I8	= 20,	*  VT_I8                  [T][P]     signed 64-bit int
	PROPBUF_ENTRY(LARGE_INTEGER),
	// 	VT_UI8	= 21,	*  VT_UI8                 [T][P]     unsigned 64-bit int
	PROPBUF_ENTRY(ULARGE_INTEGER),
	// 	VT_INT	= 22,	*  VT_INT              [V][T][P][S]  signed machine int
	PROPBUF_ENTRY(INT),
	// 	VT_UINT	= 23,	*  VT_UINT             [V][T]   [S]  unsigned machine int
	PROPBUF_ENTRY(UINT),
	// 	VT_VOID	= 24,	*  VT_VOID                [T]        C style void
	PROPBUF_ENTRY_EMPTY,
	// 	VT_HRESULT	= 25,	*  VT_HRESULT             [T]        Standard return type
	PROPBUF_ENTRY(HRESULT),
	// 	VT_PTR	= 26,	*  VT_PTR                 [T]        pointer type
	PROPBUF_ENTRY_EMPTY,
	// 	VT_SAFEARRAY	= 27,	 *  VT_SAFEARRAY           [T]        (use VT_ARRAY in VARIANT)
	PROPBUF_ENTRY_EMPTY,
	// 	VT_CARRAY	= 28,	*  VT_CARRAY              [T]        C style array
	PROPBUF_ENTRY_EMPTY,
	// 	VT_USERDEFINED	= 29,	*  VT_USERDEFINED         [T]        user defined type
	PROPBUF_ENTRY_EMPTY,
	// 	VT_LPSTR	= 30,	*  VT_LPSTR               [T][P]     null terminated string
	PROPBUF_ENTRY(LPSTR),
	// 	VT_LPWSTR	= 31,	*  VT_LPWSTR              [T][P]     wide null terminated string
	PROPBUF_ENTRY(LPWSTR),
	//	32
	PROPBUF_ENTRY_EMPTY,
	//	33
	PROPBUF_ENTRY_EMPTY,
	//	34
	PROPBUF_ENTRY_EMPTY,
	//	35
	PROPBUF_ENTRY_EMPTY,
	// 	VT_RECORD	= 36,	*  VT_RECORD           [V]   [P][S]  user defined type
	PROPBUF_ENTRY_EMPTY,
	// 	VT_INT_PTR	= 37,	*  VT_INT_PTR             [T]        signed machine register size width
	PROPBUF_ENTRY_EMPTY,
	// 	VT_UINT_PTR	= 38,	*  VT_UINT_PTR            [T]        unsigned machine register size width
	PROPBUF_ENTRY_EMPTY,
	//	39
	PROPBUF_ENTRY_EMPTY,
	//	40
	PROPBUF_ENTRY_EMPTY,
	//	41
	PROPBUF_ENTRY_EMPTY,
	//	42
	PROPBUF_ENTRY_EMPTY,
	//	43
	PROPBUF_ENTRY_EMPTY,
	//	44
	PROPBUF_ENTRY_EMPTY,
	//	45
	PROPBUF_ENTRY_EMPTY,
	//	46
	PROPBUF_ENTRY_EMPTY,
	//	47
	PROPBUF_ENTRY_EMPTY,
	//	48
	PROPBUF_ENTRY_EMPTY,
	//	49
	PROPBUF_ENTRY_EMPTY,
	//	50
	PROPBUF_ENTRY_EMPTY,
	//	51
	PROPBUF_ENTRY_EMPTY,
	//	52
	PROPBUF_ENTRY_EMPTY,
	//	53
	PROPBUF_ENTRY_EMPTY,
	//	54
	PROPBUF_ENTRY_EMPTY,
	//	55
	PROPBUF_ENTRY_EMPTY,
	//	56
	PROPBUF_ENTRY_EMPTY,
	//	57
	PROPBUF_ENTRY_EMPTY,
	//	58
	PROPBUF_ENTRY_EMPTY,
	//	59
	PROPBUF_ENTRY_EMPTY,
	//	60
	PROPBUF_ENTRY_EMPTY,
	//	61
	PROPBUF_ENTRY_EMPTY,
	//	62
	PROPBUF_ENTRY_EMPTY,
	//	63
	PROPBUF_ENTRY_EMPTY,
	// 	VT_FILETIME	= 64,	*  VT_FILETIME               [P]     FILETIME
	PROPBUF_ENTRY_EMPTY,
	// 	VT_BLOB	= 65,	*  VT_BLOB                   [P]     Length prefixed bytes
	PROPBUF_ENTRY_EMPTY,
	// 	VT_STREAM	= 66,	*  VT_STREAM                 [P]     Name of the stream follows
	PROPBUF_ENTRY_EMPTY,
	// 	VT_STORAGE	= 67,	*  VT_STORAGE                [P]     Name of the storage follows
	PROPBUF_ENTRY_EMPTY,
	// 	VT_STREAMED_OBJECT	= 68,	*  VT_STREAMED_OBJECT        [P]     Stream contains an object
	PROPBUF_ENTRY_EMPTY,
	// 	VT_STORED_OBJECT	= 69,	*  VT_STORED_OBJECT          [P]     Storage contains an object
	PROPBUF_ENTRY_EMPTY,
	// 	VT_BLOB_OBJECT	= 70,	*  VT_BLOB_OBJECT            [P]     Blob contains an object 
	PROPBUF_ENTRY_EMPTY,
	// 	VT_CF	= 71,	 *  VT_CF                     [P]     Clipboard format
	PROPBUF_ENTRY_EMPTY,
	// 	VT_CLSID	= 72,	*  VT_CLSID                  [P]     A Class ID
	PROPBUF_ENTRY_ATOM_AS_PTR(CLSID),
	// 	VT_VERSIONED_STREAM	= 73,	*  VT_VERSIONED_STREAM       [P]     Stream with a GUID version
	PROPBUF_ENTRY_EMPTY,
};

class CBuffStream : public CBasicStream
{
public:
	CBuffStream( VOID )
	{

	}

	CBuffStream( IUnknown* pBuffer )
		:CBasicStream( pBuffer )
	{
	}

	// Serialize functions
	template<class T>
		HRESULT Serialize( T data )
	{
		VARTYPE type = Type2Var<T>::_vt;
		CHECK_STREAM( Write( &type, sizeof(VARTYPE) ) );

		if( type==VT_EMPTY )
		{
			DWORD dwLen = sizeof(T);
			CHECK_STREAM( Write( &dwLen, sizeof(DWORD) ) )
		}
			
		RCHECK_STREAM( prop_buffer<T>::serialize( &data, this ) );
	}

	template<class T>
	HRESULT Unserialize( T* data )
	{
		VARTYPE destType = Type2Var<T>::_vt;

		VARTYPE srcType;
		DWORD dwRead;
		CHECK_STREAM( Read( &srcType, sizeof(VARTYPE), dwRead ) );
		ASSERT_STREAM( dwRead==sizeof(VARTYPE), E_FAIL );

		if( srcType==destType )
		{
			if( destType==VT_EMPTY )
			{
				DWORD dwLen;
				CHECK_STREAM( Read( &dwLen, sizeof(DWORD), dwRead ) );
				ASSERT_STREAM( dwRead==sizeof(DWORD), E_FAIL );
				ASSERT_STREAM( dwLen==sizeof(T), E_FAIL );
			}

			RCHECK_STREAM( prop_buffer<T>::unserialize( data, this ) );
		}

		if( IsNumericType(srcType) && IsNumericType(destType) )
		{
			if( GetTypeSize(srcType) == GetTypeSize(destType) )
				RCHECK_STREAM( prop_buffer<T>::unserialize( data, this ) )
			else
				RCHECK_STREAM( E_FAIL )
		}

		RCHECK_STREAM( E_FAIL );
	}

	HRESULT Serialize( IMsBuffer* data )
	{
		VARTYPE type = VT_BUFFER;
		CHECK_STREAM( Write( &type, sizeof(VARTYPE) ) );
		RCHECK_STREAM( prop_buffer<IUnknown*>::serialize( &data, this ) );
	}

	HRESULT Unserialize( IMsBuffer* data )
	{
		VARTYPE srcType;
		DWORD dwRead;
		CHECK_STREAM( Read( &srcType, sizeof(VARTYPE), dwRead ) );
		ASSERT_STREAM( dwRead==sizeof(VARTYPE), E_UNEXPECTED );

		if( srcType!= VT_BUFFER )
			RCHECK_STREAM( E_FAIL );

		RCHECK_STREAM( prop_buffer<IUnknown*>::unserialize( &data, this ) );
	}

	template<class T>
		HRESULT SerializeVec( T* data, DWORD nElem )
	{
		VARTYPE type = Type2Var<T>::_vt|VT_VECTOR;
		CHECK_STREAM( Write( &type, sizeof(VARTYPE) ) );

		if( type==(VT_EMPTY|VT_VECTOR) )
		{
			DWORD dwLen = sizeof(T);
			CHECK_STREAM( Write( &dwLen, sizeof(DWORD) ) )
		}
		
		prop_buffer_vector<T> bufVec;
		bufVec.cElems = nElem;
		bufVec.pElems = data;

		RCHECK_STREAM( prop_buffer_vector<T>::serialize( &bufVec, this ) );
	}

	template<class T>
		HRESULT UnserializeVec( T** data, DWORD& nElem )
	{
		VARTYPE destType = Type2Var<T>::_vt|VT_VECTOR;
		VARTYPE srcType;
		DWORD dwRead;
		CHECK_STREAM( Read( &srcType, sizeof(VARTYPE), dwRead ) );
		ASSERT_STREAM( dwRead==sizeof(VARTYPE), E_UNEXPECTED );

		if( srcType!=destType )
			return E_FAIL;

		if( destType==(VT_EMPTY|VT_VECTOR) )
		{
			DWORD dwLen;
			CHECK_STREAM( Read( &dwLen, sizeof(DWORD), dwRead ) );
			ASSERT_STREAM( dwRead==sizeof(DWORD), E_FAIL );
			ASSERT_STREAM( dwLen==sizeof(T), E_FAIL );
		}

		prop_buffer_vector<T> bufVec;

		CHECK_STREAM( prop_buffer_vector<T>::unserialize( &bufVec, this ) );

		*data = bufVec.pElems;
		nElem = bufVec.cElems;
		return S_OK;
	}

	template<class T>
	HRESULT UnserializeVec( T* data)
	{
		T* p = 0;
		DWORD d = 0;

		CHECK_STREAM(UnserializeVec(&p,d));
		
		memcpy(data, p, d*sizeof(T));
		return S_OK;
	}

	HRESULT Serialize(PROPVARIANT var)
	{
		RFAILED(Serialize(var.vt));

		VARTYPE idEntry = (VARTYPE)(var.vt & VT_TYPEMASK);
		RASSERT(idEntry < sizeof(g_tblPropBufEntry) / sizeof(g_tblPropBufEntry[0]), E_INVALIDARG);

		if (var.vt & VT_VECTOR)
		{
			RASSERT(g_tblPropBufEntry[idEntry].pfn_vector_serialize, E_NOTIMPL);
			return g_tblPropBufEntry[idEntry].pfn_vector_serialize(&var.cai, this);
		}
		else
		{
			RASSERT(g_tblPropBufEntry[idEntry].pfn_serialize, E_NOTIMPL);
			return g_tblPropBufEntry[idEntry].pfn_serialize(&var.iVal, this);
		}
	}
	HRESULT Unserialize(PROPVARIANT* pvar)
	{
		RFAILED(Unserialize(&pvar->vt));

		VARTYPE idEntry = (VARTYPE)(pvar->vt & VT_TYPEMASK);
		RASSERT(idEntry < sizeof(g_tblPropBufEntry) / sizeof(g_tblPropBufEntry[0]), E_INVALIDARG);

		if (pvar->vt & VT_VECTOR)
		{
			RASSERT(g_tblPropBufEntry[idEntry].pfn_vector_unserialize, E_NOTIMPL);
			return g_tblPropBufEntry[idEntry].pfn_vector_unserialize(&pvar->cai, this);
		}
		else
		{
			RASSERT(g_tblPropBufEntry[idEntry].pfn_unserialize, E_NOTIMPL);
			return g_tblPropBufEntry[idEntry].pfn_unserialize(&pvar->iVal, this);
		}
	}

	HRESULT Serialize(CBuffStream* pSrcStream )
	{
		LPBYTE pSrc;
		CHECK_STREAM( pSrcStream->Tellp( &pSrc ) );
		DWORD dwSize = pSrcStream->GetUnreadSize();
		if( dwSize == 0 )
			return S_OK;

		CHECK_STREAM( SerializeVec( pSrc, dwSize ) );
		pSrcStream->Seekp( 0, ST_END );

		return S_OK;
	}

	// Shift operators
	//
	template<class T>
		CBuffStream& operator << (T data)
	{
		Serialize(data);

		return *this;
	}

	template<class T>
		CBuffStream& operator >> (T& data)
	{
		Unserialize(&data);
		return *this;
	}

	CBuffStream& operator >> (IMsBuffer* pBuffer)
	{
		Unserialize(pBuffer);
		return *this;
	}

	BOOL IsValid( VOID )
	{
		return m_pBuffer && !(m_ulStatus&STREAM_ERROR);
	}

private:

#define Type2Size(type, var) case var: return sizeof(type);

	DWORD GetTypeSize( VARTYPE vt )
	{
		switch( vt )
		{
			Type2Size( char,				VT_I1 )
			Type2Size( BYTE,				VT_UI1 )
			Type2Size( short,				VT_I2 )
			Type2Size( unsigned short,		VT_UI2 )
			Type2Size( int,					VT_INT )
			Type2Size( unsigned int,		VT_UINT )
			Type2Size( long,				VT_I4 )
			Type2Size( unsigned long,		VT_UI4 )
			Type2Size( LONGLONG,			VT_I8 )
		}

		return 0;
	}
};
