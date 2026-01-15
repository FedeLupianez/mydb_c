#include "../../include/DB/table.h"
#include <stdio.h>

Table table_init(char* name, char** columns)
{
    Table new_table;
    new_table.arena = arena_create(KB(1));
    new_table.rows = NULL;
    new_table.size = 0;
    new_table.name = name;
    new_table.columns = columns;
    printf("Table %s created\n", new_table.name);
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
        table->rows = (Row*)arena_alloc(table->arena, sizeof(Row) * TABLE_SIZE);
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
    free(table->arena);
}
