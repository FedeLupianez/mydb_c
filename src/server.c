#include "../include/base/exec.h"
#include <stdio.h>
#include <stdlib.h>

int accept_client(socket_t socket)
{
    int client = accept_connection(socket);
    if (client < 0) {
        perror("Error accepting connection\n");
        return -1;
    }
    send_data(client, "You are connected to the database\n");
    return client;
}

int main()
{
    printf("Running from server");
    socket_t main_socket = create_socket(8080);
    if (main_socket.socket < 0) {
        perror("Error creating socket\n");
        return EXIT_FAILURE;
    }
    if (!bind_socket(main_socket)) {
        close_socket(main_socket);
        printf("Error binding socket\n");
        return EXIT_FAILURE;
    }
    printf("Listening on port 8080\n");

    int client = -1;
    client = accept_client(main_socket);
    int is_running = 1;
    Database* db = db_init("test.db");

    while (is_running) {
        if (client < 0)
            printf("Waiting for client...\n");
        while (client < 0)
            client = accept_client(main_socket);
        char* input = get_data(client);
        execute(db, input, &client);
        printf("%s\n", input);
        free(input);
    }
    if (client > 0) {
        close(client);
    }
    printf("Server closed\n");
    close_socket(main_socket);
    return EXIT_SUCCESS;
}
