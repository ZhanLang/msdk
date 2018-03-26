#ifndef __MS_STD_COM_H__
#define __MS_STD_COM_H__
#include <tchar.h>
#define MAX_PROGIDLEN	100

namespace msdk {
namespace mscom {
//////////////////////////////////////////////////////////////////////////
inline bool operator < (const GUID& left, const GUID& right)
{
	return memcmp(&left, &right, sizeof(GUID)) < 0;
}

class CUnknownImp
{
public:
	ULONG m_RefCount;
	CUnknownImp(): m_RefCount(0) {}
	virtual ~CUnknownImp(){}
};


#define QIBEGIN	\
	STDMETHOD(QueryInterface)(REFGUID riid, void **ppv) {

#define QIUNKNOWN	\
	if(re_uuidof(IMSBase) == riid) { *ppv = static_cast<IMSBase*>(this); AddRef(); return S_OK; }

#define QIUNKNOWN_(icast)	\
	if(re_uuidof(IMSBase) == riid) { *ppv = static_cast<IMSBase*>(static_cast<icast*>(this)); AddRef(); return S_OK; }

#define QIENTRY(iface)	\
	if(re_uuidof(iface) == riid) { *ppv = static_cast<iface*>(this); AddRef(); return S_OK; }

#define QIENTRY_(iface, icast)	\
	if(re_uuidof(iface) == riid) { *ppv = static_cast<iface*>(static_cast<icast*>(this)); AddRef(); return S_OK; }

#define QIEND \
	return E_NOINTERFACE; }

// 我要看对象计数
#ifdef _DEBUG_INTERLOCKED_VIEW
class CInterlockedView_
{
public:
	ULONG& m_RefCount;
	void* m_this;
	CInterlockedView_(void* pthis, ULONG& RefCount):m_this(pthis),m_RefCount(RefCount)
	{
		TCHAR out[64] = {0};
		wsprintf(out, _T("Enter %x = %d ------> "), m_this, m_RefCount);
		OutputDebugString(out);
	}
	~CInterlockedView_()
	{
		TCHAR out[64] = {0};
		wsprintf(out, _T("Leave %x = %d\n"), m_this, m_RefCount);
		OutputDebugString(out);
	}
};
#define ADDREF	\
	STDMETHOD_(ULONG, AddRef)()	\
	{ CInterlockedView_ v(this, m_RefCount); return InterlockedIncrement((LONG*)(LPVOID)&m_RefCount); }

#define RELEASE	\
	STDMETHOD_(ULONG, Release)()	\
	{ CInterlockedView_ v(this, m_RefCount); if(InterlockedDecrement((LONG*)(LPVOID)&m_RefCount)) return m_RefCount; delete this; return 0; }

#else


#define ADDREF	\
	STDMETHOD_(ULONG, AddRef)()	\
	{return InterlockedIncrement((LONG*)(LPVOID)&m_RefCount); }

#define RELEASE	\
	STDMETHOD_(ULONG, Release)()	\
	{if(InterlockedDecrement((LONG*)(LPVOID)&m_RefCount)) return m_RefCount; delete this; return 0; }

#endif

#define UNKNOWN_IMP_SPEC(entrys) \
	QIBEGIN  QIUNKNOWN  entrys  QIEND  ADDREF  RELEASE

#define UNKNOWN_IMP_SPEC_(icast, entrys) \
	QIBEGIN  QIUNKNOWN_(icast)  entrys  QIEND  ADDREF  RELEASE


#define ADDREF_EX	\
	STDMETHOD_(ULONG, AddRef)()	\
{return InterlockedIncrement((LONG*)(LPVOID)&m_RefCount); }

#define RELEASE_EX	\
	STDMETHOD_(ULONG, Release)()	\
{ if(InterlockedDecrement((LONG*)(LPVOID)&m_RefCount)) return m_RefCount; /*delete this;*/ return 0; }

#define UNKNOWN_IMP_SPEC_EX(entrys) \
	QIBEGIN  QIUNKNOWN  entrys  QIEND  ADDREF_EX  RELEASE_EX

#define UNKNOWN_IMP1(i) \
	UNKNOWN_IMP_SPEC( QIENTRY(i) )
#define UNKNOWN_IMP2(i1, i2) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_IMP3(i1, i2, i3) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_IMP4(i1, i2, i3, i4) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_IMP5(i1, i2, i3, i4, i5) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_IMP6(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_IMP7(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )
#define UNKNOWN_IMP8(i1, i2, i3, i4, i5, i6, i7, i8) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) )
#define UNKNOWN_IMP9(i1, i2, i3, i4, i5, i6, i7, i8, i9) \
	UNKNOWN_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) QIENTRY(i9))


