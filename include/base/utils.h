#pragma once

#include "./mem_arena.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define EQUAL(a, b) (!strcmp(a, b))

void strip(char* str);

char** split(char* str, char* delim);
char** split_arena(char* str, char* delim, mem_arena* arena);

char** tokenize(char* str);
char** tokenize_arena(char* str, mem_arena* arena);

void replace(char* str, char old, char new_c);
