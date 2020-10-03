#ifndef LUAAMQP_CHANNEL_H
#define LUAAMQP_CHANNEL_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include<string.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_channel_open(lua_State *L);

LUALIB_API int lua_amqp_channel_exchange(lua_State *L);
LUALIB_API int lua_amqp_channel_queue(lua_State *L);
LUALIB_API int lua_amqp_channel_basic_ack(lua_State *L);
LUALIB_API int lua_amqp_channel_basic_nack(lua_State *L);
LUALIB_API int lua_amqp_channel_basic_qos(lua_State *L);

LUALIB_API int lua_amqp_channel_free(lua_State *L);

static const struct luaL_reg channel_reg[] = {
  { "exchange", lua_amqp_channel_exchange },
  { "ack",  lua_amqp_channel_basic_ack },
  { "nack", lua_amqp_channel_basic_nack },
  { "queue", lua_amqp_channel_queue },
  { "qos", lua_amqp_channel_basic_qos },
  { "__gc", lua_amqp_channel_free} ,
  { NULL, NULL }
};

#endif

