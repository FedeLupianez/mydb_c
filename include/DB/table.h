#pragma once

#include "../base/mem_arena.h"
#include "./row.h"

#define TABLE_SIZE 20

Type get_type(char* name);

typedef struct {
    char* name;
    Type type;
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
char* table_describe(Table* table, mem_arena* arena);
void table_free(Table* table);
Row get_row_columns(Table* table, Row* row, char** columns);
