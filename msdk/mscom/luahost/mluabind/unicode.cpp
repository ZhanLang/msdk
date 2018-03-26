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
#include <windows.h>
#include <assert.h>
#include <string.h>

namespace mluabind
{

namespace impl
{

wchar_t *ConvertToUnicode(const char *_from)
{
	// adjust values below to reach threshold you need
	const int buffdepth = 16384;
	const int buffsize  = 8;

	static wchar_t buffer[buffsize][buffdepth];
	static int currentindex = 0;

#ifdef _DEBUG
	if(strlen(_from) >= buffdepth)
		assert(0);
#endif

	int out = currentindex;
	++currentindex;
	if(currentindex == buffsize)
		currentindex = 0;

	MultiByteToWideChar(
		CP_ACP,
		0,
		_from,
		-1,
		&buffer[out][0],
		buffdepth);

	return buffer[out];
};

char *ConvertFromUnicode(const wchar_t *_from)
{
	// adjust values below to reach threshold you need
	const int buffdepth = 16384;
	const int buffsize  = 8;

	static char buffer[buffsize][buffdepth];
	static int currentindex = 0;

#ifdef _DEBUG
	if(wcslen(_from) >= buffdepth)
		assert(0);
#endif

	int out = currentindex;
	++currentindex;
	if(currentindex == buffsize)
		currentindex = 0;

	WideCharToMultiByte(
		CP_ACP,
		0,
		_from,
		-1,
		&buffer[out][0],
		buffdepth,
		NULL,
		NULL);

	return buffer[out];
};

}; // namespace impl

}; // namespace mluabind
