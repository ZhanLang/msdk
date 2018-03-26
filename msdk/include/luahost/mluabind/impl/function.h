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

#include "lokiinclude.h"
#include "declarator.h"

namespace mluabind
{

namespace impl
{

/////////////////////////////////////////////////////////////////////////// GENERIC FUNCTION
struct GenericFunction : DeclaratedClass
{
	GenericFunction(const char* _luaname,
		int (*_pm)(lua_State *L, int, int, bool),
		String (*_pi)(CHost *host),
		const GenericMethodPolicy& _p);
	int PerformCall(CHost* _host, lua_State *L);
	virtual int PerformCallV(CHost* _host, lua_State *L) = 0;
	int MatchParams(lua_State *L);
	String GetVerboseName(lua_State *L) const;
	int (*m_ParameterMatcher)(lua_State *L, int, int, bool);
	String (*m_ParametersInfo)(CHost *host);
	GenericMethodPolicy *m_Policy;
};

#define __MLUABIND_CLASSES_LIST_0  class _Ret
#define __MLUABIND_CLASSES_LIST_1 __MLUABIND_CLASSES_LIST_0, class _P1
#define __MLUABIND_CLASSES_LIST_2 __MLUABIND_CLASSES_LIST_1, class _P2
#define __MLUABIND_CLASSES_LIST_3 __MLUABIND_CLASSES_LIST_2, class _P3
#define __MLUABIND_CLASSES_LIST_4 __MLUABIND_CLASSES_LIST_3, class _P4
#define __MLUABIND_CLASSES_LIST_5 __MLUABIND_CLASSES_LIST_4, class _P5
#define __MLUABIND_CLASSES_LIST_6 __MLUABIND_CLASSES_LIST_5, class _P6
#define __MLUABIND_CLASSES_LIST_7 __MLUABIND_CLASSES_LIST_6, class _P7
#define __MLUABIND_CLASSES_LIST_8 __MLUABIND_CLASSES_LIST_7, class _P8
#define __MLUABIND_CLASSES_LIST_9 __MLUABIND_CLASSES_LIST_8, class _P9
#define __MLUABIND_PARAM_LIST_0 _Ret
#define __MLUABIND_PARAM_LIST_1 __MLUABIND_PARAM_LIST_0, _P1
#define __MLUABIND_PARAM_LIST_2 __MLUABIND_PARAM_LIST_1, _P2
#define __MLUABIND_PARAM_LIST_3 __MLUABIND_PARAM_LIST_2, _P3
#define __MLUABIND_PARAM_LIST_4 __MLUABIND_PARAM_LIST_3, _P4
#define __MLUABIND_PARAM_LIST_5 __MLUABIND_PARAM_LIST_4, _P5
#define __MLUABIND_PARAM_LIST_6 __MLUABIND_PARAM_LIST_5, _P6
#define __MLUABIND_PARAM_LIST_7 __MLUABIND_PARAM_LIST_6, _P7
#define __MLUABIND_PARAM_LIST_8 __MLUABIND_PARAM_LIST_7, _P8
#define __MLUABIND_PARAM_LIST_9 __MLUABIND_PARAM_LIST_8, _P9
#define __MLUABIND_MPARAM_LIST_0
#define __MLUABIND_MPARAM_LIST_1 _P1
#define __MLUABIND_MPARAM_LIST_2 __MLUABIND_MPARAM_LIST_1, _P2
#define __MLUABIND_MPARAM_LIST_3 __MLUABIND_MPARAM_LIST_2, _P3
#define __MLUABIND_MPARAM_LIST_4 __MLUABIND_MPARAM_LIST_3, _P4
#define __MLUABIND_MPARAM_LIST_5 __MLUABIND_MPARAM_LIST_4, _P5
#define __MLUABIND_MPARAM_LIST_6 __MLUABIND_MPARAM_LIST_5, _P6
#define __MLUABIND_MPARAM_LIST_7 __MLUABIND_MPARAM_LIST_6, _P7
#define __MLUABIND_MPARAM_LIST_8 __MLUABIND_MPARAM_LIST_7, _P8
#define __MLUABIND_MPARAM_LIST_9 __MLUABIND_MPARAM_LIST_8, _P9
#define __MLUABIND_CPM_0 
#define __MLUABIND_CPM_1 ParameterMatcher<_P1>::ExtractParam(L, 1)
#define __MLUABIND_CPM_2 __MLUABIND_CPM_1, ParameterMatcher<_P2>::ExtractParam(L, 2)
#define __MLUABIND_CPM_3 __MLUABIND_CPM_2, ParameterMatcher<_P3>::ExtractParam(L, 3)
#define __MLUABIND_CPM_4 __MLUABIND_CPM_3, ParameterMatcher<_P4>::ExtractParam(L, 4)
#define __MLUABIND_CPM_5 __MLUABIND_CPM_4, ParameterMatcher<_P5>::ExtractParam(L, 5)
#define __MLUABIND_CPM_6 __MLUABIND_CPM_5, ParameterMatcher<_P6>::ExtractParam(L, 6)
#define __MLUABIND_CPM_7 __MLUABIND_CPM_6, ParameterMatcher<_P7>::ExtractParam(L, 7)
#define __MLUABIND_CPM_8 __MLUABIND_CPM_7, ParameterMatcher<_P8>::ExtractParam(L, 8)
#define __MLUABIND_CPM_9 __MLUABIND_CPM_8, ParameterMatcher<_P9>::ExtractParam(L, 9)


#define __MLUABIND_MAKEFUNCTIONCLASSX(_N)\
template <__MLUABIND_CLASSES_LIST_##_N>\
struct FunctionClass##_N : GenericFunction\
{\
	_Ret (*m_F) ( __MLUABIND_MPARAM_LIST_##_N);\
	typedef FunctionClass##_N<__MLUABIND_PARAM_LIST_##_N> Me;\
	inline FunctionClass##_N(const char* _name, _Ret (_F) (__MLUABIND_MPARAM_LIST_##_N), const GenericMethodPolicy& _p):\
		GenericFunction(\
			_name,\
			&MultipleParameterMatcher##_N<__MLUABIND_MPARAM_LIST_##_N>::MatchParams,\
			&ParametersVerboseInfo##_N<__MLUABIND_MPARAM_LIST_##_N>::Info,\
			_p),\
		m_F(_F)\
	{};\
	virtual int PerformCallV(CHost* _host, lua_State *L)\
	{\
		return HackVoid<std::tr1::is_void<_Ret>::value>::Do(_host, L, this);\
	};\
	template <bool _> struct HackVoid\
	{\
		inline static int Do(CHost* _host, lua_State *L, Me *_this)\
		{\
			return CreateCustomLuaVariable<ResolvCallType<_Ret>::CallType, _Ret>\
				::Do(L, _host, _this->m_F(__MLUABIND_CPM_##_N), ResolvCallType<_Ret>::IsConst);\
		};\
	};\
	template <> struct HackVoid<true>\
	{\
		inline static int Do(CHost* _host, lua_State *L, Me *_this)\
		{\
			_this->m_F(__MLUABIND_CPM_##_N);\
			return 0;\
		};\
	};\
};

__MLUABIND_MAKEFUNCTIONCLASSX(0)
__MLUABIND_MAKEFUNCTIONCLASSX(1)
__MLUABIND_MAKEFUNCTIONCLASSX(2)
__MLUABIND_MAKEFUNCTIONCLASSX(3)
__MLUABIND_MAKEFUNCTIONCLASSX(4)
__MLUABIND_MAKEFUNCTIONCLASSX(5)
__MLUABIND_MAKEFUNCTIONCLASSX(6)
__MLUABIND_MAKEFUNCTIONCLASSX(7)
__MLUABIND_MAKEFUNCTIONCLASSX(8)
__MLUABIND_MAKEFUNCTIONCLASSX(9)

}; // namespace impl

/////////////////////////////////////////////////////////////////////////// WRAPPER FUNCTIONS
template < class _Ret >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass0<_Ret> (_name, _F, _p)); };

template < class _Ret, class _P1 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass1<_Ret, _P1> (_name, _F, _p)); };

template < class _Ret, class _P1, class _P2 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass2<_Ret, _P1, _P2> (_name, _F, _p)); };

