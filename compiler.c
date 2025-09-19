#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "chunk.h"
#include "debug.h"


typedef struct Parser
{
    Token current;
    Token previous;
    bool hadError;
    /* For error recovery: whenever in panic mode, sync after the scope */
    bool panicMode;
} Parser;

/*
    Precedence for Pratt parser
*/
typedef enum
{
    // Lowest
    PREC_NONE,
    PREC_ASSIGNMENT,    // =
    PREC_OR,            // or
    PREC_AND,           // and
    PREC_EQUALITY,      // == !=
    PREC_COMPARISON,    // < > <= >=
    PREC_TERM,          // + -
    PREC_FACTOR,        // * /
    PREC_UNARY,         // - !
    PREC_CALL,          // . ()
    PREC_PRIMARY
    // Highest
} Precedence;

typedef void (*ParseFn)();

typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

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

    *************************************************************
    * NOTE: each parsing function like number() does NOT know precedence, unlike recursive descendent parsing,
    * where each function would call into the highest precedent operator and go up from there
    * 
    * In a Pratt's parser, the precedences are stored in a table, and when the parser advances, 
    * it is "blocked" by any operators of higher precedences.
    * Take 5+2*4 as an example, we first reaches +, then it reaches *, which has higher precedence than +,
    * so the parser is blocked and only reads 5.
*/
static void expressions()
{
    /* NOTE: we simply starts from the lowest precedence */
    parsePrecedence(PREC_ASSIGNMENT);
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

    /*
        NOTE: parsePrecedence() is run before the unary op is emitted
        So for example, given -(5+6), OP_NEGATE is emitted as the last item
        This makes sense as the value first goes onto the stack and then the negate gets pushed
        During evaluation, the negation gets read first
    */
    parsePrecedence(PREC_UNARY);

    switch (op)
    {
        case TOKEN_MINUS:
        {
            emitByte(OP_NEGATE);
            break;
        }
        default:
        {
            return;
        }
    }
}

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  /* TODO: BANG eventually should have an unary prefix and etc. */
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

/* NOTE: Algo similar to wiki: https://en.wikipedia.org/wiki/Operator-precedence_parser#Pseudocode */
static void parsePrecedence(Precedence precedence) 
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;

    if (prefixRule == NULL)
    {
        errorAt(&parser.previous, "Expect expressions.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule* getRule(TokenType type)
{
    return &rules[type];
}

static void binary()
{
    /* 
        Something like 1+2,
        so we first parse the left side, then find a binary operator, and this get called
        The left operand was taken care by something else e.g. number() already
    */

    /*
        PREC_NONE,
        PREC_ASSIGNMENT,    // =
        PREC_OR,            // or
        PREC_AND,           // and
        PREC_EQUALITY,      // == !=
        PREC_COMPARISON,    // < > <= >=
        PREC_TERM,          // + -
        PREC_FACTOR,        // * /
        PREC_UNARY,         // - !
        PREC_CALL,          // . ()
        PREC_PRIMARY
    */

    TokenType op = parser.previous.type;
    ParseRule* rule = getRule(op);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (op)
    {
        case TOKEN_PLUS:
        {
            emitByte(OP_ADD);
            break;
        }
        case TOKEN_MINUS:
        {
            emitByte(OP_SUB);
            break;
        }
        case TOKEN_STAR:
        {
            emitByte(OP_MUL);
            break;
        }
        case TOKEN_SLASH:
        {
            emitByte(OP_DIV);
            break;
        }
        default:
        {
            return;
        }
    }


}


static void endCompiler()
{
    // NOTE: Right now we only deal with expressions
    // And we need to return/emit the expression, right?
    // WHY: to print that value, we are temporarily using the OP_RETURN instruction
    // So we have the compiler add one to the end of the chunk
    disassembleChunk(currentChunk(), "code");
    emitReturn();
}

static void expression() 
{
  // What goes here?
}

/* Forward declaration */

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

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