#define UNKNOWN_IMP1_(i) \
	UNKNOWN_IMP_SPEC_(i,  QIENTRY(i))
#define UNKNOWN_IMP2_(i1, i2) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_IMP3_(i1, i2, i3) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_IMP4_(i1, i2, i3, i4) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_IMP5_(i1, i2, i3, i4, i5) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_IMP6_(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_IMP7_(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )
#define UNKNOWN_IMP8_(i1, i2, i3, i4, i5, i6, i7, i8) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) )
#define UNKNOWN_IMP9_(i1, i2, i3, i4, i5, i6, i7, i8, i9) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) QIENTRY(i9))
	
#define UNKNOWN_IMP10_(i1, i2, i3, i4, i5, i6, i7, i8, i9, i10) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) QIENTRY(i9) QIENTRY(i10))

#define UNKNOWN_IMP11_(i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11) \
	UNKNOWN_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) QIENTRY(i8) QIENTRY(i9) QIENTRY(i10) QIENTRY(i11))

//////////////////////////////////////////////////////////////////////////


class IUnknown_Nondelegate
{
public:
	STDMETHOD(QueryInterface_Nondelegate)(REFGUID riid, void **ppv) = 0;
	STDMETHOD_(ULONG, AddRef_Nondelegate)() = 0;
	STDMETHOD_(ULONG, Release_Nondelegate)() = 0;
};

struct INoRefCom : public IUnknown
{
	STDMETHOD_(ULONG,AddRef)(void){return 1;}
	STDMETHOD_(ULONG,Release)(void){return 1;}
	STDMETHOD(QueryInterface)(const IID & Want_IID,void **ppObject){return E_NOTIMPL;}
};
MS_DEFINE_IID(INoRefCom,"{FD00FF7A-1429-4310-80DB-BF33C04211BE}");


class CUnknownImp_Inner
{
public:
	ULONG m_RefCount;
	CUnknownImp_Inner(): m_RefCount(0), m_punkOuter(0) {}
public:
	IMSBase *m_punkOuter;
	HRESULT init_class_inner(IMSBase *punkOuter)
	{
		m_punkOuter = punkOuter;
		return S_OK;
	}
};


