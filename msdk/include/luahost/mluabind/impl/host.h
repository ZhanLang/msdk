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

#include <string>
#include <typeinfo>
#include <assert.h>
#include <map>
#include <set>
#include <stack>
#include <vector>

#include "lokiinclude.h"
#include "class.h"
#include "function.h"
#include "declarator.h"
#include "callluafunction.h"

namespace mluabind
{

namespace impl
{
class CLuaClassesManager;
};

class CHost
{
public:
	CHost(lua_State *_L);
	virtual ~CHost();
	void Insert(impl::GenericClass &_class);
	void Insert(impl::GenericFunction &_function);
	void Insert(impl::DeclaratorClass &_declarator);
	void Insert(impl::GenericConstant &_constant);

	impl::GenericClass *FindCPPGenericClass(const char *_rtti_name);
	impl::GenericClass *FindLuaGenericClass(const char *_lua_name);
	static CHost *GetFromLua(lua_State *L);
	lua_State *GetLua();

	virtual void Error(const char *_format, ...);

	template <class _Ret>
		_Ret CallLuaFunction(const std::string &_name);
	template <class _Ret, class _P1>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1);
	template <class _Ret, class _P1, class _P2>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2);
	template <class _Ret, class _P1, class _P2, class _P3>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9);
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9, class _P10>
		_Ret CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9, _P10 _p10);

	//implicit created variables section
	template <class _T> void PutNewImplicitCreated(_T *_obj);
	size_t GetImplicitCreatedCount() const;
	void   RollBackImplicitCreated(size_t _newcount);

private:
	CHost(const CHost&); // do not copy it!
	void InsertRec(impl::GenericClass &_class, int _tind, const char *_dn); 
	void InsertRec(impl::GenericFunction &_function, int _tind, const char *_dn);
	void InsertRec(impl::GenericConstant &_constant, int _tind, const char *_dn); // -""-

	void RegisterClass(const char* _luaclass, impl::GenericClass *_class, int _tind); // tind - index of a table in stack to write
	void RegisterCommons();
	static int LuaGlobalFunctionDispatcher(lua_State *L);
	static int LuaGlobalConstructorsDispatcher(lua_State *L);

	typedef std::map</*const char **//*impl::String*/std::string, impl::GenericClass *> CPPClassesT; // maps RTTI name of class to generic class
	typedef std::map<std::string, impl::GenericClass *> LuaClassesT; // maps full lua name of class to generic class
	typedef std::map<std::string, std::vector<impl::GenericFunction*> > LuaFunctionsT;
	typedef std::stack<impl::ImplicitCreatedHolder*> ImplicitCreatedT;

	CPPClassesT m_CPPClasses;
	LuaClassesT m_LuaClasses;
	LuaFunctionsT m_LuaFunctions;
	ImplicitCreatedT m_ImplicitCreated;
	lua_State *L;
};

#include "hostinl.h"

}; // namespace mluabind
