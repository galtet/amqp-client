#include "session.h"

void fetch_connection_params(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);

  lua_getfield(L, 1, "timeout");
  lua_getfield(L, 1, "key");
  lua_getfield(L, 1, "cert");
  lua_getfield(L, 1, "cacert");
  lua_getfield(L, 1, "ssl");
  lua_getfield(L, 1, "vhost");
  lua_getfield(L, 1, "password");
  lua_getfield(L, 1, "username");
  lua_getfield(L, 1, "port");
  lua_getfield(L, 1, "host");
}

/**
 * :lua_amqp_session_open_channel
 *
 * opens a new channel on the session
 */
LUALIB_API int lua_amqp_session_open_channel(lua_State *L) {
  connection_t *conn = (connection_t *)luaL_checkudata(L, 1, "session");
  amqp_connection_state_t connection = conn -> amqp_connection;

  channel_t *chan = (channel_t *) lua_newuserdata(L, sizeof(channel_t));
  chan -> id = 1;
  chan -> connection = conn;
  setmeta(L, "channel");

  amqp_channel_open(connection, 1);
  die_on_amqp_error(L, amqp_get_rpc_reply(connection), "Opening channel");

  return 1;
}

/**
* :connect_ssl
*
* open a new ss lconnection
*
* @params connection - the opened amqp connection
* @params hostname
* @params port
* @params key
* @params cert
* @params cacert
* @params verify_peer
* @params verify_hostname
* @params timeout - time for the ssl connection init
*/
void connect_ssl(
  lua_State *L,
  amqp_connection_state_t connection,
  const char* hostname,
  int port,
  const char* key,
  const char* cert,
  const char* cacert ,
  int verify_peer,
  int verify_hostname,
  int timeout) {
  amqp_socket_t *socket;

  socket = amqp_ssl_socket_new(connection);
  if (!socket) {
    die(L, "creating SSL/TLS socket");
  }

  amqp_ssl_socket_set_verify_peer(socket, 0);
  amqp_ssl_socket_set_verify_hostname(socket, 0);

  die_on_error(L, amqp_ssl_socket_set_cacert(socket, cacert), "setting CA certificate");

  if (verify_peer) {
      amqp_ssl_socket_set_verify_peer(socket, 1);
    }
  if (verify_hostname) {
      amqp_ssl_socket_set_verify_hostname(socket, 1);
    }

  die_on_error(L, amqp_ssl_socket_set_key(socket, cert, key), "setting client key");

  struct timeval tval;
  struct timeval* tv;

  tv = get_timeout(&tval, timeout);
  die_on_error(L, amqp_socket_open_noblock(socket, hostname, port, tv), "opening SSL/TLS connection");
}

void connect_regular(lua_State *L, amqp_connection_state_t connection, const char* hostname, int port) {
  amqp_socket_t *socket = NULL;
  int status;

  socket = amqp_tcp_socket_new(connection);
  if (!socket) {
    die(L, "creating TCP socket");
  }

  status = amqp_socket_open(socket, hostname, port);
  if (status) {
    die(L, "opening TCP socket");
  }
}

/**
* :lua_amqp_session_new
*
* opens a new amqp session
*
* @params[1] table with the following fields: host, port, username, password, vhost
* SSL fields - cacert, cert, key, timeout, ssl
*/
LUALIB_API int lua_amqp_session_new(lua_State *L) {
  int port;
  const char *host;
  const char *username;
  const char *password;
  const char *vhost;
  int ssl;

  // SSL fields
  const char *key;
  const char *cert;
  const char *cacert;
  int timeout;

  amqp_connection_state_t conn;


  fetch_connection_params(L);

  host = luaL_optstring(L, -1, DEFAULT_HOST);
  port = luaL_optint (L, -2, DEFAULT_PORT);
  username = luaL_optstring(L, -3, DEFAULT_USERNAME);
  password = luaL_optstring(L, -4, DEFAULT_PASSWORD);
  vhost = luaL_optstring(L, -5, DEFAULT_VHOST);
  ssl = lua_toboolean(L, -6);

  conn = amqp_new_connection();

  if (ssl) {
    port = luaL_optint (L, -2, DEFAULT_SSL_PORT);
    cacert = luaL_checkstring(L, -7);
    cert = lua_tostring(L, -8);
    key = lua_tostring(L, -9);
    timeout = luaL_optint (L, -10, DEFAULT_SSL_TIMEOUT);
    connect_ssl(L, conn, host, port, key, cert, cacert, 0, 0, timeout);
  } else {
    connect_regular(L, conn, host, port);
  }

  die_on_amqp_error(L, amqp_login(conn, vhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username, password), "Logging in");

  connection_t *c = (connection_t *) lua_newuserdata(L, sizeof(connection_t));
  setmeta(L, "session");
  c->amqp_connection= conn;

  return 1;
}

/**
* :lua_amqp_session_free
*
* closing the amqp session
*/
LUALIB_API int lua_amqp_session_free(lua_State *L) {
  connection_t *conn = (connection_t *)luaL_checkudata(L, 1, "session");
  if (conn -> amqp_connection) {
    die_on_amqp_error(L, amqp_connection_close(conn -> amqp_connection, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(L, amqp_destroy_connection(conn -> amqp_connection), "Ending connection");

    conn -> amqp_connection = NULL;
  }

  return 0;
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
