#include "../../include/base/hashmap.h"
#include <string.h>

unsigned int hash(char* key)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

hashmap hashmap_init(int capacity, int data_size)
{
    hashmap map;
    map.size = 0;
    map.capacity = capacity;
    map.data_size = data_size;
    int avg_key_size = 32;
    size_t total_mem = ((sizeof(char*) * capacity) + (data_size * capacity) + (avg_key_size * capacity));
    map.arena = mem_arena_create(total_mem);
    map.keys = (char**)mem_arena_alloc(&map.arena, sizeof(char*) * capacity);
    map.values = mem_arena_alloc(&map.arena, data_size * capacity);
    if (!map.keys || !map.values) {
        map.capacity = 0;
        return map;
    }
    for (int i = 0; i < capacity; i++) {
        map.keys[i] = NULL;
        memset((char*)map.values + (i * data_size), 0, data_size);
    }
    return map;
}

void hashmap_free(hashmap* map)
{
    if (map) {
        mem_arena_free(&map->arena);
        map->keys = NULL;
        map->values = NULL;
        map->capacity = 0;
        map->size = 0;
    }
}

static int find_index(hashmap* map, char* key, int for_insert)
{
    if (map->capacity == 0 || !key)
        return -1;
    unsigned int hash_key = hash(key);
    int index = hash_key % map->capacity;
    int start = index;
    if (for_insert) {
        do {
            if (map->keys[index] == NULL || strcmp(map->keys[index], key) == 0)
                return index;
            index = (index + 1) % map->capacity;
        } while (index != start);
        return -1;
    } else {
        do {
            if (map->keys[index] == NULL)
                return -1;
            if (strcmp(map->keys[index], key) == 0)
                return index;
            index = (index + 1) % map->capacity;
        } while (index != start);
        return -1;
    }
}

void hashmap_set(hashmap* map, char* key, void* value)
{
    if (map->size >= map->capacity || !key || !value)
        return;
    int index = find_index(map, key, 1);
    if (index == -1)
        return;
    if (map->keys[index] == NULL) {
        size_t key_len = strlen(key);
        char* key_copy = (char*)mem_arena_alloc(&map->arena, key_len + 1);
        if (!key_copy)
            return;
        strcpy(key_copy, key);
        map->keys[index] = key_copy;
        map->size++;
    }
    void* dest = (char*)map->values + (index * map->data_size);
    memcpy(dest, value, map->data_size);
}

void* hashmap_get(hashmap* map, char* key)
{
    int index = find_index(map, key, 0);
    if (index == -1)
        return NULL;
    return (char*)map->values + (index * map->data_size);
}
