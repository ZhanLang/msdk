#ifndef _RSDK_EXPORT_PROPVARIANT_H_
#define _RSDK_EXPORT_PROPVARIANT_H_


#if defined(PLATFORM_TYPE_NATIVE) || defined(PLATFORM_TYPE_POSIX)

/*
 * VARENUM usage key,
 *
 * * [V] - may appear in a VARIANT
 * * [T] - may appear in a TYPEDESC
 * * [P] - may appear in an OLE property set
 * * [S] - may appear in a Safe Array
 *
 *
 *  VT_EMPTY            [V]   [P]     nothing
 *  VT_NULL             [V]   [P]     SQL style Null
 *  VT_I2               [V][T][P][S]  2 byte signed int
 *  VT_I4               [V][T][P][S]  4 byte signed int
 *  VT_R4               [V][T][P][S]  4 byte real
 *  VT_R8               [V][T][P][S]  8 byte real
 *  VT_CY               [V][T][P][S]  currency
 *  VT_DATE             [V][T][P][S]  date
 *  VT_BSTR             [V][T][P][S]  OLE Automation string
 *  VT_DISPATCH         [V][T]   [S]  IDispatch *
 *  VT_ERROR            [V][T][P][S]  SCODE
 *  VT_BOOL             [V][T][P][S]  True=-1, False=0
 *  VT_VARIANT          [V][T][P][S]  VARIANT *
 *  VT_UNKNOWN          [V][T]   [S]  IUnknown *
 *  VT_DECIMAL          [V][T]   [S]  16 byte fixed point
 *  VT_RECORD           [V]   [P][S]  user defined type
 *  VT_I1               [V][T][P][s]  signed char
 *  VT_UI1              [V][T][P][S]  unsigned char
 *  VT_UI2              [V][T][P][S]  unsigned short
 *  VT_UI4              [V][T][P][S]  unsigned long
 *  VT_I8                  [T][P]     signed 64-bit int
 *  VT_UI8                 [T][P]     unsigned 64-bit int
 *  VT_INT              [V][T][P][S]  signed machine int
 *  VT_UINT             [V][T]   [S]  unsigned machine int
 *  VT_INT_PTR             [T]        signed machine register size width
 *  VT_UINT_PTR            [T]        unsigned machine register size width
 *  VT_VOID                [T]        C style void
 *  VT_HRESULT             [T]        Standard return type
 *  VT_PTR                 [T]        pointer type
 *  VT_SAFEARRAY           [T]        (use VT_ARRAY in VARIANT)
 *  VT_CARRAY              [T]        C style array
 *  VT_USERDEFINED         [T]        user defined type
 *  VT_LPSTR               [T][P]     null terminated string
 *  VT_LPWSTR              [T][P]     wide null terminated string
 *  VT_FILETIME               [P]     FILETIME
 *  VT_BLOB                   [P]     Length prefixed bytes
 *  VT_STREAM                 [P]     Name of the stream follows
 *  VT_STORAGE                [P]     Name of the storage follows
 *  VT_STREAMED_OBJECT        [P]     Stream contains an object
 *  VT_STORED_OBJECT          [P]     Storage contains an object
 *  VT_VERSIONED_STREAM       [P]     Stream with a GUID version
 *  VT_BLOB_OBJECT            [P]     Blob contains an object 
 *  VT_CF                     [P]     Clipboard format
 *  VT_CLSID                  [P]     A Class ID
 *  VT_VECTOR                 [P]     simple counted array
 *  VT_ARRAY            [V]           SAFEARRAY*
 *  VT_BYREF            [V]           void* for local use
 *  VT_BSTR_BLOB                      Reserved for system use
 */

//////////////////////////////////////////////////////////////////////////

