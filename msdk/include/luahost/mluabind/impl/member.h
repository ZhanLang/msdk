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

struct GenericMember
{
	virtual ~GenericMember() {};
	virtual int PerformIndexOperator(lua_State *L, LuaCustomVariable *_lcv) {assert(0); return 0;};
	virtual int PerformNewIndexOperator(lua_State *L, LuaCustomVariable *_lcv, int _valind) {assert(0); return 0;};
};

template <class _Base, class _P>
struct MemberClass : GenericMember
{
	typedef MemberClass<_Base, _P> Me;
	_P _Base::*m_Obj;

	MemberClass(_P _Base::*_obj) : m_Obj(_obj) {};

	virtual int PerformIndexOperator(lua_State *L, LuaCustomVariable *_lcv)
	{
		_Base *_this = ExtractObjectPointer<_Base, false>(_lcv);

		return FundamentalBranch<ResolvCallType<_P>::CallType == ResolvCallTypeBase::Fundamental>::Do(L, _this, this, _lcv);
	};
	
	template <bool b> struct FundamentalBranch
	{
		inline static int Do(lua_State *L, _Base *_this, Me *_me, LuaCustomVariable *_lcv)
		{
			typedef ExtractRefValueFromMember<_Base, _P> tmp;
			int ret = CreateCustomLuaVariable<ResolvCallTypeBase::ByRef, typename tmp::Type&>::
				Do(L, CHost::GetFromLua(L), tmp::Do(_this, _me->m_Obj), std::tr1::is_const<typename tmp::Type>::value);
			// hack - override "const" field on lcv if parent is const;
			if(_lcv->IsConst())
			{
				LuaCustomVariable *newlcv = (LuaCustomVariable *)lua_touserdata(L, -1);
				newlcv->SetConst(true);
			}
			return ret;
		};
	};

	template <> struct FundamentalBranch <true>
	{
		inline static int Do(lua_State *L, _Base *_this, Me *_me, LuaCustomVariable *_lcv)
		{
			typedef ExtractRefValueFromMember<_Base, _P> tmp;
			return CreateCustomLuaVariable<ResolvCallTypeBase::Fundamental, typename tmp::Type>::
				Do(L, CHost::GetFromLua(L), tmp::Do(_this, _me->m_Obj), false);
		};
	};

	virtual int PerformNewIndexOperator(lua_State *L, LuaCustomVariable *_lcv, int _valind)
	{
		typedef ExtractRefValueFromMember<_Base, _P> tmp;
		if(std::tr1::is_const<typename tmp::Type>::value)
		{
			CHost::GetFromLua(L)->Error("MemberClass::PerformNewIndexOperator: can't assign to a const variable.\n");
			return 0;
		};

		enum { can_perform = (__has_nothrow_assign(typename tmp::Type) ||
			__has_assign(typename tmp::Type) ||
			IsFundamentalCVRef<typename tmp::Type>::value) &&
			!std::tr1::is_const<typename tmp::Type>::value };
		if( can_perform != 0 )
		{
			return DummyBranch<can_perform != 0>::PerformNewIndexOperatorImpl(L, _lcv, _valind, this);
		}
		else
		{
			CHost::GetFromLua(L)->Error("MemberClass::PerformNewIndexOperator: left value has no assign operator.\n");
			return 0;
		}

		return 0;
	};

	template <bool b> struct DummyBranch
	{ static inline int PerformNewIndexOperatorImpl(lua_State *L, LuaCustomVariable *_lcv, int _valind, Me *_me) {return 0;} };

	template <> struct DummyBranch <true>
	{
		static inline int PerformNewIndexOperatorImpl(lua_State *L, LuaCustomVariable *_lcv, int _valind, Me *_me)
		{
			typedef ExtractRefValueFromMember<_Base, _P> tmp;
			typedef ExtractRefValueFromMember<_Base, const _P> const_tmp;
			int res = ParameterMatcher<typename const_tmp::Type, ResolvCallType<typename const_tmp::Type>::CallType >::MatchParam(L, _valind, true);
			if(res < ParameterMatcherBase::CantBeConverted)
			{
				_Base *_this = ExtractObjectPointer<_Base, false>(_lcv);
				HostImplicitCreatedStackFrameGuard g(CHost::GetFromLua(L));
				tmp::Do(_this, _me->m_Obj) = 
					ParameterMatcher<typename const_tmp::Type, ResolvCallType<typename const_tmp::Type>::CallType>::ExtractParam(L, _valind);
			}
			else
				CHost::GetFromLua(L)->Error("MemberClass::PerformNewIndexOperator: can't convert a variable to assign.\n");
			return 0;
		}
	};
};

