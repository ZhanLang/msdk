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

#include <cstring>

#include "util.h"
#include "luacustomvariable.h"

namespace mluabind
{

namespace impl
{

struct ParameterMatcherBase
{
	enum {SameType = 1, CanBeConverted = 2, ImplicitConstructor = 3, CantBeConverted = 1000000};
};

template <class P, int _calltype = ResolvCallType<P>::CallType>
struct ParameterMatcher;

template <class PP>
struct ParameterMatcher<PP, ResolvCallTypeBase::Fundamental> : public ParameterMatcherBase
{
	// a lot of brutal workarounds for enum-hack.
	// (treat it as int, but convert into it's type transparently)

	typedef typename std::tr1::remove_reference<PP>::type PPP;

	typedef typename Loki::Select<
		__is_enum(PPP),
		int,
		PPP>::Result P;

	inline static int MatchParam(lua_State *L, int _pind, bool _accept_implicit)
	{
		typedef typename Loki::Select<
			__is_enum(PP),
			int,
			PP>::Result type; // enum workaround
		return Impl<type>::Match(L, _pind);
	};

	inline static PPP ExtractParam(lua_State *L, int _pind)
	{
		typedef typename Loki::Select<
			__is_enum(PP),
			int,
			PP>::Result type; // enum workaround
		return (PPP)Impl<type>::Extract(L, _pind);
	};

	template <class Type> struct Impl {};

/////////////////////////////////////////////////////////////////////// number types section
#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind)\
{\
	if(lua_type(L, _pind) == LUA_TNUMBER) return SameType;\
	else if(lua_type(L, _pind) == LUA_TBOOLEAN) return CanBeConverted;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if(lua_type(L, _pind) == LUA_TNUMBER) return (P)lua_tonumber(L, _pind);\
	else if(lua_type(L, _pind) == LUA_TBOOLEAN) return (P)lua_toboolean(L, _pind);\
	assert(0);\
	return P();\
}};
	template <> struct Impl <signed   int>         __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned int>         __MLUABIND_DO_IMPL__
	template <> struct Impl <signed   short>       __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned short>       __MLUABIND_DO_IMPL__
	template <> struct Impl <signed   long>        __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned long>        __MLUABIND_DO_IMPL__
	template <> struct Impl <signed  long long>    __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned long long>   __MLUABIND_DO_IMPL__

	template <> struct Impl <float>                __MLUABIND_DO_IMPL__
	template <> struct Impl <double>               __MLUABIND_DO_IMPL__
	template <> struct Impl <long double>          __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed   int>   __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned int>   __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed   short> __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned short> __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed   long>  __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned long>  __MLUABIND_DO_IMPL__
	template <> struct Impl <const float>          __MLUABIND_DO_IMPL__
	template <> struct Impl <const double>         __MLUABIND_DO_IMPL__
	template <> struct Impl <const long double>    __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned short&>__MLUABIND_DO_IMPL__
	template <> struct Impl <const signed short&>  __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed&>        __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned&>      __MLUABIND_DO_IMPL__
	template <> struct Impl <const float&>         __MLUABIND_DO_IMPL__
	template <> struct Impl <const double&>        __MLUABIND_DO_IMPL__
	template <> struct Impl <const long double&>   __MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__

/////////////////////////////////////////////////////////////////////// string type section
#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind)\
{\
	if(lua_isnil(L, _pind)) return SameType;\
	if(lua_type(L, _pind) == LUA_TSTRING) return SameType;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if(lua_isnil(L, _pind)) return (P)0;\
	if(lua_type(L, _pind) == LUA_TSTRING) return (P)lua_tostring(L, _pind);\
	assert(0);\
	return P();\
}};
	template <> struct Impl <char*>               __MLUABIND_DO_IMPL__
	template <> struct Impl <const char*>         __MLUABIND_DO_IMPL__
	template <> struct Impl <signed char*>        __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned char*>      __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed char*>  __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned char*>__MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__

#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind)\
{\
	if(lua_isnil(L, _pind)) return CanBeConverted;\
	if(lua_type(L, _pind) == LUA_TSTRING) return CanBeConverted;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if(lua_isnil(L, _pind)) return (P)0;\
	if(lua_type(L, _pind) == LUA_TSTRING) return (P)ConvertToUnicode(lua_tostring(L, _pind));\
	assert(0);\
	return P();\
}};
	template <> struct Impl <wchar_t*>               __MLUABIND_DO_IMPL__
	template <> struct Impl <const wchar_t*>         __MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__

