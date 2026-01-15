#include "../../include/DB/cell.h"

cell cell_init(Type type, ...)
{
    cell new_cell;
    new_cell.type = type;
    va_list args;
    va_start(args, type);
    if (type == INT) {
        new_cell.data.i = va_arg(args, int);
    }
    if (type == FLOAT) {
        new_cell.data.f = va_arg(args, double);
    }
    if (type == STR) {
        new_cell.data.s = va_arg(args, char*);
    }
    if (type == BYTE) {
        new_cell.data.b = va_arg(args, int);
    }
    va_end(args);
    return new_cell;
}

void cell_free(cell* c)
{
    if (c == NULL)
        return;

    if (c->type == STR) {
        free(c->data.s);
        return;
    }
}

void* cell_get_value(cell* c)
{
    if (c->type == VOID) {
        return NULL;
    }
    if (c->type == INT) {
        return &c->data.i;
    }
    if (c->type == FLOAT) {
        return &c->data.f;
    }
    if (c->type == STR) {
        return &c->data.s;
    }
    if (c->type == BYTE) {
        return &c->data.b;
    }
    return NULL;
}
