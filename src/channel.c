#include "channel.h"

 /**
* :lua_amqp_channel_queue_declare
*
* declare a new queue
*
* @params[1] channel
* @params[2] queue name
* @params[3] - optional - passive
* @params[4] - optional - durable
* @params[5] - optional -exclusive
* @params[6] - optional -auto_delete
* @params[7] - optional - headers
* @returns queue object
*/
LUALIB_API int lua_amqp_channel_queue_declare(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* queue_name = luaL_checkstring(L, 2);
  int passive = luaL_optboolean(L, 3, 0);
  int durable = luaL_optboolean(L, 4, 1);
  int exclusive = luaL_optboolean(L, 5, 0);
  int auto_delete =  luaL_optboolean(L, 6, 0);

  amqp_table_t table = amqp_empty_table;
  amqp_table_t *table_ref = NULL;

  if (lua_gettop(L) == 7) {
    create_amqp_table(L, -1, &table);
    table_ref = &table;
  }

  amqp_queue_declare(chan -> connection -> amqp_connection, chan -> id, amqp_cstring_bytes(queue_name),
                     passive, durable, exclusive, auto_delete,
                     table);
  if (table_ref) {
    free(table_ref->entries);
  }
  die_on_amqp_error(L, amqp_get_rpc_reply(chan -> connection -> amqp_connection), "Declaring queue");

  queue_t *queue = (queue_t *) lua_newuserdata(L, sizeof(queue_t));
  setmeta(L, "queue");
  queue -> name = queue_name;
  queue -> channel = chan;

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
  lua_pushboolean(L, 1);
  lua_amqp_channel_queue_declare(L);
  return 1;
}

 /**
* :lua_amqp_channel_exchange_declare
*
* declare a new exchange
*
* @params[1] channel
* @params[2] exchange name
* @params[3] - optional - exchange type
* @params[4] - optional - passive
* @params[5] - optional - durable
* @params[6] - optional - auto_delete
* @params[7] - optional - internal
* @returns exchange object
*/
LUALIB_API int lua_amqp_channel_exchange_declare(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  const char* exchange_name = luaL_checkstring(L, 2);
  const char* exchange_type = luaL_optstring(L, 3, DEFAULT_EXCHANGE_TYPE);
  int passive = luaL_optboolean(L, 4, 0);
  int durable = luaL_optboolean(L, 5, 1);
  int auto_delete = luaL_optboolean(L, 6, 0);
  int internal = luaL_optboolean(L, 7, 0);

  amqp_exchange_declare(chan -> connection -> amqp_connection, chan -> id, amqp_cstring_bytes(exchange_name),
                        amqp_cstring_bytes(exchange_type), passive, durable, auto_delete, internal,
                        amqp_empty_table);
  die_on_amqp_error(L, amqp_get_rpc_reply(chan -> connection -> amqp_connection), "Declaring exchange");

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
  lua_amqp_channel_exchange_declare(L);
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
* @params[3] - optional - multiple - ack all msg's until this tag
*/
LUALIB_API int lua_amqp_channel_basic_ack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple =  luaL_optboolean(L, 3, 0);

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
* @params[3] - optional - multiple - nack all msg's until this tag
* @params[4] - optional - requeue - whether to requeue the msg or not
*/
LUALIB_API int lua_amqp_channel_basic_nack(lua_State *L) {
  channel_t *chan = (channel_t *)luaL_checkudata(L, 1, "channel");
  uint64_t delivery_tag = luaL_checknumber(L, 2);
  amqp_boolean_t multiple = luaL_optboolean(L, 3, 0);
  amqp_boolean_t requeue = luaL_optboolean(L, 4, 1);

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
  if (chan -> connection -> amqp_connection) { // if connection was already closed - no need to close the channel, it was already closed
    die_on_amqp_error(L, amqp_channel_close(chan -> connection -> amqp_connection, 1, AMQP_REPLY_SUCCESS), "Closing channel");
  }

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
