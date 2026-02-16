#pragma once
#define OK 200
#define NOT_FOUND 404
#define BAD_REQUEST 400
#define SERVER_ERROR 500
#define UNAUTHORIZED 401

typedef struct {
    char* message;
    int status_code;
} Response;

void response(Response* response, int client);
Response parse_to_response(char* input);
