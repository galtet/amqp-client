#ifndef LUAAMQP_CONSUMER_H
#define LUAAMQP_CONSUMER_H

#include <lua.h>                              
#include <lauxlib.h>                           
#include <lualib.h>                            

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_consumer_open(lua_State *L);

LUALIB_API int lua_amqp_consumer_consume_messsage(lua_State *L);

static const struct luaL_reg consumer_reg[] = {
  { "consume", lua_amqp_consumer_consume_messsage},
  { NULL, NULL }
};

#endif

