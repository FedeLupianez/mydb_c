#ifndef HASHMAP_H
#define HASHMAP_H
#include "commons.h"

typedef struct node node;
struct node {
    uint8_t used : 1;
    uint8_t deleted : 1;
    char* key;
    void* value;
    node* next;
};

typedef struct {
    node** buckets;
    uint32_t capacity, size, data_size;
} hashmap;

uint32_t hash(hashmap* map, char* key);

hashmap* hashmap_init(uint32_t capacity, uint32_t data_size);
void hashmap_free(hashmap* map);
void hashmap_insert(hashmap* map, char* key, void* value);
void* hashmap_get(hashmap* map, char* key);
uint8_t hashmap_delete(hashmap* map, char* key);

#endif
