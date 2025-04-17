#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"

size_t line_allocated = 0;

Arena *arena_create(size_t size) {
    Arena *arena = malloc(sizeof(Arena));
    if (!arena) return NULL;

    arena->memory = malloc(size);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }

    arena->capacity = size;
    arena->used = 0;
    return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
    size = (size + 7) & (size_t)~7;

    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "Arena out of memory!\n");
        exit(EXIT_FAILURE);
    }

    void *ptr = arena->memory + arena->used;
    arena->used += size;
    line_allocated += size;
    return ptr;
}

void arena_destroy(Arena *arena) {
    if (arena) {
        free(arena->memory);
        free(arena);
    }
}

void arena_new_line(void) {
    printf("[arena] allocated %zu bytes for this line\n", line_allocated);
    line_allocated = 0;
}