/////////////////////////////////////////////////////////////////////// char type section
#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind)\
{\
	if((lua_type(L, _pind) == LUA_TSTRING) && (std::strlen(lua_tostring(L, _pind)) == 1)) return SameType;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if((lua_type(L, _pind) == LUA_TSTRING) && (std::strlen(lua_tostring(L, _pind)) == 1)) return (P)(lua_tostring(L, _pind)[0]);\
	assert(0);\
	return P();\
}};
	template <> struct Impl <char>                __MLUABIND_DO_IMPL__
	template <> struct Impl <signed char>         __MLUABIND_DO_IMPL__
	template <> struct Impl <unsigned char>       __MLUABIND_DO_IMPL__
	template <> struct Impl <const char>          __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed char>   __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned char> __MLUABIND_DO_IMPL__
	template <> struct Impl <const char&>         __MLUABIND_DO_IMPL__
	template <> struct Impl <const signed char&>  __MLUABIND_DO_IMPL__
	template <> struct Impl <const unsigned char&>__MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__

#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind)\
{\
	if((lua_type(L, _pind) == LUA_TSTRING) && (std::strlen(lua_tostring(L, _pind)) == 1)) return SameType;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if((lua_type(L, _pind) == LUA_TSTRING) && (std::strlen(lua_tostring(L, _pind)) == 1))\
		return (P)(ConvertToUnicode(lua_tostring(L, _pind))[0]);\
	assert(0);\
	return P();\
}};
	template <> struct Impl <wchar_t>                __MLUABIND_DO_IMPL__
	template <> struct Impl <const wchar_t&>         __MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__

/////////////////////////////////////////////////////////////////////// bool type section
#define __MLUABIND_DO_IMPL__ { inline static int Match(lua_State *L, int _pind) \
{\
	if(lua_type(L, _pind) == LUA_TBOOLEAN) return SameType;\
	else if(lua_type(L, _pind) == LUA_TNUMBER) return CanBeConverted;\
	return CantBeConverted;\
}\
inline static P Extract(lua_State *L, int _pind)\
{\
	if(lua_type(L, _pind) == LUA_TBOOLEAN) return (lua_toboolean(L, _pind) != 0);\
	else if(lua_type(L, _pind) == LUA_TNUMBER) return ((int)lua_tonumber(L, _pind)) != 0;\
	assert(0);\
	return P();\
}};
	template <> struct Impl <bool>                __MLUABIND_DO_IMPL__
	template <> struct Impl <const bool>          __MLUABIND_DO_IMPL__
	template <> struct Impl <const bool&>         __MLUABIND_DO_IMPL__
#undef __MLUABIND_DO_IMPL__
};

template <class P, bool _Const>
struct ParameterMatcherForClasses : public ParameterMatcherBase
{
	static int MatchParam(lua_State *L, int _pind, bool _accept_implicit = true)
	{
		typedef typename std::tr1::remove_cv<P>::type clean_type;
		static Loki::TypeInfo ti(typeid(CustomClass<clean_type>));
		if(lua_isuserdata(L, _pind) && !lua_islightuserdata(L, _pind))
		{
			// check if current variable can be converted into desired type
			LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, _pind);
	
			if(lcv->getclass()->GetTypeID() == ti)
			{
				if(_Const)
					return SameType;
				else if(!lcv->IsConst()) // should be non-const to pass
					return SameType;
			}
			else if(lcv->getclass()->CanBeUpCastedTo(CHost::GetFromLua(L), ti))
			{
				if(_Const)
					return CanBeConverted;
				else if(!lcv->IsConst()) // should be non-const to pass
					return CanBeConverted;
			}
		}

