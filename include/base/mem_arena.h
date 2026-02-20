#pragma once

#define KB(n) (n << 10)
#define MB(n) (n << 20)
#define GB(n) (n << 30)

#define ALIGNMENT 16

#define alloc(T, size, arena) ((T*)(mem_arena_alloc(arena, sizeof(T) * size)))

#include "commons.h"

typedef struct {
    void* base;
    uint commited;
    uint capacity;
    uint offset;
} mem_arena;

mem_arena mem_arena_create(int capacity);
void mem_arena_free(mem_arena* arena);
void* mem_arena_alloc(mem_arena* arena, int size);
void mem_arena_realloc(mem_arena* arena, int size);
