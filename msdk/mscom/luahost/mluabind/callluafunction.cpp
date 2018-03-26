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

#include <string>

#pragma warning(disable:4996)

namespace mluabind
{

namespace impl
{

static bool FindFuntionLocal(lua_State* L, const char *_name, int _tind)
{
	const char *s = strchr(_name, '.');
	if(!s)
	{
		lua_getfield(L, _tind, _name);
		
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return false;
		}
		return true;
	}
	else
	{
		char tmp[256];
		strncpy(&tmp[0], _name, s - _name);
		tmp[s - _name] = 0;

		lua_getfield(L, _tind, tmp);
		int n = lua_gettop(L);
		bool b = false;
		if(lua_istable(L, -1) || (lua_isuserdata(L, -1) && !lua_islightuserdata(L, -1)))
			b = FindFuntionLocal(L, s+1, n);
		lua_remove(L, n);
		return b;
	}
};

bool FindLuaFunction(lua_State* L, const std::string &_name)
{
	return FindFuntionLocal(L, _name.c_str(), LUA_GLOBALSINDEX);
};

}; // namespace impl

}; // namespace mluabind

