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
#include "mluabind.h"

namespace mluabind
{

namespace impl
{

GenericMethod::GenericMethod(const String& _luaname,
		bool _is_const,
		int (*_pm)(lua_State *L, int, int, bool),
		String (*_pi)(CHost *host),
		const GenericMethodPolicy& _p,
		bool _is_opmeth):
	m_LuaName(_luaname),
	m_ParameterMatcher(_pm),
	m_ParametersInfo(_pi),
	m_IsConst(_is_const),
	m_IsOperatorMethod(_is_opmeth)
{
	if(_p.m_Group.empty())
		m_Policy = 0;
	else
		m_Policy = (GenericMethodPolicy *)&_p;
};

GenericMethod::~GenericMethod()
{
	delete m_Policy;
};

int GenericMethod::PerformCall(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const
{
	if(m_IsOperatorMethod)
	{
		assert(lua_gettop(L) >= 1);
		lua_remove(L, 1);
	}

	if(m_Policy)
		m_Policy->BeforePerformCall(_host, L, _lcv, m_IsConst);

	int res = PerformCallV(_host, L, _lcv);

	if(m_Policy)
		res = m_Policy->AfterPerformCall(_host, L, _lcv, m_IsConst, res);

	return res;
};

int GenericMethod::PerformCallV(CHost* _host, lua_State *L, LuaCustomVariable *_lcv) const
{
	assert(0);
	return 0;
};

int GenericMethod::MatchParams(lua_State *L, LuaCustomVariable *_lcv) const
{
	if(_lcv->IsConst() && !m_IsConst)
		return ParameterMatcherBase::CantBeConverted;
	int mul = 1;
	if(!_lcv->IsConst() && m_IsConst)
		mul = ParameterMatcherBase::CanBeConverted;

	if( m_IsOperatorMethod )
	{
		// extra check for operator-methods that first parameter in stack has our variable,
		// since operator-methods support only MyObj Op Other expressions
		if(lua_type(L, 1) != LUA_TUSERDATA ||
			(LuaCustomVariable *)lua_touserdata(L, 1) != _lcv)
			return ParameterMatcherBase::CantBeConverted;
	}

	int res = m_ParameterMatcher(L, m_IsOperatorMethod ? 2 : 1, 0, true);
	if(res == ParameterMatcherBase::CantBeConverted)
		return res;
	res *= mul;
	assert(res <= ParameterMatcherBase::CantBeConverted);
	return res;
};

String GenericMethod::GetVerboseName(lua_State *L) const
{
	String name = m_LuaName + String("(") + m_ParametersInfo(CHost::GetFromLua(L)) + String(")");
	if(m_IsConst)
		name += String(" const");
	return name;
};

}; // namespace impl

}; // namespace mluabind