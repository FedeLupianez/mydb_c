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
        sprintf(buffer, "%d/%s", response->status_code, response->message);
        send_data(client, buffer, strlen(buffer));
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
