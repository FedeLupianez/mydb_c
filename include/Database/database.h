#pragma once

#include "base/hashmap.h"
#include "table.h"

typedef struct {
    hashmap* tables;
    char* name;
} Database;

Database* db_init(char* name);
void db_free(Database* db);
Table* db_add_table(Database* db, char* name, char** columns);
Table* db_get_table(Database* db, char* name);
uint8_t db_delete_table(Database* db, char* table_name);
