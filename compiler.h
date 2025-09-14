#ifndef clox_compiler_h
#define clox_compiler_h

#include "scanner.h"

/* Pass chunk for writing into */
/* Right now compilingChunk is global, but in the future I think we will have multiple chunks, so we need a pointer to it */
/* WHY: I think whence we need to parse functions, each function would have its own stack/chunk? */
bool compile(const char* source, Chunk* chunk);

static void advance();
static void expressions();
/* Number literal */
static void number();
/* Parenthesis Grouping */
static void grouping();
/* Unary operators */
static void unary();
/* Pratt parsing */
static void parsePrecedence(Precedence precedence);

static void consume(TokenType type, const char* message);
static void endCompiler();

static void errorAtCurrent(const char* message);
static void errorAt(Token* token, const char* message);

/* Chunk writing functions */
static void emitByte(uint8_t byte);
// We have some cases to write two bytes (e.g. push something onto stack, so one byte for PUSH and one byte for value)
static void emitBytes(uint8_t byte1, uint8_t byte2);
static void emitReturn();

static void emitConstant(double value);
static uint16_t getConstantIndex(double value);

#endif