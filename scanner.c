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
    int offset = scanner.offset;
    int line = scanner.line;

    if (isAtEnd())
    {
        return makeToken(TOKEN_EOF, offset, line);
    }
    /* Note that we increment current pointer immedaitely after the read */
    char firstChar = (char)(*(scanner.current));
    
    /* Check for numericals */
    if (isNumerical(firstChar))
    {
        return processNumerical(offset, line);
    }
    /* Check for identifiers and keywords*/
    else if (isAlpha(firstChar))
    {
        return processIdent(offset, line);
    }
    else
    {
        /* Default mode is to advance(), whic processNumerical() and processIdent() already take care of */
        advance();
    }

    switch (firstChar)
    {
        case '(':
        {
            return makeToken(TOKEN_LEFT_PAREN, offset, line);
        }
        case ')':
        {
            return makeToken(TOKEN_RIGHT_PAREN, offset, line);
        }
        case '{':
        {
            return makeToken(TOKEN_LEFT_BRACE, offset, line);
        }
        case '}':
        {
            return makeToken(TOKEN_RIGHT_BRACE, offset, line);
        }
        case ',':
        {
            return makeToken(TOKEN_COMMA, offset, line);
        }
        case '.':
        {
            return makeToken(TOKEN_DOT, offset, line);
        }
        case '-':
        {
            return makeToken(TOKEN_MINUS, offset, line);
        }
        case '+':
        {
            return makeToken(TOKEN_PLUS, offset, line);
        }
        case ';':
        {
            return makeToken(TOKEN_SEMICOLON, offset, line);
        }
        case '/':
        {
            return makeToken(TOKEN_SLASH, offset, line);
        }
        case '*':
        {
            return makeToken(TOKEN_STAR, offset, line);
        }
        case '"':
        {
            /* 
                Scan for the second " and post an error if none 
                A few edge cases to consider:
                - Empty string literal ""
                - Multiple line string literal
            */
            /* Empty string */
            if (*(scanner.current) == '"')
            {
                advance();
            }
            /* Non-empty strings */
            else 
            {
                while (peekChar() != '"')
                {
                    advance();
                    if (isAtEnd())
                    {
                        printf("Error: Cannot locate end quote for string literal\n");
                        return makeToken(TOKEN_ERROR, offset, line);
                    }
                }
                advance();  /* @ closing quote */
                advance();
            }
            return makeToken(TOKEN_STRING, offset, line);

        }
        default:
        {
            /* TODO: Think how do we architecture the codebase so that we can call panic() here */
            return makeToken(TOKEN_ERROR, offset, line);
        }
    }
}

