#include "../include/base/communication.h"
#include "../include/base/socket_t.h"
#include "../include/base/utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int client_id = -1;

int get_client_id() { return client_id; }

// funtion to handle signint signal to close connection with server
void handler_singint(int sig)
{
    char* message = "exit";
    send_data(client_id, message, strlen(message));
    exit(0);
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

    signal(SIGINT, handler_singint);
    socket_t client = create_client(ip, port);
    bind_socket(client);
    client_id = client.socket;
    char* server_data = get_data(client_id);
    Response response = parse_to_response(server_data);
    printf("%s\n", response.message);
    free(response.message);
    free(server_data);

    int is_running = 1;
    while (is_running) {
        printf("Enter command: ");
        char input[1024] = {};
        fgets(input, 1023, stdin);
        strip(input);

        if (EQUAL(input, "clear")) {
            system("clear");
            continue;
        }
        send_data(client_id, input, strlen(input));
        char* server_data = get_data(client_id);
        if (server_data == NULL) {
            printf("Server not responding\nClosing your connection\n");
            is_running = 0;
            continue;
        }
        Response response = parse_to_response(server_data);

        switch (response.status_code) {
        case 200:
            printf("%s", "\e[0;32m");
            break;
        case 400:
            printf("%s", "\e[0;31m");
            break;
        }
        printf("%s\e[0m\n", response.message);

        if (EQUAL(response.message, "close")) {
            printf("Server close your connection\n");
            is_running = 0;
        }
        free(response.message);
        free(server_data);
    }
    close_socket(client);
    return EXIT_SUCCESS;
}
