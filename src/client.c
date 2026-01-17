#include "../include/client_t.h"
#include "../include/communication.h"
#include "../include/utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int client_id = -1;

int get_client_id() { return client_id; }

// funtion to handle signint signal to close connection with server
void handler_singint(int sig)
{
    char* message = "exit";
    send_data(get_client_id(), message);
    exit(0);
}

int main(void)
{
    signal(SIGINT, handler_singint);
    client_t c = create_client(1, 8080);
    client_id = c.socket.socket;
    bind_socket(c.socket);
    char* server_data = get_data(client_id);
    sleep(1);

    int is_running = 1;
    while (is_running) {
        printf("Enter command: ");
        char input[1024] = {};
        fgets(input, 1023, stdin);
        send_data(client_id, input);
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
    }
    close_client(c);
    return EXIT_SUCCESS;
}
