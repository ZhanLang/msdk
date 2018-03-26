
//=============================================================================

#ifndef _BASETYPES_INCLUDE_H_
#define _BASETYPES_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif

namespace msdk {
//-----------------------------------------------------------------------------
// 目的: 方便使用一个临时的堆内存
//-----------------------------------------------------------------------------
template<class T>
class CTempMem {
	typedef T*	PT;
	typedef const T*	PCT;
	PT p;
	size_t m_iSize;
public:
	CTempMem(long iSize):m_iSize(iSize) {
		// 自动尾部加一个,方便标记和使用整个内存
		p = new T[iSize+1];
	}
	CTempMem(PCT t, long iSize):m_iSize(iSize) {
		// 自动尾部加一个,方便标记和使用整个内存
		p = new T[iSize+1];
		memcpy(p, t, m_iSize*sizeof(T));
		// 自动尾部清零
		p[m_iSize] = 0;
	}
	~CTempMem() {
		delete []p;
	}
	operator PT() const {
		return p;
	}
	operator void*() const {
		return (void*)p;
	}
	PT& operator=(PT t) {
		memcpy(p, t, m_iSize*sizeof(T));
		// 自动尾部清零
		p[m_iSize] = 0;
		return p;
	}
	T& operator[](int nIndex) const {
		return p[nIndex];
	}
	inline size_t Len() {
		return m_iSize;
	}
};

} //namespace msdk

#endif // _BASETYPES_INCLUDE_H_