		if(_accept_implicit)
		{
			// check if we can use implicit object creation to produce desired type
			CHost *host = CHost::GetFromLua(L);
			GenericClass *c = host->FindCPPGenericClass(ti.name());
			if(!c)
			{
				host->Error("ParameterMatcherForClasses::MatchParam: panic - can't find generic class for '%s'.", ti.name());
				return CantBeConverted;
			}
			if(c->CanBeImplicityConstructedFrom(host, L, _pind))
				return ImplicitConstructor;
		}
		return CantBeConverted;
	};
};

template <class P>
struct ParameterMatcher<P, ResolvCallTypeBase::ByVal> : public ParameterMatcherBase
{
	inline static int MatchParam(lua_State *L, int _pind, bool _accept_implicit)
	{
		return ParameterMatcherForClasses<P, std::tr1::is_const<P>::value>::MatchParam(L, _pind, _accept_implicit);
	};

	static P &ExtractParam(lua_State *L, int _pind)
	{
		typedef typename std::tr1::remove_cv<P>::type clean_type;
		static Loki::TypeInfo ti(typeid(CustomClass<clean_type>));
		int pm = MatchParam(L, _pind, true);

		if(pm == CantBeConverted)
		{
			CHost::GetFromLua(L)->Error("ParameterMatcher::ExtractParam: panic - MLuaBind internal error: trying to extract an unconvertable parameter!");
			assert(0);
		}

		if(pm == ImplicitConstructor)
		{
			CHost *host = CHost::GetFromLua(L);
			GenericClass *c = host->FindCPPGenericClass(ti.name());
			if(!c)
			{
				host->Error("ParameterMatcher::ExtractParam: panic - can't find generic class for '%s'.", ti.name());
				assert(0);
			}

			clean_type *obj = (clean_type *)c->ConstructObjectFrom(host, L, _pind);
			host->PutNewImplicitCreated(obj);

			return *obj;
		}
		else
		{
			if(!lua_isuserdata(L, _pind) || lua_islightuserdata(L, _pind))
			{
				CHost::GetFromLua(L)->Error("ParameterMatcher::ExtractParam: panic - CPP object is not a userdata!\n");
				assert(0);
			}
			LuaCustomVariable *ud = (LuaCustomVariable*)lua_touserdata(L, _pind);
			void *targetdata = ud->getclass()->UpCastToType(CHost::GetFromLua(L), ti,	ud->value());
			return *(P*)targetdata;
		}
	};	
};

template <class P> struct ParameterMatcher<P, ResolvCallTypeBase::ByRef> : 
ParameterMatcher<typename std::tr1::remove_reference<P>::type, ResolvCallTypeBase::ByVal>{};

template <class P> struct ParameterMatcher<P, ResolvCallTypeBase::ByPtr> : public ParameterMatcherBase
{
	inline static int MatchParam(lua_State *L, int _pind, bool _accept_implicit)
	{
		if( lua_isnil(L, _pind) ) // converts nil to (T*)0
			return CanBeConverted;

		return ParameterMatcherForClasses<typename std::tr1::remove_pointer<P>::type,
			std::tr1::is_const<typename std::tr1::remove_pointer<P>::type>::value>::
			MatchParam(L, _pind, _accept_implicit);
	};

	inline static P ExtractParam(lua_State *L, int _pind)
	{
		if( lua_isnil(L, _pind) )
			return (P)0;

		return &ParameterMatcher<typename std::tr1::remove_pointer<P>::type, ResolvCallTypeBase::ByVal>::
			ExtractParam(L, _pind);
	};	
};

template <> struct ParameterMatcher<void*, ResolvCallTypeBase::ByPtr> : public ParameterMatcherBase
{
	inline static int MatchParam(lua_State *L, int _pind, bool _accept_implicit)
	{
		if( lua_islightuserdata(L, _pind) )
			return SameType;

		return CantBeConverted;
	};

	inline static void *ExtractParam(lua_State *L, int _pind)
	{
		if(!lua_islightuserdata(L, _pind))
			assert(0); // panic

		return lua_touserdata(L, _pind);
	};	
};

template <> struct ParameterMatcher<const void*, ResolvCallTypeBase::ByPtr> : public ParameterMatcherBase
{
	inline static int MatchParam(lua_State *L, int _pind, bool _accept_implicit)
	{
		if( lua_islightuserdata(L, _pind) )
			return SameType;

		return CantBeConverted;
	};

	inline static const void *ExtractParam(lua_State *L, int _pind)
	{
		if(!lua_islightuserdata(L, _pind))
			assert(0); // panic

		return lua_touserdata(L, _pind);
	};	
};

