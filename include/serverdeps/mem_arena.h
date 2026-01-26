#pragma once

#define KB(n) (n << 10)
#define MB(n) (n << 20)
#define GB(n) (n << 30)

#define ALIGNMENT 64

#define make(T, size, arena) ((T*)(mem_arena_alloc(arena, sizeof(T) * size)))
#define release(arena) (mem_arena_free(arena))

#include <stdlib.h>

typedef struct {
    void* base;
    int commited;
    int capacity;
    int offset;
} mem_arena;

mem_arena mem_arena_create(int capacity);
void mem_arena_free(mem_arena* arena);
void* mem_arena_alloc(mem_arena* arena, int size);
