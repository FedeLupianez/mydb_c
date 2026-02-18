#include "../../include/base/communication.h"
#include "../../include/base/socket_t.h"
#include "../../include/base/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void response(Response* response, int client)
{
    if (client >= 0) {
        char buffer[1024] = {};
        char* message_copy = strdup(response->message);
        int written = snprintf(buffer, 1024, "%d/%s\n", response->status_code, message_copy);
        buffer[written] = '\0';
        send_data(client, buffer, written);
        free(message_copy);
        return;
    }
}

Response parse_to_response(char* input)
{
    Response response = (Response) { "", 0 };
    char** tokens = split(input, "/");
    response.status_code = atoi(tokens[0]);
    response.message = strdup(tokens[1]);
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
    return response;
}
