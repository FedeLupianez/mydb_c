#include "base/communication.h"
#include "base/socket_t.h"
#include "base/utils.h"
#include "printing.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t is_running = 1;

int client_id = -1;

// funtion to handle signint signal to close connection with server
void close_client(int sig)
{
    char* message = "exit";
    send_data(client_id, message, strlen(message));
    is_running = 0;
}

int main(int argc, char** argv)
{
    char* ip = "127.0.0.1";
    if (argc > 1) {
        ip = argv[1];
    }

    int port = 8080;
    if (argc > 2) {
        port = atoi(argv[2]);
    }

    signal(SIGINT, close_client);
    socket_t* client = malloc(sizeof(socket_t));
    *client = create_client(ip, port);
    bind_socket(*client);
    client_id = client->socket;
    Response response;
    char* server_data = NULL;

    while (is_running) {
        server_data = get_data(client_id);
        response = parse_to_response(server_data);
        switch (response.status_code) {
        case OK:
            print_ok(response.data);
            break;
        case BAD_REQUEST:
            print_error(response.data);
            break;
        default:
            print_warning(response.data);
            break;
        }

        if (EQUAL(response.data, "close")) {
            print_info("Server closed connection");
            is_running = 0;
            continue;
        }
        free(response.data);
        free(server_data);

        printf("> ");
        char input[1024] = {};
        fgets(input, 1023, stdin);
        strip(input);

        if (EQUAL(input, "clear")) {
            system("clear");
            continue;
        }
        send_data(client_id, input, strlen(input));
        if (server_data == NULL) {
            print_error("Server not responding\nClosing your connection");
            is_running = 0;
            continue;
        }
    }
    if (response.data != NULL) {
        free(response.data);
        print_trace("response freed");
    }
    if (server_data != NULL) {
        free(server_data);
        print_trace("server_data freed");
    }
    close_socket(*client);
    free(client);
    return EXIT_SUCCESS;
}
