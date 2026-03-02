#pragma once

#include "Database/row.h"
#include "base/mem_arena.h"
#include "serverdeps/filemanager.h"
#include <stdint.h>

#define TABLE_SIZE 20

typedef struct {
    char name[64];
    Type type;
    uint size;
    uint offset;
} Column;

typedef struct {
    char name[64];
    Column* columns;
    uint root_page;
    uint root_page_offset;
    uint columns_count;
    uint rows_count;
    uint root_row_offset;
    uint last_row_offset;
} TableMeta;

typedef struct {
    uint8_t dirty : 1;
    TableMeta* meta;
    Row* rows;
    mem_arena arena;
} Table;

TableMeta* table_meta_init(char* name, char** columns);
Table* table_init(TableMeta* meta);
void table_add_row(Table* table, Row* row);
char* table_describe(Table* table, mem_arena* arena);
void table_free(Table* table);
Row get_row_columns(Table* table, Row* row, char** columns);
void table_save_meta(TableMeta* meta, FileManager* filemanager);
void table_save_rows(Table* table, FileManager* filemanager);
void table_save(Table* table, FileManager* filemanager);
TableMeta* table_meta_from_string(char* buffer);
