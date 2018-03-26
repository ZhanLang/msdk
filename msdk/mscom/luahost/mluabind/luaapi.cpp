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
#pragma warning(disable:4996)
namespace mluabind
{

extern const char *g_LuaNamespaceDescriptionTag;

namespace impl
{

int LuaMakeConst(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	if(!lua_gettop(L))
	{
		lua_pushnil(L);
		return 1;
	}
	if(!lua_isuserdata(L, 1) || lua_islightuserdata(L, 1) )
	{
		lua_pushvalue(L, 1);
		return 1;
	}

	bool turn_to = true;
	if(lua_gettop(L) >= 2)
		turn_to = lua_toboolean(L, 2) != 0;

	((LuaCustomVariable*)lua_touserdata(L, 1))->SetConst(turn_to);

	lua_pushvalue(L, 1);
	return 1;
};

int LuaGetType(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	if(!lua_gettop(L))
	{
		lua_pushstring(L, lua_typename(L, LUA_TNIL));
		return 1;
	}	
	if(!lua_isuserdata(L, -1))
	{
		lua_pushstring(L, lua_typename(L, lua_type(L, -1)));
		return 1;
	}
	if(lua_islightuserdata(L, -1))
	{
		lua_pushstring(L, "raw pointer");
		return 1;
	}
	const GenericClass *gclass = ((LuaCustomVariable*)lua_touserdata(L, -1))->getclass();
	assert(gclass != 0);
	lua_pushstring(L, gclass->m_FullLuaName.c_str());
	return 1;
};

int LuaGetTypeEx(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	if(!lua_gettop(L))
	{
		lua_pushstring(L, lua_typename(L, LUA_TNIL));
		return 1;
	}
	if(!lua_isuserdata(L, -1))
	{
		lua_pushstring(L, lua_typename(L, lua_type(L, -1)));
		return 1;
	}
	if(lua_islightuserdata(L, -1))
	{
		lua_pushstring(L, "raw pointer");
		return 1;
	}
	const GenericClass *gclass = ((LuaCustomVariable*)lua_touserdata(L, -1))->getclass();
	assert(gclass != 0);
	char buf[4096];
	strcpy(buf, gclass->m_FullLuaName.c_str());
	if(((LuaCustomVariable*)lua_touserdata(L, -1))->IsConst())
		strcat(buf, " const");
	if(((LuaCustomVariable*)lua_touserdata(L, -1))->IsOwn())
		strcat(buf, " owned");
	lua_pushstring(L, buf);

	return 1;
};

int LuaGetRTTIType(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);

	if(!lua_gettop(L))
	{
		lua_pushstring(L, lua_typename(L, LUA_TNIL));
		return 1;
	}

	if(!lua_isuserdata(L, -1) || lua_islightuserdata(L, -1))
	{
		//lua_pushstring(L, lua_typename(L, lua_type(L, -1)));
		return LuaGetType(L);
	}

	const GenericClass *gclass = ((LuaCustomVariable*)lua_touserdata(L, -1))->getclass();
	assert(gclass != 0);
	lua_pushstring(L, gclass->GetTypeID().name());

	return 1;
};

int LuaCast(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	if(lua_gettop(L) < 2 || !lua_isuserdata(L, 1) || lua_islightuserdata(L, 1))
	{
		host->Error("LuaCast: missings parameters");
		return 0;
	}

	const char *destname = 0;
	if(lua_isstring(L, 2))
		destname = lua_tostring(L, 2);
	if(lua_istable(L,2))
	{
		lua_pushstring(L, g_LuaNamespaceDescriptionTag);
		lua_gettable(L, -2);
		if(lua_isstring(L, -1))
			destname = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	if(!destname)
	{
		host->Error("LuaCast: can't retreive target type name");
		return 0;
	}

	LuaCustomVariable *lcv = (LuaCustomVariable *) lua_touserdata(L, 1);
	assert(lcv != 0);

	GenericClass *targetclass = host->FindLuaGenericClass(destname);
	if(!targetclass)
	{
		host->Error("LuaCast: can't target generic class for %s", destname);
		return 0;
	}

	void *result = lcv->getclass()->SafeCastToType(host, targetclass->GetTypeID(), lcv->value());
	if(!result)
	{
		lua_pushnil(L);
		return 1;
	}

	LuaCustomVariable *newlcv = targetclass->ConstructLuaUserdataObject(L);
	newlcv->SetValue(result);
	newlcv->SetConst(lcv->IsConst());
	newlcv->SetOwning(false);

	return 1;
};

int LuaCastUnsafe(lua_State *L)
{
	CHost *host = CHost::GetFromLua(L);
	if(lua_gettop(L) < 2 || !lua_isuserdata(L, 1))
	{
		host->Error("LuaCastUnsafe: missings parameters");
		return 0;
	}

	const char *destname = 0;
	if(lua_isstring(L, 2))
		destname = lua_tostring(L, 2);
	if(lua_istable(L,2))
	{
		lua_pushstring(L, g_LuaNamespaceDescriptionTag);
		lua_gettable(L, -2);
		if(lua_isstring(L, -1))
			destname = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	if(!destname)
	{
		host->Error("LuaCastUnsafe: can't retreive target type name");
		return 0;
	}

	GenericClass* gc = host->FindLuaGenericClass(destname);
	if(!gc)
	{
		lua_pushnil(L); // can't find class convert to. returns nil as error.
		return 1;
	}

	if(!lua_islightuserdata(L, 1))
	{
		void *ud = lua_touserdata(L, 1);
		LuaCustomVariable *lcv = (impl::LuaCustomVariable *) ud;

		LuaCustomVariable *newlcv = gc->ConstructLuaUserdataObject(L);
		newlcv->SetValue(lcv->value());
		newlcv->SetConst(lcv->IsConst());
		newlcv->SetOwning(false);
	}
	else
	{
		// absolutely dirty code - convert any pointer to any class
		void *ud = lua_touserdata(L, 1);

		LuaCustomVariable *newlcv = gc->ConstructLuaUserdataObject(L);
		newlcv->SetValue(ud);
		newlcv->SetConst(false);
		newlcv->SetOwning(false);
	}
	return 1;
};

}; // namespace impl

}; // namespace mluabind