template < class _Ret, class _P1, class _P2, class _P3 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass3<_Ret, _P1, _P2, _P3> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass4<_Ret, _P1, _P2, _P3, _P4> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4, _P5), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass5<_Ret, _P1, _P2, _P3, _P4, _P5> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4, _P5, _P6), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass6<_Ret, _P1, _P2, _P3, _P4, _P5, _P6> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass7<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass8<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8> (_name, _F, _p));};

template < class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9 >
inline impl::GenericFunction &Function(const char* _name, _Ret (_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9), const impl::GenericMethodPolicy &_p = impl::GenericMethodPolicy(0)){
	return *(new impl::FunctionClass9<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9> (_name, _F, _p));};


#undef __MLUABIND_CLASSES_LIST_0
#undef __MLUABIND_CLASSES_LIST_1
#undef __MLUABIND_CLASSES_LIST_2
#undef __MLUABIND_CLASSES_LIST_3
#undef __MLUABIND_CLASSES_LIST_4
#undef __MLUABIND_CLASSES_LIST_5
#undef __MLUABIND_CLASSES_LIST_6
#undef __MLUABIND_CLASSES_LIST_7
#undef __MLUABIND_CLASSES_LIST_8
#undef __MLUABIND_CLASSES_LIST_9
#undef __MLUABIND_PARAM_LIST_0
#undef __MLUABIND_PARAM_LIST_1
#undef __MLUABIND_PARAM_LIST_2
#undef __MLUABIND_PARAM_LIST_3
#undef __MLUABIND_PARAM_LIST_4
#undef __MLUABIND_PARAM_LIST_5
#undef __MLUABIND_PARAM_LIST_6
#undef __MLUABIND_PARAM_LIST_7
#undef __MLUABIND_PARAM_LIST_8
#undef __MLUABIND_PARAM_LIST_9
#undef __MLUABIND_CPM_0 
#undef __MLUABIND_CPM_1
#undef __MLUABIND_CPM_2
#undef __MLUABIND_CPM_3
#undef __MLUABIND_CPM_4
#undef __MLUABIND_CPM_5
#undef __MLUABIND_CPM_6
#undef __MLUABIND_CPM_7
#undef __MLUABIND_CPM_8
#undef __MLUABIND_CPM_9
#undef __MLUABIND_MPARAM_LIST_0
#undef __MLUABIND_MPARAM_LIST_1
#undef __MLUABIND_MPARAM_LIST_2
#undef __MLUABIND_MPARAM_LIST_3
#undef __MLUABIND_MPARAM_LIST_4
#undef __MLUABIND_MPARAM_LIST_5
#undef __MLUABIND_MPARAM_LIST_6
#undef __MLUABIND_MPARAM_LIST_7
#undef __MLUABIND_MPARAM_LIST_8
#undef __MLUABIND_MPARAM_LIST_9

}; // namespace mluabind
