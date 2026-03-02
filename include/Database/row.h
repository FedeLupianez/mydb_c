
#pragma once

#include "cell.h"
#define COL_SEPARATOR '|'
#define VAL_SEPARATOR '\n'

typedef struct {
    uint id;
    uint cells_count;
    Cell* cells;
} Row;

void alloc_cells(Row* row);
Row row_init(int id, int size);
int row_to_string(Row* row, char* buffer, uint buffer_size, uint* offset);
void row_free(Row* row);
void row_print(Row* row);
int row_to_save_format(Row* row, char* buffer, uint buffer_size);
