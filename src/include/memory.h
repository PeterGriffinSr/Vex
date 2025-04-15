#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
typedef struct Arena {
    char *memory;
    size_t capacity, used;
} Arena;

Arena *arena_create(size_t size);
void *arena_alloc(Arena *arena, size_t size);
void arena_destroy(Arena *arena);

#endif // MEMORY_H
