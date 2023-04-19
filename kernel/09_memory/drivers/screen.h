#ifndef SCREEN_H
#define SCREEN_H

#include "../cpu/types.h"
#include "../libc/string.h"

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define SCREEN_SIZE (MAX_COLS * MAX_ROWS)
#define SCREEN_SIZE_BYTES (2 * SCREEN_SIZE)

#define WHITE_ON_BLACK 0x0f
#define BLACK_ON_WHITE 0xf0
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public kernel API */
void clear_screen();
void kprint_at_until(const char *message, char sentinel, int col, int row);
void kprint_at(const char *message, int col, int row);
void kprint(const char *message);
void kprintln(const char *message);
void kprint_until(const char *message, char sentinel);
void kprintln_until(const char *message, char sentinel);
void kprintf(const char *format, ...);
void kprintlnf(const char *format, ...);
void kprint_backspace();
void paint(char c, char attr, int col, int row);
void paint_rect(char c, char attr, int origin_col, int origin_row, int width, int height, bool fill);
void copy_screen_to(uint8_t *address);
void copy_screen_from(uint8_t *address);
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

typedef struct ScreenState {
    uint8_t video_memory[SCREEN_SIZE_BYTES];
    int offset;
} screenstate;
void save_screen_to(screenstate *state);
void load_screen_from(screenstate *state);

/* Utilities for the shell */
#define PROMPT "> "
void print_prompt();

/* Debug Macros */

#define __DEBUG(name, variable) kprintlnf(#name ": {}", variable)
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

#define DEBUG_UINT_P(variable, length)  \
    {                                   \
        char __tmp[length];             \
        uint_to_ascii(variable, __tmp); \
        __DEBUG(variable, __tmp);       \
    }
#define DEBUG_UINT(variable) DEBUG_UINT_P(variable, 16)

#define DEBUG_UHEX_P(variable, length)  \
    {                                   \
        char __tmp[length];             \
        uhex_to_ascii(variable, __tmp); \
        __DEBUG(variable, __tmp);       \
    }
#define DEBUG_UHEX(variable) DEBUG_UHEX_P(variable, 16)

#define DEBUG_POINTER(variable) DEBUG_UHEX((size_t)variable)

#endif
