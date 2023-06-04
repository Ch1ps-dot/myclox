/**
 * the header contain definition of bytecode 
*/

#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// operator code
typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_FALSE,
    OP_TRUE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_CONSTANT_LONG, // prepared for future implementation
    OP_PRINT,
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_LOOP,
    OP_CALL,
    OP_RETURN,
} OpCode;

// dynamic array of instructions
// typedef struct Chunk
// {
//     int count;     // the amount of elemnet in array
//     int capacity;  // the capacity of dynamic array
//     uint8_t* code; // memory space stores instructions
//     int* lines;
    
//     uint16_t* runlines;   // the map between weight and line number
//                           // higher 8 bits represent line number, 
//                           // lower 8 bits represent instruction number

//     int cnt_runlines;     // the count of run-lines code
//     int cur_line;         // current line
//     ValueArray constants; // constant pool
// } Chunk;

void initChunk (Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk (Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line); // implementation of OP_CONSTANT_LONG
int getLine(Chunk* chunk, int index);
#endif