Token makeToken(TokenType type, int offset, int line)
{
    // printf("%s\n", __func__);
    Token t;
    t.type = type;
    t.start = scanner.start;
    t.length = (int)(scanner.current - scanner.start);
    t.line = line;
    t.offset = offset;

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
                advance();
                break;
            }
            case ' ':
            case '\r':
            case '\t':
            {
                advance();
                break;
            }
            case '/':
            {
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

static void advance()
{
    if (*(scanner.current) == '\n')
    {
        scanner.offset = 0;
        scanner.line += 1;
    }
    else 
    {
        scanner.offset ++;
    }
    scanner.current ++;
}

bool isNumerical(char c)
{
    return ((c >= '0') && (c <= '9'));
}

Token processNumerical(int offset, int line)
{
    /* We first exhaust all numbers, and then find a decimal point, if found we again exhaust all numbers */
    while (isNumerical(peekChar()))
    {
        advance();
    }

    if (peekChar() == '.')
    {
        advance();
        while (isNumerical(peekChar()))
        {
            advance();
        }
    }

    /* Make sure current char points to the first non-numerical char */
    advance();

    return makeToken(TOKEN_NUMBER, offset, line);
}

bool isAlpha(char c)
{
    return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_'));
}

Token processIdent(int offset, int line)
{
    /* once the first char is confirmed, the rest can be numerical or alpha or underscore, like a123_45z */
    while (isNumerical(peekChar()) || isAlpha(peekChar()))
    {
        advance();
    }

    /* Make sure current char points to the first non-numerical char */
    advance();

    /* Check for keywords */

    char leadingChar = *(scanner.start);
    int len = scanner.current - scanner.start;
    switch(leadingChar)
    {
        case 'a':
        {
            if (scanner.current - scanner.start == strlen("and") && memcmp(scanner.start, "and", strlen("and")) == 0)
            {
                return makeToken(TOKEN_AND, offset, line);;
            }
            break;
        }
        case 'c':
        {
            if (scanner.current - scanner.start == strlen("class") && memcmp(scanner.start, "class", strlen("class")) == 0)
            {
                return makeToken(TOKEN_CLASS, offset, line);;
            }
            break;
        }
        case 'e':
        {
            if (scanner.current - scanner.start == strlen("else") && memcmp(scanner.start, "else", strlen("else")) == 0)
            {
                return makeToken(TOKEN_ELSE, offset, line);;
            }
            break;
        }
        case 'f':
        {
            if (scanner.current - scanner.start == strlen("for") && memcmp(scanner.start, "for", strlen("for")) == 0)
            {
                return makeToken(TOKEN_FOR, offset, line);;
            }
            if (scanner.current - scanner.start == strlen("fun") && memcmp(scanner.start, "fun", strlen("fun")) == 0)
            {
                return makeToken(TOKEN_FUN, offset, line);;
            }
            if (scanner.current - scanner.start == strlen("false") && memcmp(scanner.start, "false", strlen("false")) == 0)
            {
                return makeToken(TOKEN_FALSE, offset, line);;
            }
            break;
        }
        case 'i':
        {
            if (scanner.current - scanner.start == strlen("if") && memcmp(scanner.start, "if", strlen("if")) == 0)
            {
                return makeToken(TOKEN_IF, offset, line);;
            }
            break;
        }
        case 'n':
        {
            if (scanner.current - scanner.start == strlen("nil") && memcmp(scanner.start, "nil", strlen("nil")) == 0)
            {
                return makeToken(TOKEN_NIL, offset, line);;
            }
            break;
        }
        case 'o':
        {
            if (scanner.current - scanner.start == strlen("or") && memcmp(scanner.start, "or", strlen("or")) == 0)
            {
                return makeToken(TOKEN_OR, offset, line);;
            }
            break;
        }
        case 'p':
        {
            if (scanner.current - scanner.start == strlen("print") && memcmp(scanner.start, "print", strlen("print")) == 0)
            {
                return makeToken(TOKEN_PRINT, offset, line);;
            }
            break;
        }
        case 'r':
        {
            if (scanner.current - scanner.start == strlen("return") && memcmp(scanner.start, "return", strlen("return")) == 0)
            {
                return makeToken(TOKEN_RETURN, offset, line);;
            }
            break;
        }
        case 's':
        {
            if (scanner.current - scanner.start == strlen("super") && memcmp(scanner.start, "super", strlen("super")) == 0)
            {
                return makeToken(TOKEN_RETURN, offset, line);;
            }
            break;
        }
        case 't':
        {
            if (scanner.current - scanner.start == strlen("this") && memcmp(scanner.start, "this", strlen("this")) == 0)
            {
                return makeToken(TOKEN_RETURN, offset, line);;
            }
            if (scanner.current - scanner.start == strlen("true") && memcmp(scanner.start, "true", strlen("true")) == 0)
            {
                return makeToken(TOKEN_TRUE, offset, line);;
            }
            break;
        }
        case 'v':
        {
            if (scanner.current - scanner.start == strlen("var") && memcmp(scanner.start, "var", strlen("var")) == 0)
            {
                return makeToken(TOKEN_VAR, offset, line);;
            }
            break;
        }
        case 'w':
        {
            if (scanner.current - scanner.start == strlen("while") && memcmp(scanner.start, "while", strlen("while")) == 0)
            {
                return makeToken(TOKEN_WHILE, offset, line);;
            }
            break;
        }
        default:
        {
            return makeToken(TOKEN_IDENTIFIER, offset, line);
            break;
        }
    }
}

void dumpToken(Token t, const char* source)
{
    printf("\t%s @", TokenTypeName[t.type]);
    printf("\tline %d, offset %d", t.line, t.offset);
    printf("\tLength: %d", t.length);
    printf("\tLexeme: ");

    for (int i = 0; i < t.length; i++)
    {
        putchar(*(t.start + i));
    }

    putchar('\n');
}