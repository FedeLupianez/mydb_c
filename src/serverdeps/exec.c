#include "../../include/serverdeps/exec.h"
#include <stdio.h>

Response describe(ServerContext* ctx, char* table_name);
Response create_table(ServerContext* ctx, char** tokens);
Response insert(ServerContext* ctx, char** tokens);
Response select_command(ServerContext* ctx, char** tokens);

// main function to execute endpoints
Response execute(ServerContext* ctx, char* input)
{
    char** tokens = tokenize_arena(input, ctx->arena);
    Database* db = ctx->db;

    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "hello")) {
        return (Response) { "hello from the database", OK };
    }

    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args", BAD_REQUEST };
        }

        if (EQUAL(tokens[1], "table")) {
            return create_table(ctx, tokens);
        }
    }

    if (EQUAL(tokens[0], "insert")) {
        return insert(ctx, tokens);
    }

    if (EQUAL(tokens[0], "select")) {
        return select_command(ctx, tokens);
    }

    if (EQUAL(tokens[0], "free")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args", BAD_REQUEST };
        }
        if (EQUAL(tokens[1], "database")) {
            db_free(db);
            db = NULL;
            return (Response) { "Database freed", OK };
        }
        if (EQUAL(tokens[1], "table")) {
            Table* table = hashmap_get(&db->tables, tokens[2]);
            table_free(table);
            return (Response) { "Table freed", OK };
        }
    }

    if (EQUAL(tokens[0], "describe")) {
        return describe(ctx, tokens[1]);
    }
    return (Response) { "Invalid command", BAD_REQUEST };
}

// Endpoints
Response describe(ServerContext* ctx, char* table_name)
{
    if (table_name == NULL) {
        return (Response) { "Invalid args", BAD_REQUEST };
    }
    Table* table = hashmap_get(&ctx->db->tables, table_name);
    if (table == NULL)
        return (Response) { "Table not found", NOT_FOUND };
    char* output = table_describe(table, ctx->arena);
    return (Response) { output, OK };
}

Response create_table(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL || tokens[3] == NULL) {
        return (Response) { "Invalid args", BAD_REQUEST };
    }
    char* name = tokens[2];
    char* cols = tokens[3];
    cols++;
    size_t len = strlen(cols);
    char* cols_copy = strndup(cols, len - 1);
    char** columns = split_arena(cols_copy, ",", ctx->arena);
    free(cols_copy);
    Table table = db_add_table(ctx->db, name, columns);
    printf("Created table %s\n", table.name);
    return (Response) { "Table created\n", OK };
}

Response insert(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL || tokens[4] == NULL)
        return (Response) { "Invalid args", BAD_REQUEST };

    //        0      1       2        3        4
    // ej : insert into table_name values (1, 2, 3, 4, 5)
    char* values = tokens[4];
    values++;
    size_t len = strlen(values);
    char* values_copy = strndup(values, len - 1);
    char** values_array = split_arena(values_copy, ",", ctx->arena);
    if (values_array == NULL)
        printf("Invalid values\n");
    size_t count = len_list(values_array);
    free(values_copy);

    if (count != ctx->db->table_heap->cols_len)
        return (Response) { "Column/value count mismatch", BAD_REQUEST };

    char* table_name = tokens[2];
    Table* table = hashmap_get(&ctx->db->tables, table_name);
    if (!table)
        return (Response) { "Table not found", NOT_FOUND };
    printf("Table : %s\n", table->name);

    Row row = row_init(table->size, table->cols_len);
    for (uint i = 0; i < table->cols_len; i++) {
        Type col_type = table->columns[i].type;
        Cell* cell = &row.cells[i];
        cell_set_value_from_string(cell, col_type, values_array[i]);
    }
    table_add_row(table, &row);
    row_print(&row);
    return (Response) { "Register created", OK };
}

Response select_command(ServerContext* ctx, char** tokens)
{
    //         0  1   2       3
    // Ej: select * from table_name
    if (tokens[3] == NULL)
        return (Response) { "Invalid args", BAD_REQUEST };
    char* table_name = tokens[3];
    char* cols = tokens[1];
    char** columns = NULL;
    if (strcmp(cols, "*") != 0) {
        cols++;
        size_t len = strlen(cols);
        char* cols_copy = strndup(cols, len - 1);
        columns = split_arena(cols_copy, ",", ctx->arena);
        free(cols_copy);
    }
    Table* table = hashmap_get(&ctx->db->tables, table_name);
    if (!table)
        return (Response) { "Table not found", NOT_FOUND };
    printf("Total rows = %d\n", table->size);
    Row* rows = (Row*)mem_arena_alloc(ctx->arena, sizeof(Row) * table->size);
    for (uint j = 0; j < table->size; j++) {
        rows[j] = get_row_columns(table, &table->rows[j], columns);
        row_print(&rows[j]);
    }

    const int string_size = 64;
    const int buffer_size = table->size * string_size;
    if (buffer_size <= 0) {
        return (Response) { "Table is empty", OK };
    }
    char* response_buffer = mem_arena_alloc(ctx->arena, buffer_size);
    uint offset = 0;
    for (uint j = 0; j < table->size; j++) {
        if (row_to_string(&rows[j], response_buffer, buffer_size, &offset) < 0) {
            return (Response) { "Error", SERVER_ERROR };
        }
    }
    return (Response) { response_buffer, OK };
}
