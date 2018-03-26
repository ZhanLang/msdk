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

#include <type_traits>
#include "lokiinclude.h"
#include "luacustomvariable.h"
#include "simplestring.h"

namespace mluabind
{

class CHost;

namespace impl
{

typedef SimpleString String;
//typedef CRefCountedConstString String;
//typedef std::string String;

struct ResolvCallTypeBase
{
	enum { ByVal, ByRef, ByPtr, Fundamental };
};

template <typename T>
struct MakeParameterType
{
	typedef typename Loki::Select<
		std::tr1::is_scalar<T>::value,
		T, 
		const T&>::Result 
	type;
};

template <typename T>
struct RemoveCVRef
{
	typedef typename std::tr1::remove_reference<
			typename std::tr1::remove_cv<T>::type
		>::type type;
};

template <typename T>
struct IsFundamentalCVRef
{
	enum {value = std::tr1::is_fundamental<typename RemoveCVRef<T>::type>::value };
};

template < class _Ret >
struct ResolvCallType : public ResolvCallTypeBase
{

	template <bool _b>struct CheckPtr {	enum {Result = ByVal}; };
	template <> struct CheckPtr<true> { enum {Result = ByPtr}; };

	template <bool _b> struct CheckRef { enum {Result = CheckPtr<std::tr1::is_pointer<_Ret>::value >::Result}; };
	template <> struct CheckRef<true>  { enum {Result = ByRef}; };

	template <bool _b> struct CheckFund { enum {Result = CheckRef<std::tr1::is_reference<_Ret>::value >::Result}; };
	template <>	 struct CheckFund<true>	{ enum {Result = Fundamental}; };

	enum {CallType = CheckFund<
		IsFundamentalCVRef<_Ret>::value ||
		std::tr1::is_same<_Ret, char*>::value ||
		std::tr1::is_same<_Ret, signed char*>::value ||
		std::tr1::is_same<_Ret, unsigned char*>::value ||
		std::tr1::is_same<_Ret, const char*>::value ||
		std::tr1::is_same<_Ret, const signed char*>::value ||
		std::tr1::is_same<_Ret, const unsigned char*>::value ||
		std::tr1::is_same<_Ret, wchar_t*>::value ||
		std::tr1::is_same<_Ret, const wchar_t*>::value ||
		__is_enum(_Ret)
		>::Result};
	enum {IsConst = std::tr1::is_const<_Ret>::value };
};

template <class T, bool _f = IsFundamentalCVRef<T>::value>
struct STLReferenceReturn
{
	template <bool _isptr>
	struct A
	{
		typedef T &Result;
	};

	template <>
	struct A<true>
	{
		typedef T Result;
	};

	typedef typename A<std::tr1::is_pointer<T>::value>::Result Result;
};

template <class T>
struct STLReferenceReturn<T, true> { typedef T Result; };

template <class T, bool _f = IsFundamentalCVRef<T>::value>
struct STLConstReferenceReturn
{
	template <bool _isptr>
	struct A
	{
		typedef const T &Result;
	};

	template <>
	struct A<true>
	{
		typedef const T Result;
	};

	typedef typename A<std::tr1::is_pointer<T>::value>::Result Result;
};

template <class T>
struct STLConstReferenceReturn<T, true> { typedef T Result; };

template <class _Class, bool _Const> struct ConstModifier   { typedef _Class Result; };
template <class _Class> struct ConstModifier <_Class, true> { typedef const _Class Result; };

template <class _To, bool _Const>
inline typename ConstModifier<_To, _Const>::Result *ExtractObjectPointer(LuaCustomVariable *_lcv)
{
	return (typename ConstModifier<_To, _Const>::Result*)_lcv->value();
};

template <class _Base, class _From> // _From can be P, P* and P&
struct ExtractRefValueFromMember
{
	template <bool _isptr> struct I1
	{
		template <bool _isref> struct I2
		{
			typedef _From Result;
			inline static Result &Do(_Base *_this, _From _Base::*_obj)
			{
				return (_this->*_obj);
			};
		};

		template <> struct I2 <true>
		{
			typedef typename std::tr1::remove_reference<_From>::type Result;
			inline static Result &Do(_Base *_this, _From _Base::*_obj)
			{
				return (_this->*_obj);
			};
		};
		typedef typename I2<std::tr1::is_reference<_From>::value>::Result Result;
		inline static Result &Do(_Base *_this, _From _Base::*_obj)
		{
			return I2<std::tr1::is_reference<_From>::value>::Do(_this, _obj);
		};
	};

