#include "../../include/DB/row.h"
#include <stdio.h>
#include <stdlib.h>

void alloc_cells(Row* row)
{
    if (row->cells != NULL) {
        for (size_t i = 0; i < row->size; i++) {
            if (row->cells[i].type != VOID) {
                cell_free(&row->cells[i]);
            }
        }
    }
    row->cells = (cell*)(malloc(row->size * sizeof(cell)));
    for (size_t i = 0; i < row->size; i++) {
        row->cells[i] = cell_init(VOID);
    }
}

Row row_init(int id, int size)
{
    Row new_row;
    new_row.id = id;
    new_row.size = size;
    new_row.cells = NULL;
    alloc_cells(&new_row);
    return new_row;
}

void row_free(Row* row)
{
    for (unsigned int i = 0; i < row->size; i++) {
        if (row->cells[i].type != VOID) {
            cell_free(&row->cells[i]);
        }
    }
    row->cells = NULL;
}

void row_add_cell(Row* row, cell c)
{
    if (row->cells == NULL) {
        alloc_cells(row);
    }
    row->cells[row->size] = c;
    row->size++;
}

void row_print(Row* row)
{
    printf("Row %d : \n", row->id);
    if (row->cells == NULL) {
        printf("Empty\n");
        return;
    }
    for (unsigned int i = 0; i < row->size; i++) {
        printf("Cell %d : ", i);
        void* value = cell_get_value(&row->cells[i]);
        printf("%d\n", *(int*)value);
    }
    printf("\n");
}
