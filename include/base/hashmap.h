#ifndef HASHMAP_H
#define HASHMAP_H

#include "mem_arena.h"
#include <string.h>
typedef struct {
    uint size;
    uint capacity;
    uint data_size;
    char** keys;
    void** values;
    mem_arena arena;
} hashmap;

uint hash(char* key);
uint murmur_hash(char* key, int seed);

hashmap hashmap_init(uint capacity, uint data_size);
void hashmap_free(hashmap* map);
void hashmap_set(hashmap* map, char* key, void* value);
void* hashmap_get(hashmap* map, char* key);

#endif
