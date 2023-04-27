// the module define bytecode

#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// operator code
typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN,
} OpCode;

// dynamic array of instructions
typedef struct
{
    int count;     // the amount of elemnet in array
    int capacity;  // the capacity of dynamic array
    uint8_t* code; // memory space stores instructions
    int* lines;
    
    uint16_t* runlines;   // the map between weight and line number
                          // higher 8 bits represent line number, 
                          // lower 8 bits represent instruction number

    int cnt_runlines;     // the count of run-lines code
    int cur_line;         // current line
    ValueArray constants; // constant pool
} Chunk;

void initChunk (Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk (Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line); // implementation of OP_CONSTANT_LONG
int getLine(Chunk* chunk, int index);
#endif