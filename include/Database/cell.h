#pragma once

#include "commons.h"
#include "types.h"
typedef union {
    int i;
    unsigned char b;
    float f;
    char* s;
    char c;
} DataValue;

typedef struct {
    Type type;
    DataValue data;
} Cell;

Cell cell_init_from_string(Type type, char* value);
Cell cell_init_void(void);
void cell_set_value_from_string(Cell* c, Type type, char* value);
void cell_free(Cell* c);
void* cell_get_value(Cell* c);
