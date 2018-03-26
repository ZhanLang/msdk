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

//                   +      -    *    /    %    -   ==  <   <=  __tostring __concat __call
enum OperatorTypes{ Add=0, Sub, Mul, Div, Mod, Unm, Eq, Lt, Le, Str,       ConCat,  Call,  op_amount };

namespace impl
{
extern char *g_LuaOperatorNames[mluabind::op_amount];
};

namespace impl
{

struct SelfTypeTag {};
struct ConstSelfTypeTag {};
template <int _Op, bool _const_self, bool _first_self, class _Other>
struct BinaryOperatorBaseHolder{};

template <class _Base, int _Op, bool _Const, bool _First, class _Other>
struct BinaryOperatorGenerator : GenericMethod
{
	template <bool b> struct ConstResolver { typedef _Base Result; };
	template <> struct ConstResolver<true> { typedef const _Base Result; };
	typedef typename ConstResolver<_Const>::Result MyType;

	typedef typename Loki::Select<std::tr1::is_same<_Other, SelfTypeTag>::value,
		_Base, 
			typename Loki::Select<std::tr1::is_same<_Other, ConstSelfTypeTag>::value,
			const _Base, _Other>::Result >::Result Other;

	template <bool b> struct MakeTL { 
		typedef typename MakeParameterType<Other>::type First;
		typedef typename MyType& Second;
		};
	template <> struct MakeTL<true> {
		typedef typename MakeParameterType<Other>::type Second;
		typedef typename MyType& First;
	};

	template <int _IOp, bool _IFirst> struct Impl {};
#define __MLUABIND_MAKEIMPL(_OP, _COP)\
	template <> struct Impl<_OP, true> {\
		static inline int Do(CHost* _host, lua_State *L){\
			return TemplateCreateCLV(_host, L,\
				ParameterMatcher<MyType&>::ExtractParam(L, 1) _COP \
				ParameterMatcher<typename MakeParameterType<Other>::type>::ExtractParam(L, 2) ); }; };\
	template <> struct Impl<_OP, false> {\
		static inline int Do(CHost* _host, lua_State *L) {\
			return TemplateCreateCLV(_host, L,\
				ParameterMatcher<typename MakeParameterType<Other>::type>::ExtractParam(L, 1) _COP \
				ParameterMatcher<MyType&>::ExtractParam(L, 2) ); }; };

__MLUABIND_MAKEIMPL(Add, +)
__MLUABIND_MAKEIMPL(ConCat, +)
__MLUABIND_MAKEIMPL(Sub, -)
__MLUABIND_MAKEIMPL(Mul, *)
__MLUABIND_MAKEIMPL(Div, /)
__MLUABIND_MAKEIMPL(Mod, %)
__MLUABIND_MAKEIMPL(Eq, ==)
__MLUABIND_MAKEIMPL(Lt, <)
__MLUABIND_MAKEIMPL(Le, <=)

	inline BinaryOperatorGenerator():
		GenericMethod(
			g_LuaOperatorNames[_Op],
			_Const,
			&MultipleParameterMatcher2<typename MakeTL<_First>::First, typename MakeTL<_First>::Second>::MatchParams,
			&ParametersVerboseInfo2<typename MakeTL<_First>::First, typename MakeTL<_First>::Second>::Info,
			GenericMethodPolicy(0),
			false)
	{
	};

	virtual int PerformCallV(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const
	{
		HostImplicitCreatedStackFrameGuard g(_host);
		return Impl<_Op, _First>::Do(_host, L);
	};
};

}; // namespace impl

template <class _T> struct other{};
extern impl::SelfTypeTag self;
extern impl::ConstSelfTypeTag const_self;

#define __MLUABIND_MAKEBINOPERATOR(_OP, _COP)\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, false, true, _R>\
operator _COP(const impl::SelfTypeTag&, const other<_R>&) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, true, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, true, true, _R>\
operator _COP(const impl::ConstSelfTypeTag&, const other<_R>&) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, true, _R>(); };\
\
inline impl::BinaryOperatorBaseHolder<_OP, false, true, impl::SelfTypeTag>\
operator _COP(const impl::SelfTypeTag&, const impl::SelfTypeTag&) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, true, impl::SelfTypeTag>(); };\
\
inline impl::BinaryOperatorBaseHolder<_OP, true, true, impl::ConstSelfTypeTag>\
operator _COP(const impl::ConstSelfTypeTag&, const impl::ConstSelfTypeTag&) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, true, impl::ConstSelfTypeTag>(); };\
\
inline impl::BinaryOperatorBaseHolder<_OP, false, true, impl::ConstSelfTypeTag>\
operator _COP(const impl::SelfTypeTag&, const impl::ConstSelfTypeTag&) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, true, impl::ConstSelfTypeTag>(); };\
\
inline impl::BinaryOperatorBaseHolder<_OP, true, true, impl::SelfTypeTag>\
operator _COP(const impl::ConstSelfTypeTag&, const impl::SelfTypeTag&) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, true, impl::SelfTypeTag>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, false, false, _R>\
operator _COP(const other<_R>&, const impl::SelfTypeTag &a) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, false, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, true, false, _R>\
operator _COP(const other<_R>&, const impl::ConstSelfTypeTag &a) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, false, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, false, true, _R>\
operator _COP(const impl::SelfTypeTag&, const _R&) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, true, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, true, true, _R>\
operator _COP(const impl::ConstSelfTypeTag&, const _R&) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, true, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, false, false, _R>\
operator _COP(const _R&, const impl::SelfTypeTag &a) {\
	return impl::BinaryOperatorBaseHolder<_OP, false, false, _R>(); };\
\
template<class _R> inline impl::BinaryOperatorBaseHolder<_OP, true, false, _R>\
operator _COP(const _R&, const impl::ConstSelfTypeTag &a) {\
	return impl::BinaryOperatorBaseHolder<_OP, true, false, _R>(); };

__MLUABIND_MAKEBINOPERATOR(Add, +)
__MLUABIND_MAKEBINOPERATOR(Sub, -)
__MLUABIND_MAKEBINOPERATOR(Mul, *)
__MLUABIND_MAKEBINOPERATOR(Div, /)
__MLUABIND_MAKEBINOPERATOR(Mod, %)
__MLUABIND_MAKEBINOPERATOR(Eq, ==)
__MLUABIND_MAKEBINOPERATOR(Lt, <)
__MLUABIND_MAKEBINOPERATOR(Le, <=)

#undef __MLUABIND_MAKEBINOPERATOR
#undef __MLUABIND_MAKEIMPL

}; // namespace mluabind

