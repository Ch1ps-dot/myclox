#include <stdio.h>

#include "debug.h"

// disassemble chunk
void
disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        // advance offset
        offset = disassembleInstruction(chunk, offset);
    }
}

// display the instruction and advance offset
static int
simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

// disassemble instruction 
int
disassembleInstruction(Chunk* chunk, int offset) {
    // display offset of the instruction
    printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction)
    {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    
    default:
        printf("Unkown opcode %d\n", instruction);
        return offset + 1;
    }
}
