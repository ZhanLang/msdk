// Copyright (c) 2008 Michael Kazakov
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
#include "function.h"

namespace mluabind
{

namespace impl
{

GenericFunction::GenericFunction(const char* _luaname,
	int (*_pm)(lua_State *L, int, int, bool),
	String (*_pi)(CHost *host),
	const GenericMethodPolicy& _p) : 
	DeclaratedClass(_luaname, _luaname),
	m_ParameterMatcher(_pm),
	m_ParametersInfo(_pi)
{
	if(_p.m_Group.empty())
		m_Policy = 0;
	else
		m_Policy = (GenericMethodPolicy *)&_p;
};

int GenericFunction::PerformCall(CHost* _host, lua_State *L)
{
	if(m_Policy)
		m_Policy->BeforePerformCall(_host, L, 0, false);

	int res = PerformCallV(_host, L);

	if(m_Policy)
		res = m_Policy->AfterPerformCall(_host, L, 0, false, res);

	return res;
};

int GenericFunction::MatchParams(lua_State *L)
{
	return m_ParameterMatcher(L, 1, 0, true);
};

String GenericFunction::GetVerboseName(lua_State *L) const
{
	return m_FullLuaName + "(" + m_ParametersInfo(CHost::GetFromLua(L)) + ")";
};

} // namespace impl

} // namespace mluabind