#define QIBEGIN_NONDELEGATE	\
	STDMETHOD(QueryInterface_Nondelegate)(REFGUID riid, void **ppv) {

#define QIUNKNOWN_NONDELEGATE	\
	if(re_uuidof(IMSBase) == riid) { *ppv = static_cast<IUnknown_Nondelegate*>(this); AddRef_Nondelegate(); return S_OK; }

#define QIUNKNOWN_NONDELEGATE_(icast)	\
	if(re_uuidof(IMSBase) == riid) { *ppv = static_cast<IUnknown_Nondelegate*>(static_cast<icast*>(this)); AddRef_Nondelegate(); return S_OK; }

#define ADDREF_NONDELEGATE	\
	STDMETHOD_(ULONG, AddRef_Nondelegate)()	\
	{ return InterlockedIncrement((LONG*)(LPVOID)&m_RefCount); }

#define RELEASE_NONDELEGATE	\
	STDMETHOD_(ULONG, Release_Nondelegate)()	\
	{ if(InterlockedDecrement((LONG*)(LPVOID)&m_RefCount)) return m_RefCount; delete this ; return 0; }

#define QI_INNER	\
	STDMETHOD(QueryInterface)(REFGUID riid, void **ppv)	\
	{ if(m_punkOuter) { return m_punkOuter->QueryInterface(riid, ppv); } else { return QueryInterface_Nondelegate(riid, ppv); } }

#define ADDREF_INNER	\
	STDMETHOD_(ULONG, AddRef)()	\
	{ if(m_punkOuter) { return m_punkOuter->AddRef(); } else { return AddRef_Nondelegate(); } }

#define RELEASE_INNER	\
	STDMETHOD_(ULONG, Release)()	\
	{ if(m_punkOuter) { return m_punkOuter->Release(); } else { return Release_Nondelegate(); } }

#define UNKNOWN_INNER_IMP_SPEC(entrys) \
	QIBEGIN_NONDELEGATE  QIUNKNOWN_NONDELEGATE  entrys  QIEND  ADDREF_NONDELEGATE  RELEASE_NONDELEGATE  QI_INNER  ADDREF_INNER  RELEASE_INNER

#define UNKNOWN_INNER_IMP_SPEC_(icast, entrys) \
	QIBEGIN_NONDELEGATE  QIUNKNOWN_NONDELEGATE_(icast)  entrys  QIEND  ADDREF_NONDELEGATE  RELEASE_NONDELEGATE  QI_INNER  ADDREF_INNER  RELEASE_INNER

#define UNKNOWN_INNER_IMP1(i) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i) )
#define UNKNOWN_INNER_IMP2(i1, i2) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_INNER_IMP3(i1, i2, i3) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_INNER_IMP4(i1, i2, i3, i4) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_INNER_IMP5(i1, i2, i3, i4, i5) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_INNER_IMP6(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_INNER_IMP7(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_INNER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )

#define UNKNOWN_INNER_IMP2_(i1, i2) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_INNER_IMP3_(i1, i2, i3) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_INNER_IMP4_(i1, i2, i3, i4) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_INNER_IMP5_(i1, i2, i3, i4, i5) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_INNER_IMP6_(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_INNER_IMP7_(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_INNER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )


class CUnknownImp_Outer
{
public:
	ULONG m_RefCount;
	CUnknownImp_Outer(): m_RefCount(0) {}
public:
	UTIL::com_ptr<IMSBase> m_punkInner;
	HRESULT init_class_outer(IMSBase *prot, IMSBase *punkThisOuter, const CLSID& rclsid)
	{
		using namespace UTIL;
		RASSERT(prot, E_INVALIDARG);

		com_ptr<IMscomRunningObjectTable> pRot(prot);
		RASSERT(pRot, E_INVALIDARG);

		com_ptr<IMsObjectLoader> pol;
		RFAILED(pRot->GetObject(CLSID_ObjectLoader, __uuidof(IMsObjectLoader), (void**)&pol));

		RFAILED(pol->CreateInstance(prot, rclsid, punkThisOuter, re_uuidof(IMSBase), (void**)&m_punkInner));
		
		return S_OK;
	}
};


#define QIEND_OUTER \
	return m_punkInner->QueryInterface(riid, ppv); }

#define UNKNOWN_OUTER_IMP_SPEC(entrys) \
	QIBEGIN  QIUNKNOWN  entrys  QIEND_OUTER  ADDREF  RELEASE

#define UNKNOWN_OUTER_IMP_SPEC_(icast, entrys) \
	QIBEGIN  QIUNKNOWN_(icast)  entrys  QIEND_OUTER  ADDREF  RELEASE

#define UNKNOWN_OUTER_IMP0 \
	QIBEGIN  QIUNKNOWN  QIEND_OUTER  ADDREF  RELEASE
#define UNKNOWN_OUTER_IMP1(i) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i) )
#define UNKNOWN_OUTER_IMP2(i1, i2) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_OUTER_IMP3(i1, i2, i3) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_OUTER_IMP4(i1, i2, i3, i4) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_OUTER_IMP5(i1, i2, i3, i4, i5) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_OUTER_IMP6(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_OUTER_IMP7(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_OUTER_IMP_SPEC( QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )
	
#define UNKNOWN_OUTER_IMP2_(i1, i2) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) )
#define UNKNOWN_OUTER_IMP3_(i1, i2, i3) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) )
#define UNKNOWN_OUTER_IMP4_(i1, i2, i3, i4) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) )
#define UNKNOWN_OUTER_IMP5_(i1, i2, i3, i4, i5) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) )
#define UNKNOWN_OUTER_IMP6_(i1, i2, i3, i4, i5, i6) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) )
#define UNKNOWN_OUTER_IMP7_(i1, i2, i3, i4, i5, i6, i7) \
	UNKNOWN_OUTER_IMP_SPEC_(i1, QIENTRY(i1) QIENTRY(i2) QIENTRY(i3) QIENTRY(i4) QIENTRY(i5) QIENTRY(i6) QIENTRY(i7) )
	

