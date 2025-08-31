#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

const char* TokenTypeName[] = {
    /* Single-character tokens */
    "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
    "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",
    "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_MINUS", "TOKEN_PLUS",
    "TOKEN_SEMICOLON", "TOKEN_SLASH", "TOKEN_STAR",

    // One or two character tokens.
    "TOKEN_BANG", "TOKEN_BANG_EQUAL",
    "TOKEN_EQUAL", "TOKEN_EQUAL_EQUAL",
    "TOKEN_GREATER", "TOKEN_GREATER_EQUAL",
    "TOKEN_LESS", "TOKEN_LESS_EQUAL",
    // Literals.
    "TOKEN_IDENTIFIER", "TOKEN_STRING", "TOKEN_NUMBER",
    // Keywords.
    "TOKEN_AND", "TOKEN_CLASS", "TOKEN_ELSE", "TOKEN_FALSE",
    "TOKEN_FOR", "TOKEN_FUN", "TOKEN_IF", "TOKEN_NIL", "TOKEN_OR",
    "TOKEN_PRINT", "TOKEN_RETURN", "TOKEN_SUPER", "TOKEN_THIS",
    "TOKEN_TRUE", "TOKEN_VAR", "TOKEN_WHILE",

    "TOKEN_ERROR", "TOKEN_EOF", "TOKEN_DUMMY"
};

void initScanner(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 0;
    scanner.offset = 0;
}

Token scanToken()
{
    // printf("%s\n", __func__);
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
    // printf("%s\n", __func__);
    Token t;
    t.type = type;
    t.start = scanner.start;
    t.length = (int)(scanner.current - scanner.start);
    t.line = scanner.line;
    /* TODO: Update Scanner offset */
    scanner.offset += t.length;
    t.offset = scanner.offset;

    return t;
}

bool isAtEnd()
{
    // printf("%s\n", __func__);
    return (*(scanner.current) == '\0');
}

/* Dealing with newline, whitespaces and comments */
void processNLWSC()
{
    while (1)
    {
        char currentChar = *(scanner.current);
        // printf("Current Char -> %c, %d\n", currentChar, (int)currentChar);
        switch (currentChar)
        {
            case 'q':
            {
                exit(0);
            }
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
                /* FIXME: stuck in while loop for single /, why? */
                if (peekChar() == '/')
                {
                    /* Skip to the next line */
                    while (*(scanner.current) != '\n' && (!isAtEnd()))
                    {
                        // printf("Skipping: %c\n", *(scanner.current));
                        scanner.current ++;
                        scanner.offset ++;
                    }
                    /* Now current points to '\n' which will be dealt by next loop */
                }
                else
                {
                    return;
                }
                break;
            }
            default:
            {
                return;
            }
        }
    }
}

/* Return the next char without moving the current pointer */

char peekChar()
{
    return *(scanner.current + 1);
}

void dumpToken(Token t)
{
    printf("\t%s @", TokenTypeName[t.type]);
    printf("line %d, offset %d\n", t.line, t.offset);
}