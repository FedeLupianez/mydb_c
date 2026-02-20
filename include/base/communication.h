#pragma once
#include <stdint.h>

typedef enum {
    OK = 0,
    NOT_FOUND = 1,
    BAD_REQUEST = 2,
    SERVER_ERROR = 3,
    UNAUTHORIZED = 4
} STATUS_CODE;

typedef enum {
    pkg_string = 0,
    pkg_int = 1,
    pkg_float = 2
} pkg_type;

typedef struct {
    char* message;
    STATUS_CODE status_code;
    uint8_t lenght;
    pkg_type type;
} Response;

void send_response(Response* response, int client);
Response parse_to_response(char* input);

Response invalid_args(void);
Response server_error(void);
Response ok(char* message, pkg_type type);
Response not_found(char* message);
