#pragma once

#include "./base/socket_t.h"
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct {
    unsigned int id;
    socket_t socket;
} client_t;

client_t create_client(int id, int port);
void close_client(client_t c);
