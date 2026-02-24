#include "Database/database.h"
#include "printing.h"

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
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            node* next = (node*)current->next;
            free(current->key);
            print_trace("key freed");
            table_free(current->value);
            print_trace("value freed");
            free(current->value);
            free(current);
            print_trace("node freed");
            current = next;
        }
    }
    free(db->tables->buckets);
    print_trace("hashmap buckets freed");
    free(db->tables);
    print_trace("hashmap freed");
    free(db);
    print_trace("Database freed");
}

Table* db_get_table(Database* db, char* table_name)
{
    return (Table*)hashmap_get(db->tables, table_name);
}

uint8_t db_delete_table(Database* db, char* table_name)
{
    return hashmap_delete(db->tables, table_name);
}
