#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[])
{
    Chunk chunk;
    initChunk(&chunk);

    /* 
        Manually insert a constant chunk for testing
        Recall that addConstant returns the index of 1.2 in the array
    */
    int constantIndex = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, constantIndex);

    writeChunk(&chunk, OP_RETURN);
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}