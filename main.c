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
    // int constantIndex = addConstant(&chunk, 1.2);
    // 10 is just a random line number
    // writeChunk(&chunk, OP_CONSTANT, 10, 0);
    // writeChunk(&chunk, constantIndex, 10, 0);

    // NOTE: Test OP_CONSTANT and OP_CONSTANT_LONG
    writeConstant(&chunk, 1.2, 10, 0);
    writeConstant(&chunk, 100.08, 10, 2);
    writeConstant(&chunk, 999, 10, 20);

    // 11 is just a random line number
    writeChunk(&chunk, OP_RETURN, 11, 0);
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}