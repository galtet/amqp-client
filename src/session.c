#include "session.h"

LUALIB_API int lua_amqp_session_open_channel(lua_State *L) {
  connection_t *conn = (connection_t *)luaL_checkudata(L, 1, "session");
  amqp_connection_state_t connection = conn -> connection;

  channel_t *chan = (channel_t *) lua_newuserdata(L, sizeof(channel_t));
  chan -> id = 1;
  chan -> connection = connection;
  setmeta(L, "channel");

  amqp_channel_open(connection, 1);
  die_on_amqp_error(amqp_get_rpc_reply(connection), "Opening channel");

  return 1;
}

LUALIB_API int lua_amqp_session_new(lua_State *L) {
  int status;
  int port;
  const char *host;
  const char *username;
  const char *password;
  const char *vhost;


  luaL_checktype(L, 1, LUA_TTABLE);
  lua_getfield(L, 1, "host");
  lua_getfield(L, 1, "port");
  lua_getfield(L, 1, "username");
  lua_getfield(L, 1, "password");
  lua_getfield(L, 1, "vhost");


  host = lua_tostring(L, -5); 
  port = lua_tonumber(L, -4);
  username = lua_tostring(L, -3); 
  password = lua_tostring(L, -2); 
  vhost = lua_tostring(L, -1); 

  amqp_socket_t *socket = NULL;
  amqp_connection_state_t conn;

  conn = amqp_new_connection();
  socket = amqp_tcp_socket_new(conn);
  if (!socket) {
    die("creating TCP socket");
  }

  status = amqp_socket_open(socket, host, port);
  if (status) {
    die("opening TCP socket");
  }
  die_on_amqp_error(amqp_login(conn, vhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username, password), "Logging in");

  connection_t *c = (connection_t *) lua_newuserdata(L, sizeof(connection_t));
  setmeta(L, "session");
  c->connection = conn;

  return 1;
}

static luaL_Reg session_module[] = {
  { "new", lua_amqp_session_new },
  { NULL, NULL }
};

LUALIB_API int lua_amqp_session_open(lua_State *L) {
  createmeta(L, "session", session_reg);
  luaL_setfuncs(L, session_module, 0);
  return 0;
}