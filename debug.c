#include <stdio.h>

#include "debug.h"
#include "value.h"

// disassemble chunk
void
disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        // advance offset
        offset = disassembleInstruction(chunk, offset);
    }
}

// display constant instruction
static int 
constantInstruction(const char* name, Chunk* chunk,
                               int offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

// display the simple instruction
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
    if (offset > 0 &&
        chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction){
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        
        default:
            printf("Unkown opcode %d\n", instruction);
            return offset + 1;
    }
}