template <class _Base, class _P, class _R>
struct PropertyClass : GenericMember
{
	typedef _R (_Base::*GetterType) () const;
	typedef void (_Base::*SetterType) (_P);

	GetterType m_Getter;
	SetterType m_Setter;
	PropertyClass(GetterType _getter, SetterType _setter):
		m_Getter(_getter),
		m_Setter(_setter)
	{
		assert(m_Getter != 0);
	};

	virtual int PerformIndexOperator(lua_State *L, LuaCustomVariable *_lcv)
	{
		const _Base *_this = ExtractObjectPointer<_Base, true>(_lcv);
		return CreateCustomLuaVariable<ResolvCallType<_R>::CallType, _R>
			::Do(L, CHost::GetFromLua(L), (_this->*m_Getter)(), ResolvCallType<_R>::IsConst);
	};

	virtual int PerformNewIndexOperator(lua_State *L, LuaCustomVariable *_lcv, int _valind)
	{
		if(m_Setter == 0)
		{
			CHost::GetFromLua(L)->Error("PropertyClass::PerformNewIndexOperator: can't assign - no setter available.\n");
			return 0;
		}

		if(_lcv->IsConst())
		{
			CHost::GetFromLua(L)->Error("PropertyClass::PerformNewIndexOperator: can't change property at const object.\n");
			return 0;
		}

		if(ParameterMatcher<_P>::MatchParam(L, _valind, true) == ParameterMatcherBase::CantBeConverted)
		{
			CHost::GetFromLua(L)->Error("PropertyClass::PerformNewIndexOperator: can't convert params to call setter.\nParameters are:\n%s", GetParametersInfo(L, _valind).c_str());
			return 0;
		}

		_Base *_this = ExtractObjectPointer<_Base, false>(_lcv);	
		HostImplicitCreatedStackFrameGuard g(CHost::GetFromLua(L));
		(_this->*m_Setter)(ParameterMatcher<_P>::ExtractParam(L, _valind));
		return 0;
	};
};

template <class _Base, class _P, class _R>
struct FreePropertyClass : GenericMember
{
	typedef _R (*GetterType) (const _Base*);
	typedef void (*SetterType) (_Base*, _P);

	GetterType m_Getter;
	SetterType m_Setter;
	FreePropertyClass(GetterType _getter, SetterType _setter):
		m_Getter(_getter),
		m_Setter(_setter)
	{
		assert(m_Getter != 0);
	};

	virtual int PerformIndexOperator(lua_State *L, LuaCustomVariable *_lcv)
	{
		const _Base *_this = ExtractObjectPointer<_Base, true>(_lcv);
		return CreateCustomLuaVariable<ResolvCallType<_R>::CallType, _R>
			::Do(L, CHost::GetFromLua(L), m_Getter(_this), ResolvCallType<_R>::IsConst);
	};

	virtual int PerformNewIndexOperator(lua_State *L, LuaCustomVariable *_lcv, int _valind)
	{
		if(m_Setter == 0)
		{
			CHost::GetFromLua(L)->Error("FreePropertyClass::PerformNewIndexOperator: can't assign - no setter available.\n");
			return 0;
		}

		if(_lcv->IsConst())
		{
			CHost::GetFromLua(L)->Error("FreePropertyClass::PerformNewIndexOperator: can't change property at const object.\n");
			return 0;
		}

		if(ParameterMatcher<_P>::MatchParam(L, _valind, true) == ParameterMatcherBase::CantBeConverted)
		{
			CHost::GetFromLua(L)->Error("FreePropertyClass::PerformNewIndexOperator: can't convert params to call setter.\nParameters are:\n%s", GetParametersInfo(L, _valind).c_str());
			return 0;
		}

		_Base *_this = ExtractObjectPointer<_Base, false>(_lcv);
		HostImplicitCreatedStackFrameGuard g(CHost::GetFromLua(L));
		m_Setter(_this, ParameterMatcher<_P>::ExtractParam(L, _valind));
		return 0;
	};
};

}; // namespace impl

}; // namespace mluabind