/* VARIANT STRUCTURE
 *
 *  VARTYPE vt;
 *  WORD wReserved1;
 *  WORD wReserved2;
 *  WORD wReserved3;
 *  union {
 *    LONGLONG       VT_I8
 *    LONG           VT_I4
 *    BYTE           VT_UI1
 *    SHORT          VT_I2
 *    FLOAT          VT_R4
 *    DOUBLE         VT_R8
 *    VARIANT_BOOL   VT_BOOL
 *    SCODE          VT_ERROR
 *    CY             VT_CY
 *    DATE           VT_DATE
 *    BSTR           VT_BSTR
 *    IUnknown *     VT_UNKNOWN
 *    IDispatch *    VT_DISPATCH
 *    SAFEARRAY *    VT_ARRAY
 *    BYTE *         VT_BYREF|VT_UI1
 *    SHORT *        VT_BYREF|VT_I2
 *    LONG *         VT_BYREF|VT_I4
 *    LONGLONG *     VT_BYREF|VT_I8
 *    FLOAT *        VT_BYREF|VT_R4
 *    DOUBLE *       VT_BYREF|VT_R8
 *    VARIANT_BOOL * VT_BYREF|VT_BOOL
 *    SCODE *        VT_BYREF|VT_ERROR
 *    CY *           VT_BYREF|VT_CY
 *    DATE *         VT_BYREF|VT_DATE
 *    BSTR *         VT_BYREF|VT_BSTR
 *    IUnknown **    VT_BYREF|VT_UNKNOWN
 *    IDispatch **   VT_BYREF|VT_DISPATCH
 *    SAFEARRAY **   VT_BYREF|VT_ARRAY
 *    VARIANT *      VT_BYREF|VT_VARIANT
 *    PVOID          VT_BYREF (Generic ByRef)
 *    CHAR           VT_I1
 *    USHORT         VT_UI2
 *    ULONG          VT_UI4
 *    ULONGLONG      VT_UI8
 *    INT            VT_INT
 *    UINT           VT_UINT
 *    DECIMAL *      VT_BYREF|VT_DECIMAL
 *    CHAR *         VT_BYREF|VT_I1
 *    USHORT *       VT_BYREF|VT_UI2
 *    ULONG *        VT_BYREF|VT_UI4
 *    ULONGLONG *    VT_BYREF|VT_UI8
 *    INT *          VT_BYREF|VT_INT
 *    UINT *         VT_BYREF|VT_UINT
 *  }
 */

//////////////////////////////////////////////////////////////////////////

enum VARENUM
{
	VT_EMPTY	= 0,
	VT_NULL	= 1,
	VT_I2	= 2,
	VT_I4	= 3,
	VT_R4	= 4,
	VT_R8	= 5,
	VT_CY	= 6,
	VT_DATE	= 7,
	VT_BSTR	= 8,
	VT_DISPATCH	= 9,
	VT_ERROR	= 10,
	VT_BOOL	= 11,
	VT_VARIANT	= 12,
	VT_UNKNOWN	= 13,
	VT_DECIMAL	= 14,
	VT_I1	= 16,
	VT_UI1	= 17,
	VT_UI2	= 18,
	VT_UI4	= 19,
	VT_I8	= 20,
	VT_UI8	= 21,
	VT_INT	= 22,
	VT_UINT	= 23,
	VT_VOID	= 24,
	VT_HRESULT	= 25,
	VT_PTR	= 26,
	VT_SAFEARRAY	= 27,
	VT_CARRAY	= 28,
	VT_USERDEFINED	= 29,
	VT_LPSTR	= 30,
	VT_LPWSTR	= 31,
	VT_RECORD	= 36,
	VT_INT_PTR	= 37,
	VT_UINT_PTR	= 38,
	VT_FILETIME	= 64,
	VT_BLOB	= 65,
	VT_STREAM	= 66,
	VT_STORAGE	= 67,
	VT_STREAMED_OBJECT	= 68,
	VT_STORED_OBJECT	= 69,
	VT_BLOB_OBJECT	= 70,
	VT_CF	= 71,
	VT_CLSID	= 72,
	VT_VERSIONED_STREAM	= 73,
	//
	VT_BSTR_BLOB	= 0xfff,
	//
	VT_VECTOR	= 0x1000,
	VT_ARRAY	= 0x2000,
	VT_BYREF	= 0x4000,
	VT_RESERVED	= 0x8000,
	VT_ILLEGAL	= 0xffff,
	VT_ILLEGALMASKED	= 0xfff,
	VT_TYPEMASK	= 0xfff
};

//////////////////////////////////////////////////////////////////////////

typedef unsigned short VARTYPE;

typedef WORD PROPVAR_PAD1;
typedef WORD PROPVAR_PAD2;
typedef WORD PROPVAR_PAD3;

typedef struct tagPROPVARIANT PROPVARIANT;

#define TYPEDEF_CA(type, name) \
    typedef struct tag ## name {\
        ULONG cElems;\
        type *pElems;\
        } name

