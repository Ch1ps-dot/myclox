#include "common.h"
#include "chunk.h"
#include "debug.h"

int
main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 1);
    writeChunk(&chunk, constant, 1);

    writeChunk(&chunk, OP_RETURN, 2);
    writeChunk(&chunk, OP_RETURN, 2);
    writeChunk(&chunk, OP_RETURN, 5);
    disassembleChunk(&chunk, "test chunk");
    
    return 0;
}
