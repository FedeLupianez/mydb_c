#include "Database/database.h"
#include "Database/transactionmanager.h"
#include "printing.h"

void save_tables_metadata(Database* db);

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tm = tm_init();
    new_db->fm = file_manager_init(name);
    db_load_metadata(new_db);
    if (new_db->table_count == -1) {
        new_db->table_count = 1;
        new_db->table_end = 1;
        new_db->name = name;
        print_trace("Database init from scratch with 1 tables\n");
    }
    print_trace("Initializing database");
    new_db->tables = hashmap_init(new_db->table_count, sizeof(TableMeta));
    print_trace("hashmap initialized");
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
    writed += db->fm->write_in(db->fm, db->name, strlen(db->name) + 1, DATABASE_META_PAGE, 0);
    char* buffer = calloc(32, sizeof(char));
    int table_count = 0;
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            table_count++;
            current = current->next;
        }
    }
    int len = snprintf(buffer, sizeof(buffer), "%d", table_count);
    db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    free(buffer);
    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", db->tables->capacity);
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    free(buffer);
    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", db->table_end);
    writed += db->fm->write_in(db->fm, buffer, len + 1, DATABASE_META_PAGE, writed);
    db->fm->write_in(db->fm, "\n", 1, DATABASE_META_PAGE, writed);
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
    while (buffer[name_len] != '\0')
        name_len++;
    db->name = malloc(name_len + 1);
    strcpy(db->name, buffer);
    db->name[name_len] = '\0';
    db->table_count = atoi(buffer + name_len + 1);
    if (db->table_count == 0) {
        db->table_count = 2;
    }
    int pos = name_len + 1;
    while (buffer[pos] != '\0')
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
    char** tables_strings = split(buffer, "\n");
    print_debug("Loaded tables string");
    for (uint i = 0; i < len_list(tables_strings); i++) {
        print_debug("loading table metadata");
        TableMeta* meta = table_meta_from_string(tables_strings[i], 0, strlen(tables_strings[i]));
        print_debug("loaded table metadata");
        hashmap_insert(db->tables, meta->name, meta);
    }
    free(tables_strings);
}

void save_tables_metadata(Database* db)
{
    page_t* page = db->fm->read_page(db->fm, TABLE_META_PAGE);
    char* buffer = page->data;
    int offset = 0;
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            TableMeta* meta = (TableMeta*)current->value;
            for (size_t j = 0; j < sizeof(meta->name); j++) {
                buffer[offset++] = meta->name[j];
            }
            buffer[offset++] = '\0';
            buffer[offset++] = meta->rows_count;
            buffer[offset++] = '\0';
            buffer[offset++] = meta->columns_count;
            buffer[offset++] = '\0';
            buffer[offset++] = meta->root_row_offset;
            buffer[offset++] = '\0';
            buffer[offset++] = meta->last_row_offset;
            for (uint j = 0; j < meta->columns_count; j++) {
                for (size_t k = 0; k < sizeof(meta->columns[j].name); k++) {
                    buffer[offset++] = meta->columns[j].name[k];
                }
                buffer[offset++] = '\0';
                char* type = get_type_name(meta->columns[j].type);
                for (size_t k = 0; k < strlen(type); k++) {
                    buffer[offset++] = type[k];
                }
                buffer[offset++] = '\0';
            }
            buffer[offset++] = '\n';
            current = current->next;
        }
    }
    db->fm->write_page(db->fm, TABLE_META_PAGE, buffer, offset);
}
