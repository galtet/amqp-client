#include "queue.h"

/**
* :lua_amqp_queue_bind
*
* binding the queue to a specific exchange with binding key
*
* @params[1] queue
* @params[2] exchange name
* @params[3] binding key
* @params[4] - optional - headers
*/
LUALIB_API int lua_amqp_queue_bind(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  amqp_channel_t channel = queue -> channel -> id;
  const char* exchange = luaL_checkstring(L, 2);
  const char* bindingkey = luaL_checkstring(L, 3);

  amqp_table_t table = amqp_empty_table;
  amqp_table_t *table_ref = NULL;

  if (lua_gettop(L) == 4) {
    create_amqp_table(L, -1, &table);
    table_ref = &table;
  }

  amqp_queue_bind(connection, channel, amqp_cstring_bytes(queue -> name),
                  amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                  table);
  if (table_ref) {
    free(table_ref->entries);
  }

  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "binding");

  return 0;
}

/**
* :lua_amqp_queue_unbind
*
* unbinding the queue from an exchange with binding key
*
* @params[1] queue
* @params[2] exchange name
* @params[3] binding key
* @params[4] - optional - headers
*/
LUALIB_API int lua_amqp_queue_unbind(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  amqp_channel_t channel = queue -> channel -> id;
  const char* exchange = luaL_checkstring(L, 2);
  const char* bindingkey = luaL_checkstring(L, 3);

  amqp_table_t table = amqp_empty_table;
  amqp_table_t *table_ref = NULL;

  if (lua_gettop(L) == 4) {
    create_amqp_table(L, -1, &table);
    table_ref = &table;
  }

  amqp_queue_unbind(connection, channel, amqp_cstring_bytes(queue -> name),
                  amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                  table);

  if (table_ref) {
    free(table_ref->entries);
  }

  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Unbinding");

  return 0;
}

/**
* :lua_amqp_queue_consume_message
*
* consuming 1 message from the queue - blocking call
*
* @params[1] queue
* @params[2] - optional - no_local
* @params[3] - optional - no_ack
* @params[4] - optional - no_exclusive
* @params[5] - optional - timeout
* @returns the string message, delivery tag, properties
*/
LUALIB_API int lua_amqp_queue_consume_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  int no_local = luaL_optboolean(L, 2, 0);
  int no_ack = luaL_optboolean(L, 3, 0);
  int no_exclusive = luaL_optboolean(L, 4, 0);
  int timeout = luaL_optnumber(L, 5, 0); //milliseconds

  struct timeval tval;
  struct timeval* tv;

  tv = get_timeout(&tval, timeout);

  // subscribing for the queue - blocking call
  amqp_basic_consume(connection, 1, amqp_cstring_bytes(queue -> name), amqp_empty_bytes, no_local, no_ack, no_exclusive, amqp_empty_table);
  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Consuming");
  amqp_envelope_t *envelope = malloc(sizeof(amqp_envelope_t));

  amqp_maybe_release_buffers(connection);
  die_on_amqp_error(L, amqp_consume_message(connection, envelope, tv, 0), "Fetching message"); // blocking call - waiting for a message

  // returning the msg + delivery tag
  char msg[envelope->message.body.len + 1];
  extract_envelope_message(envelope->message.body.bytes, envelope->message.body.len, msg);
  lua_pushstring(L, msg);
  lua_pushnumber(L, envelope->delivery_tag);

  // adding the msg properties
  properties_t *props = (properties_t *) lua_newuserdata(L, sizeof(properties_t));
  props -> envelope = envelope;//&envelope->message.properties;
  setmeta(L, "properties");


  return 3;
}

/**
* :lua_amqp_queue_publish_message
*
* publishing a message to the queue
*
* @params[1] queue
* @params[2] the message to be published
* @params[3] - optional - the exchange name
* @params[4] - optional - arguments
*/
LUALIB_API int lua_amqp_queue_publish_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  const char *msg = luaL_checkstring(L,2);
  const char* exchange = luaL_optstring(L,3, "");

  amqp_basic_properties_t props;
  amqp_basic_properties_t* props_ref = NULL;

  // regular args
  if (lua_gettop(L) > 3) {
    create_amqp_properties(L, -2, &props);
    props_ref = &props;
  }
  // headers
  if (lua_gettop(L) > 4) {
    create_amqp_headers(L, -1, &props);
  }

  die_on_error(
    L,
    amqp_basic_publish(connection, queue -> channel -> id, amqp_cstring_bytes(exchange), amqp_cstring_bytes(queue -> name), 0, 0, props_ref, amqp_cstring_bytes(msg)),
    "Publishing"
  );

  if (props_ref && (props_ref->headers).entries) {
    free((props_ref->headers).entries);
  }

  return 1;
}

static luaL_Reg queue_module[] = {
  { NULL, NULL }
};

LUALIB_API int lua_amqp_queue_open(lua_State *L) {
  createmeta(L, "queue", queue_reg);
  luaL_setfuncs(L, queue_module, 0);
  return 0;
}
