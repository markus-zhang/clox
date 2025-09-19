#ifndef clox_compiler_h
#define clox_compiler_h

#include "vm.h"

/* Pass chunk for writing into */
/* Right now compilingChunk is global, but in the future I think we will have multiple chunks, so we need a pointer to it */
/* WHY: I think whence we need to parse functions, each function would have its own stack/chunk? */
bool compile(const char* source, Chunk* chunk);

#endif