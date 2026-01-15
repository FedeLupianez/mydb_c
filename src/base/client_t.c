#include "../../include/client_t.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

client_t create_client(int id, int port)
{
    client_t c;
    c.id = id;
    c.socket = create_socket(port);
    if (connect(c.socket.socket, (struct sockaddr*)&c.socket.addr, sizeof(c.socket.addr)) < 0) {
        close_client(c);
        perror("Error connecting to server\n");
        return c;
    }
    printf("Created client %d\n", id);
    return c;
}

void close_client(client_t c)
{
    if (c.socket.socket > 0) {
        close_socket(c.socket);
        c.id = -1;
    }
};
