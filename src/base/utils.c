#include "../../include/base/utils.h"
#include <stddef.h>
#include <stdio.h>

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
    int i = 0;
    while (tokens[i] != NULL) {
        printf("token tokenize %d: %s\n", i, tokens[i]);
        i++;
    }
    free(copy);
    return tokens;
}

char** tokenize(char* str)
{
    if (!str)
        return NULL;
    int count = 0;
    int capacity = 4;
    char** tokens = malloc(2 * sizeof(char*));
    if (!tokens)
        return NULL;

    int token_cap = 16;
    int token_len = 0;
    char* actual_token = malloc(token_cap);
    if (!actual_token) {
        free(tokens);
        return NULL;
    }
    actual_token[0] = '\0';

    int parentesis = 0;
    int str_lenght = strlen(str);
    for (int i = 0; i < str_lenght; i++) {
        char c = str[i];

        if (c == '(')
            parentesis = 1;
        if (c == ')')
            parentesis = 0;

        if (!parentesis && c == ' ') {
            if (token_len > 0) {
                if (count + 1 >= capacity) {
                    capacity *= 2;
                    char** tmp = realloc(tokens, capacity * sizeof(char*));
                    if (!tmp) {
                        free(actual_token);
                        free(tokens);
                        return NULL;
                    }
                    tokens = tmp;
                }
                tokens[count] = strdup(actual_token);
                if (!tokens[count])
                    goto error;
                count++;
                free(actual_token);
                token_cap = 16;
                token_len = 0;
                actual_token = malloc(token_cap);
                if (!actual_token)
                    goto error;
                actual_token[0] = '\0';
            }
        } else {
            if (token_len + 1 >= token_cap) {
                token_cap *= 2;
                char* tmp = realloc(actual_token, token_cap);
                if (!tmp)
                    goto error;
                actual_token = tmp;
            }
            actual_token[token_len] = c;
            token_len++;
            actual_token[token_len] = '\0';
        }
    }
    if (token_len > 0) {
        if (count + 1 >= capacity) {
            capacity++;
            char** tmp = realloc(tokens, capacity * sizeof(char*));
            if (!tmp)
                goto error;
            tokens = tmp;
        }
        tokens[count] = strdup(actual_token);
        if (!tokens[count])
            goto error;
        count++;
    }

    if (count >= capacity) {
        char** tmp = realloc(tokens, (count + 1) * sizeof(char*));
        if (!tmp)
            goto error;
        tokens = tmp;
    }
    tokens[count] = NULL;
    free(actual_token);
    return tokens;
error:
    if (actual_token)
        free(actual_token);
    if (tokens) {
        for (int i = 0; i < count; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }
    return NULL;
}

void replace(char* str, char old, char new_c)
{
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == old)
            str[i] = new_c;
    }
}

char** copy_to_arena(char** tokens, mem_arena* arena)
{
    int count = 0;
    while (tokens[count] != NULL)
        count++;
    char** new_tokens = mem_arena_alloc(arena, count * sizeof(char*));
    for (int i = 0; i < count; i++)
        new_tokens[i] = strdup(tokens[i]);
    new_tokens[count] = NULL;
    for (int i = 0; i < count; i++)
        free(tokens[i]);
    return new_tokens;
}

char** tokenize_arena(char* str, mem_arena* arena)
{
    char** tokens = tokenize(str);
    char** arena_tokens = copy_to_arena(tokens, arena);
    free(tokens);
    return arena_tokens;
}

char** split_arena(char* str, char* delim, mem_arena* arena)
{
    char** tokens = split(str, delim);
    char** arena_tokens = copy_to_arena(tokens, arena);
    free(tokens);
    return arena_tokens;
}
