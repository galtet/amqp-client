#include <syslog.h>
#include "lua_amqp.h"

static const luaL_Reg modules[] = {
  { "session",  lua_amqp_session_open },
  { "channel", lua_amqp_channel_open },
  { "queue", lua_amqp_queue_open },
  { "exchange", lua_amqp_exchange_open },
  { "properties", lua_amqp_properties_open },
  {NULL, NULL}
};

LUALIB_API int luaopen_amqp(lua_State *L) {
  lua_newtable(L);
  int i;
  for (i = 0; modules[i].name; i++) {
    modules[i].func(L);
  }

  return 1;
}
