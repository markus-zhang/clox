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
    /* Update state for WhiteSpaces and NewLines */
    processNLWSC();

    /* At this point, current points to the first non-wsnl char */
    scanner.start = scanner.current;

    if (isAtEnd())
    {
        return makeToken(TOKEN_EOF);
    }
    /* Note that we increment current pointer immedaitely after the read */
    char firstChar = (char)(*(scanner.current++));
    switch (firstChar)
    {
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
        case '"':
        {
            /* TODO: Scan strings */
            break;
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

/* Dealing with newline, whitespaces and comments */
void processNLWSC()
{
    char currentChar = *(scanner.current);
    while (1)
    {
        switch (currentChar)
        {
            case '\n':
            {
                scanner.line ++;
                scanner.offset = 0;
                scanner.current ++;
                break;
            }
            case ' ':
            case '\r':
            case '\t':
            {
                scanner.offset ++;
                scanner.current ++;
                break;
            }
            case '/':
            {
                if (peek() == '/')
                {
                    /* Skip to the next line */
                    while (*(scanner.current) != '\n' && isAtEnd())
                    {
                        scanner.current ++;
                    }
                    /* Now current points to '\n' which will be dealt by next loop */
                }
                break;
            }
        }
    }
}

/* Return the next char without moving the current pointer */

char peek()
{
    return *(scanner.current + 1);
}

void dumpToken(Token t)
{
    printf("%s @", TokenTypeName[t.type]);
    printf("%s line %d, offset %d\n", t.line, t.offset);
}