#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* source)
{
    initScanner(source);
    /* Scanning and making tokens */
    while (1)
    {
        Token t = scanToken();
        dumpToken(t);
        if (t.type == TOKEN_EOF)
        {
            break;
        }
    }
}