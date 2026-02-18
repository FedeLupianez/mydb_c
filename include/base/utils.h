#pragma once

#include "./mem_arena.h"

#define EQUAL(a, b) (!strcmp(a, b))

int to_lower(int c);
uint len_list(char** list);

void strip(char* str);

char** split(char* str, char* delim);
char** split_arena(char* str, char* delim, mem_arena* arena);

char** tokenize(char* str);
char** tokenize_arena(char* str, mem_arena* arena);

void replace(char* str, char old, char new_c);
