#include "../../include/serverdeps/mem_arena.h"
#include "../../include/base/utils.h"

mem_arena* arena_create(uint size)
{
    mem_arena* arena = (mem_arena*)malloc(size);
    arena->size = size;
    arena->commited = 0;
    arena->offset = 0;
    return arena;
}

void* arena_alloc(mem_arena* arena, uint size)
{
    if (arena->commited + size > arena->size) {
        return NULL;
    }
    arena->commited += size;
    void* ptr = (int*)arena->base + arena->offset;
    arena->offset += size;
    return ptr;
}

void arena_free(mem_arena* arena)
{
    free(arena);
}
