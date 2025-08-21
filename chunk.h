#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/* Enumeration of all opcodes */
typedef enum
{
    /* 1 byte OpCode + 1 byte OpRand -> Maxium 0xFF entries */
    OP_CONSTANT,
    /* 
        See Challenge 2 - 24-bit number -> Maximum 0xFFFFFF entries 
        FIXME: We will use big endian for now but may change later:
        OP_CONSTANT_LONG - Top byte - Middle byte - Low byte
    */
    OP_CONSTANT_LONG,
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

void writeConstant(Chunk* chunk, Value value, int line, int pos);

/* Convenient function exposed to users to write a constant */
int addConstant(Chunk* chunk, Value value);

/*
    Free dynamic array chunk->code and re-initialize chunk
*/
void freeChunk(Chunk* chunk);

#endif