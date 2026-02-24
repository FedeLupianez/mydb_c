#include "base/communication.h"
#include "printing.h"
#include "serverdeps/exec.h"
#include <signal.h>
#define show_pkgs 0

volatile sig_atomic_t is_running = 1;
Response r = { OK, 0, pkg_string, NULL };

int accept_client(socket_t socket)
{
    int client = accept_connection(socket);
    if (client < 0) {
        if (!is_running)
            return -1;
        print_error("Error accepting connection");
        return -1;
    }
    Response r = ok("Connected", pkg_string);
    send_response(&r, client);
    free(r.data);
    return client;
}

socket_t* main_socket = NULL;

void handler_singint(int sig)
{
    is_running = 0;
    if (main_socket)
        close(main_socket->socket);
}

int main()
{
    signal(SIGINT, handler_singint);
    print_info("Starting server");
    main_socket = malloc(sizeof(socket_t));
    *main_socket = create_socket(8080);
    socket_t server_socket = *main_socket;
    mem_arena server_arena;
    if (main_socket->socket < 0) {
        print_error("Error creating server socket");
        return EXIT_FAILURE;
    }
    if (!bind_socket(server_socket)) {
        close_socket(server_socket);
        print_error("Error binding socket");
        return EXIT_FAILURE;
    }

    int client = -1;
    client = accept_client(server_socket);
    Database* db = db_init("test.db");
    char* input = NULL;

    while (1) {
        if (client < 0)
            printf("Waiting for client\n");
        while (client < 0 && is_running)
            client = accept_client(server_socket);
        if (!is_running) {
            if (input != NULL) {
                print_trace("freeing input");
                free(input);
                print_trace("input freed");
            }
            break;
        }
        input = get_data(client);
        strip(input);
        print_debug(input);

        if (EQUAL(input, "exit")) {
            r = ok("close", pkg_string);
            send_response(&r, client);
            close(client);
            client = -1;
            continue;
        }
        server_arena = mem_arena_create(KB(1));
        ServerContext ctx = { db, &server_arena };
        r = execute(&ctx, input);
#if show_pkgs
        printf("\nStatus: %d\nLenght:%d\npkg_type:%d\nMessage: %s\n\n", r.status_code, r.lenght, r.type, r.message);
#endif
        send_response(&r, client);
        // Free memory
        mem_arena_free(&server_arena);
        ctx.arena = NULL;
        print_trace("freeing input");
        if (input != NULL)
            free(input);
        input = NULL;
        free(r.data);
        r.data = NULL;
    }
    if (client > 0) {
        print_trace("Client disconnected");
        close(client);
    }

    // Free memory at end of program
    if (main_socket != NULL) {
        print_trace("Closing main socket");
        close_socket(*main_socket);
        free(main_socket);
        main_socket = NULL;
    }

    if (db != NULL) {
        print_trace("Freeing database");
        db_free(db);
        db = NULL;
    }
    mem_arena_free(&server_arena);
    print_info("Server closed");
    return EXIT_SUCCESS;
}
