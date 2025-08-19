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
        case OP_CONSTANT:
        {
            return constantInstruction("OP_CONSTANT", chunk, offset);
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

static int constantInstruction(const char* name, Chunk* chunk, int offset)
{
    /* First byte is for OpCode and the second byte is the index of the constant, thus offset + 1 */
    uint8_t constantIndex = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constantIndex);
    /* Then we need to print the actual value */
    printValue(chunk->constants.values[constantIndex]);
    printf('\n');
    /* 2 byte chunk */
    return offset + 2;
}