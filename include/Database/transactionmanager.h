#pragma once

#include "Database/table.h"
#include "base/utils.h"

#define DATABASE_META_PAGE 0
#define TABLE_META_PAGE 1

Table* parse_to_table(TableMeta* metadata, FileManager* fm);

#define TABLE_CACHE_SIZE 10
typedef struct TransactionManager TransactionManager;

struct TransactionManager {
    Table* table_cache[TABLE_CACHE_SIZE];
    uint8_t table_cache_size;
    Table* (*load_table)(TransactionManager* tm, TableMeta* metadata, FileManager* fm);
    Table* (*get_table)(TransactionManager* tm, TableMeta* metadata, FileManager* fm);
    void (*realease_table)(TransactionManager* tm, Table* table);
};

TransactionManager* tm_init();
void tm_free(TransactionManager* tm);
Table* load_table(TransactionManager* tm, TableMeta* metadata, FileManager* fm);
Table* get_table(TransactionManager* tm, TableMeta* metadata, FileManager* fm);
void realease_table(TransactionManager* tm, Table* table);
