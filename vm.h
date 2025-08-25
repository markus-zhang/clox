#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"

#define STACK_MAX 256

typedef struct
{
    Chunk* chunk;
    uint8_t* ip;
    /* Stack for e.g. expression evaluation */
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILER_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpreterResult;

typedef enum
{
    DUMP_CONSOLE,
    DUMP_FILE
} DumpTarget;

void initVM();
void freeVM();
InterpreterResult interpret(Chunk* chunk);
static InterpreterResult run();
void push(Value value);
Value pop();
Value peek();

/* Operations */
static void BinaryOP(Opcode op);

/* Like a kernel panic */
void panic(char* panicMessage);

/* For debugging */
void DumpStack(DumpTarget target);

#endif