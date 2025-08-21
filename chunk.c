#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "memory.h"

/* Initialization of the Chunk */
void initChunk(Chunk* chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->line = 0;
    chunk->pos = 0;
    chunk->code = NULL;
    initValueArray(&(chunk->constants));
}

/*
    If the capacity is 0, initialize some capacities;
    If there is enough capacity, insert
    If capacity is not enough, grow
*/
void writeChunk(Chunk* chunk, uint8_t byte, int line, int pos)
{
    if (chunk->count == chunk->capacity)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->line = line;
    chunk->pos = pos;
    chunk->count++;
}

void writeConstant(Chunk* chunk, Value value, int line, int pos)
{
    /*
        Write OP_CONSTANT + constantIndex into chunk, if constantIndex <= 0xFF, or OP_CONSTANT_LONG + constantIndex into chunk, if constantIndex > 0xFF && constantIndex <= 0xFFFFFF.

        Panic if constantIndex > 0xFFFFFF
    */
    
    int constantIndex = addConstant(chunk, value);
    // FIXME: change to 0xFF
    if (constantIndex <= 0x01)
    {
        /* We only need OP_CONSTANT */
        writeChunk(chunk, OP_CONSTANT, line, pos);
        writeChunk(chunk, constantIndex, line, pos);
    }
    else if (constantIndex <= 0xFFFFFF)
    {
        /* We need OP_CONSTANT_LONG */
        writeChunk(chunk, OP_CONSTANT_LONG, line, pos);
        // TODO: Write 3 bytes one by one, Top byte - Middle byte - Low byte for now
        writeChunk(chunk, (uint8_t)(constantIndex >> 16), line, pos);
        writeChunk(chunk, (uint8_t)(constantIndex >> 8), line, pos);
        writeChunk(chunk, (uint8_t)(constantIndex >> 0), line, pos);
    }
}

/*
    Also return the index of the Value for easy indexing
*/
int addConstant(Chunk* chunk, Value value)
{
    writeValueArray(&(chunk->constants), value);
    return (chunk->constants.count - 1);
}



void freeChunk(Chunk* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeValueArray(&(chunk->constants));
    initChunk(chunk);
}