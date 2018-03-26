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
#include "luainclude.h"
#include "class.h"
#include "host.h"

#include <stdio.h>
#pragma warning(disable:4996)

namespace mluabind
{

impl::SelfTypeTag self;
impl::ConstSelfTypeTag const_self;

namespace impl
{

char *g_LuaOperatorNames[op_amount] =
{
	"__add", "__sub", "__mul", "__div", "__mod", "__unm", "__eq", "__lt", "__le", "__tostring", "__concat", "__call"
};

static const char *g_LuaOperatorClosuresStorageTag = "__MLUABIND_LUAOPERATORCLOSURESSTORAGE";

////////////////////////


ClassUpCastConvertAdapter::~ClassUpCastConvertAdapter(){};

ClassInheritanceConvertAdapter::~ClassInheritanceConvertAdapter()
{
};

void *ClassUpCastConvertAdapter::ConvertToTarget(void *_current) const
{
	assert(m_UpCaster != 0);
	if(!m_UpCaster)
		return 0;
	return m_UpCaster(_current);
};

GenericClass *ClassUpCastConvertAdapter::GetTargetGenericClass(CHost &_host) const
{
	if(m_TargetClass)
		return m_TargetClass;
	GenericClass *cl = _host.FindCPPGenericClass(m_TargetClassTypeInfo.name());
	if(!cl)
	{
		_host.Error("ClassUpCastConvertAdapter::GetTargetGenericClass: panic - can't find generic class for %s!\n", m_TargetClassTypeInfo.name());
		assert(0);
		return 0;
	}
	return m_TargetClass = cl;
};

///////////////////////////////////////////////////////////
// GenericClass implementation
///////////////////////////////////////////////////////////
GenericClass::GenericClass(const char* _luaname, int (*_destructor)(lua_State* L), Loki::TypeInfo _myti):
	DeclaratedClass(!_luaname[0] ? _myti.name() : _luaname,
		!_luaname[0] ? _myti.name() : _luaname),
	m_Destructor(_destructor),
	m_MineTypeInfo(_myti)
{
	assert(m_Destructor != 0);
};

GenericClass::~GenericClass()
{
	{
		MethodsT::iterator i = m_Methods.begin(), e = m_Methods.end();
		for(;i!=e;++i)
		{
			std::vector<GenericMethod*>::iterator ii = (*i).second.begin(),
				ee = (*i).second.end();
			for(;ii!=ee;++ii)
				delete *ii;

			i->second.clear();
		}
	}
	{
		OperatorsT::iterator i = m_Operators.begin(), e = m_Operators.end();
		for(;i!=e;++i)
		{
			std::vector<GenericMethod*>::iterator ii = (*i).second.begin(),
				ee = (*i).second.end();
			for(;ii!=ee;++ii)
				delete *ii;

			(*i).second.clear();
		}
	}
	{
		ConstructorsT::iterator i = m_Constructors.begin(), e = m_Constructors.end();
		for(;i!=e;++i)
			delete *i;
	}
	{
		MembersT::iterator i = m_Members.begin(), e = m_Members.end();
		for(;i!=e;++i)
			delete (*i).second;
	}
	{
		ConstantsT::iterator i = m_Constants.begin(), e = m_Constants.end();
		for(;i!=e;++i)
			delete (*i).second;
	}
	{
		ConvertersT::iterator i = m_Converters.begin(), e = m_Converters.end();
		for(;i!=e;++i)
			delete (*i).second;
	}

	m_Methods.clear();
	m_Constructors.clear();
	m_Members.clear();
	m_Constants.clear();
	m_Operators.clear();
	m_ReachableClasses.clear();
};

Loki::TypeInfo GenericClass::GetTypeID() const
{ 
	return m_MineTypeInfo;
};

bool GenericClass::HasConverters() const
{
	return !m_Converters.empty();
};

bool GenericClass::InsertMethod(const char* _name, GenericMethod* _gm)
{
	m_Methods[_name].push_back(_gm);
	return true;
};

bool GenericClass::InsertMember(const char* _name, GenericMember* _gm)
{
	MembersT::iterator i = m_Members.find(_name);
	if(i != m_Members.end())
		delete (*i).second;
	m_Members[_name] = _gm;
	return true;
};

int GenericClass::LuaMethodCallProxieFunction(lua_State *L)
{
	LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, lua_upvalueindex(1));
	const char *name = (const char*)lua_touserdata(L, lua_upvalueindex(2));
	const GenericClass *gc = lcv->getclass();
	assert(gc != 0);
	MethodsT::const_iterator i = gc->m_Methods.find(name);
	if(i != gc->m_Methods.end())
	{
		return lcv->getclass()->MethodCallDispatcher(CHost::GetFromLua(L), L, lcv, name, (*i).second);
	}
	else
	{
		CHost::GetFromLua(L)->Error("GenericClass::LuaMethodCallProxieFunction: panic - can't find GenericMethod %s!\n", name);
		return 0;
	}
};

int GenericClass::MethodCallDispatcher(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, const char *_name, const std::vector<GenericMethod*> &_overloads) const
{
	const int maximum_overloaded_methods = 50;

	// perform arguments match for all overloaded methods
	const std::vector<GenericMethod*>& methods = _overloads;

	int minimum_fval = ParameterMatcherBase::CantBeConverted; // maximum value by default - can't be called
	size_t overloaded_count = 0;
	GenericMethod* overloads[maximum_overloaded_methods];
			
	for(size_t i = 0; i < methods.size(); ++i)
	{
		int res = methods[i]->MatchParams(L, _lcv);
		assert(res >= 1);
		assert(res <= ParameterMatcherBase::CantBeConverted);
		if( res < minimum_fval ) // more suitable function found
		{
			minimum_fval = res;
			overloaded_count = 0;
			overloads[overloaded_count ] = methods[i];
			++overloaded_count;
		}
		else if(res == minimum_fval)
		{
			if(overloaded_count < maximum_overloaded_methods - 1)
			{
				overloads[overloaded_count] = methods[i];
				++overloaded_count;
			}
		}
	}

	if( minimum_fval == ParameterMatcherBase::CantBeConverted || overloaded_count > 1)
	{
		char buf[65536];
		strcpy(buf, "");
		for(size_t n = 0; n < overloaded_count; ++n)
		{
			strcat(buf,  overloads[n]->GetVerboseName(L).c_str() );
			strcat(buf,  "\n");
		}
		_host->Error("Can't match method to call '%s.%s%s', candidates are:\n%sParameters are:\n%sCaused by: %s.\n",
			m_FullLuaName.c_str(),
			_name,
			_lcv->IsConst() ? " const" : "",
			buf,
			GetParametersInfo(L).c_str(),
			minimum_fval == ParameterMatcherBase::CantBeConverted ? "no one can be used" : "ambiguity");
		return 0;
	}

	if(overloaded_count == 0)
	{
		_host->Error("GenericClass::MethodCallDispatcher: panic - can't find any methods for %s!\n", _name);
		assert(0);
		return 0;
	}

	HostImplicitCreatedStackFrameGuard g(_host);
	return overloads[0]->PerformCall(_host, L, _lcv);
};

int GenericClass::ConstructorCallDispatcher(CHost *_host, lua_State *L) const
{
	const int maximum_overloaded_constructors = 50;

	// perform arguments match for all overloaded constructors
	const std::vector<GenericConstructor*>& constructors = m_Constructors;
			
	int minimum_fval = ParameterMatcherBase::CantBeConverted; // maximum value by default - can't be called
	size_t overloaded_count = 0;
	GenericConstructor* overloads[maximum_overloaded_constructors];

	lua_remove(L, 1); // remove first element since it's a class's table

	for(size_t i = 0; i < constructors.size(); ++i)
	{
		int res = constructors[i]->MatchParams(L);
		if( res < minimum_fval ) // more suitable constructor found
		{
			minimum_fval = res;
			overloaded_count = 0;
			overloads[overloaded_count ] = constructors[i];
			++overloaded_count;
		}
		else if(res == minimum_fval)
		{
			if(overloaded_count < maximum_overloaded_constructors - 1)
			{
				overloads[overloaded_count] = constructors[i];
				++overloaded_count;
			}
		}
	}

	if( minimum_fval == ParameterMatcherBase::CantBeConverted || overloaded_count > 1)
	{
		char buf[65536];
		strcpy(buf, "");
		for(size_t n = 0; n < overloaded_count; ++n)
		{
			strcat(buf, overloads[n]->GetVerboseName(L).c_str());			
			strcat(buf, "\n");
		}
		_host->Error("Can't match constructor to create %s, candidates are:\n%sParameters are:\n%sCaused by: %s.",
			m_FullLuaName.c_str(),
			buf,
			GetParametersInfo(L).c_str(),
			minimum_fval == ParameterMatcherBase::CantBeConverted ? "no one can be used" : "ambiguity");
		return 0;
	}

	if(overloaded_count == 0)
	{
		_host->Error("GenericClass::ConstructorCallDispatcher: panic - can't find any constuctor for %s!\n", m_FullLuaName.c_str());
		assert(0);
		return 0;
	}

	HostImplicitCreatedStackFrameGuard g(_host);
	return overloads[0]->PerformCall(_host, L);
};

bool GenericClass::ProcessOperatorIndexForMembers(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const
{
	// try to find _name in members first
	MembersT::const_iterator i = m_Members.find(_name);
	if(i != m_Members.end())
	{
		_res =  (*i).second->PerformIndexOperator(L, _lcv);
		return true;
	}
	
	// then try it in constants
	ConstantsT::const_iterator i2 = m_Constants.find(_name);
	if(i2 != m_Constants.end())
	{
		i2->second->PutOnStack(L);
		_res = 1;
		return true;
	}

	if(HasConverters())
	{
		ConvertersT::const_iterator i = m_Converters.begin(),
			e = m_Converters.end();
		for(;i!=e;++i)
		{
			GenericClass *base = (*i).second->GetTargetGenericClass(*_host);

			void *orig_ptr = _lcv->value();
			void *base_ptr = (*i).second->ConvertToTarget(orig_ptr);

			new (&m_LCVCache) LuaCustomVariable(false, _lcv->IsConst(), base, base_ptr);
			bool res = base->ProcessOperatorIndexForMembers(_host, L, _name, &m_LCVCache, _res);
			if(res)
				return true;
		}
	}
	return false;
};

bool GenericClass::ProcessOperatorIndexForMethods(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const
{
	MethodsT::const_iterator i = m_Methods.find(_name);
	if(i != m_Methods.end()) // there's a method with this name
	{
		lua_pushlightuserdata(L, (void*)_lcv);					// param 1 - pointer to LuaCustomVariable
		lua_pushlightuserdata(L, (void*)(*i).first.c_str());	// param 2 - const char * to function name
		lua_pushcclosure(L, LuaMethodCallProxieFunction, 2);
		_res = 1;
		return true;
	}
	else if(HasConverters())
	{
		ConvertersT::const_iterator i = m_Converters.begin(),
			e = m_Converters.end();
		for(;i!=e;++i)
		{
			GenericClass *base = (*i).second->GetTargetGenericClass(*_host);

			void *orig_ptr = _lcv->value();
			void *base_ptr = (*i).second->ConvertToTarget(orig_ptr);

			new (&m_LCVCache) LuaCustomVariable(false, _lcv->IsConst(), base, base_ptr);
			bool res = base->ProcessOperatorIndexForMethods(_host, L, _name, &m_LCVCache, _res);
			if(res)
				return true;
		}
	}
	return false;
};

int GenericClass::MetaOperatorIndex(lua_State* L)
{
	assert(lua_isuserdata(L, -2) != 0); // should be our object always
	assert(lua_islightuserdata(L, -2) == 0); // should be our object always
	CHost *host = CHost::GetFromLua(L);

	if(lua_type(L, -1) == LUA_TSTRING) // x["y"] or x.y
	{
		const char *name = lua_tostring(L, -1);
		LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, -2);
		GenericClass *_this = (GenericClass *)lcv->getclass();
		int res = 0;
		// "y" in methods first
		if( _this->ProcessOperatorIndexForMethods(host, L, name, lcv, res) )
		{
			return res;
		}
		else
		{
			// then search in members
			if( _this->ProcessOperatorIndexForMembers(host, L, name, lcv, res) )
			{
				return res;
			}
			else
			{
				return CHost::GetFromLua(L)->Error("MetaOperatorIndex: error - unknown symbol %s.\n", name), 0;
			}
		}
	}
	else
	{
		CHost::GetFromLua(L)->Error("MetaOperatorIndex: panic - currently supports only string indexes.\n");
		return 0;
	}
	return 0;
};

int GenericClass::MetaOperatorNewIndex(lua_State* L)
{
	assert(lua_isuserdata(L, -3) != 0); // should be our object always
	assert(lua_islightuserdata(L, -3) == 0); // should be our object always
	CHost *host = CHost::GetFromLua(L);
	if(lua_type(L, -2) == LUA_TSTRING) // x["y"] or x.y
	{
		const char *name = lua_tostring(L, -2);
		LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, -3);
		if(lcv->IsConst())
		{
			CHost::GetFromLua(L)->Error("MetaOperatorNewIndex: can't assign a value to a member '%s' of a const object.\n", name);
			return 0;
		}

		GenericClass *_this = (GenericClass *)lcv->getclass();
		int res;
		if( _this->ProcessOperatorNewIndexForMembers(host, L, name, lcv, res) )
		{
			return res;
		}
		else
		{
			CHost::GetFromLua(L)->Error("MetaOperatorIndex: error - unknown symbol %s.\n", name);
		}
	}
	else
		CHost::GetFromLua(L)->Error("MetaOperatorNewIndex: panic - currently supports only string indexes.\n");
	return 0;
};

bool GenericClass::ProcessOperatorNewIndexForMembers(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const
{
	MembersT::const_iterator i = m_Members.find(_name);
	if(i != m_Members.end())
	{
		_res =  (*i).second->PerformNewIndexOperator(L, _lcv, lua_gettop(L));
		return true;
	}
	else if(HasConverters())
	{
		ConvertersT::const_iterator i = m_Converters.begin(),
			e = m_Converters.end();
		for(;i!=e;++i)
		{
			GenericClass *base = (*i).second->GetTargetGenericClass(*_host);

			void *orig_ptr = _lcv->value();
			void *base_ptr = (*i).second->ConvertToTarget(orig_ptr);

			new (&m_LCVCache) LuaCustomVariable(false, _lcv->IsConst(), base, base_ptr);
			bool res = base->ProcessOperatorNewIndexForMembers(_host, L, _name, &m_LCVCache, _res);
			if(res)
				return true;
		}
	}
	return false;
};

bool GenericClass::CanBeUpCastedTo(CHost *_host, Loki::TypeInfo _to) const
{
	if(_to == GetTypeID())
		return true;

	ReachableClassesT::const_iterator i = m_ReachableClasses.find(_to);
	if(i != m_ReachableClasses.end())
		return true;

	return false;
};

void *GenericClass::UpCastToType(CHost *_host, Loki::TypeInfo _to, void *_from) const
{
	if(_to == GetTypeID())
		return _from;
	ReachableClassesT::const_iterator i = m_ReachableClasses.find(_to);
	assert(i != m_ReachableClasses.end());
	if(i->second.size() == 1) // target class can be reached directly thru adapter
	{
		ConvertersT::const_iterator ci = m_Converters.find( i->first );
		assert(ci != m_Converters.end());
		void *base_ptr = ci->second->ConvertToTarget(_from);
		assert(base_ptr != 0);
		return base_ptr;
	}
	else
	{
		ConvertersT::const_iterator ci = m_Converters.find( i->second[i->second.size() - 2]->GetTypeID() );
		assert(ci != m_Converters.end());
		GenericClass *base = ci->second->GetTargetGenericClass(*_host);
		void *base_ptr = ci->second->ConvertToTarget(_from);
		assert(base_ptr != 0);
		return base->UpCastToType(_host, _to, base_ptr);
	}
	assert(0);
	return 0;
};

void GenericClass::RegisterClassNamespaceDetails(CHost *_host, lua_State *L) const
{
	if(!m_Constants.empty())
	{
		ConstantsT::const_iterator i = m_Constants.begin(),
			e = m_Constants.end();
		for(;i!=e;++i)
		{
			lua_pushstring(L, (*i).first.c_str());
			(*i).second->PutOnStack(L);
			lua_rawset(L, -3); // inserts a constant in a class namespace
		}
	}

	ConvertersT::const_iterator i = m_Converters.begin(),
		e = m_Converters.end();
	for(;i!=e;++i)
		if((*i).second->ShouldRegisterNameSpaceDetails())
			(*i).second->GetTargetGenericClass(*_host)->RegisterClassNamespaceDetails(_host, L);
};

// At top of stack should be a user data, where current metatables should be set
int GenericClass::SetMetatables(lua_State *L) const
{
	assert(this != 0); // paranoid
	assert(lua_gettop(L) > 0);
	assert(lua_isuserdata(L, -1));
	assert(!lua_islightuserdata(L, -1));

	// store our operator closures in Lua, then grab it and put into metatable,
	// so in diffirent object with same types operators will be same too.

	// try to locate storage table
	lua_getfield(L, LUA_GLOBALSINDEX, g_LuaOperatorClosuresStorageTag);
	if(lua_isnil(L, -1))
	{
		// no table was created yet, make it now
		lua_pop(L, 1);
		lua_newtable(L);
	}

	// try to locate storage for our generic class
	lua_getfield(L, -1, m_FullLuaName.c_str());
	if(lua_isnil(L, -1))
	{
		// no table was created yet, make it now
		lua_pop(L, 1);
		lua_pushstring(L, m_FullLuaName.c_str());
		lua_newtable(L);

		lua_pushstring(L, "__gc");	
		lua_pushcclosure(L, m_Destructor, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__index");	
		lua_pushcclosure(L, &MetaOperatorIndex, 0); 
		lua_rawset(L, -3);

		lua_pushstring(L, "__newindex");	
		lua_pushcclosure(L, &MetaOperatorNewIndex, 0); 
		lua_rawset(L, -3);

		OperatorsT::const_iterator i = m_Operators.begin(),
			e = m_Operators.end();
		for(;i!=e;++i)
		{
			lua_pushstring(L, g_LuaOperatorNames[(*i).first]);	
			lua_pushinteger(L, (*i).first);					// param 1 - operator code
			lua_pushcclosure(L, LuaOperatorCallProxieFunction, 1);
			lua_rawset(L, -3);
		}

		lua_rawset(L, -3); // sets our table to global storage table
		assert(lua_istable(L, -1)); // check our results

		// store our results it global metatable storage
		lua_setfield(L, LUA_GLOBALSINDEX, g_LuaOperatorClosuresStorageTag);

		// get again global metatable storage
		lua_getfield(L, LUA_GLOBALSINDEX, g_LuaOperatorClosuresStorageTag);
		assert(lua_istable(L, -1)); // check our results

		// search get again our table for current generic class
		lua_getfield(L, -1, m_FullLuaName.c_str());
	}
	// at the top - table for current generic class, at -2 global metatables storage

	if(!lua_setmetatable(L, -3))
		CHost::GetFromLua(L)->Error("can't set metatable!\n");

	lua_pop(L, 1);

	return 1;
};

LuaCustomVariable *GenericClass::ConstructLuaUserdataObject(lua_State *L) const
{
	void *s = lua_newuserdata(L, sizeof(LuaCustomVariable));
	new (s) LuaCustomVariable(true, false, this);
	SetMetatables(L);
	return ((LuaCustomVariable*)s);
};

int GenericClass::LuaOperatorCallProxieFunction(lua_State *L)
{
	int operatorindex = (int)lua_tonumber(L, lua_upvalueindex(1));	

	// searches for LuaCustomVariable, from which operator handler should be called
	LuaCustomVariable *lcv = 0;

	// tries in first param
	if(lua_type(L, 1) == LUA_TUSERDATA)
	{
		LuaCustomVariable *tmp = (LuaCustomVariable *)lua_touserdata(L, 1);
		// assume that if first class contains handler for
		// this operation - it is what we search... (really?)
		if(tmp->getclass()->m_Operators.find(operatorindex) != tmp->getclass()->m_Operators.end())
			lcv = tmp;
	}

	// if not - check in second parameter
	if(!lcv && lua_type(L, 2) == LUA_TUSERDATA)
	{
		LuaCustomVariable *tmp = (LuaCustomVariable *)lua_touserdata(L, 2);
		if(tmp->getclass()->m_Operators.find(operatorindex) != tmp->getclass()->m_Operators.end())
			lcv = tmp;
	}

	if(!lcv)
	{
		CHost::GetFromLua(L)->Error(
			"GenericClass::LuaOperatorCallProxieFunction: panic - can't find a variable to call operator handler for '%s'",
			g_LuaOperatorNames[operatorindex]
			);
		return 0;
	}

	if(operatorindex == Unm)
		lua_remove(L, -1); // hack's lua bug with two similar parameters in unary minus operator

	const GenericClass *gc = lcv->getclass();
	OperatorsT::const_iterator i = gc->m_Operators.find(operatorindex);
	if(i != gc->m_Operators.end())
	{
		return lcv->getclass()->MethodCallDispatcher(CHost::GetFromLua(L), L, lcv, g_LuaOperatorNames[operatorindex], (*i).second);
	}
	else
	{
		CHost::GetFromLua(L)->Error(
			"GenericClass::LuaOperatorCallProxieFunction: panic - can't find operator %s for class %s!\n",
			g_LuaOperatorNames[operatorindex],
			gc->m_FullLuaName.c_str()
			);
		return 0;
	}
};

bool GenericClass::CanBeImplicityConstructedFrom(CHost* _host, lua_State *L, int _ind) const
{
	ConstructorsT::const_iterator i = m_Constructors.begin(),
		e = m_Constructors.end();
	for(;i!=e;++i)
		if((*i)->CanWorkAsImplicitConstructor(_host, L, _ind))
			return true;
	return false;
};

void *GenericClass::ConstructObjectFrom(CHost* _host, lua_State *L, int _ind) const
{
	ConstructorsT::const_iterator i = m_Constructors.begin(),
		e = m_Constructors.end();
	for(;i!=e;++i)
		if((*i)->CanWorkAsImplicitConstructor(_host, L, _ind))
			return (*i)->CreateObject(_host, L, _ind);

	_host->Error("GenericClass::ConstructObjectFrom: panic - can't find constructor to impicity create object '%s'", m_FullLuaName.c_str());
	assert(0);
	return 0;
};

bool GenericClass::InsertConverter(ClassConvertAdapter* _what)
{
	m_Converters.insert(std::make_pair(_what->GetTarget(), _what));
	return true;
};

bool GenericClass::InsertConstructor(GenericConstructor* _gc)
{
	m_Constructors.push_back(_gc);
	return true;
};

bool GenericClass::InsertOperator(int _type, GenericMethod *_gm)
{
	m_Operators[_type].push_back(_gm);
	return true;
};

bool GenericClass::InsertConstant(const char* _name, GenericConstant *_gc)
{
	ConstantsT::iterator i = m_Constants.find(_name);
	if(i != m_Constants.end())
		delete (*i).second;
	m_Constants[_name] = _gc;
	return true;
};

void GenericClass::CreateReachableClassesCache(CHost &_host)
{
	assert(m_ReachableClasses.size() == 0);

	// first of all - put all classes that we can reach directly thru our converters
	{
	ConvertersT::iterator i = m_Converters.begin(),
		e = m_Converters.end();
	for(;i!=e;++i)
		m_ReachableClasses.insert(
			std::make_pair(i->first, std::vector<GenericClass*>(1, this)));
	}

	// for each directly accessed classes get it's reachable cache and merge it with ours
	// compare access-path vectors size to get the shortest
	{
	ConvertersT::iterator i = m_Converters.begin(),
		e = m_Converters.end();
	for(;i!=e;++i)
	{
		GenericClass *cgc = i->second->GetTargetGenericClass(_host);
		assert(cgc != 0);
		ReachableClassesT& cache = cgc->m_ReachableClasses;
		ReachableClassesT::iterator ci = cache.begin(), ce = cache.end();
		for(;ci!=ce;++ci)
		{
			size_t oursize = 10000;
			ReachableClassesT::iterator fi = m_ReachableClasses.find(ci->first);
			if(fi != m_ReachableClasses.end())
				oursize = fi->second.size();
			if(ci->second.size() + 1 < oursize)
			{
				// some way to ci->first type is shorter than our, use it
				m_ReachableClasses[ci->first] = ci->second;
				m_ReachableClasses[ci->first].push_back(this);
			}
		}
	}
	}

};

void *GenericClass::SafeCastToType(CHost *_host, Loki::TypeInfo _to, void *_from) const
{
	// uppfff....

	// first - check if we're this type
	if(_to == GetTypeID())
		return _from;

	// second - try our upcast convertion cache, check if _to is there already
	if(CanBeUpCastedTo(_host, _to))
		// ok, we're performing a upcast, it's easy enough
		return UpCastToType(_host, _to, _from);

	// third - check if our class is somewhere upper _to, so we can reach it using downcast(if can, yes)
	GenericClass *target = _host->FindCPPGenericClass(_to.name());
	assert(target != 0);
	if(target->CanBeUpCastedTo(_host, GetTypeID()))
	{
		//we're reachable from _to by upcast, so there's a change that can be perfomed a downcast from us
		ReachableClassesT::iterator i = target->m_ReachableClasses.find(GetTypeID());
		assert(i != target->m_ReachableClasses.end());
		std::vector<GenericClass*> &path = i->second;
		void *from = _from;
		Loki::TypeInfo currti = GetTypeID();
		for(size_t i = 0; i < path.size(); ++i)
		{
			ConvertersT::iterator ti = path[i]->m_Converters.find(currti);
			assert(ti != path[i]->m_Converters.end());
			if(!ti->second->CanPerformDowncast())
			{
				from = 0;
				break;
			}
			from = ti->second->ConvertFromTarget(from);
			if(!from)
				break;
			currti = path[i]->GetTypeID();
		}
		if(from)
			return from;
	}

	// fourth - bruteforce casting. for each adapter tries to convert to target class via him
	ConvertersT::const_iterator i = m_Converters.begin(), e = m_Converters.end();
	for(;i!=e;++i)
	{
		void *ptr = i->second->ConvertToTarget(_from);
		GenericClass *gc = _host->FindCPPGenericClass(i->first.name());
		ptr = gc->SafeCastToType(_host, _to, ptr);
		if(ptr)
			return ptr;
	}

	return 0;
};

}; // namespace impl

}; // namespace mluabind
