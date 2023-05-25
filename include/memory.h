#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"
#include "object.h"

// allocate heap memory for obj field
#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

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
void freeObjects();

#endif