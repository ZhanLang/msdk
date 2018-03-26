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
#include "declarator.h"
#include "class.h"
#include "function.h"

namespace mluabind
{

namespace impl
{

DeclaratorClass::DeclaratorClass(const char* _name)
	:m_Name(_name)
{
};

DeclaratorClass& DeclaratorClass::operator+(GenericClass &_c)
{
	m_Classes.push_back(&_c);
	if(!GetName().empty())
		_c.m_FullLuaName = GetName() + String(".") + _c.m_FullLuaName;
	return *this;
};

DeclaratorClass& DeclaratorClass::operator+(GenericFunction &_f)
{	
	m_Functions.push_back(&_f);
	if(!GetName().empty())
		_f.m_FullLuaName = GetName() + String(".") + _f.m_FullLuaName.c_str();
	return *this;
};

DeclaratorClass& DeclaratorClass::operator+(GenericConstant &_c)
{
	m_Constants.push_back(&_c);
	if(!GetName().empty())
		_c.m_FullLuaName = GetName() + String(".") + _c.m_FullLuaName.c_str();
	return *this;
};

DeclaratorClass& DeclaratorClass::operator+(DeclaratorClass &_d)
{
	{
		ClassesT::iterator i = _d.m_Classes.begin(),
			e = _d.m_Classes.end();
		for(;i!=e;++i)
			*this + **i;
	}

	{
		FunctionsT::iterator i = _d.m_Functions.begin(),
			e = _d.m_Functions.end();
		for(;i!=e;++i)
			*this + **i;
	}

	{
		ConstantsT::iterator i = _d.m_Constants.begin(),
			e = _d.m_Constants.end();
		for(;i!=e;++i)
			*this + **i;
	}
	delete &_d;

	return *this;
};

const String& DeclaratorClass::GetName() const
{
	return m_Name;
};

DeclaratedClass::DeclaratedClass()
{
};

DeclaratedClass::DeclaratedClass(const char* _ln, const char* _fln):
	m_LuaName(_ln),
	m_FullLuaName(_fln)
{
}

DeclaratedClass::~DeclaratedClass()
{
};

}; // namespace impl

impl::DeclaratorClass &Declare(const char* _name)
{
	return *(new impl::DeclaratorClass(_name));
};

}; // namespace mluabind
