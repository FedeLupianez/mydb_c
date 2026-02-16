
#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
    int socket;
    struct sockaddr_in addr;
} socket_t;

socket_t create_socket(int port);
socket_t create_client(char* ip, int port);
int bind_socket(socket_t s);
void close_socket(socket_t s);
int accept_connection(socket_t s);
char* get_data(int client);
int send_data(int client, const void* buffer, size_t lenght);
