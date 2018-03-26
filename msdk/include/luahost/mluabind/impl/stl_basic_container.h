// Copyright (c) 2007-2008 Michael Kazakov
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

#include "stl_workarounds.h"

namespace mluabind
{

namespace impl
{

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_BeginWrapper(_C *_this)
{
	return _this->begin();
};

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_EndWrapper(_C *_this)
{
	return _this->end();
};

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_RBeginWrapper(_C *_this)
{
	return _this->rbegin();
};

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_REndWrapper(_C *_this)
{
	return _this->rend();
};

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_EraseWrapper1(_C *_this, const STLIteratorHolder<_T, typename _C::value_type>& _t)
{
	return _this->erase(_t.v);
};

template<class _T, class _C>
STLIteratorHolder<_T, typename _C::value_type> STL_EraseWrapper2(_C *_this, const STLIteratorHolder<_T, typename _C::value_type>& _t1,  const STLIteratorHolder<_T, typename _C::value_type>& _t2)
{
	return _this->erase(_t1.v, _t2.v);
};

template<class _T1, class _T2, class _C>
void STL_InsertWrapper(_C *_this, const STLIteratorHolder<_T1, typename _C::value_type>& _t1,  const STLIteratorHolder<_T2, typename _C::value_type>& _t2,  const STLIteratorHolder<_T2, typename _C::value_type>& _t3)
{
	_this->insert(_t1.v, _t2.v, _t3.v);
};

template <class _T>
inline impl::CustomClass<_T> &BindBasicSTLContainer(const char* _name)
{
//	OutputDebugStringA(typeid(_T).name());
	return Class<_T>(_name)
		.Constructor()
		.Constructor<const _T&>()
		.Method("begin", &STL_BeginWrapper<_T::iterator, _T> )
		.Method("begin", &STL_BeginWrapper<_T::const_iterator, const _T>)
		.Method("clear", &_T::clear)
		.Method("empty", &_T::empty)
		.Method("_end", &STL_EndWrapper<_T::iterator, _T> )
		.Method("_end", &STL_EndWrapper<_T::const_iterator, const _T>)
		.Method("erase", &STL_EraseWrapper1<_T::iterator, _T>)
		.Method("erase", &STL_EraseWrapper2<_T::iterator, _T>)
		.Method("insert", &STL_InsertWrapper<_T::iterator, _T::iterator, _T>)
		.Method("insert", &STL_InsertWrapper<_T::iterator, _T::const_iterator, _T>)
		.Method("insert", &STL_InsertWrapper<_T::iterator, _T::reverse_iterator, _T>)
		.Method("insert", &STL_InsertWrapper<_T::iterator, _T::const_reverse_iterator, _T>)
		.Method("rbegin", &STL_RBeginWrapper<_T::reverse_iterator, _T> )
		.Method("rbegin", &STL_RBeginWrapper<_T::const_reverse_iterator, const _T>)
		.Method("rend", &STL_REndWrapper<_T::reverse_iterator, _T> )
		.Method("rend", &STL_REndWrapper<_T::const_reverse_iterator, const _T>)
		.Method("size", &_T::size)
//		.Method("swap", &_T::swap)
		;
};

}; // namespace impl


}; // namespace mluabind
