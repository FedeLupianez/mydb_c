#pragma once

typedef struct {
    char* message;
    int status_code;
} Response;

void response(char* response, int client);
