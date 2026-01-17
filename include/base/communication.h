#pragma once

typedef struct {
    char* message;
    int status_code;
} Response;

void response(Response* response, int client);
Response parse_to_response(char* input);
