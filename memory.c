#include <stdlib.h>

#include "memory.h"

// dynamic memory management
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    // realloc is implementation of malloc.
    // If the memory after the block is unused,
    // realloc return the same pointer. Otherwise,
    // it allocates a new block of memory of the desired size.
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1); 
    return result;
}