#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdlib.h>

int main(int argc, const char* argv[])
{
    initVM();

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
    writeChunk(&chunk, OP_NEGATE, 22, 0);
    /* Test Binary OpCodes */
    writeChunk(&chunk, OP_ADD, 23, 0);
    writeChunk(&chunk, OP_SUB, 24, 0);
    // This should panic
    // writeChunk(&chunk, OP_MUL, 24, 0);


    // 11 is just a random line number
    writeChunk(&chunk, OP_RETURN, 11, 0);
    // disassembleChunk(&chunk, "test chunk");

    /* Run VM */
    interpret(&chunk);
    // DumpStack(DUMP_CONSOLE);

    /* Dial down all resources */
    freeVM();
    freeChunk(&chunk);

    return 0;
}