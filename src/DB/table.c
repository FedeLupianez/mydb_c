#include "../../include/DB/table.h"
#include "../../include/base/utils.h"
#include <stdio.h>
#include <string.h>

TYPE get_type(char* name)
{
    if (strcmp(name, "int") == 0) {
        return INT_t;
    }
    if (strcmp(name, "float") == 0) {
        return FLOAT_t;
    }
    if (strcmp(name, "char") == 0) {
        return CHAR_t;
    }
    if (strcmp(name, "string") == 0) {
        return STRING_t;
    }
    return INT_t;
}
char* get_type_name(TYPE type)
{
    if (type == INT_t) {
        return "int";
    }
    if (type == FLOAT_t) {
        return "float";
    }
    if (type == CHAR_t) {
        return "char";
    }
    if (type == STRING_t) {
        return "string";
    }
    return "int";
}

Table table_init(char* name, char** columns)
{
    Table new_table;
    new_table.arena = mem_arena_create(KB(1));
    new_table.rows = NULL;
    new_table.size = 0;
    new_table.name = name;

    unsigned int i = 0;
    while (columns[i] != NULL)
        i++;
    new_table.columns = (column*)mem_arena_alloc(&new_table.arena, sizeof(column) * i);
    new_table.cols_len = i;
    printf("%d\n", new_table.cols_len);

    for (unsigned int j = 0; j < new_table.cols_len; j++) {
        char** tmp = split(columns[j], " ");
        printf("%s is type %s\n", tmp[0], tmp[1]);
        new_table.columns[j] = (column) { strdup(tmp[0]), get_type(tmp[1]) };
        free(tmp);
    }
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
        return;
    }

    if (table->size == TABLE_SIZE) {
        printf("Table full\n");
        return;
    }
    table->rows[table->size++] = *row;
}

char* table_describe(Table* table, mem_arena* arena)
{
    char* desc = (char*)mem_arena_alloc(arena, 32 * table->cols_len + 1);
    memset(desc, 0, 32 * table->cols_len + 1);
    strcpy(desc, table->name);
    printf("Table %s : \n", table->name);
    printf("Columns: \n");
    printf("%d\n", table->cols_len);
    for (unsigned int i = 0; i < table->cols_len; i++) {
        printf("%s : %d\n", table->columns[i].name, table->columns[i].type);
        char* col_type = get_type_name(table->columns[i].type);
        strcat(desc, "\n");
        strcat(desc, table->columns[i].name);
        strcat(desc, " : ");
        strcat(desc, col_type);
    }
    return desc;
}

void table_free(Table* table)
{
    for (unsigned int i = 0; i < table->size; i++) {
        row_free(&table->rows[i]);
    }
    for (unsigned int i = 0; i < table->cols_len; i++) {
        free(table->columns[i].name);
    }
    free(table->columns);
    mem_arena_free(&table->arena);
}
