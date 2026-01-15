#ifndef CELL_H
#define CELL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    INT,
    BYTE,
    FLOAT,
    STR,
    VOID
} Type;

typedef struct {
    Type type;
    union {
        int i;
        unsigned char b;
        float f;
        char* s;
    } data;
} cell;

cell cell_init(Type type, ...);
void cell_free(cell* c);
void* cell_get_value(cell* c);

#endif
