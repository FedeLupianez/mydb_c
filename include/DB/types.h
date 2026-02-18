#pragma once
#include <stdint.h>

typedef enum __attribute__((packed)) {
    INT = 0x00,
    BYTE = 0x01,
    FLOAT = 0x02,
    STRING = 0x03,
    CHAR = 0x04,
    VOID = 0x05
} Type;

Type get_type(char* name);
char* get_type_name(Type type);
