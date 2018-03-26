// Copyright (c) 2007-2009 Michael Kazakov
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
/////////////////////////////////////////////////////////////////////
#pragma once

namespace mluabind
{

namespace impl
{

template <class T, class V>
struct STLIteratorHolder
{
	typedef V value_type;

	inline STLIteratorHolder(const T& _v) : v(_v) {};
	T v;
	inline bool operator ==(const STLIteratorHolder<T,V> &_r) const { return v == _r.v; };
	inline bool operator <(const STLIteratorHolder<T,V> &_r) const { return v < _r.v; };
	inline bool operator <=(const STLIteratorHolder<T,V> &_r) const { return v <= _r.v; };

	template <typename R>
	inline STLIteratorHolder<T,V> operator +(R _r) const { return v + _r; };
	template <typename R>
	inline STLIteratorHolder<T,V> operator -(R _r) const { return v - _r; };

	void inc() { ++v; };
	void dec() { --v; };
};

template <class _T>
inline typename STLReferenceReturn<typename _T::value_type>::Result IteratorGetter(const _T* _i)
{
	return *(_i->v);
};

template <class _T>
inline typename STLConstReferenceReturn<typename _T::value_type>::Result ConstIteratorGetter(const _T* _i)
{
	return *(_i->v);
};

template <class _T>
inline void IteratorSetter(_T* _i, typename MakeParameterType<typename _T::value_type>::type _p)
{
	*(_i->v) = _p;
};

template <class _T>
inline typename STLReferenceReturn<typename _T::value_type>::Result STL_AtMethod(_T *_c, typename _T::size_type _pos)
{
	return _c->at(_pos);
};

template <class _T>
inline typename STLConstReferenceReturn<typename _T::value_type>::Result STL_AtMethodConst(const _T *_c, typename _T::size_type _pos)
{
	return _c->at(_pos);
};

template <class _T>
inline typename STLReferenceReturn<typename _T::value_type>::Result STL_BackMethod(_T *_c)
{
	return _c->back();
};

template <class _T>
inline typename STLConstReferenceReturn<typename _T::value_type>::Result STL_BackMethodConst(const _T *_c)
{
	return _c->back();
};

template <class _T>
inline typename STLReferenceReturn<typename _T::value_type>::Result STL_FrontMethod(_T *_c)
{
	return _c->front();
};

template <class _T>
inline typename STLConstReferenceReturn<typename _T::value_type>::Result STL_FrontMethodConst(const _T *_c)
{
	return _c->front();
};

template <class _T>
inline typename STLReferenceReturn<typename _T::value_type>::Result STL_TopMethod(_T *_c)
{
	return _c->top();
};

template <class _T>
inline typename STLConstReferenceReturn<typename _T::value_type>::Result STL_TopMethodConst(const _T *_c)
{
	return _c->top();
};

}; //namespace impl

}; // namespace mluabind

