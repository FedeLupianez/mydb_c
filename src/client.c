#include "../include/client_t.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    client_t c = create_client(1, 8080);
    bind_socket(c.socket);
    char* response = get_data(c.socket.socket);
    sleep(1);

    int is_running = 1;
    while (is_running) {
        printf("Enter command: ");
        char input[1024] = {};
        fgets(input, 1023, stdin);
        send_data(c.socket.socket, input);
        response = get_data(c.socket.socket);
        printf("%s\n", response);
        if (EQUAL(response, "close")) {
            printf("Server close your connection\n");
            is_running = 0;
        }
    }
    close_client(c);
    return EXIT_SUCCESS;
}
