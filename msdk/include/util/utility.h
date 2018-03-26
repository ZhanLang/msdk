#pragma once

// #include <exception>
// #include <new>
#ifndef _RE_UTILITY_H_
#define _RE_UTILITY_H_



#define RFAILED(x) { HRESULT _h_r_ = (x); if(FAILED(_h_r_)) return _h_r_; }
#define RFAILEDV(x)	{ if(FAILED(x)) return; }
#define RFAILEDP(x, _h_r_)	{ if(FAILED(x)) return _h_r_; }
#define RFAILEDOK(x)  RFAILEDP(x, S_OK;)
#define RFAILEDESFALSE(x) RFAILEDP(x, S_FALSE;)
#define RFAILED2(x, hr) { HRESULT _h_r_ = (x); if (FAILED(_h_r_)) return _h_r_; if (hr == _h_r_) return hr; }

#define RASSERT(x, _h_r_) { if(!(x)) return _h_r_; }
#define RASSERTV(x) { if(!(x)) return; }
#define RASSERTP(x, _h_r_) { if(NULL==(x)) return _h_r_; }
#define RASSERTPV(x) RASSERTP(x, ; )
#define RASSERT2(x, _t) { if(!(x)) {_t;} }
#define TASSERT(x, e) { if (!(x)) throw (e); }
#define RTEST(x, _h_r_) { if((x)) return _h_r_; }


#define NOTIMPL { return E_NOTIMPL; }
#define NOTIMPL_ { ASSERT0; return E_NOTIMPL; }


#define SAFE_RELEASE(p) {if(p) p = INULL;}
#define SAFE_DELETE(p) {if (p) {delete p; p = NULL;}}
#define SAFE_DELETE_BUFFER(p){if (p) {delete[] p; p = NULL;}}
#define SAFE_CLOSEHANDLE(h){if(h) {CloseHandle(h);h=NULL;}}

#define SIZEOF_BUFFER(buf) (sizeof(buf)/sizeof(buf[0]))

#define _OLD 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
#ifndef _i64tot_s
#	define _i64tot_s(value,buffer,sizeInCharacters,radix) _i64tot(value,buffer,radix)
#endif

#ifndef _itot_s
#	define _itot_s(value,buffer,sizeInCharacters,radix) _itot(value,buffer,radix)
#endif
#ifndef _tcscpy_s
#	define _tcscpy_s(strDestination,numberOfElements,strSource) _tcscpy(strDestination,strSource)
#endif

#ifndef _tcscat_s
#	define _tcscat_s(strDestination, numberOfElements,strSource ) _tcscat(strDestination,strSource)
#endif

#ifndef _countof
#	define _countof(x) SIZEOF_BUFFER(buf) (sizeof(buf)/sizeof(buf[0]))
#endif

// #ifndef _tcsncpy_s
// #	define _tcsncpy_s(strDest, sizeInBytes,strSource,count) _tcsncpy(strDest,strSource,count)
// #endif

// #ifndef _tcsupr_s
// #	define _tcsupr_s(str,numberOfElements) _tcsupr(str)
// #endif

// #ifndef _strupr_s
// #	define _strupr_s(str,numberOfElements) _strupr(str)
// #endif

#ifndef _tcstok_s
#	define _tcstok_s(strToken,strDelimit,context) _tcstok(strToken,strDelimit)
#endif


// static void* operator new(size_t size) 
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	return HeapAlloc(hProcessHeap, 0, size);
// }

// static __declspec(nothrow) void*  operator new (size_t size)
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	return HeapAlloc(hProcessHeap, 0, size);
// 	return NULL;
// }

// static void* operator new (size_t size, const std::nothrow_t& nothrow_value) throw()
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	return HeapAlloc(hProcessHeap, 0, size);
// 
// 	return NULL;
// }

// static void * operator new[](size_t count)
// {	// try to allocate count bytes for an array
// 	return (operator new(count));
// }

// static void* operator new(size_t size) _THROW1(_STD bad_alloc)
// {
// 	return (operator new(size));
// }

// static void operator delete(void *pUserData)
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	HeapFree(hProcessHeap, HEAP_NO_SERIALIZE, pUserData);
// }

// static void operator delete[]( void * p )
// {
// 	operator delete(p);
// }

