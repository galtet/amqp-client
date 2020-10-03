#ifndef LUAAMQP_STRUCTS_H
#define LUAAMQP_STRUCTS_H

#include <amqp.h>
#include <amqp_tcp_socket.h>

typedef struct {
  amqp_connection_state_t amqp_connection;
} connection_t;

typedef struct {
  connection_t* connection;
  int id;
} channel_t;

typedef struct {
  channel_t* channel;
  const char* name;
} queue_t;

typedef struct {
  channel_t* channel;
  const char* name;
} exchange_t;

#endif
