#include "../../include/base/utils.h"
#include <stddef.h>

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

char** split(const char* str, const char* delim)
{
    char* copy = strdup(str);
    if (!copy)
        return NULL;

    size_t cap = 4, len = 0;
    char** tokens = malloc(cap * sizeof(char*));
    if (!tokens) {
        free(copy);
        return NULL;
    }

    char* token = strtok(copy, delim);
    while (token) {
        if (len + 1 >= cap) {
            cap *= 2;
            char** tmp = realloc(tokens, cap * sizeof(char*));
            if (!tmp) {
                free(copy);
                free(tokens);
                return NULL;
            }
            tokens = tmp;
        }
        tokens[len] = strdup(token);
        len++;
        token = strtok(NULL, delim);
    }
    tokens[len] = NULL;
    free(copy);
    return tokens;
}

char** tokenize(char* str)
{
    char** tokens = malloc(2 * sizeof(char*));
    char* actual_token = malloc(1);
    int index_token = 0;
    int tokens_cant = 2;
    int parentesis = 0;
    actual_token[0] = '\0';
    int str_lenght = strlen(str);
    for (int i = 0; i < str_lenght; i++) {
        char c = str[i];
        if (c == '(')
            parentesis = 1;
        if (c == ')')
            parentesis = 0;
        if (parentesis == 0 && c == ' ') {
            tokens[index_token] = strdup(actual_token);
            index_token++;
            if (index_token >= tokens_cant) {
                tokens_cant = index_token + 1;
                tokens = realloc(tokens, sizeof(char*) * (tokens_cant + 1));
            }
            free(actual_token);
            actual_token = malloc(1);
            actual_token[0] = '\0';
        } else {
            int len = strlen(actual_token);
            actual_token = realloc(actual_token, len + 2);
            actual_token[len] = c;
            actual_token[len + 1] = '\0';
        }
    }
    if (strlen(actual_token) > 0) {
        tokens[index_token] = strdup(actual_token);
        index_token++;
    }
    tokens[index_token] = NULL;
    free(actual_token);
    return tokens;
}

void replace(char* str, char old, char new_c)
{
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == old)
            str[i] = new_c;
    }
}
