
#pragma once

#include "cell.h"

typedef struct {
    unsigned int id;
    unsigned int cells_count;
    Cell* cells;
} Row;

void alloc_cells(Row* row);
Row row_init(int id, int size);
void row_free(Row* row);
void row_set_cell(Row* row, void* value, Type type, int index);
void row_add_cell(Row* row, void* value, Type type);
void row_print(Row* row);
