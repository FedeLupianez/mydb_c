#ifndef DB_H
#define DB_H
#include "./table.h"

typedef struct {
    Table* tables;
    char* name;
    int size;
    int capacity;
} Database;

Database* db_init(char* name);
void db_free(Database* db);
Table* db_add_table(Database* db, char* name, char** columns);

#endif
