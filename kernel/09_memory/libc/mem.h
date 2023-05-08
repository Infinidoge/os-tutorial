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
size_t kmalloc_naive(size_t size, bool align, size_t *phys_addr);

void init_memory();

enum FitType { FIRST, BEST, WORST };

size_t kmalloc(size_t size);
size_t kcalloc(size_t n, size_t size);
void kfree(size_t address);

typedef struct MemoryNode {
    size_t address;
    size_t size;
    struct MemoryNode *next;
    struct MemoryNode *prev;
} memorynode;

memorynode *create_memorynode(size_t address, size_t size);

// All functions that manipulate the list in some way return the head of the list
// This is necessary in case the head of the list changes after insertion, deletion, etc.
// Would otherwise require a lot of indirection or manual re-finding of the head of the list
memorynode *insert_after(memorynode *target, memorynode *new);
memorynode *insert_before(memorynode *target, memorynode *new);
memorynode *add_new(memorynode *list, size_t address, size_t size);
memorynode *sort_by_address(memorynode *list);
memorynode *sort_by_size(memorynode *list, bool ascending);
memorynode *delete_by_address(memorynode *list, size_t address);

memorynode *find(memorynode *list, size_t address);

void print_memory();
void memory_map();

#endif
