#include "channel.h"

 /**
* :lua_amqp_channel_declare_exchange
*
* declare a new exchange
*
* @params[1] channel
* @params[2] exchange name
* @params[3] exchange type
* @params[3] passive
* @params[4] durable
* @params[5] auto_delete
* @params[6] internal
* @returns exchange object
*/
LUALIB_API int lua_amqp_channel_declare_exchange(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* exchange_name = luaL_checkstring(L, 2);
  const char* exchange_type = luaL_checkstring(L, 3);
  int passive = lua_toboolean(L, 4);
  int durable = lua_toboolean(L, 5);
  int auto_delete = lua_toboolean(L, 6);
  int internal =  lua_toboolean(L, 7);

  amqp_exchange_declare(chan -> connection -> amqp_connection, chan -> id, amqp_cstring_bytes(exchange_name),
                        amqp_cstring_bytes(exchange_type), passive, durable, auto_delete, internal,
                        amqp_empty_table);
  die_on_amqp_error(amqp_get_rpc_reply(chan -> connection -> amqp_connection), "Declaring exchange");

  exchange_t *exchange = (exchange_t *) lua_newuserdata(L, sizeof(exchange_t));
  setmeta(L, "exchange");
  exchange -> name = exchange_name;
  exchange -> channel = chan;

  return 1;
}

/**
* :lua_amqp_channel_exchange
*
* get an exchange object
*
* @params[1] channel
* @params[2] exchange name
* @returns exchange object
*/
LUALIB_API int lua_amqp_channel_exchange(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* name = luaL_checkstring(L, 2);

  exchange_t *exchange = (exchange_t *) lua_newuserdata(L, sizeof(exchange_t));
  setmeta(L, "exchange");
  exchange -> name = name;
  exchange -> channel = chan;

  return 1;
}

/**
* :lua_amqp_channel_queue
*
* get a queue object
*
* @params[1] channel
* @params[2] queue name
* @returns queue object
*/
LUALIB_API int lua_amqp_channel_queue(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* queuename = luaL_checkstring(L, 2);

  queue_t *queue = (queue_t *) lua_newuserdata(L, sizeof(queue_t));
  setmeta(L, "queue");
  queue -> name = queuename;
  queue -> channel = chan;

  return 1;
}

/**
* :lua_amqp_channel_basic_qos
*
* set a QOS for a channel
*
* @params[1] channel
* @params[2] prefetch_count - how many unacked msg's should we fetch
* @returns queue object
*/
LUALIB_API int lua_amqp_channel_basic_qos(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  int prefetch_count = luaL_checknumber(L, 2);
  amqp_connection_state_t connection = chan -> connection -> amqp_connection;

  amqp_basic_qos(connection, chan -> id, 0,  prefetch_count, 0);

  return 1;
}

/**
* :lua_amqp_channel_basic_ack
*
* sending an ack for a message
*
* @params[1] channel
* @params[2] delivery tag - the massage indentifier
* @params[3] multiple - ack all msg's until this tag
*/
LUALIB_API int lua_amqp_channel_basic_ack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);

  amqp_connection_state_t connection = chan -> connection -> amqp_connection;
  amqp_basic_ack(connection, chan -> id, delivery_tag, multiple);

  return 1;
}

/**
* :lua_amqp_channel_basic_nack
*
* sending an nack for a message
*
* @params[1] channel
* @params[2] delivery tag - the massage indentifier
* @params[3] multiple - nack all msg's until this tag
* @params[4] requeue - whether to requeue the msg or not
*/
LUALIB_API int lua_amqp_channel_basic_nack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_checknumber(L,3);
  amqp_boolean_t requeue = luaL_checknumber(L,4);

  amqp_connection_state_t connection = chan -> connection -> amqp_connection;
  amqp_basic_nack(connection, chan -> id, delivery_tag, multiple, requeue);

  return 1;
}

/**
* :lua_amqp_channel_free
*
* freeing the channel
*
* @params[1] channel
*/
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
