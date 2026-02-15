#include "../../include/base/socket_t.h"
#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

socket_t create_socket(int port)
{
    socket_t new_listener;
    new_listener.socket = -1;
    new_listener.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_listener.socket < 0) {
        printf("error creating socket\n");
        return new_listener;
    }
    int opt = 1;
    setsockopt(new_listener.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    new_listener.addr.sin_family = AF_INET;
    new_listener.addr.sin_port = htons(port);
    new_listener.addr.sin_addr.s_addr = INADDR_ANY;
    printf("socket created at port %d\n", port);
    return new_listener;
}

socket_t create_client(char* ip, int port)
{
    socket_t client;
    client.socket = -1;
    client.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client.socket < 0) {
        perror("Error creating client\n");
        return client;
    }

    client.addr.sin_family = AF_INET;
    client.addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &client.addr.sin_addr) <= 0) {
        close_socket(client);
        client.socket = -1;
        perror("Error connecting to server\n");
        return client;
    }
    if (connect(client.socket, (struct sockaddr*)&client.addr, sizeof(client.addr)) < 0) {
        close_socket(client);
        client.socket = -1;
        perror("Error connecting to server\n");
        return client;
    }
    printf("Connected to server at %s:%d\n", ip, port);
    return client;
}
int bind_socket(socket_t s)
{
    if (s.socket < 0) {
        perror("invalid Socket\n");
        return 0;
    }
    if (bind(s.socket, (struct sockaddr*)&s.addr, sizeof(s.addr)) < 0) {
        return 0;
    }

    if (listen(s.socket, 5) < 0) {
        perror("Error listening\n");
        return 0;
    }
    printf("listening on port %d\n", ntohs(s.addr.sin_port));
    return 1;
}

int accept_connection(socket_t s)
{
    if (s.socket < 0) {
        perror("Invalid socket\n");
        return 0;
    }

    int client = accept(s.socket, NULL, NULL);
    if (client < 0) {
        printf("Error accepting connection\n");
        return -1;
    }
    printf("Accepted connection from client %d\n", client);
    return client;
}

void close_socket(socket_t s)
{
    if (s.socket >= 0) {
        close(s.socket);
    }
}

char* get_data(int client)
{
    if (client < 0) {
        return NULL;
    }

    const size_t BUFFER_SIZE = 1024;

    char* buffer = malloc(BUFFER_SIZE + 1);
    if (!buffer) {
        return NULL;
    }

    int bytes = recv(client, buffer, BUFFER_SIZE, 0);
    if (bytes < 0) {
        free(buffer);
        return NULL;
    }
    if ((size_t)bytes > BUFFER_SIZE) {
        free(buffer);
        return NULL;
    }
    buffer[bytes] = '\0';
    return buffer;
}

int send_data(int client, char* message)
{
    if (client < 0 || message == NULL) {
        return 0;
    }

    size_t message_len = strlen(message);
    ssize_t bytes_sent = send(client, message, message_len, 0);

    if (bytes_sent < 0) {
        perror("Error sending data");
        return 0;
    }

    if ((size_t)bytes_sent != message_len) {
        printf("Warning: Only sent %zd/%zu bytes\n", bytes_sent, message_len);
    }

    return 1;
}
