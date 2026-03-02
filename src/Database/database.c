#include "Database/database.h"
#include "Database/row.h"
#include "Database/transactionmanager.h"
#include "printing.h"

void save_tables_metadata(Database* db);

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tm = tm_init();
    new_db->fm = file_manager_init(name);
    db_load_metadata(new_db);
    uint init_from_scratch = (new_db->table_count == -1) ? 1 : 0;
    if (init_from_scratch) {
        new_db->table_count = 1;
        new_db->table_end = 1;
        new_db->name = name;
        print_trace("Database init from scratch with 1 tables\n");
    }
    print_trace("Initializing database");
    new_db->tables = hashmap_init(new_db->table_count, sizeof(TableMeta));
    print_trace("hashmap initialized");
    if (!init_from_scratch)
        load_tables_metadata(new_db);
    print_trace("Database initialized");
    return new_db;
}

Table* db_add_table(Database* db, char* name, char** columns)
{
    TableMeta* meta = table_meta_init(name, columns);
    if (!meta)
        return NULL;
    print_debug("Meta created");
    meta->root_page = db->table_end++;
    hashmap_insert(db->tables, name, meta);
    print_debug("Meta inserted");
    TableMeta* stored_meta = hashmap_get(db->tables, name);
    print_debug("Meta got");
    return db->tm->get_table(db->tm, stored_meta, db->fm);
}

void db_free(Database* db)
{
    if (!db)
        return;
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            node* next = current->next;
            free(current->key);
            TableMeta* meta = (TableMeta*)current->value;
            free(meta->columns);
            free(meta);
            free(current);
            current = next;
        }
    }
    free(db->tables->buckets);
    free(db->tables);
    tm_free(db->tm);
    db->tm = NULL;
    file_manager_close(db->fm);
    db->fm = NULL;
    free(db);
    print_trace("Database freed");
}

Table* db_get_table(Database* db, char* table_name)
{
    TableMeta* meta = (TableMeta*)hashmap_get(db->tables, table_name);
    return db->tm->get_table(db->tm, meta, db->fm);
}

uint8_t db_delete_table(Database* db, char* table_name)
{
    return hashmap_delete(db->tables, table_name);
}

void db_save_meta(Database* db)
{

    int writed = 0;
    char* buffer = calloc(32, sizeof(char));
    int len = snprintf(buffer, sizeof(buffer), "%s", db->name);
    buffer[len] = COL_SEPARATOR;
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, 0);
    free(buffer);
    int table_count = 0;

    buffer = calloc(32, sizeof(char));
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            table_count++;
            current = current->next;
        }
    }
    len = snprintf(buffer, sizeof(buffer), "%d", table_count);
    buffer[len] = COL_SEPARATOR;
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", db->tables->capacity);
    buffer[len] = COL_SEPARATOR;
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    sprintf(buffer, "%d", db->table_end);
    buffer[len] = VAL_SEPARATOR;
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    free(buffer);
}

void db_save(Database* db)
{
    db_save_meta(db);
    save_tables_metadata(db);
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            TableMeta* meta = (TableMeta*)current->value;
            Table* table = db->tm->get_table(db->tm, meta, db->fm);
            table_save(table, db->fm);
            current = current->next;
        }
    }
    print_trace("Database saved");
}

void db_load_metadata(Database* db)
{
    db->table_count = -1;
    page_t* page = db->fm->read_page(db->fm, DATABASE_META_PAGE);
    char* buffer = page->data;
    if (buffer[0] == '\0' || !buffer) {
        printf("Database is empty\n");
        return;
    }
    // first, load database metadata
    int name_len = 0;
    while (buffer[name_len] != COL_SEPARATOR)
        name_len++;
    db->name = malloc(name_len + 1);
    strcpy(db->name, buffer);
    db->name[name_len] = COL_SEPARATOR;
    db->table_count = atoi(buffer + name_len + 1);
    if (db->table_count == 0) {
        db->table_count = 2;
    }
    int pos = name_len + 1;
    while (buffer[pos] != COL_SEPARATOR)
        pos++;
    db->table_end = atoi(buffer + pos + 1);
    if (db->table_end == 0) {
        db->table_end = 2;
    }
    printf("Database %s loaded with %d tables\n", db->name, db->table_count);
}

void load_tables_metadata(Database* db)
{
    page_t* page = db->fm->read_page(db->fm, TABLE_META_PAGE);
    print_debug("Loading tables metadata");
    char* buffer = page->data;
    if (!buffer || buffer[0] == '\0') {
        printf("No tables found, starting with empty database\n");
        return;
    }

    printf("Page offset : %d\n", page->offset);
    int last_len = 0;
    for (int i = 0; i < db->table_count; i++) {
        // Separar lo strings de las tablas
        int len = 0;
        print_debug("Len of string");
        printf("last_len + len = %d\n", last_len + len);
        while (buffer[last_len + len] != VAL_SEPARATOR)
            len++;
        printf("len = %d\n", len);
        char* tmp = calloc(len + 1, sizeof(char));
        memcpy(tmp, buffer + last_len, len);

        TableMeta* meta = table_meta_from_string(tmp, 0, len);
        print_debug("loaded table metadata");
        hashmap_insert(db->tables, meta->name, meta);
        last_len += len + 1;
        free(tmp);
    }
}

void save_tables_metadata(Database* db)
{
    page_t* page = db->fm->read_page(db->fm, TABLE_META_PAGE);
    char* buffer = page->data;
    int offset = 0;
    for (uint i = 0; i < db->tables->size; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            TableMeta* meta = (TableMeta*)current->value;
            table_save_meta(meta, db->fm);
        }
    }
    db->fm->write_page(db->fm, TABLE_META_PAGE, buffer, offset);
}
