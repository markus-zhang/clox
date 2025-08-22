#include "common.h"
#include "debug.h"
#include "vm.h"

/* Global variable just to keep simple */
VM vm;

void initVM()
{
    vm.stackTop = vm.stack;
}

void freeVM()
{

}

InterpreterResult interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = chunk->code;
    return run();
}

/*
    The core of the VM, just execute code and manage IP
*/
static InterpreterResult run()
{
#ifdef DEBUG_TRACE_EXECUTION
    /* NOTE: second argument is the offset */
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    while (1)
    {
        /* NOTE: Always point ip to the next byte */
        switch(*(vm.ip++))
        {
            case OP_RETURN:
            {
                /* NOTE: Temp pop and print, will change later */
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_CONSTANT:
            {
                int constantIndex = *(vm.ip++);
                printValue(vm.chunk->constants.values[constantIndex]);
                printf("\n");
                push(constantIndex);
                break;
            }
            case OP_CONSTANT_LONG:
            {
                int constantIndex = (vm.chunk->code[*(vm.ip)] << 16) + (vm.chunk->code[*(vm.ip) + 1] << 8) + vm.chunk->code[*(vm.ip) + 2];
                printValue(vm.chunk->constants.values[constantIndex]);
                printf("\n");
                push(constantIndex);
                break;
            }
        }
    }
}

void push(Value value)
{
    if ((int)(vm.stackTop - vm.stack) >= STACK_MAX)
    {
        panic("Stack overflow");
    }
    *(vm.stackTop) = value;
    vm.stackTop ++;
}

Value pop()
{
    /* Note: It's OK to reduce stackTop first, because it is always pointing to the next available index */
    vm.stackTop --;
    return *(vm.stackTop);
}

void panic(char* panicMessage)
{
    printf("%s\n", panicMessage);
    /* TODO: Design different panic code */
    exit(1);
}

void DumpStack(DumpTarget target)
{
    if (target == DUMP_CONSOLE)
    {
        /* Just print to terminal window */
        /* We want number of elements, list of elements in order, etc. */
        printf("---------- BEGIN STACK DUMP ------------\n");
        printf("Number of elements: %d\n", (int)(vm.stackTop - vm.stack));

        // for (int i = 0; i < (int)(vm.stackTop - vm.stack); i++)
        // {
        //     printf("Index %d ->", i);
        //     printValue(vm.stack[i]);
        //     printf("\n");
        // }

        /* The book's version is much better */
        for (Value* index = vm.stack; index < vm.stackTop; index++)
        {
            printf("Index %d ->", (int)(vm.stackTop - vm.stack));
            printValue(*(index));
            printf("\n");
        }

        printf("Top of Stack ->\n");

        printf("---------- END STACK DUMP ------------\n");
    }
    else if (target == DUMP_FILE)
    {
        /* Create and replace ./stack.dump */
    }
}