	template <> struct I1 <true>
	{
		typedef typename std::tr1::remove_pointer<_From>::type Result;
		inline static Result &Do(_Base *_this, _From _Base::*_obj)
		{
			return *(_this->*_obj);
		};
	};

	typedef typename I1<std::tr1::is_pointer<_From>::value>::Result Type;
	typedef typename std::tr1::remove_cv<Type>::type UnqualifiedType;
	inline static Type &Do(_Base *_this, _From _Base::*_obj)
	{
		return I1<std::tr1::is_pointer<_From>::value>::Do(_this, _obj);
	};
};

template <class _T>
inline int TemplateCreateCLV(CHost* _host, lua_State *L, _T _t)
{
	return CreateCustomLuaVariable<
		ResolvCallType<_T>::CallType,
		_T>
		::Do(L, _host, _t, ResolvCallType<_T>::IsConst);
};

struct ImplicitCreatedHolder
{
	inline ImplicitCreatedHolder() : obj(0) {};
	virtual ~ImplicitCreatedHolder() {};
	void *obj;
};

template <class _T>
struct ConcreteImplicitCreatedHolder : public ImplicitCreatedHolder
{
	ConcreteImplicitCreatedHolder(_T *_obj) { obj = _obj; };
	~ConcreteImplicitCreatedHolder() { delete (_T*)obj;  };
};

struct HostImplicitCreatedStackFrameGuard
{
	HostImplicitCreatedStackFrameGuard(CHost *_host);
	~HostImplicitCreatedStackFrameGuard();
	size_t now;
	CHost *host;
};

template <class _TL, class _T>
struct IsCopyConstructor
{
	template <bool b> struct ParamBranch
	{ 
		typedef typename _TL::Head curr;
		typedef typename Loki::Select<std::tr1::is_pointer<curr>::value,
			typename std::tr1::remove_pointer<curr>::type,
			typename Loki::Select<std::tr1::is_reference<curr>::value,
				typename std::tr1::remove_reference<curr>::type,
				curr>::Result
		>::Result tmp;
		typedef typename std::tr1::remove_cv<tmp>::type clean_type;
		enum {Result = std::tr1::is_same<_T, clean_type>::value};
	};
	template <> struct ParamBranch <false>   { enum {Result = false}; };
	enum {Result = ParamBranch< Loki::TL::Length<_TL>::value == 1 >::Result};
};

template <class _Class, class _Base>
struct InheritedCaster
{
	static void *UpCast(void *_from)
	{
		return (void *) &( (_Base&) * ((_Class*)_from) );
	};

