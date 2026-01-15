#include "../../include/DB/db.h"
#include <stdio.h>
#include <stdlib.h>

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tables = NULL;
    new_db->size = 0;
    new_db->name = name;
    new_db->capacity = 2;
    return new_db;
}

Table* db_add_table(Database* db, char* name, char** columns)
{
    if (db->tables == NULL) {
        db->tables = (Table*)malloc(sizeof(Table) * db->capacity);
    }

    if (db->size == db->capacity) {
        db->capacity *= 2;
        db->tables = realloc(db->tables, db->capacity * sizeof(Table));
    }
    printf("%d\n", db->size);
    db->tables[db->size] = table_init(name, columns);
    db->size++;
    return &db->tables[db->size - 1];
}

void db_free(Database* db)
{
    for (int i = 0; i < db->size; i++) {
        table_free(&db->tables[i]);
    }
    free(db);
}
