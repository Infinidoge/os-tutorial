#ifndef TYPES_H
#define TYPES_H

// IWYU pragma: begin_exports
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// IWYU pragma: end_exports

#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#endif