	static void *DownCast(void *_from)
	{
		// uses RTTI check to perform, can return 0.
		_Base *from = (_Base*) _from;
		_Class *me  = dynamic_cast<_Class*> (from);
		return me;
	};
};

template <class _Class, class _Base>
struct SmartPtrUpCaster
{
	static void *UpCast(void *_from)
	{
		return (void*)(_Base*)&(*(*(_Class*)_from));
	};
};

template <class _Class, class _Base>
struct ConvertUpCaster
{
	static void *UpCast(void *_from)
	{
		return (void*)
			&(_Base&)
			(*(_Class*)_from);
	};
};


template <class P, int _calltype = ResolvCallType<P>::CallType>
struct ParameterVerboseInfo;

template <class Type> struct FundamentalTypeVerboseInfo {};
#define __MLUABIND_MAKEIMPL(_A)\
template<>struct FundamentalTypeVerboseInfo<_A>{static const char*Info(){return #_A;}};
__MLUABIND_MAKEIMPL(int)
__MLUABIND_MAKEIMPL(unsigned)
__MLUABIND_MAKEIMPL(short)
__MLUABIND_MAKEIMPL(unsigned short)
__MLUABIND_MAKEIMPL(long)
__MLUABIND_MAKEIMPL(unsigned long)
__MLUABIND_MAKEIMPL(long long)
__MLUABIND_MAKEIMPL(unsigned long long)
__MLUABIND_MAKEIMPL(float)
__MLUABIND_MAKEIMPL(double)
__MLUABIND_MAKEIMPL(long double)
__MLUABIND_MAKEIMPL(const int&)
__MLUABIND_MAKEIMPL(const unsigned&)
__MLUABIND_MAKEIMPL(const short&)
__MLUABIND_MAKEIMPL(const unsigned short&)
__MLUABIND_MAKEIMPL(const long&)
__MLUABIND_MAKEIMPL(const unsigned long&)
__MLUABIND_MAKEIMPL(const long long&)
__MLUABIND_MAKEIMPL(const unsigned long long&)
__MLUABIND_MAKEIMPL(const float&)
__MLUABIND_MAKEIMPL(const double&)
__MLUABIND_MAKEIMPL(const long double&)
__MLUABIND_MAKEIMPL(char*)
__MLUABIND_MAKEIMPL(const char*)
__MLUABIND_MAKEIMPL(signed char*)
__MLUABIND_MAKEIMPL(unsigned char*)
__MLUABIND_MAKEIMPL(const signed char*)
__MLUABIND_MAKEIMPL(const unsigned char*)
__MLUABIND_MAKEIMPL(wchar_t*)
__MLUABIND_MAKEIMPL(const wchar_t*)
__MLUABIND_MAKEIMPL(char)
__MLUABIND_MAKEIMPL(signed char)
__MLUABIND_MAKEIMPL(unsigned char)
__MLUABIND_MAKEIMPL(wchar_t)
__MLUABIND_MAKEIMPL(const char&)
__MLUABIND_MAKEIMPL(const signed char&)
__MLUABIND_MAKEIMPL(const unsigned char&)
__MLUABIND_MAKEIMPL(const wchar_t&)
__MLUABIND_MAKEIMPL(bool)
__MLUABIND_MAKEIMPL(const bool&)
#undef __MLUABIND_MAKEIMPL

template <class P>
struct ParameterVerboseInfo<P, ResolvCallTypeBase::Fundamental>
{
	inline static String Info(CHost *host)
	{
		typedef typename Loki::Select<
			std::tr1::is_enum<P>::value,
			int,
			P>::Result type; // enum workaround

		return FundamentalTypeVerboseInfo<type>::Info();
	};
};

template <class P>
struct ParameterVerboseInfo<P, ResolvCallTypeBase::ByVal>
{
	inline static String Info(CHost *host)
	{
		typedef typename std::tr1::remove_cv<P>::type clean_type;
		return GetVerboseClassName(host, typeid(CustomClass<clean_type>), std::tr1::is_const<P>::value);
	};
};

template <class P>
struct ParameterVerboseInfo<P, ResolvCallTypeBase::ByRef>
{
	inline static String Info(CHost *host)
	{
		return ParameterVerboseInfo<
			typename std::tr1::remove_reference<P>::type,
			ResolvCallTypeBase::ByVal>::
			Info(host) + "&";
	};
};

template <class P>
struct ParameterVerboseInfo<P, ResolvCallTypeBase::ByPtr>
{
	inline static String Info(CHost *host)
	{
		return ParameterVerboseInfo<
			typename std::tr1::remove_pointer<P>::type,
			ResolvCallTypeBase::ByVal>::
			Info(host) + "*";
	};
};

template <>
struct ParameterVerboseInfo<void*, ResolvCallTypeBase::ByPtr>
{
	inline static String Info(CHost *host)
	{
		return "void *";
	};
};

template <>
struct ParameterVerboseInfo<const void*, ResolvCallTypeBase::ByPtr>
{
	inline static String Info(CHost *host)
	{
		return "const void *";
	};
};

template <class _Dummy = void>
struct ParametersVerboseInfo0
{
	inline static String Info(CHost *host)
	{
		const static String s("");
		return s;
	};
};

template <class T1>
struct ParametersVerboseInfo1
{
	inline static String Info(CHost *host)
	{
		return ParameterVerboseInfo<T1>::Info(host);
	};
};

template <class T1, class T2>
struct ParametersVerboseInfo2
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) ;
	};
};

template <class T1, class T2, class T3>
struct ParametersVerboseInfo3
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4>
struct ParametersVerboseInfo4
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4, class T5>
struct ParametersVerboseInfo5
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) + ", " +
			ParameterVerboseInfo<T5>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct ParametersVerboseInfo6
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) + ", " +
			ParameterVerboseInfo<T5>::Info(host) + ", " +
			ParameterVerboseInfo<T6>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ParametersVerboseInfo7
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) + ", " +
			ParameterVerboseInfo<T5>::Info(host) + ", " +
			ParameterVerboseInfo<T6>::Info(host) + ", " +
			ParameterVerboseInfo<T7>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ParametersVerboseInfo8
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) + ", " +
			ParameterVerboseInfo<T5>::Info(host) + ", " +
			ParameterVerboseInfo<T6>::Info(host) + ", " +
			ParameterVerboseInfo<T7>::Info(host) + ", " +
			ParameterVerboseInfo<T8>::Info(host) ;
	};
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ParametersVerboseInfo9
{
	inline static String Info(CHost *host)
	{
		return
			ParameterVerboseInfo<T1>::Info(host) + ", " +
			ParameterVerboseInfo<T2>::Info(host) + ", " +
			ParameterVerboseInfo<T3>::Info(host) + ", " +
			ParameterVerboseInfo<T4>::Info(host) + ", " +
			ParameterVerboseInfo<T5>::Info(host) + ", " +
			ParameterVerboseInfo<T6>::Info(host) + ", " +
			ParameterVerboseInfo<T7>::Info(host) + ", " +
			ParameterVerboseInfo<T8>::Info(host) + ", " +
			ParameterVerboseInfo<T9>::Info(host) ;
	};
};

