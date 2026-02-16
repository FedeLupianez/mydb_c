#pragma once

#include "../base/hashmap.h"
#include "./table.h"

typedef struct {
    hashmap tables;
    Table* table_heap;
    char* name;
    uint size;
    uint capacity;
} Database;

Database* db_init(char* name);
void db_free(Database* db);
Table db_add_table(Database* db, char* name, char** columns);
