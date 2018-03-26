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
#include "host.h"
#include "ParameterMatcher.h"
#include "luacustomvariable.h"
#include "declarator.h"
#include "luaapi.h"
#include "util.h"

#include <stdio.h>

#pragma warning(disable:4996)

namespace mluabind
{

const char *g_LuaNamespaceDescriptionTag = "__MLUABIND_LUANAMESPACEDESCRIPTIONTAG";

CHost::CHost(lua_State *_L):
	L(_L)
{
	assert(_L != 0);
	RegisterCommons();
};

CHost::~CHost()
{
	{
		CPPClassesT::iterator i = m_CPPClasses.begin(), e = m_CPPClasses.end();
		for(;i!=e;++i)
			delete (*i).second;
	}

	{
		LuaFunctionsT::iterator i = m_LuaFunctions.begin(), e = m_LuaFunctions.end();
		for(;i!=e;++i)
		{
			std::vector<impl::GenericFunction*>::iterator ii = (*i).second.begin(),
				ee = (*i).second.end();
			for(;ii!=ee;++ii)
				delete *ii;
		}
	}
// 	m_CPPClasses.clear();
// 	m_LuaClasses.clear();
// 	m_LuaFunctions.clear();
// 	
// 	while (!m_ImplicitCreated.empty())
// 	{
// 		delete m_ImplicitCreated.top();
// 		m_ImplicitCreated.pop();
// 	}
};

lua_State *CHost::GetLua()
{
	return L;
};

void CHost::Error(const char *_format, ...)
{
	va_list va;
	va_start(va, _format);
	static char buf[4096];
	_vsnprintf(buf, 4096, _format, va);
	va_end(va);
	lua_pushstring(L, buf);
	lua_error(L);
};

void CHost::Insert(impl::GenericClass &_class)
{
	InsertRec(_class, LUA_GLOBALSINDEX, _class.m_FullLuaName.c_str());
};

void CHost::Insert(impl::GenericFunction &_function)
{
	InsertRec(_function, LUA_GLOBALSINDEX, _function.m_FullLuaName.c_str());
};

void CHost::Insert(impl::DeclaratorClass &_declarator)
{
	for(impl::DeclaratorClass::ClassesT::iterator i = _declarator.m_Classes.begin();
		i<_declarator.m_Classes.end();
		++i)
		InsertRec(**i, LUA_GLOBALSINDEX, (*i)->m_FullLuaName.c_str());
	for(impl::DeclaratorClass::FunctionsT::iterator i = _declarator.m_Functions.begin();
		i<_declarator.m_Functions.end();
		++i)
		InsertRec(**i, LUA_GLOBALSINDEX, (*i)->m_FullLuaName.c_str());
	for(impl::DeclaratorClass::ConstantsT::iterator i = _declarator.m_Constants.begin();
		i<_declarator.m_Constants.end();
		++i)
		InsertRec(**i, LUA_GLOBALSINDEX, (*i)->m_FullLuaName.c_str());
	delete &_declarator;
};

void CHost::Insert(impl::GenericConstant &_constant)
{
	InsertRec(_constant, LUA_GLOBALSINDEX, _constant.m_FullLuaName.c_str());
}

void CHost::InsertRec(impl::GenericClass &_class, int _ind, const char *_dn)
{
	// tries to find a namespace in our full name
	const char *s = strchr(_dn, '.');
	if(!s)
	{
		CPPClassesT::iterator i = m_CPPClasses.find(_class.GetTypeID().name());
		assert(i == m_CPPClasses.end() && "Class was already inserted into host");
		_class.CreateReachableClassesCache(*this);
		m_CPPClasses[_class.GetTypeID().name()] = &_class;
		m_LuaClasses[_class.m_FullLuaName.c_str()] = &_class;
		RegisterClass(_class.m_LuaName.c_str(), &_class, _ind);
		return;		
	}
	else
	{
		// find or create left namespace first
		char left[256];
		strncpy(&left[0], _dn, s - _dn);
		left[s - _dn] = 0;

		lua_pushstring(L, left);
		lua_gettable(L, _ind);
		if(lua_isnil(L, -1))
		{ 
			// namespace with our name is not registered in current table (namespace)
			// creates a table for it
			lua_pop(L, 1);
			lua_newtable(L); // table for namespace
		}
		int newind = lua_gettop(L);
		
		InsertRec(_class, newind, s+1);

		lua_setfield(L, _ind, left);
	}
};

void CHost::InsertRec(impl::GenericFunction &_function, int _ind, const char *_dn)
{
	// tries to find a namespace in our full name
	const char *s = strchr(_dn, '.');
	if(!s)
	{
		lua_pushstring(L, _function.m_FullLuaName.c_str());
		lua_pushcclosure(L, LuaGlobalFunctionDispatcher, 1);
		lua_setfield(L, _ind, _function.m_LuaName.c_str());
		m_LuaFunctions[_function.m_FullLuaName.c_str()].push_back(&_function);
	}
	else
	{
		// find or create left namespace first
		char left[256];
		strncpy(&left[0], _dn, s - _dn);
		left[s - _dn] = 0;

		lua_pushstring(L, left);
		lua_gettable(L, _ind);
		if(lua_isnil(L, -1))
		{ 
			// namespace with our name is not registered in current table (namespace)
			// creates a table for it
			lua_pop(L, 1);
			lua_newtable(L); // table for namespace
		}
		int newind = lua_gettop(L);
		
		InsertRec(_function, newind, s+1);

		lua_setfield(L, _ind, left);
	}
};

void CHost::InsertRec(impl::GenericConstant &_constant, int _tind, const char *_dn)
{
	// tries to find a namespace in our full name
	const char *s = strchr(_dn, '.');
	if(!s)
	{
		lua_pushstring(L, _constant.m_LuaName.c_str());
		_constant.PutOnStack(L);
		lua_rawset(L, _tind);
		delete &_constant;
	}
	else
	{
		// find or create left namespace first
		char left[256];
		strncpy(&left[0], _dn, s - _dn);
		left[s - _dn] = 0;

		lua_pushstring(L, left);
		lua_gettable(L, _tind);
		if(lua_isnil(L, -1))
		{ 
			// namespace with our name is not registered in current table (namespace)
			// creates a table for it
			lua_pop(L, 1);
			lua_newtable(L); // table for namespace
		}
		int newind = lua_gettop(L);
		
		InsertRec(_constant, newind, s+1);

		lua_setfield(L, _tind, left);
	}
};

impl::GenericClass *CHost::FindCPPGenericClass(const char *_rtti_name)
{
	// comparing with map keys as ints (pointers to char), since
	// typeid names are static R/O strings. faaaast!
	CPPClassesT::iterator i = m_CPPClasses.find(_rtti_name);
	if(i != m_CPPClasses.end())
		return (*i).second;
	else
		return 0;
};

impl::GenericClass *CHost::FindLuaGenericClass(const char *_lua_name)
{
	LuaClassesT::iterator i = m_LuaClasses.find(_lua_name);
	if(i != m_LuaClasses.end())
		return (*i).second;
	else
		return 0;
};

void CHost::RegisterClass(const char* _luaclass, impl::GenericClass *_class, int _tind)
{
	if(!_luaclass[0])
		return; // does nothing with nameless classes

	// tries to find a table with or name at table at _ind
	lua_pushstring(L, _luaclass);
	lua_gettable(L, _tind);
	if(lua_isnil(L, -1))
	{ 
		// class with our name is not registered in current table (namespace)
		// creates a table for it
		lua_pop(L, 1);
		lua_newtable(L); // table for class identifier
	}

	lua_pushstring(L, g_LuaNamespaceDescriptionTag );
	lua_pushstring(L, _class->m_FullLuaName.c_str() );
	lua_rawset(L, -3);

	// tries to grab a metatable from current table at the top of stack
	if(lua_getmetatable(L, -1) == 0)
	{
		// tables has not a metatable
		lua_newtable(L); // creates a new metatable
	}

	if(!_class->m_Constructors.empty())
	{
		// sets constructor's call if there are any
		lua_pushstring(L, "__call");	
		lua_pushlightuserdata(L, (void*)_class);
		lua_pushcclosure(L, LuaGlobalConstructorsDispatcher, 1);
		lua_rawset(L, -3); // sets () handler
	}

	lua_setmetatable(L, -2);

	_class->RegisterClassNamespaceDetails(this, L);

	lua_setfield(L, _tind, _luaclass);
};

CHost *CHost::GetFromLua(lua_State *L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "___MLUABIND_HOST_PTR");
	assert(lua_isuserdata(L, -1));
	assert(lua_islightuserdata(L, -1));
	CHost *h = (CHost *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	assert(h != 0);
	return h;
};

void CHost::RegisterCommons()
{
	lua_register(L, "__get_rtti_type", &impl::LuaGetRTTIType);
	lua_register(L, "__get_type", &impl::LuaGetType);
	lua_register(L, "__get_typeex", &impl::LuaGetTypeEx);
	lua_register(L, "__cast_unsafe", &impl::LuaCastUnsafe);
	lua_register(L, "__const", &impl::LuaMakeConst);
	lua_register(L, "__cast", &impl::LuaCast);
	
	lua_pushstring(L, "___MLUABIND_HOST_PTR");
	lua_pushlightuserdata(L, (void*) this);
	lua_settable(L, LUA_GLOBALSINDEX);
};

int CHost::LuaGlobalConstructorsDispatcher(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	assert(lua_isuserdata(L, lua_upvalueindex(1)));
	assert(lua_islightuserdata(L, lua_upvalueindex(1)));
	impl::GenericClass *gclass = (impl::GenericClass *)lua_touserdata(L, lua_upvalueindex(1));
	return gclass->ConstructorCallDispatcher(host, L);
};

int CHost::LuaGlobalFunctionDispatcher(lua_State *L) 
		// closure params: lua function name (char *)
{
	const int maximum_overloaded_functions = 20;

	CHost *host = CHost::GetFromLua(L);

	assert( lua_isstring(L, lua_upvalueindex(1)) );
	const char *lua_func_name = lua_tostring(L, lua_upvalueindex(1));

	LuaFunctionsT::iterator i = host->m_LuaFunctions.find(lua_func_name);
	if(i != host->m_LuaFunctions.end())
	{
		// perform arguments match for all overloaded functions
		const std::vector<impl::GenericFunction*>& functions = (*i).second;
			
		int minimum_fval = impl::ParameterMatcherBase::CantBeConverted; // maximum value by default - can't be called
		size_t overloaded_count = 0;
		impl::GenericFunction* overloads[maximum_overloaded_functions];
			
		for(size_t i = 0; i < functions.size(); ++i)
		{
			int res = functions[i]->MatchParams(L);
			if( res < minimum_fval ) // more suitable function found
			{
				minimum_fval = res;
				overloaded_count = 0;
				overloads[overloaded_count ] = functions[i];
				++overloaded_count;
			}
			else if(res == minimum_fval)
			{
				if(overloaded_count < maximum_overloaded_functions - 1)
				{
					overloads[overloaded_count ] = functions[i];
					++overloaded_count;
				}
			}
		}

		if( minimum_fval == impl::ParameterMatcherBase::CantBeConverted || overloaded_count > 1)
		{
			char buf[4096];
			strcpy(buf, "");
			for(size_t n = 0; n < overloaded_count; ++n)
			{
				strcat(buf, overloads[n]->GetVerboseName(L).c_str());
				strcat(buf, "\n");
			}			
			host->Error("Can't match function to call %s, candidates are:\n%sParameters are:\n%sCaused by: %s.\n",
				lua_func_name,				
				buf,
				impl::GetParametersInfo(L).c_str(),
				minimum_fval == impl::ParameterMatcherBase::CantBeConverted ? "no one can be used" : "ambiguity");

			return 0;
		}

		if(overloaded_count == 0)
		{
			CHost::GetFromLua(L)->Error("LuaGlobalFunctionDispatcher: panic - can't find any functions for %s!\n", lua_func_name);
			return 0;
		}

		impl::HostImplicitCreatedStackFrameGuard g(host);
		return overloads[0]->PerformCall(host, L);
	}
	else
	{
		CHost::GetFromLua(L)->Error("LuaGlobalFunctionDispatcher: panic - can't find GenericFunction %s!\n", lua_func_name);
		return 0;
	}
};

size_t CHost::GetImplicitCreatedCount() const
{
	return m_ImplicitCreated.size();
};

void CHost::RollBackImplicitCreated(size_t _newcount)
{
	while(_newcount < m_ImplicitCreated.size())
	{
		impl::ImplicitCreatedHolder* p = m_ImplicitCreated.top();
		if (p)
		{
			 delete (p);
		}
		
		m_ImplicitCreated.pop();
	}
};

}; // namespace mluabind

