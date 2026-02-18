#include "../../include/Database/database.h"

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tables = hashmap_init(2, sizeof(Table));
    new_db->table_heap = NULL;
    new_db->size = 0;
    new_db->name = name;
    new_db->capacity = 2;
    return new_db;
}

Table db_add_table(Database* db, char* name, char** columns)
{
    if (db->table_heap == NULL) {
        db->table_heap = (Table*)malloc(sizeof(Table) * db->capacity);
    }

    if (db->size == db->capacity) {
        db->capacity *= 2;
        db->table_heap = (Table*)realloc(db->table_heap, db->capacity * sizeof(Table));
    }
    Table new_table = table_init(name, columns);
    db->table_heap[db->size] = new_table;
    hashmap_set(&db->tables, name, &db->table_heap[db->size]);
    db->size++;
    return new_table;
}

void db_free(Database* db)
{
    for (uint i = 0; i < db->size; i++) {
        table_free(&db->table_heap[i]);
        free(&db->table_heap[i]);
    }
    free(db->table_heap);
    hashmap_free(&db->tables);
    free(db);
}
