#include <stdio.h>
#include "debug.h"

/*
    We use disassembleInstruction() to move offset,
    because instructions have different sizes
*/
void disassembleChunk(Chunk* chunk, const char* name)
{
    printf("Name of chunk: %s\n", name);

    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(Chunk* chunk, int offset)
{
    printf("Offset -> %04d ", offset);
    uint8_t instr = chunk->code[offset];

    switch(instr)
    {
        case OP_RETURN:
        {
            return simpleInstruction("OP_RETURN", offset);
        }
        default:
        {
            printf("Unknown opcode %d\n", instr);
            return offset + 1;
        }
    }
}

static int simpleInstruction(const char* name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}