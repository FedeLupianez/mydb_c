#pragma once

#include "../serverdeps/mem_arena.h"
#include "./row.h"

#define TABLE_SIZE 20

typedef struct {
    char* name;
    char** columns;
    unsigned int size;
    Row* rows;
    mem_arena* arena;
} Table;

Table table_init(char* name, char** columns);
void table_add_row(Table* table, Row* row);
void table_print(Table* table);
void table_free(Table* table);
