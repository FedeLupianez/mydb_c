#pragma once

#define KB(n) (n << 10)
#define MB(n) (n << 20)
#define GB(n) (n << 30)

typedef unsigned int uint;

typedef struct {
    void* base;
    uint size;
    uint commited;
    uint offset;
} mem_arena;

mem_arena* arena_create(uint size);
void* arena_alloc(mem_arena* arena, uint size);
void arena_free(mem_arena* arena);
