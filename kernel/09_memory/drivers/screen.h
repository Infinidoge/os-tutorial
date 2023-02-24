#ifndef SCREEN_H
#define SCREEN_H

#include "../cpu/types.h"

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public kernel API */
void clear_screen();
void kprint_at_until(const char *message, char sentinel, int col, int row);
void kprint_at(const char *message, int col, int row);
void kprint(const char *message);
void kprint_until(const char *message, char sentinel);
void kprintf(const char *format, ...);
void kprint_backspace();

#define __DEBUG(name, variable) kprintf(#name ": {}\n", variable)
#define DEBUG(variable) __DEBUG(variable, variable)

#define DEBUG_INT_P(variable, length)  \
    {                                  \
        char __tmp[length];            \
        int_to_ascii(variable, __tmp); \
        __DEBUG(variable, __tmp);      \
    }
#define DEBUG_INT(variable) DEBUG_INT_P(variable, 16)

#define DEBUG_HEX_P(variable, length)  \
    {                                  \
        char __tmp[length];            \
        hex_to_ascii(variable, __tmp); \
        __DEBUG(variable, __tmp);      \
    }
#define DEBUG_HEX(variable) DEBUG_HEX_P(variable, 16)

#define DEBUG_CHAR(variable)                       \
    {                                              \
        char __tmp[] = {'`', variable, '`', '\0'}; \
        __DEBUG(variable, __tmp);                  \
    }

#endif