String GetParameterInfo(lua_State *L, int _pind);
String GetParametersInfo(lua_State *L, int _start_from = 1);
wchar_t *ConvertToUnicode(const char *_from);
char *ConvertFromUnicode(const wchar_t *_from);
String ConCatTwoParameterStrings(const String &_first, const String &_second);
String GetVerboseClassName(CHost *host, Loki::TypeInfo _ti, bool _isconst);
const char *GetClassNameFromDistinguishedName(const String &_from);

class CRefCountedConstString
{
public:
	CRefCountedConstString();
	CRefCountedConstString(const std::string& _what);
	CRefCountedConstString(const CRefCountedConstString& _what);
	CRefCountedConstString(const char*_what);
	~CRefCountedConstString();
	bool operator<(const CRefCountedConstString& _right) const;
	bool operator==(const CRefCountedConstString& _right) const;
	bool operator<(const std::string& _right) const;
	bool operator==(const std::string& _right) const;
	bool operator<(const char* _right) const;
	bool operator==(const char* _right) const;
	operator const std::string&() const;
	inline const std::string& string() const { return (const std::string&)(*this); };
	const char* c_str() const;
	const CRefCountedConstString& operator=(const CRefCountedConstString& _right);
	const CRefCountedConstString& operator=(const std::string& _right);
	const CRefCountedConstString& operator=(const char*_right);

	typedef std::map<std::string, size_t> PoolT;  // string itself and refcount to it
private:
	PoolT::iterator m_Index;
};

}; // namespace impl

}; // namespace mluabind
