#include "Database/table.h"
#include "base/utils.h"

Table table_init(char* name, char** columns)
{
    Table new_table;
    new_table.arena = mem_arena_create(KB(1));
    new_table.rows = NULL;
    new_table.size = 0;
    new_table.name = (char*)mem_arena_alloc(&new_table.arena, strlen(name) + 1);
    strcpy(new_table.name, name);

    uint cols_count = len_list(columns);
    new_table.columns = (column*)mem_arena_alloc(&new_table.arena, sizeof(column) * cols_count);
    new_table.cols_len = cols_count;
    printf("%d\n", new_table.cols_len);

    for (uint j = 0; j < new_table.cols_len; j++) {
        char** tmp = split(columns[j], " ");
        printf("%s is type %s\n", tmp[0], tmp[1]);
        new_table.columns[j] = (column) { strdup(tmp[0]), get_type(tmp[1]) };
        uint tmp_len = len_list(tmp);
        for (uint k = 0; k < tmp_len; k++)
            free(tmp[k]);
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
        printf("Allocated %d rows\n", TABLE_SIZE);
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
    for (uint i = 0; i < table->cols_len; i++) {
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
    for (uint i = 0; i < table->size; i++) {
        row_free(&table->rows[i]);
    }
    for (uint i = 0; i < table->cols_len; i++) {
        free(table->columns[i].name);
    }
    free(table->columns);
    mem_arena_free(&table->arena);
}

Row get_row_columns(Table* table, Row* row, char** columns)
{
    if (columns == NULL)
        return *row;
    uint cols_len = len_list(columns);
    Row new_row = row_init(row->id, cols_len);
    for (uint i = 0; i < cols_len; i++) {
        for (uint j = 0; j < table->cols_len; j++) {
            if (strcmp(columns[i], table->columns[j].name) == 0) {
                new_row.cells[i] = row->cells[j];
                break;
            }
        }
    }
    return new_row;
}
