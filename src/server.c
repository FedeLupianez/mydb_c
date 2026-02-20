#include "base/communication.h"
#include "serverdeps/exec.h"
#include <signal.h>
#define show_pkgs 0

volatile sig_atomic_t is_running = 1;
Response r = { NULL, OK, 0, pkg_string };

int accept_client(socket_t socket)
{
    int client = accept_connection(socket);
    if (client < 0) {
        if (!is_running)
            return -1;
        perror("\e[0;31mError accepting connection\n");
        return -1;
    }
    Response r = ok("Connected", pkg_string);
    send_response(&r, client);
    free(r.message);
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
    printf("Running server");
    main_socket = malloc(sizeof(socket_t));
    *main_socket = create_socket(8080);
    socket_t server_socket = *main_socket;
    if (main_socket->socket < 0) {
        perror("\e[0;31mError creating socket\n");
        return EXIT_FAILURE;
    }
    if (!bind_socket(server_socket)) {
        close_socket(server_socket);
        printf("\e[0;31mError binding socket\n");
        return EXIT_FAILURE;
    }

    int client = -1;
    client = accept_client(server_socket);
    Database* db = db_init("test.db");

    while (1) {
        if (client < 0)
            printf("Waiting for client...\n");
        while (client < 0 && is_running)
            client = accept_client(server_socket);
        if (!is_running)
            break;
        char* input = get_data(client);
        strip(input);
        printf("%s\n", input);

        if (EQUAL(input, "exit")) {
            r = ok("close", pkg_string);
            send_response(&r, client);
            close(client);
            client = -1;
            free(input);
            continue;
        }
        mem_arena tmp_arena = mem_arena_create(KB(1));
        ServerContext ctx = { db, &tmp_arena };
        r = execute(&ctx, input);
#if show_pkgs
        printf("\nStatus: %d\nLenght:%d\npkg_type:%d\nMessage: %s\n\n", r.status_code, r.lenght, r.type, r.message);
#endif
        send_response(&r, client);
        // Free memory
        mem_arena_free(&tmp_arena);
        ctx.arena = NULL;
        free(input);
        free(r.message);
        r.message = NULL;
    }
    if (client > 0)
        close(client);

    // Free memory at end of program
    if (main_socket != NULL)
        free(main_socket);

    if (r.message != NULL)
        free(r.message);
    r.message = NULL;
    db_free(db);
    printf("\n\e[0;31mServer closed\n");
    return EXIT_SUCCESS;
}
