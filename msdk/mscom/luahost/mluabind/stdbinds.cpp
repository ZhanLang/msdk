// Copyright (c) 2007 Michael Kazakov
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
#include "stdbinds.h"
#include "stl_basic_string.h"
#include "stl_vector.h"

#include <cmath>
#include <string>

namespace mluabind
{

bool BindStdLibrary(CHost *_host)
{
	_host->Insert
		(Declare("std")
		+BindBasicString<std::string>("string")
		+BindBasicString<std::wstring>("wstring")
		// math functions
		+Function("abs", (double (*)(double)) &::std::abs)
		+Function("acos", (double (*)(double)) &::std::acos)
		+Function("asin", (double (*)(double)) &::std::asin)
		+Function("atan", (double (*)(double)) &::std::atan)
		+Function("atan2", (double (*)(double, double)) &::std::atan2)
		+Function("atof",  &::std::atof)
		+Function("ceil", (double (*)(double)) &::std::ceil)
		+Function("cos", (double (*)(double)) &::std::cos)
		+Function("sin", (double (*)(double)) &::std::sin)
		+Function("tan", (double (*)(double)) &::std::tan)
		+Function("cosh", (double (*)(double)) &::std::cosh)
		+Function("sinh", (double (*)(double)) &::std::sinh)
		+Function("tanh", (double (*)(double)) &::std::tanh)
		+Function("exp", (double (*)(double)) &::std::exp)
		+Function("floor", (double (*)(double)) &::std::floor)
		+Function("log", (double (*)(double)) &::std::log)
		+Function("log10", (double (*)(double)) &::std::log10)
		+Function("pow", (double (*)(double, double)) &::std::pow)
		+Function("rand", &::std::rand)
		+Function("srand", (void (*)(int))&::std::srand)
		+Function("sqrt", (double (*)(double)) &::std::sqrt)
		);

	return true;
};

} // namespace mluabind
