
#ifndef _IMSTPROPERTY_H
#define _IMSTPROPERTY_H

#include <SyncObject/criticalsection.h>
#include <mscom/tproperty.impl.h>

namespace msdk {
namespace mscom {

	template<class T,class LockType, class KeyLess = less<T>>
	class TProperty_Impl
	{
	public: // __tCLSID_CSrvProperty2<T>:
		HRESULT set_property(const T& rpid, const PROPVARIANT *pv)
		{
			typename SRVATUOLOCK lock(m_lock);

			typename PROPMAP::iterator it = m_map.lower_bound(rpid);
			if(it != m_map.end() && it->first == rpid) { variant_destroy(&it->second); }
			else { it = m_map.insert(it, std::make_pair(rpid, PROPVARIANT())); }
			HRESULT hr = variant_construct_with(&it->second, pv);
			if(FAILED(hr)) { m_map.erase(it); }
			return hr;
		}

		HRESULT get_property(const T& rpid, const PROPVARIANT **ppv)
		{
			typename SRVATUOLOCK lock(m_lock);

			typename PROPMAP::iterator it = m_map.find(rpid);
			RASSERT(m_map.end() != it, E_INVALIDARG);
			*ppv = &it->second;
			return S_OK;
		}

		HRESULT delete_property(const T& rpid)
		{
			typename SRVATUOLOCK lock(m_lock);

			typename PROPMAP::iterator it = m_map.find(rpid);
			RASSERT(m_map.end() != it, E_INVALIDARG);
			HRESULT hr = variant_destroy(&it->second);
			m_map.erase(it);
			return hr;
		}

		HRESULT clear()
		{
			typename SRVATUOLOCK lock(m_lock);

			for(typename PROPMAP::iterator it = m_map.begin(); it != m_map.end(); ++it)
			{
				variant_destroy(&it->second);
			}

			m_map.clear();

			return S_OK;
		}

	public:
		~TProperty_Impl() { clear(); }

	private:
		HRESULT variant_construct_with(PROPVARIANT *pprop, const PROPVARIANT *prhs)
		{
			RASSERT(!((pprop->vt = prhs->vt) & VT_BYREF), (pprop->pszVal = prhs->pszVal, S_OK));
			VARTYPE idEntry = (VARTYPE)(pprop->vt & VT_TYPEMASK);
			RASSERT(idEntry < sizeof(g_tblVariantEntry)/sizeof(g_tblVariantEntry[0]), E_INVALIDARG);
			if(pprop->vt == VT_NULL) //啥都不干
			{
				return S_OK;
			}
			else if(pprop->vt & VT_ARRAY)
			{
#if(IS_COMPILER_VC6)
				SAFEARRAY **pparray = (SAFEARRAY**)&pprop->pszVal, *parray_rhs = (SAFEARRAY*)prhs->pszVal;
#else
				SAFEARRAY **pparray = &pprop->parray, *parray_rhs = prhs->parray;
#endif
				RASSERT(g_tblVariantEntry[idEntry].pfn_safearray_construct_with, E_NOTIMPL);
				size_t cb = sizeof(SAFEARRAY);
				if(parray_rhs->cDims > 1) { cb += sizeof(SAFEARRAYBOUND)*(parray_rhs->cDims-1); }
				RASSERTP(*pparray = (SAFEARRAY*)(new BYTE[cb]), E_OUTOFMEMORY);
				HRESULT hr = g_tblVariantEntry[idEntry].pfn_safearray_construct_with(*pparray, parray_rhs);
				if(FAILED(hr)) { delete *pparray; }
				return hr;
			}
			else if(pprop->vt & VT_VECTOR)
			{
				RASSERT(g_tblVariantEntry[idEntry].pfn_vector_construct_with, E_NOTIMPL);
				return g_tblVariantEntry[idEntry].pfn_vector_construct_with(&pprop->cai, &prhs->cai);
			}
			else
			{
				RASSERT(g_tblVariantEntry[idEntry].pfn_construct_with, E_NOTIMPL);
				return g_tblVariantEntry[idEntry].pfn_construct_with(&pprop->iVal, &prhs->iVal);
			}
		}

