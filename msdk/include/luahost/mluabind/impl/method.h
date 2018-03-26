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

#include "policies.h"
#include "util.h"

namespace mluabind
{

namespace impl
{

struct GenericMethod
{
	virtual ~GenericMethod();
	int PerformCall(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const;
	int MatchParams(lua_State *L, LuaCustomVariable *_lcv) const;
	String GetVerboseName(lua_State *L) const;

protected:
	GenericMethod(
			const String& _luaname,
			bool _is_const,
			int (*_pm)(lua_State *L, int, int, bool),
			String (*_pi)(CHost *host),
			const GenericMethodPolicy& _p,
			bool _is_opmeth);
	virtual int PerformCallV(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const;

private:
	bool m_IsConst; // method is const?
	bool m_IsOperatorMethod; // was current GM binded as .Operator(XXX, &MyClass::SomeMethod) ?
	String m_LuaName;
	int (*m_ParameterMatcher)(lua_State *L, int, int, bool);
	String (*m_ParametersInfo)(CHost *host);
	GenericMethodPolicy *m_Policy;
};

#define __MLUABIND_CLASSES_LIST_0 class _Base, bool _Const, class _Ret
#define __MLUABIND_CLASSES_LIST_1 __MLUABIND_CLASSES_LIST_0, class _P1
#define __MLUABIND_CLASSES_LIST_2 __MLUABIND_CLASSES_LIST_1, class _P2
#define __MLUABIND_CLASSES_LIST_3 __MLUABIND_CLASSES_LIST_2, class _P3
#define __MLUABIND_CLASSES_LIST_4 __MLUABIND_CLASSES_LIST_3, class _P4
#define __MLUABIND_CLASSES_LIST_5 __MLUABIND_CLASSES_LIST_4, class _P5
#define __MLUABIND_CLASSES_LIST_6 __MLUABIND_CLASSES_LIST_5, class _P6
#define __MLUABIND_CLASSES_LIST_7 __MLUABIND_CLASSES_LIST_6, class _P7
#define __MLUABIND_CLASSES_LIST_8 __MLUABIND_CLASSES_LIST_7, class _P8
#define __MLUABIND_CLASSES_LIST_9 __MLUABIND_CLASSES_LIST_8, class _P9
#define __MLUABIND_PARAM_LIST_0 _Base, _Const, _Ret
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
#define __MLUABIND_GMPARAM_LIST_0 _Base*
#define __MLUABIND_GMPARAM_LIST_1 __MLUABIND_GMPARAM_LIST_0, _P1
#define __MLUABIND_GMPARAM_LIST_2 __MLUABIND_GMPARAM_LIST_1, _P2
#define __MLUABIND_GMPARAM_LIST_3 __MLUABIND_GMPARAM_LIST_2, _P3
#define __MLUABIND_GMPARAM_LIST_4 __MLUABIND_GMPARAM_LIST_3, _P4
#define __MLUABIND_GMPARAM_LIST_5 __MLUABIND_GMPARAM_LIST_4, _P5
#define __MLUABIND_GMPARAM_LIST_6 __MLUABIND_GMPARAM_LIST_5, _P6
#define __MLUABIND_GMPARAM_LIST_7 __MLUABIND_GMPARAM_LIST_6, _P7
#define __MLUABIND_GMPARAM_LIST_8 __MLUABIND_GMPARAM_LIST_7, _P8
#define __MLUABIND_GMPARAM_LIST_9 __MLUABIND_GMPARAM_LIST_8, _P9
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
#define __MLUABIND_GMCPM_0 ExtractObjectPointer<_Base, _Const>(_lcv)
#define __MLUABIND_GMCPM_1 __MLUABIND_GMCPM_0, ParameterMatcher<_P1>::ExtractParam(L, 1)
#define __MLUABIND_GMCPM_2 __MLUABIND_GMCPM_1, ParameterMatcher<_P2>::ExtractParam(L, 2)
#define __MLUABIND_GMCPM_3 __MLUABIND_GMCPM_2, ParameterMatcher<_P3>::ExtractParam(L, 3)
#define __MLUABIND_GMCPM_4 __MLUABIND_GMCPM_3, ParameterMatcher<_P4>::ExtractParam(L, 4)
#define __MLUABIND_GMCPM_5 __MLUABIND_GMCPM_4, ParameterMatcher<_P5>::ExtractParam(L, 5)
#define __MLUABIND_GMCPM_6 __MLUABIND_GMCPM_5, ParameterMatcher<_P6>::ExtractParam(L, 6)
#define __MLUABIND_GMCPM_7 __MLUABIND_GMCPM_6, ParameterMatcher<_P7>::ExtractParam(L, 7)
#define __MLUABIND_GMCPM_8 __MLUABIND_GMCPM_7, ParameterMatcher<_P8>::ExtractParam(L, 8)
#define __MLUABIND_GMCPM_9 __MLUABIND_GMCPM_8, ParameterMatcher<_P9>::ExtractParam(L, 9)

///////////////////////////////////////////////////////////
// MethodClassX
///////////////////////////////////////////////////////////
#define __MLUABIND_MAKEMETHODCLASSX(_N)\
template <__MLUABIND_CLASSES_LIST_##_N>\
struct MethodClass##_N : GenericMethod\
{\
	template <bool b> struct ConstResolver { typedef _Ret (_Base::*Result)  (__MLUABIND_MPARAM_LIST_##_N); };\
	template <> struct ConstResolver<true> { typedef _Ret (_Base::*Result)  (__MLUABIND_MPARAM_LIST_##_N) const; };\
	typedef typename ConstResolver<_Const>::Result FT;\
	typedef MethodClass##_N<__MLUABIND_PARAM_LIST_##_N> Me;\
	FT m_F;\
	inline MethodClass##_N( const String& _name, FT _F, const GenericMethodPolicy& _p, bool _is_opmeth = false):\
		GenericMethod(\
			_name,\
			_Const,\
			&MultipleParameterMatcher##_N<__MLUABIND_MPARAM_LIST_##_N>::MatchParams,\
			&ParametersVerboseInfo##_N<__MLUABIND_MPARAM_LIST_##_N>::Info,\
			_p,\
			_is_opmeth),\
		m_F(_F)\
	{};\
	virtual int PerformCallV(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const\
	{\
		return HackVoid<std::tr1::is_void<_Ret>::value>::Do(_host, L, this, _lcv);\
	};\
	template <bool _> struct HackVoid\
	{\
		inline static int Do(CHost* _host, lua_State *L, const Me *_this, LuaCustomVariable *_lcv)\
		{\
			return CreateCustomLuaVariable<ResolvCallType<_Ret>::CallType, _Ret>\
			::Do(L, _host, (ExtractObjectPointer<_Base, _Const>(_lcv)->*(_this->m_F))(__MLUABIND_CPM_##_N), ResolvCallType<_Ret>::IsConst);\
		}\
	};\
	template <> struct HackVoid<true>\
	{\
		inline static int Do(CHost* _host, lua_State *L, const Me *_this, LuaCustomVariable *_lcv)\
		{\
		(ExtractObjectPointer<_Base, _Const>(_lcv)->*(_this->m_F))(__MLUABIND_CPM_##_N);\
			return 0;\
		}\
	};\
};

__MLUABIND_MAKEMETHODCLASSX(0)
__MLUABIND_MAKEMETHODCLASSX(1)
__MLUABIND_MAKEMETHODCLASSX(2)
__MLUABIND_MAKEMETHODCLASSX(3)
__MLUABIND_MAKEMETHODCLASSX(4)
__MLUABIND_MAKEMETHODCLASSX(5)
__MLUABIND_MAKEMETHODCLASSX(6)
__MLUABIND_MAKEMETHODCLASSX(7)
__MLUABIND_MAKEMETHODCLASSX(8)
__MLUABIND_MAKEMETHODCLASSX(9)

///////////////////////////////////////////////////////////
// GlobalMethodClassX
///////////////////////////////////////////////////////////
#define __MLUABIND_MAKEGLOBALMETHODCLASSX(_N)\
template <__MLUABIND_CLASSES_LIST_##_N>\
struct GlobalMethodClass##_N : GenericMethod\
{\
	template <bool b> struct ConstResolver { typedef _Ret (*Result)  (__MLUABIND_GMPARAM_LIST_##_N); };\
	template <> struct ConstResolver<true> { typedef _Ret (*Result)  (const __MLUABIND_GMPARAM_LIST_##_N); };\
	typedef typename ConstResolver<_Const>::Result FT;\
	typedef GlobalMethodClass##_N<__MLUABIND_PARAM_LIST_##_N> Me;\
	FT m_F;\
	inline GlobalMethodClass##_N( const String& _name, FT _F, const GenericMethodPolicy& _p, bool _is_opmeth = false):\
		GenericMethod(\
			_name,\
			_Const,\
			&MultipleParameterMatcher##_N<__MLUABIND_MPARAM_LIST_##_N>::MatchParams,\
			&ParametersVerboseInfo##_N<__MLUABIND_MPARAM_LIST_##_N>::Info,\
			_p,\
			_is_opmeth),\
		m_F(_F)\
	{};\
	virtual int PerformCallV(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const\
	{\
		return HackVoid<std::tr1::is_void<_Ret>::value>::Do(_host, L, this, _lcv);\
	};\
	template <bool _> struct HackVoid\
	{\
		inline static int Do(CHost* _host, lua_State *L, const Me *_this, LuaCustomVariable *_lcv)\
		{\
			return CreateCustomLuaVariable<ResolvCallType<_Ret>::CallType, _Ret>\
			::Do(L, _host, _this->m_F(__MLUABIND_GMCPM_##_N), ResolvCallType<_Ret>::IsConst);\
		}\
	};\
	template <> struct HackVoid<true>\
	{\
		inline static int Do(CHost* _host, lua_State *L, const Me *_this, LuaCustomVariable *_lcv)\
		{\
			_this->m_F(__MLUABIND_GMCPM_##_N);\
			return 0;\
		}\
	};\
};

__MLUABIND_MAKEGLOBALMETHODCLASSX(0)
__MLUABIND_MAKEGLOBALMETHODCLASSX(1)
__MLUABIND_MAKEGLOBALMETHODCLASSX(2)
__MLUABIND_MAKEGLOBALMETHODCLASSX(3)
__MLUABIND_MAKEGLOBALMETHODCLASSX(4)
__MLUABIND_MAKEGLOBALMETHODCLASSX(5)
__MLUABIND_MAKEGLOBALMETHODCLASSX(6)
__MLUABIND_MAKEGLOBALMETHODCLASSX(7)
__MLUABIND_MAKEGLOBALMETHODCLASSX(8)
__MLUABIND_MAKEGLOBALMETHODCLASSX(9)

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
#undef __MLUABIND_GMPARAM_LIST_0
#undef __MLUABIND_GMPARAM_LIST_1
#undef __MLUABIND_GMPARAM_LIST_2
#undef __MLUABIND_GMPARAM_LIST_3
#undef __MLUABIND_GMPARAM_LIST_4
#undef __MLUABIND_GMPARAM_LIST_5
#undef __MLUABIND_GMPARAM_LIST_6
#undef __MLUABIND_GMPARAM_LIST_7
#undef __MLUABIND_GMPARAM_LIST_8
#undef __MLUABIND_GMPARAM_LIST_9
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
#undef __MLUABIND_GMCPM_0 
#undef __MLUABIND_GMCPM_1
#undef __MLUABIND_GMCPM_2
#undef __MLUABIND_GMCPM_3
#undef __MLUABIND_GMCPM_4
#undef __MLUABIND_GMCPM_5
#undef __MLUABIND_GMCPM_6
#undef __MLUABIND_GMCPM_7
#undef __MLUABIND_GMCPM_8
#undef __MLUABIND_GMCPM_9
#undef __MLUABIND_MAKEMETHODCLASSX
#undef __MLUABIND_MAKEGLOBALMETHODCLASSX

}; // namespace impl

}; // namespace mluabind
