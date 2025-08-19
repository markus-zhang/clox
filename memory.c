#include "memory.h"
#include <stdlib.h>

void* reallocate(void* pointer, size_t oldSize, size_t newSize)
{
    if (newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    void* temp = realloc(pointer, newSize);
    if (temp == NULL)
    {
        exit(1);
    }
    return temp;
}