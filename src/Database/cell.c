#include "Database/cell.h"
#include "base/utils.h"

Cell cell_init_from_string(Type type, char* value)
{
    Cell new_cell;
    new_cell.type = type;
    switch (type) {
    case INT:
        new_cell.data.i = atoi(value);
        break;
    case FLOAT:
        new_cell.data.f = atof(value);
        break;
    case BYTE:
        new_cell.data.b = atoi(value);
        break;
    case CHAR:
        new_cell.data.c = atoi(value);
        break;
    case STRING:
        new_cell.data.s = value;
        break;
    default:
        break;
    }
    return new_cell;
}

void cell_set_value_from_string(Cell* c, Type type, char* value)
{

    if (c->type == STRING && c->data.s != NULL)
        free(c->data.s);
    switch (type) {
    case INT:
        c->data.i = atoi(value);
        break;
    case FLOAT:
        c->data.f = atof(value);
        break;
    case BYTE:
        c->data.b = atoi(value);
        break;
    case CHAR:
        c->data.c = atoi(value);
        break;
    case STRING:
        c->data.s = strdup(value);
        break;
    default:
        break;
    }
    c->type = type;
}

Cell cell_init_void(void)
{
    Cell new_cell;
    new_cell.type = VOID;
    new_cell.data.i = 0;
    return new_cell;
}

void cell_free(Cell* c)
{
    if (!c)
        return;

    if (c->type == STRING && c->data.s != NULL) {
        free(c->data.s);
        return;
    }
}

void* cell_get_value(Cell* c)
{
    if (!c)
        return NULL;

    switch (c->type) {
    case INT:
        return &c->data.i;

    case FLOAT:
        return &c->data.f;

    case BYTE:
        return &c->data.b;

    case CHAR:
        return &c->data.c;

    case STRING:
        return c->data.s;

    case VOID:
    default:
        return NULL;
    }
}

unsigned char* cell_serialize(Cell* cell)
{
    void* value = cell_get_value(cell);
    unsigned char* buffer = NULL;
    if (value == NULL)
        return NULL;

    switch (cell->type) {
    case INT:
        buffer = malloc(sizeof(int) + 1);
        get_bytes(value, sizeof(int), buffer);
        return buffer;
    case FLOAT:
        buffer = malloc(sizeof(float) + 1);
        return malloc(sizeof(float) + 1);
    case BYTE:
        buffer = malloc(sizeof(unsigned char) + 1);
        get_bytes(value, sizeof(unsigned char), buffer);
        return buffer;
    case CHAR:
        buffer = malloc(sizeof(char) + 1);
        *buffer = *(char*)value;
        return buffer;
    case STRING:
        buffer = malloc(strlen(cell->data.s) + 1);
        memcpy(buffer, cell->data.s, strlen(cell->data.s) + 1);
        return buffer;
    case VOID:
        return NULL;
    default:
        return NULL;
    }
}
