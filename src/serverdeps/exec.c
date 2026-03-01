#include "serverdeps/exec.h"
#include "base/communication.h"
#include "printing.h"
#include <stdio.h>

Response describe(ServerContext* ctx, char* table_name);
Response create_table(ServerContext* ctx, char** tokens);
Response insert(ServerContext* ctx, char** tokens);
Response select_command(ServerContext* ctx, char** tokens);
Response drop(ServerContext* ctx, char** tokens);
Response delete(ServerContext* ctx, char** tokens);
Response save(ServerContext* ctx, char** tokens);

// main function to execute endpoints
Response execute(ServerContext* ctx, char* input)
{
    char** tokens = tokenize_arena(input, ctx->arena);
    Database* db = ctx->db;

    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "hello")) {
        return ok("hello from the server", pkg_string);
    }

    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL) {
            return invalid_args();
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
    if (EQUAL(tokens[0], "drop")) {
        return drop(ctx, tokens);
    }

    if (EQUAL(tokens[0], "free")) {
        if (tokens[1] == NULL) {
            return invalid_args();
        }
        if (EQUAL(tokens[1], "database")) {
            db_free(db);
            db = NULL;
            return ok("Database freed", pkg_string);
        }
        if (EQUAL(tokens[1], "table")) {
            Table* table = db_get_table(db, tokens[2]);
            table_free(table);
            return ok("Table freed", pkg_string);
        }
    }

    if (EQUAL(tokens[0], "describe")) {
        return describe(ctx, tokens[1]);
    }
    if (EQUAL(tokens[0], "save")) {
        return save(ctx, tokens);
    }
    return invalid_args();
}

// Endpoints
Response describe(ServerContext* ctx, char* table_name)
{
    if (table_name == NULL) {
        return invalid_args();
    }
    Table* table = db_get_table(ctx->db, table_name);
    if (table == NULL)
        return not_found("Table not found");
    char* output = table_describe(table, ctx->arena);
    return ok(output, pkg_string);
}

Response create_table(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL || tokens[3] == NULL) {
        return invalid_args();
    }
    char* name = tokens[2];
    char* cols = tokens[3];
    cols++;
    size_t len = strlen(cols);
    char* cols_copy = strndup(cols, len - 1);
    char** columns = split_arena(cols_copy, ",", ctx->arena);
    free(cols_copy);
    Table* table = db_add_table(ctx->db, name, columns);
    if (!table) {
        print_error("Error creating table");
        return server_error();
    }
    printf("Created table %s\n", table->meta->name);
    return ok("Table created", pkg_string);
}

Response insert(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL || tokens[4] == NULL)
        return invalid_args();

    char* table_name = tokens[2];
    printf("Table name : %s\n", table_name);
    Table* table = db_get_table(ctx->db, table_name);
    printf("Table get flag\n");
    if (!table)
        return not_found("Table not found");
    printf("Table found\n");

    //        0      1       2        3        4
    // ej : insert into table_name values (1, 2, 3, 4, 5)
    char* values = tokens[4];
    values++;
    size_t len = strlen(values);
    char* values_copy = strndup(values, len - 1);
    printf("values copies\n");
    char** values_array = split_arena(values_copy, ",", ctx->arena);
    printf("Values = %s\n", values_array[0]);
    if (values_array == NULL)
        printf("Invalid values\n");
    size_t count = len_list(values_array);
    printf("Count = %lu\n", count);

    if (count != table->meta->columns_count) {
        char* message = "Column/value count mismatch";
        return (Response) { BAD_REQUEST, strlen(message), pkg_string, message };
    }

    printf("Table : %s\n", table->meta->name);

    Row row = row_init(table->meta->rows_count, table->meta->columns_count);
    printf("Row created\n");
    for (uint i = 0; i < table->meta->columns_count; i++) {
        Type col_type = table->meta->columns[i].type;
        Cell* cell = &row.cells[i];
        cell_set_value_from_string(cell, col_type, values_array[i]);
    }
    free(values_copy);
    table_add_row(table, &row);
    row_print(&row);
    return ok("Register created", pkg_string);
}

Response select_command(ServerContext* ctx, char** tokens)
{
    //         0  1   2       3
    // Ej: select * from table_name
    if (tokens[3] == NULL)
        return invalid_args();
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
    Table* table = db_get_table(ctx->db, table_name);
    if (!table)
        return not_found("Table not found");
    printf("Total rows = %d\n", table->meta->rows_count);
    Row* rows = (Row*)mem_arena_alloc(ctx->arena, sizeof(Row) * table->meta->rows_count);
    for (uint j = 0; j < table->meta->rows_count; j++) {
        rows[j] = get_row_columns(table, &table->rows[j], columns);
        row_print(&rows[j]);
    }

    const int string_size = 64;
    const int buffer_size = table->meta->rows_count * string_size;
    if (buffer_size <= 0) {
        return ok("Table is empty", pkg_string);
    }
    char* response_buffer = mem_arena_alloc(ctx->arena, buffer_size);
    uint offset = 0;
    for (uint j = 0; j < table->meta->rows_count; j++) {
        if (row_to_string(&rows[j], response_buffer, buffer_size, &offset) < 0) {
            return server_error();
        }
    }
    return ok(response_buffer, pkg_string);
}

Response drop(ServerContext* ctx, char** tokens)
{
    if (tokens[1] == NULL)
        return invalid_args();
    char* table_name = tokens[1];
    if (db_delete_table(ctx->db, table_name)) {
        printf("Table %s dropped\n", table_name);
        return ok("Table dropped", pkg_string);
    }
    return not_found("Table not found");
}

Response save(ServerContext* ctx, char** tokens)
{
    if (tokens[1] == NULL || tokens[2] == NULL)
        return invalid_args();
    printf("saving");

    if (EQUAL(tokens[1], "table")) {
        char* table_name = tokens[2];
        printf("saving table %s\n", table_name);
        Table* table = db_get_table(ctx->db, table_name);
        if (table == NULL)
            return not_found("Table not found");
        table_save_meta(table->meta, ctx->filemanager);
        return ok("Table saved", pkg_string);
    }
    return invalid_args();
}
