#include "../../include/Database/types.h"
#include <string.h>

Type get_type(char* name)
{
    if (strcmp(name, "int") == 0) {
        return INT;
    }
    if (strcmp(name, "float") == 0) {
        return FLOAT;
    }
    if (strcmp(name, "char") == 0) {
        return CHAR;
    }
    if (strcmp(name, "string") == 0) {
        return STRING;
    }
    if (strcmp(name, "byte") == 0) {
        return BYTE;
    }
    if (strcmp(name, "void") == 0) {
        return VOID;
    }
    return INT;
}
char* get_type_name(Type type)
{
    if (type == INT) {
        return "int";
    }
    if (type == FLOAT) {
        return "float";
    }
    if (type == CHAR) {
        return "char";
    }
    if (type == BYTE) {
        return "byte";
    }
    if (type == VOID) {
        return "void";
    }
    if (type == STRING) {
        return "string";
    }
    return "int";
}
