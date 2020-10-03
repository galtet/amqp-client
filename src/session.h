#ifndef LUAAMQP_SESSION_H
#define LUAAMQP_SESSION_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_ssl_socket.h>

#include "utils.h"
#include "structs.h"
#include "helpers.h"

#define DEFAULT_VHOST "/"
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 5672
#define DEFAULT_SSL_PORT 5671
#define DEFAULT_SSL_TIMEOUT 0
#define DEFAULT_USERNAME "guest"
#define DEFAULT_PASSWORD "guest"

LUALIB_API int lua_amqp_session_open(lua_State *L);

LUALIB_API int lua_amqp_session_open_channel(lua_State *L);
LUALIB_API int lua_amqp_session_free(lua_State *L);

static const struct luaL_reg session_reg[] = {
  { "open_channel", lua_amqp_session_open_channel },
  { "__gc", lua_amqp_session_free },
  { NULL, NULL }
};

#endif

