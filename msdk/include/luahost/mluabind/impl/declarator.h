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
#include <vector>
#include "util.h"

namespace mluabind
{

namespace impl
{

struct GenericClass;
struct GenericFunction;
struct GenericConstant;
struct DeclaratorClass;

struct DeclaratedClass
{
	DeclaratedClass();
	DeclaratedClass(const char *_ln, const char *_fln);
	virtual ~DeclaratedClass();
	String m_LuaName;
	String m_FullLuaName;
};

struct DeclaratorClass
{
	DeclaratorClass(const char* _name);

	DeclaratorClass& operator+(GenericClass &_c);
	DeclaratorClass& operator+(GenericFunction &_f);
	DeclaratorClass& operator+(GenericConstant &_c);
	DeclaratorClass& operator+(DeclaratorClass &_d);

	const String& GetName() const;
	typedef std::vector<GenericClass*> ClassesT;
	typedef std::vector<GenericFunction*> FunctionsT;
	typedef std::vector<GenericConstant*> ConstantsT;
	ClassesT m_Classes;
	FunctionsT m_Functions;
	ConstantsT m_Constants;
private:
	const String m_Name;
};

}; // namespace impl

impl::DeclaratorClass &Declare(const char* _name = "");

}; // namespace mluabind
