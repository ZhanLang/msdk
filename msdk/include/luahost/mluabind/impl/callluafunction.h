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

bool FindLuaFunction(lua_State* L, const std::string &_name);

template <class _Ret, 
	class _P1 = Loki::EmptyType,
	class _P2 = Loki::EmptyType,
	class _P3 = Loki::EmptyType,
	class _P4 = Loki::EmptyType,
	class _P5 = Loki::EmptyType,
	class _P6 = Loki::EmptyType,
	class _P7 = Loki::EmptyType,
	class _P8 = Loki::EmptyType,
	class _P9 = Loki::EmptyType,
	class _P10= Loki::EmptyType >
struct CallLuaFunction
{
	static _Ret Do(CHost *_host, const std::string &_luaname, int _amount, 
		typename MakeParameterType<_P1>::type _p1 = _P1(),
		typename MakeParameterType<_P2>::type  _p2 = _P2(),
		typename MakeParameterType<_P3>::type  _p3 = _P3(),
		typename MakeParameterType<_P4>::type  _p4 = _P4(),
		typename MakeParameterType<_P5>::type  _p5 = _P5(),
		typename MakeParameterType<_P6>::type  _p6 = _P6(),
		typename MakeParameterType<_P7>::type  _p7 = _P7(),
		typename MakeParameterType<_P8>::type  _p8 = _P8(),
		typename MakeParameterType<_P9>::type  _p9 = _P9(),
		typename MakeParameterType<_P10>::type _p10= _P10()
		)
	{
		if(!FindLuaFunction(_host->GetLua(), _luaname))
			_host->Error("CallLuaFunction: error - can't find function %s.", _luaname.c_str());

		if(_amount >= 1)
			CreateCustomLuaVariable<ResolvCallType<_P1>::CallType, _P1>::
				Do(_host->GetLua(), _host, _p1, ResolvCallType<_P1>::IsConst);

		if(_amount >= 2)
			CreateCustomLuaVariable<ResolvCallType<_P2>::CallType, _P2>::
				Do(_host->GetLua(), _host, _p2, ResolvCallType<_P2>::IsConst);

		if(_amount >= 3)
			CreateCustomLuaVariable<ResolvCallType<_P3>::CallType, _P3>::
				Do(_host->GetLua(), _host, _p3, ResolvCallType<_P3>::IsConst);

		if(_amount >= 4)
			CreateCustomLuaVariable<ResolvCallType<_P4>::CallType, _P4>::
				Do(_host->GetLua(), _host, _p4, ResolvCallType<_P4>::IsConst);

		if(_amount >= 5)
			CreateCustomLuaVariable<ResolvCallType<_P5>::CallType, _P5>::
				Do(_host->GetLua(), _host, _p5, ResolvCallType<_P5>::IsConst);

		if(_amount >= 6)
			CreateCustomLuaVariable<ResolvCallType<_P6>::CallType, _P6>::
				Do(_host->GetLua(), _host, _p6, ResolvCallType<_P6>::IsConst);

		if(_amount >= 7)
			CreateCustomLuaVariable<ResolvCallType<_P7>::CallType, _P7>::
				Do(_host->GetLua(), _host, _p7, ResolvCallType<_P7>::IsConst);

		if(_amount >= 8)
			CreateCustomLuaVariable<ResolvCallType<_P8>::CallType, _P8>::
				Do(_host->GetLua(), _host, _p8, ResolvCallType<_P8>::IsConst);

		if(_amount >= 9)
			CreateCustomLuaVariable<ResolvCallType<_P9>::CallType, _P9>::
				Do(_host->GetLua(), _host, _p9, ResolvCallType<_P9>::IsConst);

		if(_amount >= 10)
			CreateCustomLuaVariable<ResolvCallType<_P10>::CallType, _P10>::
				Do(_host->GetLua(), _host, _p10, ResolvCallType<_P10>::IsConst);
		
		int res = lua_pcall(_host->GetLua(), _amount, std::tr1::is_void<_Ret>::value? 0 : 1, 0);
		
		if (res)
		{
			const char* err_string = lua_tostring(_host->GetLua(), -1);
			if (err_string)
			{
				_host->Error(err_string);
				lua_pop(_host->GetLua(), 1);
				return _Ret();
			}
		}

		return HackVoid<std::tr1::is_void<_Ret>::value>::Do(_host);
	};

	template <bool _> struct HackVoid
	{
		static _Ret Do(CHost *_host)
		{
			int ret = ParameterMatcher<_Ret, ResolvCallType<_Ret>::CallType >::MatchParam(_host->GetLua(), lua_gettop(_host->GetLua()), true);
			if(ret == ParameterMatcherBase::CantBeConverted)
			{
				lua_pop(_host->GetLua(), 1);
				_host->Error("CallLuaFunction: return value can't be converted to desired type.");
				return _Ret();
			}
		
			_Ret r = ParameterMatcher<_Ret, ResolvCallType<_Ret>::CallType>::ExtractParam(_host->GetLua(), lua_gettop(_host->GetLua()));
			lua_pop(_host->GetLua(), 1);
			return r;
		};
	};

	template <> struct HackVoid<true>
	{
		inline static void Do(CHost *_host){};
	};
	
};

}; // namespace impl

}; // namespace mluabind
