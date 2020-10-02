#include "consumer.h"

const char* extract_envelope_message(void const *buffer, size_t len) {
  char *res  = (char*)malloc(len + 1);
  snprintf(res, len, "%s", (char*)buffer);

  return res;
}

LUALIB_API int lua_amqp_consumer_consume_messsage(lua_State *L) {
  connection_t *conn = (connection_t *)luaL_checkudata(L, 1, "session");
  const char *queuename = luaL_checkstring(L,2);
  amqp_connection_state_t connection = conn -> connection;

  amqp_basic_consume(connection, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
  die_on_amqp_error(amqp_get_rpc_reply(connection), "Consuming");

  amqp_rpc_reply_t res;
  amqp_envelope_t envelope;

  amqp_maybe_release_buffers(connection);
  res = amqp_consume_message(connection, &envelope, NULL, 0);
  lua_pushstring(L, extract_envelope_message(envelope.message.body.bytes, envelope.message.body.len));

  amqp_basic_ack(connection, 1, envelope.delivery_tag, 0);
  amqp_destroy_envelope(&envelope);

  return 1;
}

static luaL_Reg consumer_module[] = {
  { "consume", lua_amqp_consumer_consume_messsage},
  { NULL, NULL }
};

LUALIB_API int lua_amqp_consumer_open(lua_State *L) {
  createmeta(L, "consumer", consumer_reg);
  luaL_setfuncs(L, consumer_module, 0);
  return 0;
}