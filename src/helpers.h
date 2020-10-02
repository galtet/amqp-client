#ifndef LUAAMQP_HELPERS_H
#define LUAAMQP_HELPERS_H

#include <stdlib.h>
#include "lauxlib.h"

LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);
LUALIB_API void setmeta(lua_State *L, const char *name);
LUALIB_API int createmeta(lua_State *L, const char *name, const luaL_Reg *methods);

#endif
