#include "queue.h"

char* extract_envelope_message(void const *buffer, size_t len) {
  char *res  = (char*)malloc(len + 1);
  memcpy(res, buffer, len);
  res[len] = '\0';

  return res;
}

LUALIB_API int lua_amqp_queue_consume_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  const char *queuename = queue -> name;
  char *msg;

  // subscribing for the queue - blocking call
  amqp_basic_consume(connection, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
  die_on_amqp_error(amqp_get_rpc_reply(connection), "Consuming");

  amqp_envelope_t envelope;

  amqp_maybe_release_buffers(connection);
  die_on_amqp_error(amqp_consume_message(connection, &envelope, NULL, 0), "Fetching message"); // blocking call - waiting for a message

  // returning the msg + delivery tag
  msg = extract_envelope_message(envelope.message.body.bytes, envelope.message.body.len);
  lua_pushstring(L, msg);
  lua_pushnumber(L, envelope.delivery_tag);

  free(msg);
  amqp_destroy_envelope(&envelope);

  return 2;
}

LUALIB_API int lua_amqp_queue_publish_message(lua_State *L) {
  queue_t *queue = (queue_t *)luaL_checkudata(L, 1, "queue");
  amqp_connection_state_t connection = queue -> channel -> connection -> amqp_connection;
  const char *queuename = queue -> name;
  const char *msg = luaL_checkstring(L,2);
  const char* exchange = luaL_checkstring(L,3);

  die_on_error(
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