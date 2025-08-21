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
    /* Print line number and  pos */
    printf("Line %4d - Pos %4d ", chunk->line, chunk->pos);

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
        case OP_CONSTANT_LONG:
        {
            return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
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
    printf("%-16s Index %4d -> '", name, constantIndex);
    /* Then we need to print the actual value */
    printValue(chunk->constants.values[constantIndex]);
    printf("'\n");
    /* 2 byte chunk */
    return offset + 2;
}

static int constantLongInstruction(const char* name, Chunk* chunk, int offset)
{
    /*
        First byte is OpCode and the following three are High/Middle/Low byte of the index
    */
    
    int constantIndex = (chunk->code[offset + 1] << 16) + (chunk->code[offset + 2] << 8) + chunk->code[offset + 3];

    printf("%-16s Index %4d -> '", name, constantIndex);
    printValue(chunk->constants.values[constantIndex]);
    printf("'\n");
    /* 4 byte chunk */
    return offset + 4;
}