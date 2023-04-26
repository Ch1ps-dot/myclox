#include <stdlib.h>
#include "chunk.h"
#include "memory.h"

// initChunk and set all arguments as 0
void
initChunk (Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}

// dynamically allocate memory to chunk
void
writeChunk (Chunk* chunk, uint8_t byte) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

// free the memory of chunk
void
freeChunk (Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    initChunk(chunk);
}