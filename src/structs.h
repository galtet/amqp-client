#ifndef LUAAMQP_STRUCTS_H
#define LUAAMQP_STRUCTS_H

#include <amqp.h>
#include <amqp_tcp_socket.h>

typedef struct {
  amqp_connection_state_t connection;
} connection_t;

typedef struct {
  amqp_connection_state_t connection;
  int id;
} channel_t;

#endif