#ifndef LUAAMQP_EXCHANGE_H
#define LUAAMQP_EXCHANGE_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include<string.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_exchange_open(lua_State *L);

LUALIB_API int lua_amqp_exchange_publish_message(lua_State *L);
LUALIB_API int lua_amqp_exchange_delete(lua_State *L);


static const struct luaL_reg exchange_reg[] = {
  { "publish_message",  lua_amqp_exchange_publish_message },
  { "delete",  lua_amqp_exchange_delete },
  { NULL, NULL }
};

#endif

