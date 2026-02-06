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

Table table_init(char* name, char** columns)
{
    Table new_table;
    new_table.arena = mem_arena_create(KB(1));
    new_table.rows = NULL;
    new_table.size = 0;
    new_table.name = name;

    new_table.cols_len = 0;
    while (columns[new_table.cols_len] != NULL)
        new_table.cols_len++;
    new_table.columns = (column*)mem_arena_alloc(&new_table.arena, sizeof(column) * new_table.cols_len);

    // separar el nombre del tipo de dato
    for (unsigned int j = 0; j < new_table.cols_len; j++) {
        char** tmp = split(columns[j], " ");
        printf("%s is type %s\n", tmp[0], tmp[1]);
        new_table.columns[j].name = strdup(tmp[0]);
        new_table.columns[j].type = get_type(tmp[1]);
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

void table_print(Table* table)
{
    printf("Table %s : \n", table->name);
    for (unsigned int i = 0; i < table->size; i++) {
        row_print(&table->rows[i]);
    }
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
