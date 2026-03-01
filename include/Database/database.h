#pragma once

#include "base/hashmap.h"
#include "table.h"
#include "transactionmanager.h"

typedef struct {
    hashmap* tables;
    char* name;
    TransactionManager* tm;
    FileManager* fm;
    int table_count;
    int table_end;
} Database;

Database* db_init(char* name);
void db_free(Database* db);
void db_load_metadata(Database* db);
void load_tables_metadata(Database* db);
Table* db_add_table(Database* db, char* name, char** columns);
Table* db_get_table(Database* db, char* name);
uint8_t db_delete_table(Database* db, char* table_name);
void db_save_meta(Database* db);
void db_save(Database* db);
