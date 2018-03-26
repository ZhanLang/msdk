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

GenericConstructor::GenericConstructor(
	int (*_pm)(lua_State *L, int, int, bool),
	String (*_pi)(CHost *host),
	GenericConstructor::ObjectCreatorT _oc,
	Loki::TypeInfo _cti,
	const GenericMethodPolicy& _p,
	bool _is_cc,
	int _param_amount
	):
	m_ParameterMatcher(_pm),
	m_ParametersInfo(_pi),
	m_MyClassInfo(_cti),
	m_IsCopyConstructor(_is_cc),
	m_ParametersAmount(_param_amount),
	m_ObjectCreator(_oc)
{
	if(_p.m_Group.empty())
		m_Policy = 0;
	else
		m_Policy = (GenericMethodPolicy *)&_p;
};

GenericConstructor::~GenericConstructor()
{
	delete m_Policy;
};

int GenericConstructor::PerformCall(CHost* _host, lua_State *L) const
{
	if(m_Policy && (m_ParametersAmount != 1))
		m_Policy->BeforePerformCall(_host, L, 0, false);

	int startfrom = 1;

	GenericClass *c = _host->FindCPPGenericClass(m_MyClassInfo.name());
	if(!c)
	{
		_host->Error("CustomClass::Constructor panic - can't find GenericClass!\n");
		return 0;
	}
	assert(c);

	// __first__ create cpp object, and __then__ construct a LCV in stack!!!

	void *newobj = CreateObject(_host, L, startfrom); 

	LuaCustomVariable *cv = c->ConstructLuaUserdataObject(L);
	cv->SetValue( newobj );
	int res = 1;

	if(m_Policy && (m_ParametersAmount != 1))
		res = m_Policy->AfterPerformCall(_host, L, 0, false, res);
		
	return res;
};

void *GenericConstructor::CreateObject(CHost* _host, lua_State *L, int _startfrom) const
{
	int res = 1;
	if(m_Policy && (m_ParametersAmount == 1))
		m_Policy->BeforePerformCall(_host, L, 0, false);

	void *p = m_ObjectCreator(_host, L, _startfrom);

	if(m_Policy && (m_ParametersAmount == 1))
		m_Policy->AfterPerformCall(_host, L, 0, false, res);

	return p;
};

int GenericConstructor::MatchParams(lua_State *L) const
{
	return m_ParameterMatcher(L, 1, 0, true);
};

bool GenericConstructor::CanWorkAsImplicitConstructor(CHost* _host, lua_State *L, int _startfrom) const
{
	if(m_ParametersAmount != 1)
		return false;
	if(m_IsCopyConstructor)
		return false; // can't perform as implicit const, since this is meaningless
	return m_ParameterMatcher(L, _startfrom, 1, false) < ParameterMatcherBase::CantBeConverted;
};

String GenericConstructor::GetVerboseName(lua_State *L) const
{
	GenericClass *c = CHost::GetFromLua(L)->FindCPPGenericClass(m_MyClassInfo.name());
	if(!c)
		CHost::GetFromLua(L)->Error("CustomClass::Constructor panic - can't find GenericClass!\n");
	assert(c);

	String name = c->m_LuaName + String("(") + m_ParametersInfo(CHost::GetFromLua(L)) + String(")");
	return name.c_str();
};

}; // namespace impl

}; // namespace mluabind
