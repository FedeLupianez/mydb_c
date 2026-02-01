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
    send_data(client_id, message);
    exit(0);
}

int main(void)
{
    signal(SIGINT, handler_singint);
    socket_t client = create_client(8080);
    bind_socket(client);
    client_id = client.socket;
    char* server_data = get_data(client_id);

    int is_running = 1;
    while (is_running) {
        printf("Enter command: ");
        char input[1024] = {};
        fgets(input, 1023, stdin);

        if (EQUAL(input, "clear")) {
            system("clear");
            continue;
        }
        send_data(client_id, input);
        free(server_data);
        server_data = get_data(client_id);
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
    }
    close_socket(client);
    return EXIT_SUCCESS;
}
