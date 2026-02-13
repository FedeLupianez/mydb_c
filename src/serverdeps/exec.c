#include "../../include/serverdeps/exec.h"
#include <stdio.h>

Response describe(ServerContext* ctx, char* table_name);
Response create_table(ServerContext* ctx, char** tokens);
Response insert(ServerContext* ctx, char** tokens);

// main function to execute endpoints
Response execute(ServerContext* ctx, char* input)
{
    char** tokens = tokenize_arena(input, ctx->arena);
    Database* db = ctx->db;

    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "hello")) {
        return (Response) { "hello from the database\n", 200 };
    }

    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args\n", 400 };
        }

        if (EQUAL(tokens[1], "table")) {
            return create_table(ctx, tokens);
        }
    }

    if (EQUAL(tokens[0], "insert")) {
        return insert(ctx, tokens);
    }

    if (EQUAL(tokens[0], "free")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args\n", 400 };
        }
        if (EQUAL(tokens[1], "database")) {
            db_free(db);
            db = NULL;
            return (Response) { "Database freed\n", 200 };
        }
        if (EQUAL(tokens[1], "table")) {
            Table* table = hashmap_get(&db->tables, tokens[2]);
            table_free(table);
            return (Response) { "Table freed\n", 200 };
        }
    }

    if (EQUAL(tokens[0], "describe")) {
        return describe(ctx, tokens[1]);
    }
    return (Response) { "Invalid command\n", 400 };
}

// Endpoints
Response describe(ServerContext* ctx, char* table_name)
{
    if (table_name == NULL) {
        return (Response) { "Invalid args\n", 400 };
    }
    Table* table = hashmap_get(&ctx->db->tables, table_name);
    if (table == NULL)
        return (Response) { "Table not found", 404 };
    char* output = table_describe(table, ctx->arena);
    return (Response) { output, 200 };
}

Response create_table(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL || tokens[3] == NULL) {
        return (Response) { "Invalid args\n", 400 };
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
    return (Response) { "Table created\n", 200 };
}

Response insert(ServerContext* ctx, char** tokens)
{
    if (tokens[2] == NULL)
        return (Response) { "Invalid args\n", 400 };

    //        0      1       2        3        4
    // ej : insert into table_name values (1, 2, 3, 4, 5)
    char* values = tokens[4];
    values++;
    size_t len = strlen(values);
    char* values_copy = strndup(values, len - 1);
    char** values_array = split_arena(values_copy, ",", ctx->arena);
    free(values_copy);

    char* table_name = tokens[2];
    Table* table = hashmap_get(&ctx->db->tables, table_name);
    if (!table)
        return (Response) { "Table not found\n", 404 };

    Row row = row_init(table->size, table->cols_len);
    printf("Row size: %d", table->cols_len);
    for (int i = 0; i < table->cols_len; i++) {
        Type col_type = table->columns[i].type;
        Cell* cell = &row.cells[i];
        cell_set_value_from_string(cell, col_type, values_array[i]);
    }
    table_add_row(table, &row);
    row_print(&row);
    return (Response) { "Register created\n", 200 };
}
