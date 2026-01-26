#include "../../include/serverdeps/exec.h"
#include <stdio.h>

#define CELLS 10
#define ARENA_SECTIONS 5

Response execute(Database* db, char* input)
{
    mem_arena exec_arena = mem_arena_create(KB(1));
    char** tokens = make(char*, 10, &exec_arena);
    *tokens = *tokenize(input);
    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "hello")) {
        return (Response) { "hello from the database\n", 200 };
    }
    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL)
            return (Response) { "Invalid args\n", 400 };

        if (EQUAL(tokens[1], "table")) {
            char* cols = tokens[3];
            cols++;
            cols[strlen(cols) - 1] = '\0';
            char** columns = mem_arena_alloc(&exec_arena, sizeof(char*) * 10);
            *columns = *split(cols, ",");
            Table* table = db_add_table(db, tokens[2], columns);
            printf("Created table %s\n", table->name);
            char* response_str;
            asprintf(&response_str, "Table %s created\n", table->name);
            table = NULL;
            return (Response) { response_str, 200 };
        }

        if (EQUAL(tokens[1], "row")) {
            Row row = row_init((int)(db)->size, 10);
            table_add_row(&(db)->tables[(db)->size - 1], &row);
            return (Response) { "Row created\n", 200 };
        }

        if (EQUAL(tokens[1], "cell")) {
            cell new_cell = cell_init(VOID);
            Table* table = &(db)->tables[(db)->size - 1];
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
            Table* table = &db->tables[db->size - 1];
            row_free(&table->rows[table->size - 1]);
            return (Response) { "Row freed\n", 200 };
        }
    }

    if (EQUAL(tokens[0], "list")) {
        if (tokens[1] == NULL) {
            return (Response) { "Invalid args\n", 400 };
        }
        if (EQUAL(tokens[1], "table")) {
            table_print(&db->tables[db->size - 1]);
        }
        if (EQUAL(tokens[1], "row")) {
            Table* table = &db->tables[db->size - 1];
            row_print(&table->rows[table->size - 1]);
        }
    }

    if (EQUAL(tokens[0], "clear")) {
        system("clear");
    }
    release(&exec_arena);
    return (Response) { "Invalid command\n", 400 };
}
