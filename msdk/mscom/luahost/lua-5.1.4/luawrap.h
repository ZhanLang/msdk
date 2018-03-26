#ifndef LUAWRAP_H
#define LUAWRAP_H

/* Utility macros for wrapping C libraries */

#define LIB(name) static const struct luaL_Reg name[]
#define SI static int

#define LERROR(s) \
        { lua_pushstring(L, s); lua_error(L); }

#define CHECK_UD(mt, n)  ((mt *)luaL_checkudata(L, n, #mt))

#define NEW_UD(mt)  (mt *)lua_newuserdata(L, sizeof(mt *))

/* Box a C struct as a full userdata. */
#define BOX_UD(cstruct, lua_ud)  \
        typedef struct lua_ud {  \
        	cstruct *v;      \
        } lua_ud                 \

/* Associate a metatable with its name. Use in luaopen_[libname]. */
#define set_MT(mt, t) \
        luaL_newmetatable(L, mt);       \
        lua_pushvalue(L, -1);           \
        lua_setfield(L, -2, "__index"); \
        luaL_register(L, NULL, t);      \
        lua_pop(L, 1)

#endif
