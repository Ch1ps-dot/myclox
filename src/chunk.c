#include <stdlib.h>
#include "chunk.h"
#include "memory.h"
#include "vm.h"

// initChunk and set all arguments as 0
void
initChunk (Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;

    // run-line code implementation
    chunk->runlines = NULL;
    chunk->cnt_runlines = 0;
    chunk->cur_line = 0;

    // init constant pool
    initValueArray(&chunk->constants);
}

// dynamically allocate memory for chunk
void
writeChunk (Chunk* chunk, uint8_t byte, int line) {

    // grow array if lack of memory
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines,
            oldCapacity, chunk->capacity);
    }

    // allocate memory for run-length code to store line information
    if (chunk->cur_line < line) {
        chunk->cur_line = line;
        int oldCapacity = chunk->cnt_runlines;
        chunk->runlines = GROW_ARRAY(uint16_t, chunk->runlines,
            oldCapacity, oldCapacity + 1);
        chunk->cnt_runlines++;
        chunk->runlines[chunk->cnt_runlines-1] =  line << 8;
        chunk->runlines[chunk->cnt_runlines-1]++;
    } else {
        chunk->runlines[chunk->cnt_runlines-1]++;
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

// free the memory of chunk
void
freeChunk (Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    initChunk(chunk);
    freeValueArray(&chunk->constants); // release the memory of constant pool
}

// apply for OP_CONSTANT_LONG, its similar to writeChunk 
void
writeConstant(Chunk* chunk, Value value, int line) {

    // grow array if lack of memory, it requires 3 bytes memory here
    if (chunk->capacity < chunk->count + 3) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines,
            oldCapacity, chunk->capacity);
    }

    // allocate memory for run-length code to store line information
    if (chunk->cur_line < line) {
        chunk->cur_line = line;
        int oldCapacity = chunk->cnt_runlines;
        chunk->runlines = GROW_ARRAY(uint16_t, chunk->runlines,
            oldCapacity, oldCapacity + 1);
        chunk->cnt_runlines++;
        chunk->runlines[chunk->cnt_runlines-1] =  line << 8;
        chunk->runlines[chunk->cnt_runlines-1]++;
    } else {
        chunk->runlines[chunk->cnt_runlines-1]++;
    }

    int constant = addConstant(chunk, value);
    chunk->code[chunk->count] = constant;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

// add contant to contant table
int 
addConstant(Chunk* chunk, Value value) {
    push(value);
    writeValueArray(&chunk->constants, value);
    pop();
    return chunk->constants.count - 1;
}

int 
getLine(Chunk* chunk, int index) {
    int sum = 0;
    int line = 0;
    for (int i = 0; i < chunk->cnt_runlines; i++) {    
        sum += chunk->runlines[i] & 0x00ff;
        if (index <= sum) {
            line = chunk->runlines[i] >> 8;
            break;
        }
    }
    return line;
}