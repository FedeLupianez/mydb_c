#include "Database/table.h"
#include "Database/row.h"
#include "Database/transactionmanager.h"
#include "base/utils.h"
#include "printing.h"
#include <stdio.h>

TableMeta* table_meta_init(char* name, char** columns)
{
    TableMeta* meta = malloc(sizeof(TableMeta));
    strncpy(meta->name, name, sizeof(meta->name) - 1);
    meta->name[sizeof(meta->name) - 1] = COL_SEPARATOR;
    meta->rows_count = 0;
    meta->root_page = TABLE_META_PAGE;
    meta->root_page_offset = 0;
    meta->root_row_offset = 0;
    meta->last_row_offset = 0;
    meta->columns_count = len_list(columns);
    meta->columns = (Column*)malloc(sizeof(Column) * meta->columns_count);

    for (uint j = 0; j < meta->columns_count; j++) {
        char** tmp = split(columns[j], " ");
        if (len_list(tmp) != 2)
            return NULL;
        Column col = (Column) {
            .type = get_type(tmp[1]),
            .size = 0,
            .offset = 0
        };
        strncpy(col.name, tmp[0], sizeof(col.name) - 1);
        col.name[sizeof(col.name) - 1] = '\0';
        col.type = get_type(tmp[1]);
        col.size = 0;
        col.offset = 0;
        meta->columns[j] = col;
        uint tmp_len = len_list(tmp);
        for (uint k = 0; k < tmp_len; k++)
            free(tmp[k]);
        free(tmp);
    }

    return meta;
}

TableMeta* table_meta_from_string(char* buffer)
{
    TableMeta* meta = malloc(sizeof(TableMeta));
    meta->root_row_offset = 0;
    meta->last_row_offset = 0;
    meta->root_page = TABLE_META_PAGE;
    meta->rows_count = 0;
    meta->root_page_offset = 0;
    meta->columns = NULL;
    char* copy = strdup(buffer);
    char* token = strtok(copy, (char[]) { COL_SEPARATOR, '\0' });
    char* saved_name = NULL;
    int col_count = 0;
    for (int i = 0; token != NULL; i++) {
        if (i == 0) {
            saved_name = strdup(token);
        }
        if (i == 1) {
            meta->rows_count = atoi(token);
        }
        if (i == 2) {
            meta->columns_count = atoi(token);
            if (meta->columns_count > 0) {
                meta->columns = malloc(sizeof(Column) * meta->columns_count);
                for (uint j = 0; j < meta->columns_count; j++) {
                    meta->columns[j].name[0] = '\0';
                    meta->columns[j].type = 0;
                    meta->columns[j].size = 0;
                    meta->columns[j].offset = 0;
                }
            }
        }
        if (i == 3) {
            meta->root_row_offset = atoi(token);
        }
        if (i == 4) {
            meta->last_row_offset = atoi(token);
        }
        if (i == 5) {
            meta->root_page = atoi(token);
        }
        if (i > 5 && col_count < (int)meta->columns_count) {
            if (col_count == 0) {
                strncpy(meta->columns[col_count].name, token, sizeof(meta->columns[col_count].name) - 1);
                meta->columns[col_count].name[sizeof(meta->columns[col_count].name) - 1] = '\0';
            } else if (col_count == 1) {
                meta->columns[col_count - 1].type = get_type(token);
            } else if (col_count == 2) {
                strncpy(meta->columns[col_count - 1].name, token, sizeof(meta->columns[col_count - 1].name) - 1);
                meta->columns[col_count - 1].name[sizeof(meta->columns[col_count - 1].name) - 1] = '\0';
            } else if (col_count == 3) {
                meta->columns[col_count - 2].type = get_type(token);
            }
            col_count++;
        }

        token = strtok(NULL, (char[]) { COL_SEPARATOR, '\0' });
    }
    strncpy(meta->name, saved_name, sizeof(meta->name) - 1);
    meta->name[sizeof(meta->name) - 1] = '\0';
    free(saved_name);
    free(copy);
    return meta;
}

Table* table_init(TableMeta* meta)
{
    Table* new_table = malloc(sizeof(Table));
    if (!new_table)
        return NULL;
    new_table->meta = meta;
    new_table->arena = mem_arena_create(KB(1));
    new_table->rows = NULL;
    return new_table;
}