template <class _Dummy = void>
struct MultipleParameterMatcher0 : public ParameterMatcherBase
{
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if((lua_gettop(L)-(_start_from-1)) != 0)
			return CantBeConverted;

		return SameType;
	};
};

template <class T1>
struct MultipleParameterMatcher1 : public ParameterMatcherBase
{
	enum {ParamNum = 1};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2>
struct MultipleParameterMatcher2 : public ParameterMatcherBase
{
	enum {ParamNum = 2};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3>
struct MultipleParameterMatcher3 : public ParameterMatcherBase
{
	enum {ParamNum = 3};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4>
struct MultipleParameterMatcher4 : public ParameterMatcherBase
{
	enum {ParamNum = 4};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4, class T5>
struct MultipleParameterMatcher5 : public ParameterMatcherBase
{
	enum {ParamNum = 5};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T5, ResolvCallType<T5>::CallType>
			::MatchParam(L, _start_from + 4, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct MultipleParameterMatcher6 : public ParameterMatcherBase
{
	enum {ParamNum = 6};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T5, ResolvCallType<T5>::CallType>
			::MatchParam(L, _start_from + 4, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T6, ResolvCallType<T6>::CallType>
			::MatchParam(L, _start_from + 5, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct MultipleParameterMatcher7 : public ParameterMatcherBase
{
	enum {ParamNum = 7};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T5, ResolvCallType<T5>::CallType>
			::MatchParam(L, _start_from + 4, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T6, ResolvCallType<T6>::CallType>
			::MatchParam(L, _start_from + 5, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T7, ResolvCallType<T7>::CallType>
			::MatchParam(L, _start_from + 6, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct MultipleParameterMatcher8 : public ParameterMatcherBase
{
	enum {ParamNum = 8};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T5, ResolvCallType<T5>::CallType>
			::MatchParam(L, _start_from + 4, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T6, ResolvCallType<T6>::CallType>
			::MatchParam(L, _start_from + 5, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T7, ResolvCallType<T7>::CallType>
			::MatchParam(L, _start_from + 6, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T8, ResolvCallType<T8>::CallType>
			::MatchParam(L, _start_from + 7, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct MultipleParameterMatcher9 : public ParameterMatcherBase
{
	enum {ParamNum = 9};
	inline static int MatchParams(lua_State *L, int _start_from = 1, int _amount=0, bool _accept_implicit=true)
	{
		// _amount - how much variables in lua stack from _start_from position should me matched, zero - anyhow
		assert(_amount>=0);
		assert(_start_from>=1);

		if(!_amount)
		{
			if((lua_gettop(L)-(_start_from-1)) != ParamNum)
				return CantBeConverted;
		}
		else
		{
			if( (lua_gettop(L)-(_start_from-1)) < _amount)
				return CantBeConverted;
			if(_amount < ParamNum)
				return CantBeConverted;
		}

		int res = 1;
		if( (res *= ParameterMatcher<T1, ResolvCallType<T1>::CallType>
			::MatchParam(L, _start_from + 0, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T2, ResolvCallType<T2>::CallType>
			::MatchParam(L, _start_from + 1, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T3, ResolvCallType<T3>::CallType>
			::MatchParam(L, _start_from + 2, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T4, ResolvCallType<T4>::CallType>
			::MatchParam(L, _start_from + 3, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T5, ResolvCallType<T5>::CallType>
			::MatchParam(L, _start_from + 4, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T6, ResolvCallType<T6>::CallType>
			::MatchParam(L, _start_from + 5, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T7, ResolvCallType<T7>::CallType>
			::MatchParam(L, _start_from + 6, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T8, ResolvCallType<T8>::CallType>
			::MatchParam(L, _start_from + 7, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;
		if( (res *= ParameterMatcher<T9, ResolvCallType<T9>::CallType>
			::MatchParam(L, _start_from + 8, _accept_implicit)) >= CantBeConverted)
			return CantBeConverted;

		assert(res <= CantBeConverted);
		return res;
	};
};


}; // namespace impl

}; // namespace mluabind
