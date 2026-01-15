#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define EQUAL(a, b) (!strcmp(a, b))

void strip(char* str);

char** split(char* str, char* delim);

#endif
