#include "../include/base/communication.h"
#include "../include/serverdeps/exec.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

uint is_running = 1;

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

socket_t* main_socket = NULL;

void handler_singint(int sig)
{
    printf("Server closed\n");
    close_socket(*main_socket);
    free(main_socket);
    main_socket = NULL;
    is_running = 0;
    exit(0);
}

int main()
{
    signal(SIGINT, handler_singint);
    printf("Running server");
    main_socket = malloc(sizeof(socket_t));
    *main_socket = create_socket(8080);
    socket_t server_socket = *main_socket;
    if (main_socket->socket < 0) {
        perror("Error creating socket\n");
        return EXIT_FAILURE;
    }
    if (!bind_socket(server_socket)) {
        close_socket(server_socket);
        printf("Error binding socket\n");
        return EXIT_FAILURE;
    }

    int client = -1;
    client = accept_client(server_socket);
    Database* db = db_init("test.db");
    Response r = (Response) { "hello from the database\n", 200 };

    while (is_running) {
        if (client < 0)
            printf("Waiting for client...\n");
        while (client < 0)
            client = accept_client(server_socket);
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
        mem_arena tmp_arena = mem_arena_create(KB(1));
        ServerContext ctx = { db, &tmp_arena };
        r = execute(&ctx, input);
        response(&r, client);
        printf("%s\n", input);
        mem_arena_free(&tmp_arena);
        ctx.arena = NULL;
        free(input);
    }
    if (client > 0) {
        response(&(Response) { "Server closed\n", 200 }, client);
        close(client);
    }
    return EXIT_SUCCESS;
}
