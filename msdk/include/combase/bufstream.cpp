/********************************************************************
	created:	2009/10/26
	created:	26:10:2009   16:25
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\common\buffstream.cpp
	file path:	d:\C++Work\approduct2010\utm\src\unitive\common
	file base:	buffstream
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "bufstream.h"

PropBufEntryTable g_tblPropBufEntry[] =
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

HRESULT CBuffStream::Serialize( PROPVARIANT var )
{
	RFAILED( Serialize( var.vt ) );

	VARTYPE idEntry = (VARTYPE)(var.vt & VT_TYPEMASK);
	RASSERT( idEntry < sizeof(g_tblPropBufEntry)/sizeof(g_tblPropBufEntry[0]), E_INVALIDARG );

	if( var.vt & VT_VECTOR )
	{
		RASSERT( g_tblPropBufEntry[idEntry].pfn_vector_serialize, E_NOTIMPL );
		return g_tblPropBufEntry[idEntry].pfn_vector_serialize( &var.cai, this );
	}
	else
	{
		RASSERT( g_tblPropBufEntry[idEntry].pfn_serialize, E_NOTIMPL );
		return g_tblPropBufEntry[idEntry].pfn_serialize( &var.iVal, this );
	}
}

HRESULT CBuffStream::Unserialize( PROPVARIANT* pvar )
{
	RFAILED( Unserialize( &pvar->vt ) );

	VARTYPE idEntry = (VARTYPE)(pvar->vt & VT_TYPEMASK);
	RASSERT( idEntry < sizeof(g_tblPropBufEntry)/sizeof(g_tblPropBufEntry[0]), E_INVALIDARG );

	if( pvar->vt & VT_VECTOR )
	{
		RASSERT( g_tblPropBufEntry[idEntry].pfn_vector_unserialize, E_NOTIMPL );
		return g_tblPropBufEntry[idEntry].pfn_vector_unserialize( &pvar->cai, this );
	}
	else
	{
		RASSERT( g_tblPropBufEntry[idEntry].pfn_unserialize, E_NOTIMPL );
		return g_tblPropBufEntry[idEntry].pfn_unserialize( &pvar->iVal, this );
	}
}

