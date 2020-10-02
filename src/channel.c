#include "channel.h"
#include <stdio.h>

const char* extract_envelope_message(void const *buffer, size_t len) {
  char *res  = (char*)malloc(len + 1);
  snprintf(res, len, "%s", (char*)buffer);

  return res;
}

LUALIB_API int lua_amqp_channel_basic_ack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);

  amqp_connection_state_t connection = chan -> connection;
  amqp_basic_ack(connection, chan -> id, delivery_tag, multiple);

  return 1;
}

LUALIB_API int lua_amqp_channel_basic_nack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);
  amqp_boolean_t requeue = luaL_checknumber(L,4);

  amqp_connection_state_t connection = chan -> connection;
  amqp_basic_nack(connection, chan -> id, delivery_tag, multiple, requeue);

  return 1;
}

LUALIB_API int lua_amqp_channel_consume_message(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  amqp_connection_state_t connection = chan -> connection;
  const char *queuename = luaL_checkstring(L,2);

  // subscribing for the queue - blocking call
  amqp_basic_consume(connection, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
  die_on_amqp_error(amqp_get_rpc_reply(connection), "Consuming");

  amqp_rpc_reply_t res;
  amqp_envelope_t envelope;

  amqp_maybe_release_buffers(connection);
  die_on_amqp_error(amqp_consume_message(connection, &envelope, NULL, 0), "Fetching message"); // blocking call - waiting for a message

  // returning the msg + delivery tag
  lua_pushstring(L, extract_envelope_message(envelope.message.body.bytes, envelope.message.body.len));
  lua_pushnumber(L, envelope.delivery_tag);

  amqp_destroy_envelope(&envelope);

  return 2;
}

LUALIB_API int lua_amqp_channel_publish_message(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  amqp_connection_state_t connection = chan -> connection;
  const char *queuename = luaL_checkstring(L,2);
  const char *msg = luaL_checkstring(L,3);
  const char* exchange = luaL_checkstring(L,4);

  die_on_error(amqp_basic_publish(connection, chan -> id, amqp_cstring_bytes(exchange), amqp_cstring_bytes(queuename), 0, 0, NULL, amqp_cstring_bytes(msg)), "Publishing");
  return 1;
}

static luaL_Reg channel_module[] = {
  { NULL, NULL }
};

LUALIB_API int lua_amqp_channel_open(lua_State *L) {
  createmeta(L, "channel", channel_reg);
  luaL_setfuncs(L, channel_module, 0);
  return 0;
}