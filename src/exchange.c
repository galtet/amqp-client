#include "exchange.h"

 /**
* :lua_amqp_exchange_delete
*
* deletes an exchange
*
* @params[1] exchange
* @params[2] exchange name
* @params[3] if_used
*/
LUALIB_API int lua_amqp_exchange_delete(lua_State *L) {
  exchange_t *exchange = (exchange_t *)luaL_checkudata(L, 1, "exchange");
  int if_used = lua_toboolean(L, 2);
  amqp_connection_state_t connection = exchange -> channel -> connection -> amqp_connection;

  amqp_exchange_delete(connection, exchange -> channel -> id, amqp_cstring_bytes(exchange -> name), if_used);
  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Deleting exchange");

  return 0;
}

/**
* :lua_amqp_exchange_publish_message
*
* publishing a message to an exchange with a bindingkey
*
* @params[1] exchange
* @params[2] bindingkey
* @params[3] the message to be published
*/
LUALIB_API int lua_amqp_exchange_publish_message(lua_State *L) {
  exchange_t *exchange = (exchange_t *)luaL_checkudata(L, 1, "exchange");
  amqp_connection_state_t connection = exchange -> channel -> connection -> amqp_connection;
  const char *bindingkey = luaL_checkstring(L, 2);
  const char *msg = luaL_checkstring(L,3);

  die_on_error(
    L,
    amqp_basic_publish(connection, exchange -> channel -> id, amqp_cstring_bytes(exchange -> name), amqp_cstring_bytes(bindingkey), 0, 0, NULL, amqp_cstring_bytes(msg)),
    "Publishing"
  );
  return 1;
}

static luaL_Reg exchange_module[] = {
  { NULL, NULL }
};

LUALIB_API int lua_amqp_exchange_open(lua_State *L) {
  createmeta(L, "exchange", exchange_reg);
  luaL_setfuncs(L, exchange_module, 0);
  return 0;
}
