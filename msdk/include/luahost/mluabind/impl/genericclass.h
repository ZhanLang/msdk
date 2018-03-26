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
#include <map>
#include <vector>
#include <set>

#include "lokiinclude.h"
#include "util.h"
#include "createcustomluavariable.h"
#include "parametermatcher.h"
#include "declarator.h"
#include "method.h"
#include "member.h"
#include "constructor.h"
#include "operator.h"

namespace mluabind
{

namespace impl
{

struct ClassConvertAdapter
{
	virtual ~ClassConvertAdapter(){};
	virtual Loki::TypeInfo GetTarget() const = 0;
		// returns information about a type TO this adapter can conver current class
	virtual GenericClass *GetTargetGenericClass(CHost &_host) const = 0;
		// return covered information about target type.
	virtual void *ConvertToTarget(void *_current) const = 0;
		// returns pointer to target converted object
		// for instance inheritance: struct A{}; struct B:A{};
		// for B* this should return A*
	virtual bool CanPerformDowncast() const = 0;
		// for inheritance this should return true
		// and for converters as cast operators and smart pointers this should return false
	virtual void *ConvertFromTarget(void *_base) const = 0;
		// for inheritance:
		// struct A{}; struct B:A{};
		// for base = A* this sould B* if it is possible (thru dynamic_cast) or 0 otherwise.
	virtual bool ShouldRegisterNameSpaceDetails() const = 0;
};

struct ClassUpCastConvertAdapter : ClassConvertAdapter
{
	typedef void* (*CasterT)(void *_from);
	ClassUpCastConvertAdapter(Loki::TypeInfo _ti, CasterT _uc):
		m_TargetClassTypeInfo(_ti),
		m_UpCaster(_uc),
		m_TargetClass(0) {};
	~ClassUpCastConvertAdapter();
	virtual Loki::TypeInfo GetTarget() const { return m_TargetClassTypeInfo; };
	virtual GenericClass *GetTargetGenericClass(CHost &_host) const;
	virtual void *ConvertToTarget(void *_current) const;
	virtual bool CanPerformDowncast() const { return false; };
	virtual void *ConvertFromTarget(void *_base) const { assert(0); return 0; };
	virtual bool ShouldRegisterNameSpaceDetails() const { return false; };
private:
	const Loki::TypeInfo m_TargetClassTypeInfo;
	mutable GenericClass *m_TargetClass;
	const CasterT m_UpCaster;	
};

struct ClassInheritanceConvertAdapter : ClassUpCastConvertAdapter
{
	ClassInheritanceConvertAdapter(Loki::TypeInfo _ti, CasterT _uc, CasterT _dc):
		ClassUpCastConvertAdapter(_ti, _uc),
		m_DownCaster(_dc)
	{};
	~ClassInheritanceConvertAdapter();
	virtual bool ShouldRegisterNameSpaceDetails() const { return true; };
	virtual bool CanPerformDowncast() const { return true; };
	virtual void *ConvertFromTarget(void *_base) const
	{
		assert(m_DownCaster != 0);
		if(!m_DownCaster)
			return 0;
		return m_DownCaster(_base);
	};
private:
	const CasterT m_DownCaster;
};

struct GenericConstant : DeclaratedClass
{
	inline GenericConstant(const char *_ln, const char *_fln):
		DeclaratedClass(_ln, _fln){};
	virtual void PutOnStack(lua_State *L) = 0;
	virtual ~GenericConstant() {};
};

struct GenericClass : DeclaratedClass
{
	GenericClass(const char* _luaname, int (*_destructor)(lua_State* L), Loki::TypeInfo _myti);
	~GenericClass();
	Loki::TypeInfo GetTypeID() const;
	typedef std::map<String, std::vector<GenericMethod*> > MethodsT;
	typedef std::map<int, std::vector<GenericMethod*> > OperatorsT;
	typedef std::vector<GenericConstructor*> ConstructorsT;
	typedef std::map<String, GenericMember*> MembersT;
	typedef std::map<SimpleString, GenericConstant*> ConstantsT;


	int MethodCallDispatcher(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, const char *_name, const std::vector<GenericMethod*> &_overloads) const;
	int ConstructorCallDispatcher(CHost *_host, lua_State *L) const;

	int SetMetatables(lua_State *L) const;
	LuaCustomVariable *ConstructLuaUserdataObject(lua_State *L) const;
	static int LuaMethodCallProxieFunction(lua_State *L);
	static int LuaOperatorCallProxieFunction(lua_State *L);

	bool CanBeUpCastedTo(CHost *_host, Loki::TypeInfo _to) const;
	void *UpCastToType(CHost *_host, Loki::TypeInfo _to, void *_from) const;
	void *SafeCastToType(CHost *_host, Loki::TypeInfo _to, void *_from) const;

	// used by implicit object construction
	bool CanBeImplicityConstructedFrom(CHost* _host, lua_State *L, int _ind) const;
	void *ConstructObjectFrom(CHost* _host, lua_State *L, int _ind) const;

	static int MetaOperatorNewIndex(lua_State* L);
	bool ProcessOperatorNewIndexForMembers(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const;

	static int MetaOperatorIndex(lua_State* L);
	bool ProcessOperatorIndexForMembers(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const;
	bool ProcessOperatorIndexForMethods(CHost *_host, lua_State *L, const char *_name, LuaCustomVariable *_lcv, int &_res) const;
	void RegisterClassNamespaceDetails(CHost *_host, lua_State *L) const; // -1 - namespace table

protected:
	// generic class customizing methods
	bool InsertMethod(const char* _name, GenericMethod* _gm);
	bool InsertMember(const char* _name, GenericMember* _gm);
	bool InsertConstructor(GenericConstructor* _gc);
	bool InsertConverter(ClassConvertAdapter* _what);
	bool InsertOperator(int _type, GenericMethod *_gm);	
	bool InsertConstant(const char* _name, GenericConstant *_gc);

private:
	void CreateReachableClassesCache(CHost &_host); // called by CHost before it is inserted in it's map.

private:
	typedef std::map<Loki::TypeInfo, ClassConvertAdapter*> ConvertersT;
		// this map contains adapters, that can convert current type to type described by map key
	typedef std::map<Loki::TypeInfo, std::vector<GenericClass*> > ReachableClassesT;
		// this map contains information about classes, that can be reached by upcast from current type
		// for struct A{}; struct B:A{}; struct C:B{}; struct D:C{}; hierarhy
		// for class D this map will contain:
		// (A, (B,C,D)), (B, (C,D)), (C, (D))
	GenericClass(const GenericClass&); // do not copy such objects!

	bool HasConverters() const;
	ConvertersT m_Converters;
	Loki::TypeInfo m_MineTypeInfo;
	mutable LuaCustomVariable m_LCVCache;
	int (*m_Destructor)(lua_State* L);
	MethodsT m_Methods;
	ConstructorsT m_Constructors;
	MembersT m_Members;
	ConstantsT m_Constants;
	OperatorsT m_Operators;
	ReachableClassesT m_ReachableClasses;

	friend class ::mluabind::CHost;
};



}; // namespace impl

}; // namespace mluabind


