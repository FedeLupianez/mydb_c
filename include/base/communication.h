#pragma once
#include "commons.h"
#define TYPE_IDX 0
#define LENGHT_IDX 1
#define STATUS_IDX 2

typedef uint8_t status;
typedef uint8_t pkg_type;

typedef enum {
    OK = 0,
    NOT_FOUND = 1,
    BAD_REQUEST = 2,
    SERVER_ERROR = 3,
    UNAUTHORIZED = 4
} status_code_enum;

typedef enum {
    pkg_string = 0,
    pkg_int = 1,
    pkg_float = 2
} pkg_type_enum;

typedef struct {
    status status_code;
    uint8_t lenght;
    pkg_type type;
    byte* data;
} Response;

void send_response(Response* response, int client);
Response parse_to_response(byte* input);

Response invalid_args(void);
Response server_error(void);
Response ok(byte* message, pkg_type type);
Response not_found(byte* message);
