#ifndef LUAAMQP_PROPERTIES_H
#define LUAAMQP_PROPERTIES_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include<string.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_properties_open(lua_State *L);
LUALIB_API int lua_amqp_properties_headers(lua_State *L);

LUALIB_API int lua_amqp_properties_free(lua_State *L);

static const struct luaL_reg properties_reg[] = {
  { "headers",  lua_amqp_properties_headers },
  { "__gc", lua_amqp_properties_free },
  { NULL, NULL }
};

#endif