TYPEDEF_CA(CHAR, CAC);
TYPEDEF_CA(UCHAR, CAUB);
TYPEDEF_CA(SHORT, CAI);
TYPEDEF_CA(USHORT, CAUI);
TYPEDEF_CA(LONG, CAL);
TYPEDEF_CA(ULONG, CAUL);
TYPEDEF_CA(LARGE_INTEGER, CAH);
TYPEDEF_CA(ULARGE_INTEGER, CAUH);
TYPEDEF_CA(FLOAT, CAFLT);
TYPEDEF_CA(DOUBLE, CADBL);
TYPEDEF_CA(VARIANT_BOOL, CABOOL);
TYPEDEF_CA(SCODE, CASCODE);
// TYPEDEF_CA(CY, CACY);
// TYPEDEF_CA(DATE, CADATE);
// TYPEDEF_CA(FILETIME, CAFILETIME);
TYPEDEF_CA(CLSID, CACLSID);
// TYPEDEF_CA(CLIPDATA, CACLIPDATA);
// TYPEDEF_CA(BSTR, CABSTR);
// TYPEDEF_CA(BSTRBLOB, CABSTRBLOB);
TYPEDEF_CA(LPSTR, CALPSTR);
TYPEDEF_CA(LPWSTR, CALPWSTR);
// TYPEDEF_CA(PROPVARIANT, CAPROPVARIANT);

typedef struct  tagSAFEARRAYBOUND {
	ULONG              cElements;
	LONG               lLbound;
} SAFEARRAYBOUND;
 
typedef struct  tagSAFEARRAY {
	USHORT             cDims;
	USHORT             fFeatures;
	ULONG              cbElements;
	ULONG              cLocks;
	PVOID              pvData;
	SAFEARRAYBOUND     rgsabound [ 1 ];
} SAFEARRAY, *LPSAFEARRAY;

// class IDispatch;
// class IStream;
// class IStorage;
// 
// typedef struct tagDECIMAL DECIMAL;
// typedef struct tagVersionedStream *LPVERSIONEDSTREAM;

//////////////////////////////////////////////////////////////////////////

struct tagPROPVARIANT
{
	VARTYPE vt;
	PROPVAR_PAD1 wReserved1;
	PROPVAR_PAD2 wReserved2;
	PROPVAR_PAD3 wReserved3;
	union 
	{
		CHAR cVal;
		UCHAR bVal;
		SHORT iVal;
		USHORT uiVal;
		LONG lVal;
		ULONG ulVal;
		INT intVal;
		UINT uintVal;
		LARGE_INTEGER hVal;
		ULARGE_INTEGER uhVal;
		FLOAT fltVal;
		DOUBLE dblVal;
		VARIANT_BOOL boolVal;
// 		_VARIANT_BOOL bool;
		SCODE scode;
// 		CY cyVal;
// 		DATE date;
 		FILETIME filetime;
		CLSID *puuid;
// 		CLIPDATA *pclipdata;
 		BSTR bstrVal;
// 		BSTRBLOB bstrblobVal;
// 		BLOB blob;
		LPSTR pszVal;
		LPWSTR pwszVal;
		IUnknown *punkVal;
// 		IDispatch *pdispVal;
// 		IStream *pStream;
// 		IStorage *pStorage;
// 		LPVERSIONEDSTREAM pVersionedStream;
		//
		LPSAFEARRAY parray;
		//
		CAC cac;
		CAUB caub;
		CAI cai;
		CAUI caui;
		CAL cal;
		CAUL caul;
		CAH cah;
		CAUH cauh;
		CAFLT caflt;
		CADBL cadbl;
		CABOOL cabool;
		CASCODE cascode;
// 		CACY cacy;
// 		CADATE cadate;
// 		CAFILETIME cafiletime;
		CACLSID cauuid;
// 		CACLIPDATA caclipdata;
// 		CABSTR cabstr;
// 		CABSTRBLOB cabstrblob;
		CALPSTR calpstr;
		CALPWSTR calpwstr;
// 		CAPROPVARIANT capropvar;
		//
		CHAR *pcVal;
		UCHAR *pbVal;
		SHORT *piVal;
		USHORT *puiVal;
		LONG *plVal;
		ULONG *pulVal;
		INT *pintVal;
		UINT *puintVal;
		FLOAT *pfltVal;
		DOUBLE *pdblVal;
		VARIANT_BOOL *pboolVal;
// 		DECIMAL *pdecVal;
		SCODE *pscode;
// 		CY *pcyVal;
// 		DATE *pdate;
// 		BSTR *pbstrVal;
		IUnknown **ppunkVal;
// 		IDispatch **ppdispVal;
// 		LPSAFEARRAY *pparray;
// 		PROPVARIANT *pvarVal;
	};
};


#endif // #ifdef _WIN32


#endif // duplicate inclusion protection
