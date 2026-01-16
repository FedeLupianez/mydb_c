
#pragma once

#include "cell.h"

typedef struct {
    unsigned int id;
    unsigned int size;
    cell* cells;
} Row;

void alloc_cells(Row* row);
Row row_init(int id, int size);
void row_free(Row* row);
void row_add_cell(Row* row, cell cell);
void row_print(Row* row);
