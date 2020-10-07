#ifndef LUAAMQP_STRUCTS_H
#define LUAAMQP_STRUCTS_H

#include <amqp.h>
#include <amqp_tcp_socket.h>

typedef struct {
  amqp_connection_state_t amqp_connection;
  int is_ssl;
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

typedef struct structs {
  amqp_envelope_t *envelope;
} properties_t;

#endif
