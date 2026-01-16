
#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
    int socket;
    struct sockaddr_in addr;
} socket_t;

socket_t create_socket(int port);
int bind_socket(socket_t s);
void close_socket(socket_t s);
int accept_connection(socket_t s);
char* get_data(int client);
int send_data(int client, char* data);
