#include "../include/utils.h"

void strip(char* str)
{
    for (size_t i = 0; i < strlen((const char*)str); i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
        str[i] = tolower(str[i]);
    }
}

char** split(char* str, char* delim)
{
    char* token = strtok(str, delim);
    char** tokens = malloc(2 * sizeof(char*));
    tokens[0] = token;
    int i = 1;
    while (token != NULL) {
        if (i >= 2) {
            tokens = realloc(tokens, (i + 1) * sizeof(char*));
        }
        token = strtok(NULL, delim);
        tokens[i++] = token;
    }
    return tokens;
}
