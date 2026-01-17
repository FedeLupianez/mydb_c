#include "../include/client_t.h"
#include "../include/communication.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    client_t c = create_client(1, 8080);
    bind_socket(c.socket);
    char* server_data = get_data(c.socket.socket);
    sleep(1);

    int is_running = 1;
    while (is_running) {
        printf("Enter command: ");
        char input[1024] = {};
        fgets(input, 1023, stdin);
        send_data(c.socket.socket, input);
        server_data = get_data(c.socket.socket);
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
