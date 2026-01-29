#include "../include/base/communication.h"
#include "../include/serverdeps/exec.h"
#include <stdio.h>
#include <stdlib.h>

int accept_client(socket_t socket)
{
    int client = accept_connection(socket);
    if (client < 0) {
        perror("Error accepting connection\n");
        return -1;
    }
    Response r = (Response) { "Connected\n", 200 };
    response(&r, client);
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
    Response r = (Response) { "hello from the database\n", 200 };

    while (is_running) {
        if (client < 0)
            printf("Waiting for client...\n");
        while (client < 0)
            client = accept_client(main_socket);
        char* input = get_data(client);
        strip(input);

        if (EQUAL(input, "exit")) {
            r.message = "close";
            r.status_code = 200;
            response(&r, client);
            close(client);
            client = -1;
            free(input);
            continue;
        }
        r = execute(db, input);
        response(&r, client);
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
