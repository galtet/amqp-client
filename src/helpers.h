#ifndef LUAAMQP_HELPERS_H
#define LUAAMQP_HELPERS_H

#include "utils.h"

#include "lauxlib.h"

#include <amqp.h>
#include<string.h>
#include <stdlib.h>

#define MAX_ERR_LENGTH 1000

int luaL_optboolean(lua_State *L, int narg, int def);

LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);
LUALIB_API void setmeta(lua_State *L, const char *name);
LUALIB_API int createmeta(lua_State *L, const char *name, const luaL_Reg *methods);
LUALIB_API int get_num_of_keys(lua_State *L, int index);

void extract_envelope_message(void const *buffer, size_t len, char* res);
void create_amqp_table(lua_State *L, int index, amqp_table_t *table);
void create_amqp_headers(lua_State *L, int index, amqp_basic_properties_t *props);
void create_amqp_properties(lua_State *L, int index, amqp_basic_properties_t *props);

struct timeval* get_timeout(struct timeval *tv, int timeout);

#endif
