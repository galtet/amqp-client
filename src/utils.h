#ifndef LUAAMQP_UTILS_H
#define LUAAMQP_UTILS_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <amqp.h>
#include <amqp_framing.h>

#define MAX_ERROR_MSG_LENGTH 1000

void throw_error(lua_State *L, const char* error_msg);

void die(lua_State *L, const char *fmt, ...);
void die_on_error(lua_State *L, int x, char const *context);
void die_on_amqp_error(lua_State *L, amqp_rpc_reply_t x, char const *context);

#endif


