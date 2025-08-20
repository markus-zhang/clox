#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/* Enumeration of all opcodes */
typedef enum
{
    OP_CONSTANT,
    OP_RETURN,
} Opcode;

/* An array of binary instructions */
typedef struct
{
    int count;
    int capacity;
    /* WARNING: This is different from implementation in 14.6  */
    int line;
    int pos;
    uint8_t* code;
    ValueArray constants;
} Chunk;

/* Initialization of the Chunk */
void initChunk(Chunk* chunk);

/*
    If the capacity is 0, initialize some capacities;
    If there is enough capacity, insert

    I also add offset which is not in the book
*/
void writeChunk(Chunk* chunk, uint8_t byte, int line, int pos);

/* Convenient function exposed to users to write a constant */
int addConstant(Chunk* chunk, Value value);

/*
    Free dynamic array chunk->code and re-initialize chunk
*/
void freeChunk(Chunk* chunk);

#endif