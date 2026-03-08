#include "Database/database.h"
#include "Database/row.h"
#include "Database/table.h"
#include "Database/transactionmanager.h"
#include "base/utils.h"
#include "printing.h"
#include "serverdeps/filemanager.h"
#include <stdio.h>

void save_tables_data(Database* db);

Database* db_init(char* name)
{
    Database* new_db = (Database*)malloc(sizeof(Database));
    new_db->tm = tm_init();
    new_db->fm = file_manager_init(name);
    new_db->tables = hashmap_init(16, sizeof(TableMeta));
    db_load_metadata(new_db);
    uint init_from_scratch = (new_db->table_count == -1);
    if (init_from_scratch) {
        new_db->table_count = 0;
        new_db->table_end = 2;
        new_db->name = strdup(name);
        print_trace("Database init from scratch with 1 tables\n");
    }
    print_trace("Initializing database");
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
    meta->root_page = db->table_end++;
    printf("DEBUG: Created table '%s' with root_page=%d\n", name, meta->root_page);
    hashmap_insert(db->tables, name, meta);
    TableMeta* stored_meta = hashmap_get(db->tables, name);
    return db->tm->get_table(db->tm, stored_meta, db->fm);
}

void db_free(Database* db)
{
    if (!db)
        return;
    tm_free(db->tm);
    print_trace("Transaction manager freed");
    db->tm = NULL;
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
    print_trace("hashmap freed");
    file_manager_close(db->fm);
    print_trace("File manager freed");
    db->fm = NULL;
    free(db->name);
    print_trace("name freed");
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
    page_t* page = db->fm->get_page(db->fm, DATABASE_META_PAGE);
    int writed = 0;
    char* buffer = calloc(32, sizeof(char));
    int len = snprintf(buffer, sizeof(buffer), "%s", db->name);
    buffer[len] = COL_SEPARATOR;
    write_in_page(page, buffer, len + 1, 0);
    writed += len + 1;
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
    write_in_page(page, buffer, len + 1, writed);
    writed += len + 1;
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", db->tables->capacity);
    buffer[len] = COL_SEPARATOR;
    write_in_page(page, buffer, len + 1, writed);
    writed += len + 1;
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", db->table_end);
    buffer[len] = VAL_SEPARATOR;
    write_in_page(page, buffer, len + 1, writed);
    writed += len + 1;
    free(buffer);
    realease_page(db->fm, page);
}

void db_save(Database* db)
{
    db_save_meta(db);
    save_tables_data(db);
    print_trace("Database saved");
}

void db_load_metadata(Database* db)
{
    db->table_count = -1;
    page_t* page = db->fm->get_page(db->fm, DATABASE_META_PAGE);
    char* buffer = page->data;
    if (!buffer || buffer[0] == '\0') {
        printf("Database is empty\n");
        realease_page(db->fm, page);
        return;
    }
    char* first_line = malloc(page->offset + 1);
    memcpy(first_line, buffer, page->offset);
    first_line[page->offset] = '\0';
    printf("line : %s", first_line);
    realease_page(db->fm, page);

    char* saved_name = NULL;
    char delim[2] = { COL_SEPARATOR, '\0' };
    char* token = strtok(first_line, delim);
    for (uint j = 0; token != NULL; j++) {
        printf("token : %s\n", token);

        if (j == 0) {
            saved_name = strdup(token);
        }

        if (j == 1) {
            db->table_count = atoi(token);
        }
        if (j == 2) {
            db->tables->capacity = atoi(token);
        }
        if (j == 3) {
            db->table_end = atoi(token);
            break;
        }
        token = strtok(NULL, delim);
        printf("new token : %s\n", token);
    }
    db->name = strdup(saved_name);
    free(saved_name);
    free(first_line);
    printf("Database %s loaded with %d tables\n", db->name, db->table_count);
}

void load_tables_metadata(Database* db)
{
    page_t* page = db->fm->get_page(db->fm, TABLE_META_PAGE);
    print_debug("Loading tables metadata");
    if (page->offset >= PAGE_SIZE - 1) {
        printf("Invalid page offset\n");
        realease_page(db->fm, page);
        return;
    }
    char* buffer = malloc(page->offset + 1);
    memcpy(buffer, page->data, page->offset);
    if (!buffer || buffer[0] == '\0') {
        printf("No tables found, starting with empty database\n");
        realease_page(db->fm, page);
        return;
    }
    buffer[page->offset] = '\0';

    char delim[2] = { VAL_SEPARATOR, '\0' };
    char** tokens = split(buffer, delim);
    // char* token = strtok(buffer, delim);
    char* token = tokens[0];
    for (int i = 0; i < db->table_count && token != NULL; i++) {
        token = tokens[i];
        printf("\ntoken = %s\n", token);
        TableMeta* meta = table_meta_from_string(token);
        if (!meta) {
            printf("Error: failed to parse table metadata\n");
            break;
        }
        print_debug("loaded table metadata");
        hashmap_insert(db->tables, meta->name, meta);
        // token = strtok(NULL, delim);
    }
    free(buffer);
    for (int i = 0; i < db->table_count; i++)
        free(tokens[i]);
    free(tokens);
    realease_page(db->fm, page);
}

void save_tables_data(Database* db)
{
    int offset = 0;
    // clean table meta page
    page_t* page = db->fm->get_page(db->fm, TABLE_META_PAGE);
    memset(page->data, 0, PAGE_SIZE);
    realease_page(db->fm, page);
    for (uint i = 0; i < db->tables->capacity; i++) {
        node* current = db->tables->buckets[i];
        while (current != NULL) {
            TableMeta* meta = (TableMeta*)current->value;
            printf("saving table: %s (key: %s)\n", meta->name, (char*)current->key);
            Table* table = db->tm->get_table(db->tm, meta, db->fm);
            table_save(table, db->fm);
            current = current->next;
        }
    }
    (void)offset;
}
