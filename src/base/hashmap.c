#include "base/hashmap.h"
#include "printing.h"
#include <stdint.h>

uint32_t hash(hashmap* map, char* key)
{
    uint32_t sum = 0, factor = 31;
    for (int i = 0; key[i] != '\0'; i++) {
        sum = ((sum % map->capacity)
                  + ((int)key[i] * factor) % map->capacity)
            % map->capacity;
        factor = ((factor % INT16_MAX)
                     * (31 % INT16_MAX))
            % INT16_MAX;
    }
    return sum;
}

void set_node(node* node, char* key, void* value, uint32_t data_size)
{
    node->key = malloc(strlen(key) + 1);
    memcpy(node->key, key, strlen(key) + 1);
    node->value = value;
    node->used = 1;
    node->deleted = 0;
    node->next = NULL;
}

hashmap* hashmap_init(uint32_t size, uint32_t data_size)
{
    hashmap* map = (hashmap*)malloc(sizeof(hashmap));
    map->capacity = size;
    map->buckets = (node**)calloc(size, sizeof(node*));
    map->size = 0;
    map->data_size = data_size;
    return map;
}

void hashmap_insert(hashmap* map, char* key, void* value)
{
    node* entry = (node*)malloc(sizeof(node));
    set_node(entry, key, value, map->data_size);
    uint32_t index = hash(map, key);
    if (map->buckets[index] == NULL)
        map->buckets[index] = entry;
    else {
        entry->next = map->buckets[index];
        map->buckets[index] = entry;
    }
}

uint8_t hashmap_delete(hashmap* map, char* key)
{
    uint32_t index = hash(map, key);
    node* prev = NULL;
    node* current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (current == map->buckets[index]) {
                map->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            map->size--;
            printf("Deleted %s\n", key);
            return 1;
        }
        prev = current;
        current = (node*)current->next;
    }
    printf("Key %s not found\n", key);
    return 0;
}

void* hashmap_get(hashmap* map, char* key)
{
    uint32_t index = hash(map, key);
    node* current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void hashmap_free(hashmap* map)
{
    if (!map)
        return;
    for (uint32_t i = 0; i < map->capacity; i++) {
        node* current = map->buckets[i];
        while (current != NULL) {
            node* next = current->next;
            free(current->key);
            // Note: value is NOT freed here - caller owns the memory
            free(current);
            current = next;
        }
    }
    free(map->buckets);
    free(map);
    print_trace("hashmap freed");
}