//////////////////////////////////////////////////////////////////////////

template<class CLS, class IFactory = IClassFactory>
class TClsFactory : public IFactory, public CUnknownImp
{
public: // IMSBase:
	UNKNOWN_IMP1(IClassFactory);

public: 
	// IClassFactory
	STDMETHOD (CreateInstance)( 
		/* [unique][in] */ IMSBase *punkOuter,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppv)
	{
		return create_instance(punkOuter, riid, ppv);
	}

	STDMETHOD(LockServer)(BOOL fLock)
	{
		return lock_server(fLock);
	}

public:
	static HRESULT create_instance(IMSBase *punkOuter, const IID& riid, void **ppv)
	{
		*ppv = 0;
		// aggregation validate:
		RASSERT(!punkOuter || re_uuidof(IMSBase) == riid, E_INVALIDARG);
		// create new object/aggregation:
		UTIL::sentry<CLS*> p(new CLS);
		RASSERT(p, E_UNEXPECTED);
		((IUnknown_Nondelegate*)(CLS*)p)->AddRef_Nondelegate(); // nondelegation, protect reference count
		if(punkOuter)
		{
			CUnknownImp_Inner* pInner = (CUnknownImp_Inner*)(p.m_p);
			RFAILED(pInner->init_class_inner(punkOuter));
		}
		HRESULT hr = ((IUnknown_Nondelegate*)(CLS*)p)->QueryInterface_Nondelegate(riid, ppv);
		((IUnknown_Nondelegate*)p.detach())->Release_Nondelegate(); // nondelegation, balance reference count or destroy.
		return hr;
	}

	static HRESULT lock_server(BOOL /*fLock*/)
	{
		return S_OK;
	}

	static HRESULT GetClassObject(const IID& riid, void **ppv)
	{
		return TClsFactory<TClsFactory<CLS> >::create_instance(0, riid, ppv);
	}
};


template<class CLS>
class TStdClsFactory : public TClsFactory<CLS, IMSClassFactory>
{
public: // IMSBase:
	UNKNOWN_IMP2(IMSClassFactory, IClassFactory);

public: // IMSClassFactory:
	STDMETHOD(CreateInstance)(IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv)
	{
		return create_instance(prot, punkOuter, riid, ppv);
	}

	// std factory invoke:
	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter)
	{
		return !punkOuter ? S_OK : E_INVALIDARG;
	}

	STDMETHOD(init_class_inner)(IMSBase* punkOuter)
	{
		return !punkOuter ? S_OK : E_INVALIDARG;
	}

	// IMSClassFactory:
	STDMETHOD_(CLSID, GetAt)(LONG nIndex) 
	{
		return g_mapClassObject[nIndex+1].clsid;
	}

	STDMETHOD_(LONG, GetCount)()
	{
		LONG lCount = sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]);

		return (lCount > 0)?lCount-1:0;
	}


	STDMETHOD_(LPCTSTR, ProgIDFromCLSID)(REFCLSID clsid)
	{
		for(size_t i = 1; i < sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]); ++i)
		{
			if(clsid == g_mapClassObject[i].clsid)
			{
				return g_mapClassObject[i].ProgID;
			}
		}
		return _T("");
	}

public:
	static HRESULT create_instance(IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv)
	{
		*ppv = 0;
		// aggregation validate:
		RASSERT(!punkOuter || re_uuidof(IMSBase) == riid, E_INVALIDARG);
		// create new object/aggregation:
		UTIL::sentry<CLS*> p(new CLS);
		RASSERT(p, E_UNEXPECTED);
		((IUnknown_Nondelegate*)(CLS*)p)->AddRef_Nondelegate(); // nondelegation, protect reference count
		RFAILED(p->init_class(prot, punkOuter));
		HRESULT hr = ((IUnknown_Nondelegate*)(CLS*)p)->QueryInterface_Nondelegate(riid, ppv);
		((IUnknown_Nondelegate*)p.detach())->Release_Nondelegate(); // nondelegation, balance reference count or destroy.
		return hr;
	}

	static HRESULT GetClassObject(const IID& riid, void **ppv)
	{
		return TStdClsFactory<TStdClsFactory<CLS> >::create_instance(0, 0, riid, ppv);
	}
};

