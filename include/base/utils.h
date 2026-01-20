#pragma once

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define EQUAL(a, b) (!strcmp(a, b))

void strip(char* str);

char** split(const char* str, const char* delim);

char** tokenize(char* str);

void replace(char* str, char old, char new_c);
