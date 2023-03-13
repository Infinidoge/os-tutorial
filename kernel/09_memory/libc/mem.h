#ifndef MEM_H
#define MEM_H

#include "../cpu/types.h"

void memory_copy(uint8_t *source, uint8_t *dest, size_t nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

#define PAGE_SIZE 4096 // 4KiB page size

// Helpers for memory alignment
#define ALIGNMENT 8
#define ALIGN_A(size, alignment) (((size) + (alignment - 1)) & ~(alignment - 1))
#define ALIGN(size) ALIGN_A(size, ALIGNMENT)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* At this stage there is no 'free' implemented. */
size_t kmalloc(size_t size, bool align, size_t *phys_addr);
#endif
