#include "base/communication.h"
#include "base/socket_t.h"
#include "commons.h"

void send_response(Response* response, int client)
{
    if (client >= 0) {
        size_t total_lenght = response->lenght + 3;
        char* buffer = malloc(total_lenght);

        buffer[TYPE_IDX] = response->type;
        buffer[LENGHT_IDX] = response->lenght;
        buffer[STATUS_IDX] = response->status_code;

        memcpy(buffer + 3, response->data, response->lenght);
        send_data(client, buffer, total_lenght);
        free(buffer);
    }
}

Response parse_to_response(byte* bytes)
{
    Response response;
    response.type = bytes[TYPE_IDX];
    response.lenght = bytes[LENGHT_IDX];
    response.status_code = bytes[STATUS_IDX];
    printf("Status code: %d\nLenght: %d\nType: %d\n", response.status_code, response.lenght, response.type);
    response.data = malloc(response.lenght + 1);
    memcpy(response.data, bytes + 3, response.lenght);
    response.data[response.lenght] = '\0';
    return response;
}

Response invalid_args(void)
{
    Response response;
    char* message = "Invalid args";
    size_t len = strlen(message);
    response.data = malloc(len + 1);
    memcpy(response.data, message, len);
    response.status_code = BAD_REQUEST;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response not_found(byte* message)
{
    Response response;
    size_t len = strlen(message);
    response.data = malloc(len + 1);
    memcpy(response.data, message, len);
    response.status_code = NOT_FOUND;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response server_error(void)
{
    Response response;
    byte* message = "Server Error";
    size_t len = strlen(message);
    response.data = malloc(len + 1);
    memcpy(response.data, message, len);
    response.status_code = SERVER_ERROR;
    response.lenght = len;
    response.type = pkg_string;
    return response;
}

Response ok(byte* message, pkg_type type)
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
    response.data = malloc(response.lenght + 1);
    response.status_code = OK;
    memcpy(response.data, message, response.lenght);
    response.data[response.lenght] = '\0';
    response.type = type;
    return response;
}
