#ifndef LUAAMQP_QUEUE_H
#define LUAAMQP_QUEUE_H

#include <lua.h>                              
#include <lauxlib.h>                           
#include <lualib.h>                            

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include<string.h> 

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_queue_open(lua_State *L);

LUALIB_API int lua_amqp_queue_consume_message(lua_State *L);
LUALIB_API int lua_amqp_queue_publish_message(lua_State *L);

static const struct luaL_reg queue_reg[] = {
  { "consume_message", lua_amqp_queue_consume_message },
  { "publish_message", lua_amqp_queue_publish_message },
  { NULL, NULL }
};

#endif

