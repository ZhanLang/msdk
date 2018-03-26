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

#include "stl_iterator.h"
#include "stl_workarounds.h"
#include "stl_basic_container.h"

namespace mluabind
{

namespace impl
{

template<class _T, class _C>
void STL_List_AssignWrapper(_C *_this, const _T& _t1, const _T& _t2)
{
	_this->assign(_t1.v, _t2.v);
};

template<class _T, class _C>
void STL_List_Insert1(_C *_this, const _T& _t, typename _C::size_type _sz, const typename _C::value_type& _v)
{
	_this->insert(_t.v, _sz, _v);
};

template<class _T, class _C>
_T STL_List_Insert2(_C *_this, const _T& _t, const typename _C::value_type& _v)
{
	return _this->insert(_t.v, _v);
};

}

template <class _T>
inline impl::DeclaratorClass &BindList(const char* _name)
{
	return Declare()
	+impl::BindBasicSTLContainer<_T>(_name)	
		.Constructor<_T::size_type>()
		.Constructor<_T::size_type, const _T::value_type&>()
		.Method("assign", (void (_T::*) (_T::size_type, const _T::value_type&) ) &_T::assign)
		.Method("assign", &impl::STL_String_AssignWrapper<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_String_AssignWrapper<impl::STLIteratorHolder<_T::const_iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_String_AssignWrapper<impl::STLIteratorHolder<_T::reverse_iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_String_AssignWrapper<impl::STLIteratorHolder<_T::const_reverse_iterator, typename _T::value_type>, _T>)
		.Method("back", &impl::STL_BackMethod<_T>)
		.Method("back", &impl::STL_BackMethodConst<_T>)
		.Method("front", &impl::STL_FrontMethod<_T>)
		.Method("front", &impl::STL_FrontMethodConst<_T>)
		.Method("insert", &impl::STL_List_Insert1<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("insert", &impl::STL_List_Insert2<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("max_size", &_T::max_size)
		.Method("merge", (void (_T::*) (_T&) ) &_T::merge)
		.Method("pop_back", &_T::pop_back)
		.Method("pop_front", &_T::pop_front)
//		.Method("push_back", &_T::push_back)
//		.Method("push_front", &_T::push_front)
		.Method("remove", &_T::remove)
		.Method("reverse", &_T::reverse)
//		.Method("resize", (void (_T::*) (_T::size_type, _T::value_type)) &_T::resize)
		.Method("resize", (void (_T::*) (_T::size_type)) &_T::resize)
	+(Declare(_name)
		+BindIterator<_T::iterator, impl::BindBasicSTLBidirectionalIterator, _T>()
		+BindConstIterator<_T::const_iterator, impl::BindBasicSTLBidirectionalIterator, _T>()
		+BindReverseIterator<_T::reverse_iterator, impl::BindBasicSTLBidirectionalIterator, _T>()
		+BindConstReverseIterator<_T::const_reverse_iterator, impl::BindBasicSTLBidirectionalIterator, _T>())
		;
};


}; // namespace mluabind
