#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include "utils.h"

void throw_error(lua_State *L, const char* error_msg) {
  lua_pushstring(L, error_msg);
  lua_error(L);
}

void die(lua_State *L, const char *fmt, ...) {
  char err_msg[MAX_ERROR_MSG_LENGTH];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(err_msg, MAX_ERROR_MSG_LENGTH, fmt, ap);
  va_end(ap);

  throw_error(L, err_msg);
}

void die_on_error(lua_State *L, int x, char const *context) {
  char err_msg[MAX_ERROR_MSG_LENGTH];
  if (x < 0) {
    snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: %s\n", context, amqp_error_string2(x));
    throw_error(L, err_msg);
  }
}

void die_on_amqp_error(lua_State *L, amqp_rpc_reply_t x, char const *context) {
  char err_msg[MAX_ERROR_MSG_LENGTH];

  switch (x.reply_type) {
    case AMQP_RESPONSE_NORMAL:
      return;

    case AMQP_RESPONSE_NONE:
      snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: missing RPC reply type!\n", context);
      break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
      snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: %s\n", context, amqp_error_string2(x.library_error));
      break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
      switch (x.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
          amqp_connection_close_t *m =
              (amqp_connection_close_t *)x.reply.decoded;
          snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: server connection error %uh, message: %.*s\n",
                  context, m->reply_code, (int)m->reply_text.len,
                  (char *)m->reply_text.bytes);
          break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD: {
          amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
          snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: server channel error %uh, message: %.*s\n",
                  context, m->reply_code, (int)m->reply_text.len,
                  (char *)m->reply_text.bytes);
          break;
        }
        default:
          snprintf(err_msg, MAX_ERROR_MSG_LENGTH, "%s: unknown server error, method id 0x%08X\n",
                  context, x.reply.id);
          break;
      }
      break;
  }

  throw_error(L, err_msg);
}


