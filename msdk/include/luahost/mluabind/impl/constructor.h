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

namespace mluabind
{

namespace impl
{

struct GenericConstructor
{
	~GenericConstructor();
	int PerformCall(CHost* _host, lua_State *L) const;
	void *CreateObject(CHost* _host, lua_State *L, int _startfrom) const;
	int MatchParams(lua_State *L) const;
	bool CanWorkAsImplicitConstructor(CHost* _host, lua_State *L, int _ind) const;
	String GetVerboseName(lua_State *L) const;

protected:
	typedef void *(*ObjectCreatorT) (CHost* _host, lua_State *L, int _startfrom);

	GenericConstructor(int (*_pm)(lua_State *L, int, int, bool),
		String (*_pi)(CHost *host),
		ObjectCreatorT _oc,
		Loki::TypeInfo _cti,
		const GenericMethodPolicy& _p,
		bool _is_cc,
		int _param_amount);

private:
	Loki::TypeInfo m_MyClassInfo;
	int (*m_ParameterMatcher)(lua_State *L, int, int, bool);
	String (*m_ParametersInfo)(CHost *host);
	ObjectCreatorT m_ObjectCreator;
	GenericMethodPolicy *m_Policy;
	bool m_IsCopyConstructor; // true if accept T, T&, T*, const T, const T&, const T*
	int m_ParametersAmount;
};

#define __MLUABIND_CLASSES_LIST_0 class _Class
#define __MLUABIND_CLASSES_LIST_1 __MLUABIND_CLASSES_LIST_0, class _P1
#define __MLUABIND_CLASSES_LIST_2 __MLUABIND_CLASSES_LIST_1, class _P2
#define __MLUABIND_CLASSES_LIST_3 __MLUABIND_CLASSES_LIST_2, class _P3
#define __MLUABIND_CLASSES_LIST_4 __MLUABIND_CLASSES_LIST_3, class _P4
#define __MLUABIND_CLASSES_LIST_5 __MLUABIND_CLASSES_LIST_4, class _P5
#define __MLUABIND_CLASSES_LIST_6 __MLUABIND_CLASSES_LIST_5, class _P6
#define __MLUABIND_CLASSES_LIST_7 __MLUABIND_CLASSES_LIST_6, class _P7
#define __MLUABIND_CLASSES_LIST_8 __MLUABIND_CLASSES_LIST_7, class _P8
#define __MLUABIND_CLASSES_LIST_9 __MLUABIND_CLASSES_LIST_8, class _P9
#define __MLUABIND_PARAM_LIST_0 _Class
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
#define __MLUABIND_LOKI_LIST_0 Loki::NullType
#define __MLUABIND_LOKI_LIST_1 LOKI_TYPELIST_1(_P1)
#define __MLUABIND_LOKI_LIST_2 LOKI_TYPELIST_2(_P1, _P2)
#define __MLUABIND_LOKI_LIST_3 LOKI_TYPELIST_3(_P1, _P2, _P3)
#define __MLUABIND_LOKI_LIST_4 LOKI_TYPELIST_4(_P1, _P2, _P3, _P4)
#define __MLUABIND_LOKI_LIST_5 LOKI_TYPELIST_5(_P1, _P2, _P3, _P4, _P5)
#define __MLUABIND_LOKI_LIST_6 LOKI_TYPELIST_6(_P1, _P2, _P3, _P4, _P5, _P6)
#define __MLUABIND_LOKI_LIST_7 LOKI_TYPELIST_7(_P1, _P2, _P3, _P4, _P5, _P6, _P7)
#define __MLUABIND_LOKI_LIST_8 LOKI_TYPELIST_8(_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8)
#define __MLUABIND_LOKI_LIST_9 LOKI_TYPELIST_9(_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9)
#define __MLUABIND_CPM_0 
#define __MLUABIND_CPM_1 ParameterMatcher<_P1>::ExtractParam(L, _startfrom+0)
#define __MLUABIND_CPM_2 __MLUABIND_CPM_1, ParameterMatcher<_P2>::ExtractParam(L, _startfrom+1)
#define __MLUABIND_CPM_3 __MLUABIND_CPM_2, ParameterMatcher<_P3>::ExtractParam(L, _startfrom+2)
#define __MLUABIND_CPM_4 __MLUABIND_CPM_3, ParameterMatcher<_P4>::ExtractParam(L, _startfrom+3)
#define __MLUABIND_CPM_5 __MLUABIND_CPM_4, ParameterMatcher<_P5>::ExtractParam(L, _startfrom+4)
#define __MLUABIND_CPM_6 __MLUABIND_CPM_5, ParameterMatcher<_P6>::ExtractParam(L, _startfrom+5)
#define __MLUABIND_CPM_7 __MLUABIND_CPM_6, ParameterMatcher<_P7>::ExtractParam(L, _startfrom+6)
#define __MLUABIND_CPM_8 __MLUABIND_CPM_7, ParameterMatcher<_P8>::ExtractParam(L, _startfrom+7)
#define __MLUABIND_CPM_9 __MLUABIND_CPM_8, ParameterMatcher<_P9>::ExtractParam(L, _startfrom+8)
#define __MLUABIND_MAKECONSTRUCTOR(_N)\
template <__MLUABIND_CLASSES_LIST_##_N>\
struct ConstructorClass##_N : public GenericConstructor\
{\
	ConstructorClass##_N(const GenericMethodPolicy& _p) :\
		GenericConstructor(\
			&MultipleParameterMatcher##_N<__MLUABIND_MPARAM_LIST_##_N>::MatchParams,\
			&ParametersVerboseInfo##_N<__MLUABIND_MPARAM_LIST_##_N>::Info,\
			&CreateObject,\
			typeid(CustomClass<_Class>),\
			_p,\
			IsCopyConstructor<__MLUABIND_LOKI_LIST_##_N, _Class>::Result,\
			_N)\
	{\
	};\
	static void *CreateObject(CHost* _host, lua_State *L, int _startfrom)\
	{\
		return new _Class( __MLUABIND_CPM_##_N );\
	};\
};

__MLUABIND_MAKECONSTRUCTOR(0)
__MLUABIND_MAKECONSTRUCTOR(1)
__MLUABIND_MAKECONSTRUCTOR(2)
__MLUABIND_MAKECONSTRUCTOR(3)
__MLUABIND_MAKECONSTRUCTOR(4)
__MLUABIND_MAKECONSTRUCTOR(5)
__MLUABIND_MAKECONSTRUCTOR(6)
__MLUABIND_MAKECONSTRUCTOR(7)
__MLUABIND_MAKECONSTRUCTOR(8)
__MLUABIND_MAKECONSTRUCTOR(9)

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
#undef __MLUABIND_LOKI_LIST_0
#undef __MLUABIND_LOKI_LIST_1
#undef __MLUABIND_LOKI_LIST_2
#undef __MLUABIND_LOKI_LIST_3
#undef __MLUABIND_LOKI_LIST_4
#undef __MLUABIND_LOKI_LIST_5
#undef __MLUABIND_LOKI_LIST_6
#undef __MLUABIND_LOKI_LIST_7
#undef __MLUABIND_LOKI_LIST_8
#undef __MLUABIND_LOKI_LIST_9
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
#undef __MLUABIND_MAKECONSTUCTOR

}; // namespace impl

}; // namespace mluabind