		HRESULT variant_destroy(PROPVARIANT *pprop)
		{
			RASSERT(!(pprop->vt & VT_BYREF), S_OK);
			VARTYPE idEntry = (VARTYPE)(pprop->vt & VT_TYPEMASK);
			RASSERT(idEntry < sizeof(g_tblVariantEntry)/sizeof(g_tblVariantEntry[0]), E_UNEXPECTED);
			if(pprop->vt == VT_NULL) //啥都不干
			{
				return S_OK;
			}
			else if(pprop->vt & VT_ARRAY)
			{
#if(IS_COMPILER_VC6)
				SAFEARRAY *parray = (SAFEARRAY*)pprop->pszVal;
#else
				SAFEARRAY *parray = pprop->parray;
#endif
				RASSERT(g_tblVariantEntry[idEntry].pfn_safearray_destroy, E_UNEXPECTED);
				HRESULT hr = g_tblVariantEntry[idEntry].pfn_safearray_destroy(parray);
				delete parray;
				return hr;
			}
			else if(pprop->vt & VT_VECTOR)
			{
				RASSERT(g_tblVariantEntry[idEntry].pfn_vector_destroy, E_UNEXPECTED);
				return g_tblVariantEntry[idEntry].pfn_vector_destroy(&pprop->cai);
			}
			else
			{
				RASSERT(g_tblVariantEntry[idEntry].pfn_destroy, E_UNEXPECTED);
				return g_tblVariantEntry[idEntry].pfn_destroy(&pprop->iVal);
			}
		}

	private:
		typedef std::map<T, PROPVARIANT, KeyLess > PROPMAP;
		PROPMAP m_map;

		typedef CStackLockWrapper<LockType> SRVATUOLOCK;
		LockType m_lock;
	};

	

	template<class IFace,
			 class LockType = CNullCriticalSection, 
			 class KeyLess = std::less<IFace::KeyType>, 
			 class T = typename IFace::KeyType, 
			 class P = const T&
		    >
	class TProperty : public IFace
		, public IUnknown_Nondelegate
		, private TProperty_Impl<T, LockType, KeyLess>
		, private CUnknownImp_Inner
	{
		typedef	TProperty_Impl<T,LockType, KeyLess>	Impl;
	public: // IUnknown:
		UNKNOWN_INNER_IMP1(IFace);
	public: // std factory invoke:
		STDMETHOD(init_class)(IUnknown * /*prot*/, IUnknown *punkOuter)
		{
			return init_class_inner(punkOuter);
		}
	public:
		STDMETHOD(SetProperty)( P rpid, const PROPVARIANT *pv) { return Impl::set_property(rpid, pv); }
		STDMETHOD(GetProperty)( P rpid, const PROPVARIANT **ppv) { return Impl::get_property(rpid, ppv); }
		STDMETHOD(DeleteProperty)(P rpid) { return Impl::delete_property(rpid); }
		STDMETHOD(Clear)() { return Impl::clear(); }
	public:
		virtual ~TProperty() {}
	};


typedef TProperty<IProperty, CNullCriticalSection, memless<CLSID> >	CProperty;
typedef TProperty<IProperty, CAutoCriticalSection, memless<CLSID> >	CPropertyThread;

typedef TProperty<IProperty2>	CProperty2;
typedef TProperty<IProperty2, CAutoCriticalSection>	CProperty2Thread;

typedef TProperty<IPropertyStr,CNullCriticalSection, strless<std::string>, std::string, LPCSTR>	CPropertyStr;
typedef TProperty<IPropertyStr, CAutoCriticalSection, strless<std::string>, std::string,LPCSTR>	CPropertyStrThread;
} //namespace mscom
} //namespace msdk
#endif // _IMSTPROPERTY_H