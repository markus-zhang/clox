#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void repl();
void runFile(const char* filename);
static void interpretCode(char* buffer);
void test(Chunk* chunk);
bool containBackSlash(char* line, int maxLength);
static void removeTrailingBackSlash(char* line);
static void combineMultipleLine(char* target, char source[16][1024], int lineCount);

/* Global variables */
/* Buffer that contains the whole clox script */
char* cloxCodeBuffer;

int main(int argc, const char* argv[])
{
    initVM();
    Chunk chunk;
    initChunk(&chunk);

    cloxCodeBuffer = NULL;

    if (argc == 1)
    {
        /* No file loaded, jump into REPL */
        repl();
    }
    else if (argc == 2)
    {
        /* Need to load file */
        runFile(argv[1]);
    }
    else
    {
        printf("USAGE: ./clox [filename]\n");
    }

    // test(&chunk);

    /* Dial down all resources */
    freeVM();
    freeChunk(&chunk);

    return 0;
}

void test(Chunk* chunk)
{
    /* 
        Manually insert a constant chunk for testing
        Recall that addConstant returns the index of 1.2 in the array
    */
    // int constantIndex = addConstant(&chunk, 1.2);
    // 10 is just a random line number
    // writeChunk(&chunk, OP_CONSTANT, 10, 0);
    // writeChunk(&chunk, constantIndex, 10, 0);

    // NOTE: Test OP_CONSTANT and OP_CONSTANT_LONG
    writeConstant(chunk, 1.2, 10, 0);
    writeConstant(chunk, 100.08, 10, 2);
    writeConstant(chunk, 999, 10, 20);
    writeChunk(chunk, OP_NEGATE, 22, 0);
    /* Test Binary OpCodes */
    writeChunk(chunk, OP_ADD, 23, 0);
    writeChunk(chunk, OP_SUB, 24, 0);
    // This should panic
    // writeChunk(&chunk, OP_MUL, 24, 0);


    // 11 is just a random line number
    writeChunk(chunk, OP_RETURN, 11, 0);
    // disassembleChunk(&chunk, "test chunk");

    /* Run VM */
    interpret(chunk);
    // DumpStack(DUMP_CONSOLE);
}

void repl()
{
    /*
        REPL should handle multiple line segmeneted by '\' char
        For example:
        int i \
        if (i = 0) \
        { \
            print("i is 0") \
        }

        This should be saved into multiple lines

        So whenever a <newline> char hits and closes fgets(), we need to check if the last char (well, actually, the one before <newline>) of previous line is '\'. If so, we will keep consuming new lines
    */

    while (1)
    {
        // 16 lines of 1024 chars should be good enough?
        char input[16][1024];
        int lineIndex = 0;  // which line?
        int lineOffset = 0; // which offset in the line?

        printf("> ");
        while (fgets(input[lineIndex], 1024, stdin) != NULL)
        {
            // If no \ char at the end, just break
            if (!containBackSlash(input[lineIndex], 1024))
            {
                break;
            }
            else
            {
                removeTrailingBackSlash(input[lineIndex]);
                lineIndex++;
            }
        }
        /* Debugging */
        for (int i = 0; i <= lineIndex; i++)
        {
            printf("Line %d: %s", i, input[i]);
        }

        combineMultipleLine(cloxCodeBuffer, input, lineIndex);

        // interpretCode();
    }
}

void runFile(const char* filename)
{
    /*
        Load string stored in a script file into cloxCodeBuffer
    */
    int fd = open(filename, O_RDONLY);
    int fileSize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    cloxCodeBuffer = malloc(fileSize + 1);
    if (read(fd, cloxCodeBuffer, fileSize) == -1)
    {
        panic("Failed to read script file into buffer!\n");
    }

    cloxCodeBuffer[fileSize] = '\0';
    /* Debugging */
    printf("%s\n", cloxCodeBuffer);

    close(fd);

    return;
}

static void interpretCode(char* buffer)
{
    /* Just hang */
    while(1) {}
}

bool containBackSlash(char* line, int maxLength)
{
    bool result = true;

    for (int i = 1; i < maxLength; i++)
    {
        if (line[i] == '\n')
        {
            if (line[i - 1] == '\\')
            {
                return result;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

static void removeTrailingBackSlash(char* line)
{
    /* Lop off the trailing <newline> and '\' */
    int lineLength = strlen(line);

    /* 
        NOTE: Only supposed to chop off trailing chars when we have a multi-line input 
        <newline> is probably skipped by scanner, but '\' is an issue, so I'll remove it by moving '\n' and '\0' back one char
    */
    if (line[lineLength - 1] == '\n' && line[lineLength - 2] == '\\')
    {
        line[lineLength - 2] = '\n';
        line[lineLength - 1] = '\0';
    }
}

static void combineMultipleLine(char* target, char source[16][1024], int lineCount)
{
    /*
        Combine all (`lineCount` + 1) lines in `source` into `target`.
        Please call removeTrailingBackSlash() on each line before calling this
    */
    
    /* target should be NULL */
    if (target)
    {
        free(target);
        target = NULL;
    }

    int totalSize = 0;

    /* Figure out total size */
    for (int i = 0; i <= lineCount; i++)
    {
        /* strlen() does NOT count the trailing '\0' */
        totalSize += strlen(source[lineCount]);
    }
    /* Space for `\'0` */
    totalSize += 1;
    
    int charCount = 0;
    target = malloc(totalSize * sizeof(char));
    char* targetWalker = target;

    for (int i = 0; i <= lineCount; i++)
    {
        int lineLength = strlen(source[i]);
        strncat(target, source[i], lineLength);
    }

    /* Debug */
    printf("%s -> %s(): %s\n", __FILE__, __func__, target);
}