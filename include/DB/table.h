#pragma once

#include "../base/mem_arena.h"
#include "./row.h"

#define TABLE_SIZE 20

typedef enum {
    INT_t = 0,
    FLOAT_t,
    CHAR_t,
    STRING_t
} TYPE;

TYPE get_type(char* name);

typedef struct {
    char* name;
    TYPE type;
} column;

typedef struct {
    char* name;
    unsigned int size;
    column* columns;
    unsigned int cols_len;
    Row* rows;
    mem_arena arena;
} Table;

Table table_init(char* name, char** columns);
void table_add_row(Table* table, Row* row);
void table_print(Table* table);
void table_free(Table* table);
