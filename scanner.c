#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct
{
    const char* start;
    const char* current;
    int line;
    int offset; /* for debugging, we need to know the offset from the beginning of the line */
} Scanner;

Scanner scanner;
bool inString = false;

void initScanner(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 0;
    scanner.offset = 0;
}

Token scanToken()
{
    scanner.start = scanner.current;

    if (isAtEnd())
    {
        return makeToken(TOKEN_EOF);
    }
    /* Note that we increment current pointer immedaitely after the read */
    char firstChar = (char)(*(scanner.current++));
    switch (firstChar)
    {
        case '\n':
        {
            /* We need to update state */
            scanner.line ++;
            scanner.offset = 0;
            return makeToken(TOKEN_DUMMY);
        }
        case ' ':
        case '\t':
        {
            
        }
        case '(':
        {
            return makeToken(TOKEN_LEFT_PAREN);
        }
        case ')':
        {
            return makeToken(TOKEN_RIGHT_PAREN);
        }
        case '{':
        {
            return makeToken(TOKEN_LEFT_BRACE);
        }
        case '}':
        {
            return makeToken(TOKEN_RIGHT_BRACE);
        }
        case ',':
        {
            return makeToken(TOKEN_COMMA);
        }
        case '.':
        {
            return makeToken(TOKEN_DOT);
        }
        case '-':
        {
            return makeToken(TOKEN_MINUS);
        }
        case '+':
        {
            return makeToken(TOKEN_PLUS);
        }
        case ';':
        {
            return makeToken(TOKEN_SEMICOLON);
        }
        case '/':
        {
            return makeToken(TOKEN_SLASH);
        }
        case '*':
        {
            return makeToken(TOKEN_STAR);
        }
        default:
        {
            /* TODO: Think how do we architecture the codebase so that we can call panic() here */
            return makeToken(TOKEN_ERROR);
        }
    }
}

Token makeToken(TokenType type)
{
    Token t;
    t.type = type;
    t.start = scanner.start;
    t.length = (int)(scanner.current - scanner.start);
    t.line = scanner.line;
    t.offset = scanner.offset;

    return t;
}

bool isAtEnd()
{
    return (*(scanner.current) == '\0');
}

void dumpToken(Token t)
{
    printf("%s @", TokenTypeName[t.type]);
    printf("%s line %d, offset %d\n", t.line, t.offset);
}