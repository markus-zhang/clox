#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "chunk.h"


typedef struct Parser
{
    Token current;
    Token previous;
    bool hadError;
    /* For error recovery: whenever in panic mode, sync after the scope */
    bool panicMode;
} Parser;

/* Again we make this global */
Parser parser;
Chunk* compilingChunk;

bool compile(const char* source, Chunk* chunk)
{
    initScanner(source);
    compilingChunk = chunk;
    parser.panicMode = false;
    parser.hadError = false;
    
    /* Scanning and making tokens */

    /* 
        WHY: We don't loop through the whole source code as done below.
        This is a "peekhole" compiler that asks the tokenizer for the next token,
        and in most cases it only needs to know a few tokens for context
    */
    // while (1)
    // {
    //     Token t = scanToken();
    //     dumpToken(t, source);
    //     if (t.type == TOKEN_EOF)
    //     {
    //         break;
    //     }
    // }

    advance();
    expressions();
    consume(TOKEN_EOF, "Expect end of expression.");

    endCompiler();

    /* 1 for no error and 0 for error */
    return !parser.hadError;
}

static void advance()
{
    parser.previous = parser.current;

    while (true)
    {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR)
        {
            /* So usually we only walk one Token */
            break;
        }
        errorAtCurrent(parser.current.start);
    }
}

/*
    NOTE: This is the meat of the parser
    Right now we only deal with four types of values:
    - Number literals
    - Parentheses for grouping
    - Unary negation
    - +*-/
    Praat parser is table driven. The table describes the precedency.

    We define a function for each type of expression e.g. number() for number literals, that outputs 
    the bytecode.

    Then we build an array of function pointers, each pointing to one of the ^ functions. 
    The indexs of each function equals the TokenType enum values.
*/
static void expressions()
{

}

static void consume(TokenType type, const char* message)
{
    /* Same as advance() but validate the current token first */
    if (parser.current.type == type)
    {
        advance();
    }
    else
    {
        errorAtCurrent(message);
    }
}

static void endCompiler()
{
    // NOTE: Right now we only deal with expressions
    // And we need to return/emit the expression, right?
    // WHY: to print that value, we are temporarily using the OP_RETURN instruction
    // So we have the compiler add one to the end of the chunk
    emitReturn();
}

static void errorAtCurrent(const char* message)
{
    errorAt(&parser.previous, message);
}

static void errorAt(Token* token, const char* message)
{
    if (!parser.panicMode)
    {
        parser.panicMode = true;
    }

    fprintf(stderr, "Line %d offset %d Error\n", token->line, token->offset);

    /* The format says: print a string starts from token->start with token->length of bytes */
    fprintf(stderr, "Lexeme: '%.*s'", token->length, token->start);

    parser.hadError = true;
}

static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line, parser.previous.offset);
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

/* 
    NOTE: What the call chain does is to push the constant to the array of constants, 
    and grab the index and push index, then push the index into a chunk (remember we don't save value, just index)
*/
static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(value);
}

static void grouping()
{
    /* Assuming initial ( consumed at this point */
    /* NOTE: Whatever inside the parenthesis is an expression */
    expressions();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void unary()
{
    /* Save the operator */
    TokenType op = parser.previous.type;

    expression();

    switch (op)
    {
        case TOKEN_MINUS:
        {
            
        }
    }
}

static void emitConstant(double value)
{
    emitBytes(OP_CONSTANT_LONG, getConstantIndex(value));
}

/* We will only use OP_CONSTANT_LONG */
static uint16_t getConstantIndex(double value)
{
    uint16_t constantIndex = addConstant(compilingChunk, value);
    if (constantIndex >= UINT16_MAX)
    {
        errorAt(&parser.previous, "Constant Array overflow");
        return 0;
    }
    return constantIndex;
}