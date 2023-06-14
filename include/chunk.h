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
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
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
    OP_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_METHOD,
} OpCode;

/**
 * I moved some code to <common.h>,
 * because of some compiling errors caused by bad structure of header file. 
*/

void initChunk (Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk (Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line); // implementation of OP_CONSTANT_LONG
int getLine(Chunk* chunk, int index);
#endif