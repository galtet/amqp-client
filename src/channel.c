#include "channel.h"

LUALIB_API int lua_amqp_channel_queue(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* queuename = luaL_checkstring(L, 2);

  queue_t *queue = (queue_t *) lua_newuserdata(L, sizeof(queue_t));
  setmeta(L, "queue");
  queue -> name = queuename;
  queue -> channel = chan;

  return 1;
}

LUALIB_API int lua_amqp_channel_basic_ack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);

  amqp_connection_state_t connection = chan -> connection -> amqp_connection;
  amqp_basic_ack(connection, chan -> id, delivery_tag, multiple);

  return 1;
}

LUALIB_API int lua_amqp_channel_basic_nack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);
  amqp_boolean_t requeue = luaL_checknumber(L,4);

  amqp_connection_state_t connection = chan -> connection -> amqp_connection;
  amqp_basic_nack(connection, chan -> id, delivery_tag, multiple, requeue);

  return 1;
}

LUALIB_API int lua_amqp_channel_free(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  die_on_amqp_error(amqp_channel_close(chan -> connection -> amqp_connection, 1, AMQP_REPLY_SUCCESS), "Closing channel");

  return 0;
}

static luaL_Reg channel_module[] = {
  { NULL, NULL }
};

LUALIB_API int lua_amqp_channel_open(lua_State *L) {
  createmeta(L, "channel", channel_reg);
  luaL_setfuncs(L, channel_module, 0);
  return 0;
}