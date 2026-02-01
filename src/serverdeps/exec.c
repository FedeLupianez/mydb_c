#include "../../include/serverdeps/exec.h"
#include <stdio.h>

Response execute(Database* db, char* input, mem_arena* exec_arena)
{
    char** tokens = tokenize_arena(input, exec_arena);
    int i = 0;
    while (tokens[i] != NULL) {
        printf("token %d: %s\n", i, tokens[i]);
        i++;
    }
    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "hello")) {
        return (Response) { "hello from the database\n", 200 };
    }

    if (EQUAL(tokens[0], "hola")) {
        Table* table = hashmap_get(&db->tables, tokens[0]);
        printf("Listing %s\n", table->name);
        return (Response) { "hello from the database\n", 200 };
    }

    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args\n", 400 };
        }

        if (EQUAL(tokens[1], "table")) {
            char* name = tokens[2];
            printf("Table name : %s\n", name);
            char* cols = tokens[3];
            cols++;
            size_t len = strlen(cols);
            char* cols_copy = strndup(cols, len - 1);
            char** columns = split_arena(cols_copy, ",", exec_arena);
            free(cols_copy);
            printf("Table name 2 : %s\n", name);
            Table* table = db_add_table(db, name, columns);
            printf("Created table %s\n", table->name);
            return (Response) { "Table created\n", 200 };
        }

        if (EQUAL(tokens[1], "row")) {
            Row row = row_init((int)(db)->size, 10);
            Table* table = hashmap_get(&db->tables, tokens[2]);
            table_add_row(table, &row);
            return (Response) { "Row created\n", 200 };
        }

        if (EQUAL(tokens[1], "cell")) {
            cell new_cell = cell_init(VOID);
            Table* table = hashmap_get(&(db)->tables, tokens[2]);
            row_add_cell(&table->rows[table->size - 1], new_cell);
            return (Response) { "Cell created\n", 200 };
        }
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
        if (EQUAL(tokens[1], "row")) {
            Table* table = hashmap_get(&db->tables, tokens[2]);
            row_free(&table->rows[table->size - 1]);
            return (Response) { "Row freed\n", 200 };
        }
    }

    if (EQUAL(tokens[0], "list")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args\n", 400 };
        }
        if (EQUAL(tokens[1], "table")) {
            Table* table = hashmap_get(&db->tables, tokens[2]);
            if (table == NULL)
                return (Response) { "Table not found", 404 };
            table_print(table);
            return (Response) { "Table listed\n", 200 };
        }
        if (EQUAL(tokens[1], "row")) {
            Table* table = hashmap_get(&db->tables, tokens[2]);
            row_print(&table->rows[table->size - 1]);
            return (Response) { "Row listed\n", 200 };
        }
    }
    return (Response) { "Invalid command\n", 400 };
}
