#include "mem.h"
#include "meta.h"

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for (; len != 0; len--)
        *temp++ = val;
}

size_t free_mem_addr = END;

size_t kmalloc(size_t size, bool align, size_t *phys_addr) {
    if (align && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }

    if (phys_addr)
        *phys_addr = free_mem_addr;

    size_t ret = free_mem_addr;
    free_mem_addr += size;
    return ret;
}
