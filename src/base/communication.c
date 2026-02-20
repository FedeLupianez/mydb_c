#include "base/communication.h"
#include "base/socket_t.h"
#include "commons.h"
#include <stdint.h>
#include <string.h>

void send_response(Response* response, int client)
{
    if (client >= 0) {
        size_t total_lenght = response->lenght + 3;
        char* buffer = malloc(total_lenght);

        buffer[0] = response->type;
        buffer[1] = response->lenght;
        buffer[2] = response->status_code;

        memcpy(buffer + 3, response->message, response->lenght);
        send_data(client, buffer, total_lenght);
        free(buffer);
    }
}

Response parse_to_response(char* bytes)
{
    Response response;
    printf("Bytes: %d %d %d\n", bytes[0], bytes[1], bytes[2]);
    response.type = bytes[0];
    response.lenght = bytes[1];
    response.status_code = bytes[2];
    response.message = malloc(response.lenght + 1);
    memcpy(response.message, bytes + 3, response.lenght);
    response.message[response.lenght] = '\0';
    return response;
}

Response invalid_args(void)
{
    Response response;
    char* message = "Invalid args";
    size_t len = strlen(message);
    response.message = malloc(len + 1);
    memcpy(response.message, message, len);
    response.status_code = BAD_REQUEST;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response not_found(char* message)
{
    Response response;
    size_t len = strlen(message);
    response.message = malloc(len + 1);
    memcpy(response.message, message, len);
    response.status_code = NOT_FOUND;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response server_error(void)
{
    Response response;
    char* message = "Server Error";
    size_t len = strlen(message);
    response.message = malloc(len + 1);
    memcpy(response.message, message, len);
    response.status_code = SERVER_ERROR;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response ok(char* message, pkg_type type)
{
    Response response;
    switch (type) {
    case pkg_string:
        response.lenght = strlen(message);
        break;
    case pkg_int:
        response.lenght = sizeof(int);
        break;
    case pkg_float:
        response.lenght = sizeof(float);
        break;
    default:
        return server_error();
        break;
    }
    response.message = malloc(response.lenght + 1);
    memcpy(response.message, message, response.lenght);
    response.message[response.lenght] = '\0';
    response.type = type;
    return response;
}
