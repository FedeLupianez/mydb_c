#include "../../include/DB/row.h"
#include <stdio.h>
#include <stdlib.h>

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

void row_add_cell(Row* row, void* value, Type type)
{
    if (row->cells == NULL) {
        alloc_cells(row);
    }
    Cell new_cell = cell_init_void();
    row->cells[row->cells_count] = new_cell;
    row->cells_count++;
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
