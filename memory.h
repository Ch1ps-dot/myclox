#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

// expand capacity when needed extra memory
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 8)

// allocate memory to chunk
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*) reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

// free memory of chunk
#define FREE_ARRAY(type, pointer, oldCount) \
    (type*) reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif