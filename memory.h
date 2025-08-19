#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

#define CHUNK_INITIAL_CAPACITY  0x100
#define CHUNK_GROWTH_EXP        0x2

#define GROW_CAPACITY(capacity) \
    (((capacity) < CHUNK_INITIAL_CAPACITY) ? CHUNK_INITIAL_CAPACITY : (CHUNK_INITIAL_CAPACITY * CHUNK_GROWTH_EXP))

#define GROW_ARRAY(type, pointer, oldCapacity, newCapacity) \
    (type*)reallocate(pointer, sizeof(type) * (oldCapacity), sizeof(type) * (newCapacity))

#define FREE_ARRAY(type, pointer, oldCapacity) \
    reallocate(pointer, sizeof(type) * (oldCapacity), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif