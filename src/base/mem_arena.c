#include "../../include/base/mem_arena.h"
#include <stdint.h>

uintptr_t mem_arena_align_forward(uintptr_t pointer, int alignment)
{
    uintptr_t ptr, alg, module;
    if (!((pointer + alignment - 1) & (alignment - 1))) {
        return pointer;
    }
    ptr = pointer;
    alg = (uintptr_t)alignment;
    module = ptr & (alg - 1);
    if (module) {
        ptr += alg - module;
    }
    return ptr;
}

void* mem_arena_alloc_aligned(mem_arena* arena, int size)
{
    uintptr_t current_ptr = (uintptr_t)arena->base + (uintptr_t)arena->offset;
    uintptr_t offset = mem_arena_align_forward(current_ptr, ALIGNMENT);
    offset -= (uintptr_t)arena->base;

    if (offset + size > (uintptr_t)arena->capacity) {
        return NULL;
    }

    arena->commited += size;
    void* ptr = (uint8_t*)arena->base + offset;
    arena->offset = offset + size;
    return ptr;
}

void* mem_arena_alloc(mem_arena* arena, int size)
{
    if (!size) {
        return 0;
    }
    return mem_arena_alloc_aligned(arena, size);
}

void mem_arena_realloc(mem_arena* arena, int size)
{
    arena->base = realloc(arena->base, size);
    arena->capacity = size;
}

mem_arena mem_arena_create(int capacity)
{
    mem_arena new_arena;
    new_arena.base = malloc(capacity);
    new_arena.capacity = capacity;
    new_arena.offset = 0;
    new_arena.commited = 0;
    return new_arena;
}

void mem_arena_free(mem_arena* arena)
{
    if (!arena)
        return;
    if (arena->base) {
        volatile unsigned char* ptr = arena->base;
        for (unsigned int i = 0; i < arena->capacity; ++i) {
            ptr[i] = 0;
        }
        free(arena->base);
    }
    arena->base = NULL;
    arena->capacity = 0;
    arena->offset = 0;
    arena->commited = 0;
}
