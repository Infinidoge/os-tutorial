#ifndef MEM_H
#define MEM_H

#include "../cpu/types.h"

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

void *malloc(size_t size);
void free(void *ptr);
void *realloc(size_t size);

#endif
