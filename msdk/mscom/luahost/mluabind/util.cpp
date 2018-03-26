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

#include <stdio.h>
#pragma warning(disable:4996)
namespace mluabind
{

namespace impl
{

String GetParameterInfo(lua_State *L, int _pind)
{
	char buf[512];
	sprintf(buf, "* ");
	switch(lua_type(L, _pind))
	{
		case LUA_TNIL: strcat(buf, "lua type: nil"); break;
		case LUA_TSTRING: strcat(buf, "string"); break;
		case LUA_TNUMBER: strcat(buf, "number"); break;
		case LUA_TBOOLEAN: strcat(buf, "boolean"); break;
		case LUA_TTABLE: strcat(buf, "table"); break;
		case LUA_TLIGHTUSERDATA: strcat(buf, "raw pointer"); break;
		case LUA_TUSERDATA:
		{
			LuaCustomVariable *_lcv = (LuaCustomVariable *)lua_touserdata(L, _pind);
			strcat(buf, "custom class: ");
			if(_lcv->IsConst())
				strcat(buf, "const ");
			strcat(buf, _lcv->getclass()->m_FullLuaName.c_str());
			break;
		}
		default:
		{
			strcat(buf, "unknown");
			assert(0); // wtf check
		}
	}
	return buf;
};

String GetParametersInfo(lua_State *L, int _start_from)
{
	String res;
	assert(_start_from != 0);

	if(_start_from > 0)
		for(; _start_from <= lua_gettop(L); ++_start_from)
			res += GetParameterInfo(L, _start_from) + "\n";
	else
		return GetParametersInfo(L, lua_gettop(L) + _start_from + 1);

	if(!res.empty())
		return res;
	else
		return "None\n";
};


HostImplicitCreatedStackFrameGuard::HostImplicitCreatedStackFrameGuard(CHost *_host):
	host(_host),
	now(_host->GetImplicitCreatedCount())
{
	assert(host != 0);
};

HostImplicitCreatedStackFrameGuard::~HostImplicitCreatedStackFrameGuard()
{
	host->RollBackImplicitCreated(now);	
};

String ConCatTwoParameterStrings(const String &_first, const String &_second)
{
	const static String s = String(", ");

	if(_second.empty())
		return _first;
	else
		return _first + s + _second;
};

String GetVerboseClassName(CHost *host, Loki::TypeInfo _ti, bool _isconst)
{
	assert(host);
	GenericClass *c = host->FindCPPGenericClass(_ti.name());
	const String &name = c ? c->m_FullLuaName : String("unknown type");
	return _isconst ? String("const ") + name : name;
}

const char *GetClassNameFromDistinguishedName(const std::string &_from)
{
	size_t s = _from.find_last_of('.');
	if(s == std::string::npos)
	{
		return _from.c_str();
	}
	else
	{
		// check that dn does not ending with '.'
		if(_from.size() > s + 1) // ???
			return _from.c_str() + s + 1;
		else 
			return "";
	}
};

///////////////////////////////////////////////////////////////////////////////
// class CSettingsRefCountedConstString
// write anytime(maybe), read often.
///////////////////////////////////////////////////////////////////////////////
namespace // settings reference counted const string
{
//static CRefCountedConstString::PoolT g_StringsPool;
static const size_t g_ThashThreshold = 10; // percentage of thrash when garbage should be collected
static size_t g_CharsAllocatedTotal = 0;
static size_t g_CharsUsedNow = 0;

static CRefCountedConstString::PoolT& GetStringsPool()
{
	static bool created = false;

	static unsigned char buf[ sizeof(CRefCountedConstString::PoolT) ];

	if(!created)
	{
		new (&buf[0]) CRefCountedConstString::PoolT;
		created = true;
	}

	return *((CRefCountedConstString::PoolT*)&buf[0]);
};

static CRefCountedConstString::PoolT::iterator FindOrAllocateString(const std::string& _what)
{
	CRefCountedConstString::PoolT *p = &GetStringsPool();
	CRefCountedConstString::PoolT::iterator i = p->find(_what);
	if(i == p->end())
	{
		g_CharsAllocatedTotal += _what.size() + 1; // fake size - "" should be not-null size
		return p->insert(std::make_pair(_what, 0)).first;
	}
	else
	{			
		return i;
	}
};

static CRefCountedConstString::PoolT::iterator FindOrAllocateString(const char* _what)
{
	static std::string tmp;
	tmp.reserve(1024);
	tmp = _what;
	return FindOrAllocateString(tmp);
};

static void AddRef(CRefCountedConstString::PoolT::iterator _where)
{
	if(_where->second == 0) // first link to this string - update used chars amount
		g_CharsUsedNow += _where->first.size() + 1; // fake size
	++_where->second;
};

static void DecRef(CRefCountedConstString::PoolT::iterator _where)
{
	assert(g_CharsAllocatedTotal != 0);
	assert(g_CharsUsedNow != 0);
	assert(_where->second > 0);

	--_where->second;

	if(!_where->second) // kill last link to this string - update used chars amount
		g_CharsUsedNow -= _where->first.size() + 1;

	if( g_CharsAllocatedTotal > 4096 ) // do not collect garbage until we have are reasonable strings size
	if( 100*(g_CharsAllocatedTotal - g_CharsUsedNow) / g_CharsAllocatedTotal > g_ThashThreshold)
	{
		// time to collect garbage for all pools
		CRefCountedConstString::PoolT *p = &GetStringsPool();

		CRefCountedConstString::PoolT::iterator i = p->begin(), e = p->end();
		for(;i!=e;)
		{
			if(!i->second)
			{
				g_CharsAllocatedTotal -= i->first.size() + 1;
				p->erase(i++);
				continue;
			}
			++i;
		}
	}
};

} // namespace

CRefCountedConstString::CRefCountedConstString()
{
	static const std::string dummy("");
	m_Index = FindOrAllocateString(dummy);
	AddRef(m_Index);
};

CRefCountedConstString::CRefCountedConstString(const std::string& _what)
{
	m_Index = FindOrAllocateString(_what);
	AddRef(m_Index);
};

CRefCountedConstString::CRefCountedConstString(const CRefCountedConstString& _what):
	m_Index(_what.m_Index)
{
	AddRef(m_Index);
};

CRefCountedConstString::CRefCountedConstString(const char*_what)
{
	m_Index = FindOrAllocateString(_what);
	AddRef(m_Index);
};

CRefCountedConstString::~CRefCountedConstString()
{
	DecRef(m_Index);
};

bool CRefCountedConstString::operator<(const CRefCountedConstString& _right) const
	{	return m_Index->first < _right.m_Index->first; };
bool CRefCountedConstString::operator==(const CRefCountedConstString& _right) const
	{	return m_Index->first == _right.m_Index->first;	};
bool CRefCountedConstString::operator<(const std::string& _right) const
	{ return m_Index->first < _right; };
bool CRefCountedConstString::operator==(const std::string& _right) const
	{ return m_Index->first == _right; };
bool CRefCountedConstString::operator<(const char* _right) const
	{ return m_Index->first < _right; };
bool CRefCountedConstString::operator==(const char* _right) const
	{ return m_Index->first == _right; };
CRefCountedConstString::operator const std::string&() const
	{ return m_Index->first; };

const CRefCountedConstString& CRefCountedConstString::operator=(const CRefCountedConstString& _right)
{
	DecRef(m_Index);
	m_Index = _right.m_Index;
	AddRef(m_Index);
	return *this;
};

const CRefCountedConstString& CRefCountedConstString::operator=(const std::string& _right)
{
	DecRef(m_Index);
	m_Index = FindOrAllocateString(_right);
	AddRef(m_Index);
	return *this;
};

const CRefCountedConstString& CRefCountedConstString::operator=(const char*_right)
{
	DecRef(m_Index);
	m_Index = FindOrAllocateString(_right);
	AddRef(m_Index);
	return *this;
};

const char* CRefCountedConstString::c_str() const
{
	return m_Index->first.c_str();
};

}; // namespace impl

}; // namespace mluabind