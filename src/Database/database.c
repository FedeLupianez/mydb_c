#include "Database/database.h"

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tables = hashmap_init(2, sizeof(Table));
    new_db->name = name;
    return new_db;
}

Table* db_add_table(Database* db, char* name, char** columns)
{
    Table new_table = table_init(name, columns);
    hashmap_insert(db->tables, name, &new_table);
    return (Table*)hashmap_get(db->tables, name);
}

void db_free(Database* db)
{
    if (!db)
        return;
    hashmap_free(db->tables);
    free(db);
}

Table* db_get_table(Database* db, char* table_name)
{
    return (Table*)hashmap_get(db->tables, table_name);
}

uint8_t db_delete_table(Database* db, char* table_name)
{
    return hashmap_delete(db->tables, table_name);
}