namespace msdk {
namespace UTIL { ;

struct default_sentry
{
	static void* default_value() { return 0; }
	template<class _Ptr> static bool equal_to(_Ptr l, _Ptr r) { return l == r; }
	template<class _Ptr> static void destroy(_Ptr p) { delete p; }
};

struct default_array_sentry
{
	static void* default_value() { return 0; }
	template<class _Ptr> static bool equal_to(_Ptr l, _Ptr r) { return l == r; }
	template<class _Ptr> static void destroy(_Ptr p) { delete [] p; }
};

struct co_interface_sentry
{
	static void* default_value() { return 0; }
	template<class _Ptr> static bool equal_to(_Ptr, _Ptr) { return false; }
	template<class _Ptr> static void destroy(_Ptr p) { if(p) p->Release(); }
};

struct handle_sentry
{
	static void* default_value() { return 0; }
	template<class _Ptr> static bool equal_to(_Ptr l, _Ptr r) { return l == r; }
	template<class _Ptr> static void destroy(_Ptr p) { if(p)CloseHandle(p); }
};
 
template<class _Ptr,
		 class _Traits = default_sentry>
struct sentry
{
public:
	sentry(const _Traits& tr = _Traits()) : m_tr(tr) { m_p = (_Ptr)m_tr.default_value(); }
	sentry(_Ptr p, const _Traits& tr = _Traits()) : m_p(p), m_tr(tr) {}
	~sentry() { m_tr.destroy(m_p); }
	sentry& operator = (_Ptr p) { if(!m_tr.equal_to(m_p, p)) { m_tr.destroy(m_p); m_p = p; } return *this; }
	_Ptr detach() { _Ptr tmp = m_p; m_p = (_Ptr)m_tr.default_value(); return tmp; }
	operator _Ptr () const { return m_p; }
	_Ptr operator -> () const { return m_p; }
	void reset(){m_p = (_Ptr)m_tr.default_value();}
//	_Ptr* operator & () { return &m_p; }
public:
	_Ptr m_p;
	_Traits m_tr;
private:
	sentry(const sentry&);
	sentry& operator = (const sentry&);
};


#ifndef IS_COMPILER_VC6
	#ifdef WIN32
		#if(_MSC_VER < 1300)
			#define IS_COMPILER_VC6 1
		#else
			#define IS_COMPILER_VC6 0
		#endif
	#else
		#define IS_COMPILER_VC6 0
	#endif
#endif



//comÖÇÄÜÖ¸Õë
template<class I>
struct com_ptr : sentry<I*, co_interface_sentry>
{
	typedef sentry<I*, co_interface_sentry> base;
	using base::m_p; // gcc 3.4
	// default construct:
	com_ptr() : base() {}
	// construct with:
	template<class U>
		com_ptr(const com_ptr<U>& rhs) : base() { if(rhs.m_p && FAILED(rhs.m_p->QueryInterface(re_uuidof(I), (void**)&m_p))) m_p = 0; } // gcc
#if(!IS_COMPILER_VC6)
	com_ptr(const com_ptr& rhs) : base() { if(rhs.m_p && FAILED(rhs.m_p->QueryInterface(re_uuidof(I), (void**)&m_p))) m_p = 0; } // gcc
	template<class U>
		com_ptr(const sentry<U*, co_interface_sentry>& rhs) : base() { if(rhs.m_p && FAILED(rhs.m_p->QueryInterface(re_uuidof(I), (void**)&m_p))) m_p = 0; } // gcc
#endif
	template<class U>
		com_ptr(U *p) : base() { if(p && FAILED(p->QueryInterface(re_uuidof(I), (void**)&m_p))) m_p = 0; }
	// operator =:
	template<class U>
		com_ptr& operator = (const com_ptr<U>& rhs) { if((void*)m_p != (void*)rhs.m_p) *this = rhs.m_p; return *this; }
#if(!IS_COMPILER_VC6)
	com_ptr& operator = (const com_ptr& rhs) { if(m_p != rhs.m_p) *this = rhs.m_p; return *this; }
	template<class U>
		com_ptr& operator = (const sentry<U*, co_interface_sentry>& rhs) { if((void*)m_p != (void*)rhs.m_p) *this = rhs.m_p; return *this; }
#endif
	template<class U>
		com_ptr& operator = (U *p) { if((void*)m_p == (void*)p) return *this; base::operator=(0); if(p && FAILED(p->QueryInterface(re_uuidof(I), (void**)&m_p))) m_p = 0; return *this; }
};

} // namespace UTIL
} //namespace msdk


#endif // duplicate inclusion protection
