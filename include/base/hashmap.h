#ifndef HASHMAP_H
#define HASHMAP_H

#include "mem_arena.h"
#include <string.h>
typedef struct {
    int size;
    int capacity;
    int data_size;
    char** keys;
    void** values;
    mem_arena arena;
} hashmap;

unsigned int hash(char* key);

hashmap hashmap_init(int capacity, int data_size);
void hashmap_free(hashmap* map);
void hashmap_set(hashmap* map, char* key, void* value);
void* hashmap_get(hashmap* map, char* key);

#endif