class CNullObjcetUnkown :public IMSBase, private CUnknownImp
{
public:
	UNKNOWN_IMP1(IMSBase);

	// std factory invoke:
	STDMETHOD(init_class)(IMSBase* /*prot*/, IMSBase* punkOuter)
	{
		return !punkOuter ? S_OK : E_INVALIDARG;
	}

	STDMETHOD(init_class_inner)(IMSBase* punkOuter)
	{
		return !punkOuter ? S_OK : E_INVALIDARG;
	}
};



#define BEGIN_CLIDMAP \
	static const struct{ CLSID clsid; HRESULT (*pfnGetClassObject)(const IID&, void**);TCHAR ProgID[MAX_PROGIDLEN];} g_mapClassObject[] = {

#define CLIDMAPENTRY_BEGIN \
	{CLSID_MSClassFactory,&TStdClsFactory<CNullObjcetUnkown>::GetClassObject, _T("")},

#define CLIDMAPENTRY(CID,CLASS) \
	{CID, &TStdClsFactory<CLASS >::GetClassObject, _T("")},

#define CLIDMAPENTRY_NOROT(CID,CLASS) \
	{CID, &TClsFactory<CLASS >::GetClassObject, _T("")},

#define CLIDMAPENTRY_NOROT_PROGID(CID,CLASS,PROGID) \
{CID, &TClsFactory< CLASS >::GetClassObject, PROGID},

#define CLIDMAPENTRY_PROGID(CID,CLASS,PROGID) \
	{CID, &TStdClsFactory<CLASS >::GetClassObject, PROGID},


#define CLIDMAPENTRY_END 

#define END_CLIDMAP };

#define MARGIN_API  HRESULT __stdcall 
#define MARGIN_EXPORTS extern "C"  /*__declspec(dllexport)*/ MARGIN_API

#define DEFINE_ALL_EXPORTFUN \
	MARGIN_API MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);\
	MARGIN_API MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)\
{\
	for(size_t i = 0; i < sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]); ++i)\
	{\
		if(rclsid == g_mapClassObject[i].clsid)\
		{\
			return g_mapClassObject[i].pfnGetClassObject(riid, ppv);\
		}\
	}\
	return E_INVALIDARG;\
}\
\
MARGIN_API MSDllCanUnloadNow(void)\
{\
	return S_FALSE ;\
}\
\
MARGIN_API MSDllRegisterServer(void)\
{\
	RASSERT(g_hinstance , SELFREG_E_TYPELIB);\
	TCHAR strDllPath[MAX_PATH] = {0};\
	GetModuleFileName(g_hinstance,strDllPath , MAX_PATH);\
	for(size_t i = 0; i < sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]); ++i)\
	{\
		if (g_mapClassObject[i].clsid != GUID_NULL)\
		{\
			if(S_OK != STD__RegisterServer(g_mapClassObject[i].clsid,strDllPath,g_mapClassObject[i].ProgID,g_mapClassObject[i].ProgID,g_mapClassObject[i].ProgID))\
			{\
				return E_FAIL;\
			}\
		}\
	}\
	return S_OK;\
}\
\
MARGIN_API MSDllUnregisterServer(void)\
{\
	for(size_t i = 0; i < sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]); ++i)\
	{\
		if (g_mapClassObject[i].clsid != GUID_NULL)\
		{\
			STD__UnregisterServer(g_mapClassObject[i].clsid);\
		}\
	}\
	return S_OK;\
}

#define END_CLIDMAP_AND_EXPORTFUN }; \
	DEFINE_ALL_EXPORTFUN;


//////////////////////////////////////////////////////////////////////////

} //namespace mscom
} //namespace msdk

#ifndef GUID_NULL
MS_DEFINE_GUID(GUID_NULL, 
			0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
#endif


#endif // duplicate inclusion protection
