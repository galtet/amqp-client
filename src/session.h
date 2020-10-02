#ifndef LUAAMQP_SESSION_H
#define LUAAMQP_SESSION_H

#include <lua.h>                              
#include <lauxlib.h>                           
#include <lualib.h>                            

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

LUALIB_API int lua_amqp_session_open(lua_State *L);

LUALIB_API int lua_amqp_session_open_channel(lua_State *L);

static const struct luaL_reg session_reg[] = {
  { "open_channel", lua_amqp_session_open_channel },
  { NULL, NULL }
};

#endif

