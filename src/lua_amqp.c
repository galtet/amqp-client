#include <syslog.h>
#include "lua_amqp.h"

static const luaL_Reg modules[] = {
  { "session",  lua_amqp_session_open },
  { "consumer", lua_amqp_consumer_open },
  { "channel", lua_amqp_channel_open },
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
