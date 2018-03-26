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

// _T should be something like STLIteratorHolder<R::iterator>
template <class _T, class _C>
struct BindBasicSTLBidirectionalIterator
{
	inline static impl::CustomClass<_T> &Do(const char* _name)
	{
		return Class<_T>(_name)
			.Operator(const_self == const_self)
			.Method("inc", &_T::inc)
			.Method("dec", &_T::dec);
	};
};

template <class _T, class _C>
struct BindBasicSTLRandomAccessIterator
{
	inline static impl::CustomClass<_T> &Do(const char* _name)
	{
		using namespace std;
		return BindBasicSTLBidirectionalIterator<_T, _C>::Do(_name)
			.Operator(const_self <  const_self)
			.Operator(const_self <= const_self)
			.Operator(const_self + _C::difference_type())
			.Operator(const_self - _C::difference_type());
	};
};

}; // namespace impl

template <class _T, template <class _TT, class _CC> class _IteratorType, class _C>
inline impl::GenericClass &BindIterator()
{
	return _IteratorType< impl::STLIteratorHolder<_T, typename _C::value_type>, _C >::Do("iterator")
		.Member("value", &impl::IteratorGetter<impl::STLIteratorHolder<_T, typename _C::value_type> >, &impl::IteratorSetter<impl::STLIteratorHolder<_T, typename _C::value_type> >);
};

template <class _T, template <class _TT, class _CC> class _IteratorType, class _C>
inline impl::GenericClass &BindConstIterator()
{
	return _IteratorType< impl::STLIteratorHolder<_T, typename _C::value_type>, _C >::Do("const_iterator")
		.Member("value", &impl::ConstIteratorGetter<impl::STLIteratorHolder<_T, typename _C::value_type> >);
};

template <class _T, template <class _TT, class _CC> class _IteratorType, class _C>
inline impl::GenericClass &BindReverseIterator()
{
	return _IteratorType< impl::STLIteratorHolder<_T, typename _C::value_type>, _C >::Do("reverse_iterator")
		.Member("value", &impl::IteratorGetter<impl::STLIteratorHolder<_T, typename _C::value_type> >, &impl::IteratorSetter<impl::STLIteratorHolder<_T, typename _C::value_type> >);
};

template <class _T, template <class _TT, class _CC> class _IteratorType, class _C>
inline impl::GenericClass &BindConstReverseIterator()
{
	return _IteratorType< impl::STLIteratorHolder<_T, typename _C::value_type>, _C >::Do("const_reverse_iterator")
		.Member("value", &impl::ConstIteratorGetter<impl::STLIteratorHolder<_T, typename _C::value_type> >);
};

}; // namespace mluabind
