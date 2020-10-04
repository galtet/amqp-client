#include "queue.h"

char* extract_envelope_message(void const *buffer, size_t len) {
  char *res  = (char*)malloc(len + 1);
  memcpy(res, buffer, len);
  res[len] = '\0';

  return res;
}

/**
* :lua_amqp_queue_bind
*
* binding the queue to a specific exchange with binding key
*
* @params[1] queue
* @params[2] exchange name
* @params[3] binding key
*/
LUALIB_API int lua_amqp_queue_bind(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  amqp_channel_t channel = queue -> channel -> id;
  const char* exchange = luaL_checkstring(L, 2);
  const char* bindingkey = luaL_checkstring(L, 3);

  amqp_queue_bind(connection, channel, amqp_cstring_bytes(queue -> name),
                  amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                  amqp_empty_table);
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
*/
LUALIB_API int lua_amqp_queue_unbind(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  amqp_channel_t channel = queue -> channel -> id;
  const char* exchange = luaL_checkstring(L, 2);
  const char* bindingkey = luaL_checkstring(L, 3);

  amqp_queue_unbind(connection, channel, amqp_cstring_bytes(queue -> name),
                  amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                  amqp_empty_table);
  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Unbinding");

  return 0;
}

/**
* :lua_amqp_queue_consume_message
*
* consuming 1 message from the queue - blocking call
*
* @params[1] queue
* @returns the string message and the delivery tag
*/
LUALIB_API int lua_amqp_queue_consume_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  const char *queuename = queue -> name;
  char *msg;

  // subscribing for the queue - blocking call
  amqp_basic_consume(connection, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Consuming");

  amqp_envelope_t envelope;

  amqp_maybe_release_buffers(connection);
  die_on_amqp_error(L, amqp_consume_message(connection, &envelope, NULL, 0), "Fetching message"); // blocking call - waiting for a message

  // returning the msg + delivery tag
  msg = extract_envelope_message(envelope.message.body.bytes, envelope.message.body.len);
  lua_pushstring(L, msg);
  lua_pushnumber(L, envelope.delivery_tag);

  free(msg);
  amqp_destroy_envelope(&envelope);

  return 2;
}

/**
* :lua_amqp_queue_publish_message
*
* publishing a message to the queue
*
* @params[1] queue
* @params[2] the message to be published
* @params[3] the exchange name
*/
LUALIB_API int lua_amqp_queue_publish_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  const char *queuename = queue -> name;
  const char *msg = luaL_checkstring(L,2);
  const char* exchange = luaL_checkstring(L,3);

  die_on_error(
    L,
    amqp_basic_publish(connection, queue -> channel -> id, amqp_cstring_bytes(exchange), amqp_cstring_bytes(queuename), 0, 0, NULL, amqp_cstring_bytes(msg)),
    "Publishing"
  );
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
