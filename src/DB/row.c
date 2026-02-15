#include "../../include/DB/row.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void alloc_cells(Row* row)
{
    if (row->cells != NULL) {
        for (size_t i = 0; i < row->cells_count; i++) {
            if (row->cells[i].type != VOID) {
                cell_free(&row->cells[i]);
            }
        }
    }
    row->cells = (Cell*)(malloc(row->cells_count * sizeof(Cell)));
    for (size_t i = 0; i < row->cells_count; i++) {
        row->cells[i] = cell_init_void();
    }
}

Row row_init(int id, int cells)
{
    Row new_row;
    new_row.id = id;
    new_row.cells_count = cells;
    new_row.cells = NULL;
    alloc_cells(&new_row);
    return new_row;
}

void row_free(Row* row)
{
    for (unsigned int i = 0; i < row->cells_count; i++) {
        if (row->cells[i].type != VOID) {
            cell_free(&row->cells[i]);
        }
    }
    row->cells = NULL;
}

void row_print(Row* row)
{
    if (row->cells == NULL) {
        printf("Empty\n");
        return;
    }
    printf("Row %d : \n", row->id);
    printf("Cells: %d\n", row->cells_count);
    for (unsigned int i = 0; i < row->cells_count; i++) {
        printf("Cell %d : ", i);
        void* value = cell_get_value(&row->cells[i]);
        if (value == NULL) {
            printf("NULL\n");
            continue;
        }

        switch (row->cells[i].type) {
        case INT:
            printf("%d\n", *(int*)value);
            break;
        case FLOAT:
            printf("%f\n", *(float*)value);
            break;
        case STRING:
            printf("%s\n", (char*)value);
            break;
        case VOID:
            printf("NULL\n");
            break;
        case BYTE:
            printf("%d\n", *(unsigned char*)value);
            break;
        case CHAR:
            printf("%c\n", *(char*)value);
            break;
        default:
            printf("Invalid type\n");
        }
    }
    printf("\n");
}

void row_to_string(Row* row, char* buffer)
{
    for (unsigned int i = 0; i < row->cells_count; i++) {
        int is_final = (i == row->cells_count - 1);
        char* ptr = buffer + strlen(buffer);
        switch (row->cells[i].type) {
        case INT: {
            char* format = (is_final) ? " %d" : " %d |";
            sprintf(ptr, format, *(int*)cell_get_value(&row->cells[i]));
            break;
        }
        case FLOAT: {
            char* format = (is_final) ? " %f" : " %f |";
            sprintf(ptr, format, *(float*)cell_get_value(&row->cells[i]));
            break;
        }
        case STRING: {
            char* format = (is_final) ? " %s" : " %s |";
            sprintf(ptr, format, (char*)cell_get_value(&row->cells[i]));
            break;
        }
        case VOID: {
            sprintf(ptr, " NULL |");
            break;
        }
        case BYTE: {
            char* format = (is_final) ? " %d" : " %d |";
            sprintf(ptr, " %d |", *(unsigned char*)cell_get_value(&row->cells[i]));
            break;
        }
        case CHAR: {
            char* format = (is_final) ? " %c" : " %c |";
            sprintf(ptr, format, *(char*)cell_get_value(&row->cells[i]));
            break;
        }
        default:
            sprintf(ptr, " Invalid type |");
        }
    }
}
