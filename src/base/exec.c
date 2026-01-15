#include "../../include/base/exec.h"
#include <stdio.h>

#define CELLS 10

void response(char* response, int client)
{
    if (client >= 0) {
        send_data(client, response);
        return;
    }
    printf("%s\n", response);
}

void execute(Database* db, char* input, int* client)
{
    strip(input);
    char** tokens = split(input, " ");
    if (db == NULL) {
        printf("Database not initialized\n");
    }

    if (EQUAL(tokens[0], "exit")) {
        printf("Closing client %d\n", *client);
        response("close", *client);
        close(*client);
        *client = -1;
    }
    if (EQUAL(tokens[0], "hello")) {
        response("hello from the database\n", *client);
    }
    if (EQUAL(tokens[0], "create")) {
        if (tokens[1] == NULL) {
            response("Invalid args\n", *client);
            return;
        }

        if (EQUAL(tokens[1], "table")) {
            Table* table = db_add_table(db, tokens[2], NULL);
            printf("Created table %s\n", table->name);
            response("Table created\n", *client);
        }

        if (EQUAL(tokens[1], "row")) {
            Row row = row_init((int)(db)->size, 10);
            table_add_row(&(db)->tables[(db)->size - 1], &row);
            response("Row created\n", *client);
        }

        if (EQUAL(tokens[1], "cell")) {
            cell new_cell = cell_init(VOID);
            Table* table = &(db)->tables[(db)->size - 1];
            row_add_cell(&table->rows[table->size - 1], new_cell);
            response("Cell Created\n", *client);
        }
    }

    if (EQUAL(tokens[0], "free")) {
        if (tokens[1] == NULL) {
            response("Invalid args\n", *client);
        }
        if (EQUAL(tokens[1], "table")) {
            db_free(db);
            db = NULL;
            response("Table freed\n", *client);
        }
        if (EQUAL(tokens[1], "row")) {
            Table* table = &db->tables[db->size - 1];
            row_free(&table->rows[table->size - 1]);
            response("Row freed\n", *client);
        }
    }

    if (EQUAL(tokens[0], "list")) {
        if (tokens[1] == NULL) {
            response("Invalid args\n", *client);
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
    free(tokens);
}
