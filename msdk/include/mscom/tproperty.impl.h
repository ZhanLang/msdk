
#ifndef __MSPROPERTY_TEMPLATE_IMP_H__
#define __MSPROPERTY_TEMPLATE_IMP_H__

namespace msdk {
namespace mscom {
//////////////////////////////////////////////////////////////////////////
// trivial variant implement
template<class T>
struct prop_variant
{
	T var;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		((prop_variant<T>*)pThis)->var = ((const prop_variant<T>*)pRhs)->var;
		return S_OK;
	}
	static HRESULT destroy(void *) { return S_OK; }
};


//////////////////////////////////////////////////////////////////////////
// nontrivial variant implement
template<>
struct prop_variant<LPSTR>
{
	LPSTR pszVal;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		prop_variant<LPSTR> *__this = (prop_variant<LPSTR>*)pThis, *__rhs = (prop_variant<LPSTR>*)pRhs;
		RASSERT(__rhs->pszVal, (__this->pszVal = 0, S_OK));
		size_t cch = strlen(__rhs->pszVal) +1;
		RASSERTP(__this->pszVal = new CHAR[cch], E_OUTOFMEMORY);
		memcpy(__this->pszVal, __rhs->pszVal, cch*sizeof(CHAR));
		return S_OK;
	}
	static HRESULT destroy(void *pThis)
	{
		delete [] ((prop_variant<LPSTR>*)pThis)->pszVal;
		((prop_variant<LPSTR>*)pThis)->pszVal = 0;
		return S_OK;
	}
};

template<>
struct prop_variant<LPWSTR>
{
	LPWSTR pwszVal;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		prop_variant<LPWSTR> *__this = (prop_variant<LPWSTR>*)pThis, *__rhs = (prop_variant<LPWSTR>*)pRhs;
		RASSERT(__rhs->pwszVal, (__this->pwszVal = 0, S_OK));
		size_t cch = wcslen(__rhs->pwszVal) +1;
		RASSERTP(__this->pwszVal = new WCHAR[cch], E_OUTOFMEMORY);
		memcpy(__this->pwszVal, __rhs->pwszVal, cch*sizeof(WCHAR));
		return S_OK;
	}
	static HRESULT destroy(void *pThis)
	{
		delete [] ((prop_variant<LPWSTR>*)pThis)->pwszVal;
		((prop_variant<LPWSTR>*)pThis)->pwszVal = 0;
		return S_OK;
	}
};

template<>
struct prop_variant<IUnknown*>
{
	IUnknown *punkVal;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		if(((((prop_variant<IUnknown*>*)pThis)->punkVal = ((const prop_variant<IUnknown*>*)pRhs)->punkVal)) != NULL)
		{
			((prop_variant<IUnknown*>*)pThis)->punkVal->AddRef();
		}
		return S_OK;
	}
	static HRESULT destroy(void *pThis)
	{
		if(((prop_variant<IUnknown*>*)pThis)->punkVal)
		{
			((prop_variant<IUnknown*>*)pThis)->punkVal->Release();
			((prop_variant<IUnknown*>*)pThis)->punkVal = 0;
		}
		return S_OK;
	}
};

