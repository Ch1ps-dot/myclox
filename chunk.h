// the module define bytecode

#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"

// operator code
typedef enum {
    OP_RETURN,
} OpCode;

// dynamic array of instructions
typedef struct
{
    int count;     // the amount of elemnet in array
    int capacity;  // the capacity of dynamic array
    uint8_t* code; // memory space stores code
} Chunk;

void initChunk (Chunk* chunk);
void writeChunk (Chunk* chunk, uint8_t byte);
void freeChunk (Chunk* chunk);
#endif