void table_add_row(Table* table, Row* row)
{
    if (table == NULL) {
        printf("Invalid Table\n");
        return;
    }
    if (row == NULL) {
        printf("Invalid Row\n");
        return;
    }

    if (table->rows == NULL) {
        table->rows = (Row*)mem_arena_alloc(&table->arena, sizeof(Row) * TABLE_SIZE);
    }

    if (table->meta->rows_count == TABLE_SIZE) {
        printf("Table full\n");
        return;
    }
    memcpy(&table->rows[table->meta->rows_count], row, sizeof(Row));
    table->meta->rows_count++;
}

char* table_describe(Table* table, mem_arena* arena)
{
    char* desc = (char*)mem_arena_alloc(arena, 32 * table->meta->columns_count + 1);
    memset(desc, 0, 32 * table->meta->columns_count + 1);
    strcpy(desc, table->meta->name);
    printf("Table %s : \n", table->meta->name);
    printf("Columns: \n");
    printf("%d\n", table->meta->columns_count);
    for (uint i = 0; i < table->meta->columns_count; i++) {
        printf("%s : %d\n", table->meta->columns[i].name, table->meta->columns[i].type);
        char* col_type = get_type_name(table->meta->columns[i].type);
        strcat(desc, "\n");
        strcat(desc, table->meta->columns[i].name);
        strcat(desc, " : ");
        strcat(desc, col_type);
    }
    return desc;
}

void table_free(Table* table)
{
    for (uint i = 0; i < table->meta->rows_count; i++) {
        row_free(&table->rows[i]);
        print_trace("row freed");
    }
    mem_arena_free(&table->arena);
    free(table);
}

Row get_row_columns(Table* table, Row* row, char** columns)
{
    if (columns == NULL)
        return *row;
    uint cols_len = len_list(columns);
    Row new_row = row_init(row->id, cols_len);
    for (uint i = 0; i < cols_len; i++) {
        for (uint j = 0; j < table->meta->columns_count; j++) {
            if (strcmp(columns[i], table->meta->columns[j].name) == 0) {
                new_row.cells[i] = row->cells[j];
                break;
            }
        }
    }
    return new_row;
}

void table_save_meta(TableMeta* meta, FileManager* filemanager)
{
    int writed = 0;
    char* buffer = calloc(32, sizeof(char));
    int len = snprintf(buffer, sizeof(buffer), "%s", meta->name);
    buffer[len] = COL_SEPARATOR;
    writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = snprintf(buffer, sizeof(buffer), "%d", meta->rows_count);
    buffer[len] = COL_SEPARATOR;
    writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", meta->columns_count);
    buffer[len] = COL_SEPARATOR;
    writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", meta->root_row_offset);
    buffer[len] = COL_SEPARATOR;
    writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
    free(buffer);

    buffer = calloc(32, sizeof(char));
    len = sprintf(buffer, "%d", meta->last_row_offset);
    buffer[len] = COL_SEPARATOR;
    writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
    free(buffer);
    for (uint i = 0; i < meta->columns_count; i++) {
        buffer = calloc(32, sizeof(char));
        len = snprintf(buffer, sizeof(buffer), "%s", meta->columns[i].name);
        buffer[len] = COL_SEPARATOR;
        writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
        free(buffer);

        char* type = get_type_name(meta->columns[i].type);
        buffer = calloc(32, sizeof(char));
        len = snprintf(buffer, sizeof(buffer), "%s", type);
        buffer[len] = (i == meta->columns_count - 1) ? VAL_SEPARATOR : COL_SEPARATOR;
        writed += filemanager->write_in(filemanager, buffer, len + 1, meta->root_page, meta->root_page_offset + writed);
        free(buffer);
    }
}

void table_save_rows(Table* table, FileManager* filemanager)
{
    const int buffer_size = 32 * table->meta->rows_count;
    int writed = 0;
    for (uint i = 0; i < table->meta->rows_count; i++) {
        char* buffer = calloc(32, sizeof(char));
        int len = row_to_save_format(&table->rows[i], buffer, buffer_size);
        writed += filemanager->write_in(filemanager, buffer, len + 1, table->meta->root_page, table->meta->root_row_offset + writed);
        if (buffer != NULL)
            free(buffer);
    }
}

void table_save(Table* table, FileManager* filemanager)
{
    table_save_meta(table->meta, filemanager);
    table_save_rows(table, filemanager);
}
