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
#include "stl_basic_container.h"

namespace mluabind
{

namespace impl
{

template<class _T, class _C>
void STL_String_AppendWrapper(_C *_this, const _T& _t1, const _T& _t2)
{
	_this->append(_t1.v, _t2.v);
};

template<class _T, class _C>
void STL_String_AssignWrapper(_C *_this, const _T& _t1, const _T& _t2)
{
	_this->assign(_t1.v, _t2.v);
};

template<class _T, class _C>
void STL_String_Insert1(_C *_this, const _T& _t, typename _C::size_type _sz, const typename _C::value_type& _v)
{
	_this->insert(_t.v, _sz, _v);
};

template<class _T, class _C>
_T STL_String_Insert2(_C *_this, const _T& _t, const typename _C::value_type& _v)
{
	return _this->insert(_t.v, _v);
};

}

template <class _T>
inline impl::DeclaratorClass &BindBasicString(const char* _name)
{
	return Declare()
	+impl::BindBasicSTLContainer<_T>(_name)	
		.Constructor<const _T::value_type*>()
		.Constructor<const _T::value_type*, _T::size_type>()
		.Constructor<const _T&, _T::size_type>()
		.Constructor<const _T&, _T::size_type, _T::size_type>()
		.Constructor<_T::size_type, _T::value_type>()
		.Method("append", (_T& (_T::*) (const _T::value_type *) ) &_T::append)
		.Method("append", (_T& (_T::*) (const _T::value_type *, _T::size_type) ) &_T::append)
		.Method("append", (_T& (_T::*) (const _T&, _T::size_type, _T::size_type) ) &_T::append)
		.Method("append", (_T& (_T::*) (const _T&) ) &_T::append)
		.Method("append", (_T& (_T::*) (_T::size_type, _T::value_type) ) &_T::append)
		.Method("append", &impl::STL_String_AppendWrapper<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("append", &impl::STL_String_AppendWrapper<impl::STLIteratorHolder<_T::const_iterator, typename _T::value_type>, _T>)
		.Method("append", &impl::STL_String_AppendWrapper<impl::STLIteratorHolder<_T::reverse_iterator, typename _T::value_type>, _T>)
		.Method("append", &impl::STL_String_AppendWrapper<impl::STLIteratorHolder<_T::const_reverse_iterator, typename _T::value_type>, _T>)
		.Method("assign", (_T& (_T::*) (const _T::value_type *) ) &_T::assign)
		.Method("assign", (_T& (_T::*) (const _T::value_type *, _T::size_type) ) &_T::assign)
		.Method("assign", (_T& (_T::*) (const _T&, _T::size_type, _T::size_type) ) &_T::assign)
		.Method("assign", (_T& (_T::*) (const _T&) ) &_T::assign)
		.Method("assign", (_T& (_T::*) (_T::size_type, _T::value_type) ) &_T::assign)
		.Method("assign", &impl::STL_Vector_AssignWrapper<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_Vector_AssignWrapper<impl::STLIteratorHolder<_T::const_iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_Vector_AssignWrapper<impl::STLIteratorHolder<_T::reverse_iterator, typename _T::value_type>, _T>)
		.Method("assign", &impl::STL_Vector_AssignWrapper<impl::STLIteratorHolder<_T::const_reverse_iterator, typename _T::value_type>, _T>)
		.Method("at", &impl::STL_AtMethod<_T>)
		.Method("at", &impl::STL_AtMethodConst<_T>)
		.Method("c_str", &_T::c_str)
		.Method("capacity", &_T::capacity)
		.Method("compare", (int (_T::*) (const _T&) const) &_T::compare)
		.Method("compare", (int (_T::*) (_T::size_type, _T::size_type, const _T&) const ) &_T::compare)
		.Method("compare", (int (_T::*) (const _T::value_type *) const) &_T::compare)
		.Method("compare", (int (_T::*) (_T::size_type, _T::size_type, const _T::value_type *) const ) &_T::compare)
		.Method("erase", (_T& (_T::*) (_T::size_type, _T::size_type) ) &_T::erase)
		.Method("find", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::find)
		.Method("find", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::find)
		.Method("find", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::find)
		.Method("find_first_not_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::find_first_not_of)
		.Method("find_first_not_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::find_first_not_of)
		.Method("find_first_not_of", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::find_first_not_of)
		.Method("find_first_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::find_first_of)
		.Method("find_first_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::find_first_of)
		.Method("find_first_of", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::find_first_of)
		.Method("find_last_not_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::find_last_not_of)
		.Method("find_last_not_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::find_last_not_of)
		.Method("find_last_not_of", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::find_last_not_of)
		.Method("find_last_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::find_last_of)
		.Method("find_last_of", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::find_last_of)
		.Method("find_last_of", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::find_last_of)
		.Method("insert", (_T& (_T::*) (_T::size_type, const _T::value_type *) ) &_T::insert)
		.Method("insert", (_T& (_T::*) (_T::size_type, const _T::value_type *, _T::size_type) ) &_T::insert)
		.Method("insert", (_T& (_T::*) (_T::size_type, const _T&) ) &_T::insert)
		.Method("insert", (_T& (_T::*) (_T::size_type, const _T&, _T::size_type, _T::size_type) ) &_T::insert)
		.Method("insert", (_T& (_T::*) (_T::size_type, _T::size_type, _T::value_type) ) &_T::insert)
		.Method("insert", &impl::STL_String_Insert1<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("insert", &impl::STL_String_Insert2<impl::STLIteratorHolder<_T::iterator, typename _T::value_type>, _T>)
		.Method("length", &_T::length)
		.Method("max_size", &_T::max_size)
		.Method("push_back", &_T::push_back)
		.Method("replace", (_T& (_T::*) (_T::size_type, _T::size_type, const _T::value_type *) ) &_T::replace)
		.Method("replace", (_T& (_T::*) (_T::size_type, _T::size_type, const _T&) ) &_T::replace)
		.Method("replace", (_T& (_T::*) (_T::size_type, _T::size_type, const _T::value_type *, _T::size_type) ) &_T::replace)
		.Method("replace", (_T& (_T::*) (_T::size_type, _T::size_type, _T::size_type, _T::value_type) ) &_T::replace)
		.Method("reserve", &_T::reserve)
		.Method("resize", (void (_T::*) (_T::size_type, _T::value_type)) &_T::resize)
		.Method("rfind", (_T::size_type (_T::*) (_T::value_type, _T::size_type) const ) &_T::rfind)
		.Method("rfind", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type) const ) &_T::rfind)
		.Method("rfind", (_T::size_type (_T::*) (const _T::value_type*, _T::size_type, _T::size_type) const ) &_T::rfind)
		.Method("rfind", (_T::size_type (_T::*) (const _T&, _T::size_type) const ) &_T::rfind)
		.Method("substr", (_T (_T::*) (_T::size_type, _T::size_type)) &_T::substr)
		.Operator(Str, &_T::c_str)
		.Operator(const_self + const_self)
		.Operator(const_self == const_self)
		.Operator(const_self == other<const _T::value_type*>())
		.Operator(other<const _T::value_type*>() == const_self)
		.Operator(impl::BinaryOperatorBaseHolder<ConCat, true, true, const _T>())
		.Operator(impl::BinaryOperatorBaseHolder<ConCat, true, true, const _T::value_type*>())
		.Operator(impl::BinaryOperatorBaseHolder<ConCat, true, false, const _T::value_type*>())
		.Enum("npos", ~(size_t)0)
		// Dangerous behaviour place!
		// This workaround included since STLPort defines npos as -1,
		// but we need 0xFFF... to convert it into double
	+(Declare(_name)
		+BindIterator<_T::iterator, impl::BindBasicSTLRandomAccessIterator, _T>()
		+BindConstIterator<_T::const_iterator, impl::BindBasicSTLRandomAccessIterator, _T>()
		+BindReverseIterator<_T::reverse_iterator, impl::BindBasicSTLRandomAccessIterator, _T>()
		+BindConstReverseIterator<_T::const_reverse_iterator, impl::BindBasicSTLRandomAccessIterator, _T>());
};

}; // namespace mluabind
