#include "../include/communication.h"
#include "../include/base/socket_t.h"

void response(char* message, int client)
{
    if (client >= 0) {
        send_data(client, message);
        return;
    }
}
