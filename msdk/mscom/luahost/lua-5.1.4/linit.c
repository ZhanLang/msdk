/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"
#include "luazip/luazip.h"
#include "luautf8/unidata.h"
#include "LuaUnicode/Lua_Unicode.h"
#include "luasys/src/luasys.h"

static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {LFS_LIBNAME, luaopen_lfs},
  {LUA_CJSONLIBNAME, luaopen_cjson},
  {B64_LIBNAME, luaopen_b64},
 // {SOCKET_LIBNAME,luaopen_socket_core},
  {BIT_LIBNAME, luaopen_bit},
  { XML_LIBNAME,luaopen_xml},
  {LUA_ZIPLIBNAME,luaopen_zip },
  { UTF8_LIBNAME,luaopen_utf8},
  {LUA_LC_LIBNAME, luaopen_LC},

  //luasys
  {LUA_SYSLIBNAME, luaopen_sys},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}

int luaopen_rapidxml(void* L) ;
LUALIB_API int luaopen_xml(lua_State *L)
{
	return luaopen_rapidxml(L);
}