template<>
struct prop_variant<CLSID*>
{
	CLSID *puuid;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		prop_variant<CLSID*> *__this = (prop_variant<CLSID*>*)pThis, *__rhs = (prop_variant<CLSID*>*)pRhs;
		RASSERTP(__this->puuid = new CLSID, E_OUTOFMEMORY);
		*__this->puuid = *__rhs->puuid;
		return S_OK;
	}
	static HRESULT destroy(void *pThis)
	{
		delete ((prop_variant<CLSID*>*)pThis)->puuid;
		((prop_variant<CLSID*>*)pThis)->puuid = 0;
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////
// type traits for variant vector
struct prop_true_type {};
struct prop_false_type {};

template<class T>
struct prop_type_traits
{
	typedef prop_true_type	has_trivial_copy_constructor;
	typedef prop_true_type	has_trivial_destructor;
};

#define PROP_DEF_NONTRIVIAL_TYPE_TRAITS(type)	\
		template<> struct prop_type_traits<type>	\
		{	\
			typedef prop_false_type	has_trivial_copy_constructor;	\
			typedef prop_false_type	has_trivial_destructor;	\
		}

PROP_DEF_NONTRIVIAL_TYPE_TRAITS(LPSTR);
PROP_DEF_NONTRIVIAL_TYPE_TRAITS(LPWSTR);
PROP_DEF_NONTRIVIAL_TYPE_TRAITS(IUnknown*);
PROP_DEF_NONTRIVIAL_TYPE_TRAITS(CLSID*);


//////////////////////////////////////////////////////////////////////////
// variant vector implement
template<class T>
struct prop_variant_vector
{
    ULONG cElems;
    T *pElems;
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		prop_variant_vector<T> *__this = (prop_variant_vector<T>*)pThis, *__rhs = (prop_variant_vector<T>*)pRhs;
		__this->cElems = 0;
		RASSERT(__rhs->cElems, (__this->pElems = 0, S_OK));
		RASSERTP((__this->pElems = new T [__rhs->cElems]), E_UNEXPECTED);
		HRESULT hr = __this->__uninitialized_copy(__rhs, typename prop_type_traits<T>::has_trivial_copy_constructor());
		if(FAILED(hr)) { destroy(__this); }
		return hr;
	}
	static HRESULT destroy(void *pThis)
	{
		prop_variant_vector<T> *__this = (prop_variant_vector<T>*)pThis;
		__this->__destroy_elements(typename prop_type_traits<T>::has_trivial_destructor());
		delete [] __this->pElems;
		__this->cElems = 0;
		__this->pElems = 0;
		return S_OK;
	}
private:
	HRESULT __uninitialized_copy(prop_variant_vector<T> *rhs, prop_true_type)
	{
		memcpy(pElems, rhs->pElems, (cElems = rhs->cElems)*sizeof(T));
		return S_OK;
	}
	HRESULT __uninitialized_copy(prop_variant_vector<T> *rhs, prop_false_type)
	{
		for(T *left = pElems, *right = rhs->pElems; cElems < rhs->cElems; ++left, ++right, ++cElems)
		{
			RFAILED(prop_variant<T>::construct_with(left, right));
		}
		return S_OK;
	}
private:
	HRESULT __destroy_elements(prop_true_type)
	{
		return S_OK;
	}
	HRESULT __destroy_elements(prop_false_type)
	{
		for(T *p = pElems; cElems--; ++p)
		{
			prop_variant<T>::destroy(p);
		}
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////
// variant safearray implement
template<class T>
struct prop_variant_safearray : SAFEARRAY
{
	static HRESULT construct_with(void *pThis, const void *pRhs)
	{
		prop_variant_safearray<T>
			*__this = (prop_variant_safearray<T>*)pThis, *__rhs = (prop_variant_safearray<T>*)pRhs;
		memcpy(__this, __rhs, sizeof(SAFEARRAY));
		RASSERT(sizeof(T) == __this->cbElements, E_INVALIDARG);
		__this->pvData = 0;
		RASSERT(__this->cDims, S_OK);
		prop_variant_vector<T> __this_vec, __rhs_vec; __rhs_vec.cElems = 1, __rhs_vec.pElems = (T*)__rhs->pvData;
		for(USHORT i = 0; i < __rhs->cDims; __rhs_vec.cElems *= __rhs->rgsabound[i++].cElements);
		RASSERT(__rhs_vec.cElems, (__this->cDims = 0, S_OK));
		RFAILED(prop_variant_vector<T>::construct_with(&__this_vec, &__rhs_vec));
		for(USHORT j = 1; j < __this->cDims; __this->rgsabound[j] = __rhs->rgsabound[j], ++j);
		__this->pvData = __this_vec.pElems;
		return S_OK;
	}
	static HRESULT destroy(void *pThis)
	{
		prop_variant_safearray<T> *__this = (prop_variant_safearray<T>*)pThis;
		RASSERT(__this->cDims, S_OK);
		prop_variant_vector<T> __this_vec; __this_vec.cElems = 1, __this_vec.pElems = (T*)__this->pvData;
		for(USHORT i = 0; i < __this->cDims; __this_vec.cElems *= __this->rgsabound[i++].cElements);
		prop_variant_vector<T>::destroy(&__this_vec);
		__this->cDims = 0;
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////
// variant entry table
typedef HRESULT (*PFNConstruct)(void *pThis, const void *pRhs);
typedef HRESULT (*PFNDestroy)(void *pThis);

struct VariantEntryTable
{
	PFNConstruct pfn_construct_with;
	PFNDestroy   pfn_destroy;
	PFNConstruct pfn_vector_construct_with;
	PFNDestroy   pfn_vector_destroy;
	PFNConstruct pfn_safearray_construct_with;
	PFNDestroy   pfn_safearray_destroy;
};

#define VARIANT_ENTRY_EMPTY	\
		{ (PFNConstruct)0, (PFNDestroy)0, (PFNConstruct)0, (PFNDestroy)0, (PFNConstruct)0, (PFNDestroy)0 }

#define VARIANT_ENTRY(type)	\
	{	\
		&prop_variant<type>::construct_with, &prop_variant<type>::destroy,	\
		&prop_variant_vector<type>::construct_with, &prop_variant_vector<type>::destroy,	\
		&prop_variant_safearray<type>::construct_with, &prop_variant_safearray<type>::destroy,	\
	}

#define VARIANT_ENTRY_ATOM_AS_PTR(type)	\
	{	\
		&prop_variant<type*>::construct_with, &prop_variant<type*>::destroy,	\
		&prop_variant_vector<type>::construct_with, &prop_variant_vector<type>::destroy,	\
		&prop_variant_safearray<type>::construct_with, &prop_variant_safearray<type>::destroy,	\
	}

static const
VariantEntryTable g_tblVariantEntry[] =
{
	// 	VT_EMPTY	= 0,	*  VT_EMPTY            [V]   [P]     nothing
	VARIANT_ENTRY_EMPTY,
	// 	VT_NULL	= 1,	*  VT_NULL             [V]   [P]     SQL style Null
	VARIANT_ENTRY_EMPTY,
	// 	VT_I2	= 2,	*  VT_I2               [V][T][P][S]  2 byte signed int
	VARIANT_ENTRY(SHORT),
	// 	VT_I4	= 3,	*  VT_I4               [V][T][P][S]  4 byte signed int
	VARIANT_ENTRY(LONG),
	// 	VT_R4	= 4,	*  VT_R4               [V][T][P][S]  4 byte real
	VARIANT_ENTRY(FLOAT),
	// 	VT_R8	= 5,	*  VT_R8               [V][T][P][S]  8 byte real
	VARIANT_ENTRY(DOUBLE),
	// 	VT_CY	= 6,	*  VT_CY               [V][T][P][S]  currency
	VARIANT_ENTRY_EMPTY,
	// 	VT_DATE	= 7,	*  VT_DATE             [V][T][P][S]  date
	VARIANT_ENTRY_EMPTY,
	// 	VT_BSTR	= 8,	*  VT_BSTR             [V][T][P][S]  OLE Automation string
	VARIANT_ENTRY_EMPTY,
	// 	VT_DISPATCH	= 9,	*  VT_DISPATCH         [V][T]   [S]  IDispatch *
	VARIANT_ENTRY_EMPTY,
	// 	VT_ERROR	= 10,	*  VT_ERROR            [V][T][P][S]  SCODE
	VARIANT_ENTRY(SCODE),
	// 	VT_BOOL	= 11,	*  VT_BOOL             [V][T][P][S]  True=-1, False=0
	VARIANT_ENTRY(VARIANT_BOOL),
	// 	VT_VARIANT	= 12,	*  VT_VARIANT          [V][T][P][S]  VARIANT *
	VARIANT_ENTRY_EMPTY,
	// 	VT_UNKNOWN	= 13,	*  VT_UNKNOWN          [V][T]   [S]  IUnknown *
	VARIANT_ENTRY(IUnknown*),
	// 	VT_DECIMAL	= 14,	*  VT_DECIMAL          [V][T]   [S]  16 byte fixed point
	VARIANT_ENTRY_EMPTY,
	//	15
	VARIANT_ENTRY_EMPTY,
	// 	VT_I1	= 16,	*  VT_I1               [V][T][P][s]  signed char
	VARIANT_ENTRY(CHAR),
	// 	VT_UI1	= 17,	*  VT_UI1              [V][T][P][S]  unsigned char
	VARIANT_ENTRY(UCHAR),
	// 	VT_UI2	= 18,	*  VT_UI2              [V][T][P][S]  unsigned short
	VARIANT_ENTRY(USHORT),
	// 	VT_UI4	= 19,	*  VT_UI4              [V][T][P][S]  unsigned long
	VARIANT_ENTRY(ULONG),
	// 	VT_I8	= 20,	*  VT_I8                  [T][P]     signed 64-bit int
	VARIANT_ENTRY(LARGE_INTEGER),
	// 	VT_UI8	= 21,	*  VT_UI8                 [T][P]     unsigned 64-bit int
	VARIANT_ENTRY(ULARGE_INTEGER),
	// 	VT_INT	= 22,	*  VT_INT              [V][T][P][S]  signed machine int
	VARIANT_ENTRY(INT),
	// 	VT_UINT	= 23,	*  VT_UINT             [V][T]   [S]  unsigned machine int
	VARIANT_ENTRY(UINT),
	// 	VT_VOID	= 24,	*  VT_VOID                [T]        C style void
	VARIANT_ENTRY_EMPTY,
	// 	VT_HRESULT	= 25,	*  VT_HRESULT             [T]        Standard return type
	VARIANT_ENTRY(HRESULT),
	// 	VT_PTR	= 26,	*  VT_PTR                 [T]        pointer type
	VARIANT_ENTRY_EMPTY,
	// 	VT_SAFEARRAY	= 27,	 *  VT_SAFEARRAY           [T]        (use VT_ARRAY in VARIANT)
	VARIANT_ENTRY_EMPTY,
	// 	VT_CARRAY	= 28,	*  VT_CARRAY              [T]        C style array
	VARIANT_ENTRY_EMPTY,
	// 	VT_USERDEFINED	= 29,	*  VT_USERDEFINED         [T]        user defined type
	VARIANT_ENTRY_EMPTY,
	// 	VT_LPSTR	= 30,	*  VT_LPSTR               [T][P]     null terminated string
	VARIANT_ENTRY(LPSTR),
	// 	VT_LPWSTR	= 31,	*  VT_LPWSTR              [T][P]     wide null terminated string
	VARIANT_ENTRY(LPWSTR),
	//	32
	VARIANT_ENTRY_EMPTY,
	//	33
	VARIANT_ENTRY_EMPTY,
	//	34
	VARIANT_ENTRY_EMPTY,
	//	35
	VARIANT_ENTRY_EMPTY,
	// 	VT_RECORD	= 36,	*  VT_RECORD           [V]   [P][S]  user defined type
	VARIANT_ENTRY_EMPTY,
	// 	VT_INT_PTR	= 37,	*  VT_INT_PTR             [T]        signed machine register size width
	VARIANT_ENTRY_EMPTY,
	// 	VT_UINT_PTR	= 38,	*  VT_UINT_PTR            [T]        unsigned machine register size width
	VARIANT_ENTRY_EMPTY,
	//	39
	VARIANT_ENTRY_EMPTY,
	//	40
	VARIANT_ENTRY_EMPTY,
	//	41
	VARIANT_ENTRY_EMPTY,
	//	42
	VARIANT_ENTRY_EMPTY,
	//	43
	VARIANT_ENTRY_EMPTY,
	//	44
	VARIANT_ENTRY_EMPTY,
	//	45
	VARIANT_ENTRY_EMPTY,
	//	46
	VARIANT_ENTRY_EMPTY,
	//	47
	VARIANT_ENTRY_EMPTY,
	//	48
	VARIANT_ENTRY_EMPTY,
	//	49
	VARIANT_ENTRY_EMPTY,
	//	50
	VARIANT_ENTRY_EMPTY,
	//	51
	VARIANT_ENTRY_EMPTY,
	//	52
	VARIANT_ENTRY_EMPTY,
	//	53
	VARIANT_ENTRY_EMPTY,
	//	54
	VARIANT_ENTRY_EMPTY,
	//	55
	VARIANT_ENTRY_EMPTY,
	//	56
	VARIANT_ENTRY_EMPTY,
	//	57
	VARIANT_ENTRY_EMPTY,
	//	58
	VARIANT_ENTRY_EMPTY,
	//	59
	VARIANT_ENTRY_EMPTY,
	//	60
	VARIANT_ENTRY_EMPTY,
	//	61
	VARIANT_ENTRY_EMPTY,
	//	62
	VARIANT_ENTRY_EMPTY,
	//	63
	VARIANT_ENTRY_EMPTY,
	// 	VT_FILETIME	= 64,	*  VT_FILETIME               [P]     FILETIME
	VARIANT_ENTRY_EMPTY,
	// 	VT_BLOB	= 65,	*  VT_BLOB                   [P]     Length prefixed bytes
	VARIANT_ENTRY_EMPTY,
	// 	VT_STREAM	= 66,	*  VT_STREAM                 [P]     Name of the stream follows
	VARIANT_ENTRY_EMPTY,
	// 	VT_STORAGE	= 67,	*  VT_STORAGE                [P]     Name of the storage follows
	VARIANT_ENTRY_EMPTY,
	// 	VT_STREAMED_OBJECT	= 68,	*  VT_STREAMED_OBJECT        [P]     Stream contains an object
	VARIANT_ENTRY_EMPTY,
	// 	VT_STORED_OBJECT	= 69,	*  VT_STORED_OBJECT          [P]     Storage contains an object
	VARIANT_ENTRY_EMPTY,
	// 	VT_BLOB_OBJECT	= 70,	*  VT_BLOB_OBJECT            [P]     Blob contains an object 
	VARIANT_ENTRY_EMPTY,
	// 	VT_CF	= 71,	 *  VT_CF                     [P]     Clipboard format
	VARIANT_ENTRY_EMPTY,
	// 	VT_CLSID	= 72,	*  VT_CLSID                  [P]     A Class ID
	VARIANT_ENTRY_ATOM_AS_PTR(CLSID),
	// 	VT_VERSIONED_STREAM	= 73,	*  VT_VERSIONED_STREAM       [P]     Stream with a GUID version
	VARIANT_ENTRY_EMPTY,
};

}//namespace mscom
}//namespace msdk

#endif