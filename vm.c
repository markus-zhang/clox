#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"
#include <stdlib.h>

/* Global variable just to keep simple */
VM vm;

void initVM()
{
    vm.stackTop = vm.stack;
}

void freeVM()
{

}

InterpreterResult interpret(const char* source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILER_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpreterResult result = run();

    freeChunk(&chunk);
    return result;
}

/*
    The core of the VM, just execute code and manage IP
*/
static InterpreterResult run()
{
    while (1)
    {
        #ifdef DEBUG_TRACE_EXECUTION
            /* NOTE: second argument is the offset */
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        /* NOTE: Always point ip to the next byte */
        switch(*(vm.ip++))
        {
            case OP_RETURN:
            {
                /* NOTE: Temp pop and print, will change later */
                // printValue(pop());
                DumpStack(DUMP_CONSOLE);
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_CONSTANT:
            {
                int constantIndex = *(vm.ip++);
                printf("OP_CONSTANT: Index %d\n", constantIndex);
                printValue(vm.chunk->constants.values[constantIndex]);
                printf("\n");
                push((vm.chunk->constants.values)[constantIndex]);
                break;
            }
            case OP_CONSTANT_LONG:
            {
                int constantIndex = (*(vm.ip) << 16) + (*(vm.ip + 1) << 8) + *(vm.ip + 2);
                printf("OP_CONSTANT_LONG: Index %d\n", constantIndex);
                printValue(vm.chunk->constants.values[constantIndex]);
                printf("\n");
                push((vm.chunk->constants.values)[constantIndex]);
                vm.ip += 3;
                break;
            }
            /* Unary */
            case OP_NEGATE:
            {
                /* No need to push/pop, just mutate */
                // push(-pop());
                *(vm.stackTop - 1) = (*vm.stackTop - 1) * -1;
                break;
            }
            /* Binary a op b */
            case OP_ADD:
            {
                BinaryOP(OP_ADD);
                break;
            }
            case OP_SUB:
            {
                BinaryOP(OP_SUB);
                break;
            }
            case OP_MUL:
            {
                BinaryOP(OP_MUL);
                break;
            }
            case OP_DIV:
            {
                BinaryOP(OP_DIV);
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
    if (vm.stackTop < vm.stack)
    {
        /* Stack underflow */
        panic("pop(): Stack Underflow!\n");
    }
    return *(vm.stackTop);
}

Value peek()
{
    /* Note: Fetch top stack item without removing it */
    return *(vm.stackTop);
}

/* Operations */
static void BinaryOP(Opcode op)
{
    Value leftOperand = pop();
    Value rightOperand = pop();
    
    switch(op)
    {
        case (OP_ADD):
        {
            push(leftOperand + rightOperand);
            break;
        }
        case (OP_SUB):
        {
            push(leftOperand - rightOperand);
            break;
        }
        case (OP_MUL):
        {
            push(leftOperand * rightOperand);
            break;
        }
        case (OP_DIV):
        {
            push(leftOperand / rightOperand);
            break;
        }
        default:
        {
            printf("Unknown Binary OpCode %d\n", op);
        }
    }
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