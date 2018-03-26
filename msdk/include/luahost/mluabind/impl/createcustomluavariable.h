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

#include "lokiinclude.h"
#include "util.h"

namespace mluabind
{

class CHost;

namespace impl
{

template <int _type, class _C> struct CreateCustomLuaVariable {};

template <class _C>
struct CreateCustomLuaVariable<ResolvCallTypeBase::Fundamental, _C>
{
	static inline int Do(lua_State *L, CHost *_host, _C _c, bool _const)
	{
		typedef typename std::tr1::remove_cv<_C>::type clean_type;

		typedef typename Loki::Select<
			__is_enum(clean_type),
			lua_Number,
			clean_type>::Result type; // enum workaround

		return Impl<type>::Do(L, _c);
	};

	template <class Type> struct Impl {/*dummy class for errors on CT*/};

	// number types section
	template <> struct Impl <signed   int>        { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <unsigned int>        { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <signed   short>      { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <unsigned short>      { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <signed   long>       { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <unsigned long>       { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <float>               { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <double>              { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <long double>         { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, _c), 1; } };
	template <> struct Impl <signed long long>      { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, (LUA_NUMBER)_c), 1; } };
	template <> struct Impl <unsigned long long>    { static inline int Do(lua_State *L, _C _c) { return lua_pushnumber(L, (LUA_NUMBER)_c), 1; } };

	// string type section
	template <> struct Impl <char*>               { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, _c), 1; } };
	template <> struct Impl <const char*>         { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, _c), 1; } };
	template <> struct Impl <signed char*>        { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, (char*)_c), 1; } };
	template <> struct Impl <unsigned char*>      { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, (char*)_c), 1; } };
	template <> struct Impl <const signed char*>  { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, (const char*)_c), 1; } };
	template <> struct Impl <const unsigned char*>{ static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, (const char*)_c), 1; } };
	template <> struct Impl <wchar_t*>            { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, ConvertFromUnicode(_c)), 1; } };
	template <> struct Impl <const wchar_t*>      { static inline int Do(lua_State *L, _C _c) { return lua_pushstring(L, ConvertFromUnicode(_c)), 1; } };

	template <> struct Impl <char>                { static inline int Do(lua_State *L, _C _c) { char str[2]; str[0] = _c; str[1]=0; return lua_pushstring(L, str), 1; } };
	template <> struct Impl <signed char>         { static inline int Do(lua_State *L, _C _c) { char str[2]; str[0] = _c; str[1]=0; return lua_pushstring(L, str), 1; } };
	template <> struct Impl <unsigned char>       { static inline int Do(lua_State *L, _C _c) { char str[2]; str[0] = _c; str[1]=0; return lua_pushstring(L, str), 1; } };
	template <> struct Impl <wchar_t>             { static inline int Do(lua_State *L, _C _c) {wchar_t str[2];str[0]= _c; str[1]=0; return lua_pushstring(L, ConvertFromUnicode(str)), 1; } };

	// bool type section
	template <> struct Impl <bool>                { static inline int Do(lua_State *L, _C _c) { return lua_pushboolean(L, _c), 1; } };
};

template <class _C>
struct CreateCustomLuaVariable<ResolvCallTypeBase::ByVal, _C>
{
	typedef typename std::tr1::remove_cv<_C>::type clean_type;
	static int Do(lua_State *L, CHost *_host, const clean_type &_c, bool _const)
	{
		GenericClass *c = _host->FindCPPGenericClass(typeid(CustomClass<clean_type>).name());
		if(!c)
		{
			_host->Error("CreateCustomLuaVariable<ByVal> can't find GenericClass for %s!\n", typeid(CustomClass<clean_type>).name());
			assert(0);
			return 0;
		}

		return CustomClass<clean_type>::ConstructObjectByCopyConstructor(L, _host, c, _c, _const);
	};
};

template <class _C>
struct CreateCustomLuaVariable<ResolvCallTypeBase::ByRef, _C>
{
	inline static int Do(lua_State *L, CHost *_host, _C _c, bool _const)
	{
		return CreateCustomLuaVariable<
			ResolvCallTypeBase::ByPtr,
			typename std::tr1::remove_reference<_C>::type *>
			::Do(L, _host, &_c, _const);
	};
};

template <class _C>
struct CreateCustomLuaVariable<ResolvCallTypeBase::ByPtr, _C>
{
	static int Do(lua_State *L, CHost *_host, _C _c, bool _const)
	{
		if(_c == 0)
		{
			lua_pushnil(L);
			return 1;
		}

		typedef typename std::tr1::remove_cv<typename std::tr1::remove_pointer<_C>::type>::type clean_type;
		GenericClass *c = _host->FindCPPGenericClass(typeid(CustomClass<clean_type>).name());
		if(!c)
		{
			_host->Error("CreateCustomLuaVariable<ByPtr> can't find GenericClass for %s!\n", typeid(CustomClass<clean_type>).name());
			assert(0);
			return 0;
		}

		// created raw lua object
		void *s = lua_newuserdata(L, sizeof(LuaCustomVariable));

		// set metatables
		c->SetMetatables(L);

		// construct header
		new (s) LuaCustomVariable(false, std::tr1::is_const<typename std::tr1::remove_pointer<_C>::type>::value, c, (void*)_c);

		LOKI_STATIC_CHECK(!IsFundamentalCVRef<clean_type>::value, DoNotUsePointersToFundamentalTypes);

		return 1;
	};
};

template <>
struct CreateCustomLuaVariable<ResolvCallTypeBase::ByPtr, const void*>
{
	static int Do(lua_State *L, CHost *_host, const void* _c, bool _const)
	{
		// create a pointer in Lua via lightuserdata
		lua_pushlightuserdata(L, (void*)_c);
		return 1;
	};
};

template <>
struct CreateCustomLuaVariable<ResolvCallTypeBase::ByPtr, void*>
{
	static int Do(lua_State *L, CHost *_host, void* _c, bool _const)
	{
		// create a pointer in Lua via lightuserdata
		lua_pushlightuserdata(L, _c);
		return 1;
	};
};

}; // namespace impl

}; // namespace mluabind
