#include "../../include/DB/db.h"
#include <stdio.h>
#include <stdlib.h>

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tables = hashmap_init(2, sizeof(Table));
    new_db->size = 0;
    new_db->name = name;
    new_db->capacity = 2;
    return new_db;
}

Table* db_add_table(Database* db, char* name, char** columns)
{
    if (db->size == db->capacity) {
        db->capacity *= 2;
    }
    printf("%d\n", db->size);
    Table new_table = table_init(name, columns);
    hashmap_set(&db->tables, name, &new_table);
    db->size++;
    return hashmap_get(&db->tables, name);
}

void db_free(Database* db)
{
    hashmap_free(&db->tables);
    free(db);
}
