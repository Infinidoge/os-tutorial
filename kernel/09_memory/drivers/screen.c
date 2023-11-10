#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/mem.h"
#include <stdarg.h>
#include <stdint.h>

uint8_t *video_memory = (uint8_t *)VIDEO_ADDRESS;

/* Declaration of private functions */
int get_cursor_offset();
void set_cursor_offset(int offset);
void vkprintf(const char *format, va_list ptr);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 * Print until the given sentinel value.
 */
void kprint_at_until(const char *message, char sentinel, int col, int row) {
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* Loop through message and print it */
    int i = 0;
    while (message[i] != sentinel) {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

/**
 * Print a message at the specified location
 * If col, row, are negative, will use the current offset
 * Prints until a null byte
 */
void kprint_at(const char *message, int col, int row) {
    kprint_at_until(message, 0, col, row);
}

/**
 * Prints a message at the current offset
 * Prints until a null byte
 */
void kprint(const char *message) {
    kprint_at(message, -1, -1);
}

/**
 * Prints a message at the current offset, appending a newline at the end
 * Prints until a null byte
 */
void kprintln(const char *message) {
    kprint(message);
    kprint("\n");
}

/**
 * Prints a message at the current offset
 * Prints until the given sentinel value
 */
void kprint_until(const char *message, char sentinel) {
    kprint_at_until(message, sentinel, -1, -1);
}

/**
 * Prints a message at the current offset
 * Prints until the given sentinel value
 * Once the sentinel is reached, prints a newline
 */
void kprintln_until(const char *message, char sentinel) {
    kprint_until(message, sentinel);
    kprint("\n");
}

/**
 * Prints a message, formatting in variable values based on the given format string
 * Variable values must be character arrays/strings
 * Prints until a null byte in the format string
 */
void kprintf(const char *format, ...) {
    va_list ptr;
    va_start(ptr, format);
    vkprintf(format, ptr);
    va_end(ptr);
}

/**
 * Prints a message, formatting in variable values based on the given format string
 * Variable values must be character arrays/strings
 * Prints until a null byte in the format string
 * Prints a final newline at the end
 */
void kprintlnf(const char *format, ...) {
    va_list ptr;
    va_start(ptr, format);
    vkprintf(format, ptr);
    va_end(ptr);
    kprint("\n");
}

/**
 * Prints a backspace, erasing the previous character and changing the offset
 */
void kprint_backspace() {
    int offset = get_cursor_offset() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(0x08, col, row, WHITE_ON_BLACK);
}

/**
 * Prints a terminal prompt on a newline
 * If the previous output did not end on a newline, prints a Black-on-White percent sign (%) to indicate this
 */
void print_prompt() {
    int offset = get_cursor_offset();
    if (get_offset_col(offset) != 0) {
        print_char('%', get_offset_col(offset), get_offset_row(offset), BLACK_ON_WHITE);
        kprint("\n");
    }

    kprint(PROMPT);
}

#define __OFF_SCREEN(var) var >= MAX_COLS || var < 0
#define OFF_SCREEN(var1, var2) __OFF_SCREEN(var1) || __OFF_SCREEN(var2)

/**
 * Raw screen painting function
 */
void paint(char c, char attr, int col, int row) {
    if (OFF_SCREEN(col, row)) {
        video_memory[SCREEN_SIZE_BYTES - 2] = 'E';
        video_memory[SCREEN_SIZE_BYTES - 1] = RED_ON_WHITE;
        return;
    }

    int offset = get_offset(col, row);

    video_memory[offset] = c;
    video_memory[offset + 1] = attr;
}

#define ON_RECT_EDGE col == 0 || col == (width - 1) || row == 0 || row == (height - 1)

void paint_rect(char c, char attr, int origin_col, int origin_row, int width, int height, bool fill) {
    if (OFF_SCREEN(origin_col, origin_row) || OFF_SCREEN(origin_col + width - 1, origin_row + height - 1)) {
        paint('E', RED_ON_WHITE, MAX_COLS - 1, MAX_ROWS - 1);
        return;
    }

    for (int col = 0; col < width; col++) {
        for (int row = 0; row < height; row++) {
            if (fill || ON_RECT_EDGE) {
                paint(c, attr, col + origin_col, row + origin_row);
            }
        }
    }
}

#undef ON_RECT_EDGE
#undef __OFF_SCREEN
#undef OFF_SCREEN

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/

enum Specifier { NONE, INVALID, STRING, INT, UINT, HEX, BIN, BIN_8, BIN_16, BIN_32, BOOL };

enum Specifier check_specifier(const int index, const char *string) {
    int len = strlen(string);

    if (len < 2 || string[index] != '}')
        return NONE;

    if (string[index - 1] == '{')
        return STRING;

    if (len < 3)
        return NONE;

    if (string[index - 2] == '{') {
        switch (string[index - 1]) {
        case 'i':
            return INT;
        case 'u':
            return UINT;
        case 'x':
            return HEX;
        case 'b':
            return BIN;
        case '8':
            return BIN_8;
        case 'F':
            return BIN_16;
        case 'Z':
            return BIN_32;
        case 'B':
            return BOOL;
        default:
            return INVALID;
        }
    }

    return INVALID;
}

void vkprintf(const char *format, va_list ptr) {
    int last = 0;

    for (int i = 0; format[i] != 0; i++) {
        if (i == 0)
            continue;

        switch (check_specifier(i, format)) {
        case NONE:
            break;
        case INVALID:
            kprint_until(&format[last], '{');
            kprint("{INVALID SPECIFIER}");
            va_arg(ptr, void *);
            last = i + 1;
            break;
        case STRING:
            kprint_until(&format[last], '{');
            kprint(va_arg(ptr, char *));
            last = i + 1;
            break;
        case INT:
            kprint_until(&format[last], '{');
            {
                char tmp[16];
                int_to_ascii(va_arg(ptr, int), tmp);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case UINT:
            kprint_until(&format[last], '{');
            {
                char tmp[16];
                uint_to_ascii(va_arg(ptr, int), tmp);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case HEX:
            kprint_until(&format[last], '{');
            {
                char tmp[16];
                hex_to_ascii(va_arg(ptr, int), tmp);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case BIN:
            kprint_until(&format[last], '{');
            {
                char tmp[32];
                bin_to_ascii(va_arg(ptr, int), tmp);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case BIN_8:
            kprint_until(&format[last], '{');
            {
                char tmp[32];
                bin_to_ascii_padded(va_arg(ptr, int), tmp, 8);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case BIN_16:
            kprint_until(&format[last], '{');
            {
                char tmp[32];
                bin_to_ascii_padded(va_arg(ptr, int), tmp, 16);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case BIN_32:
            kprint_until(&format[last], '{');
            {
                char tmp[32];
                bin_to_ascii_padded(va_arg(ptr, int), tmp, 32);
                kprint(tmp);
            }
            last = i + 1;
            break;
        case BOOL:
            kprint_until(&format[last], '{');
            if (va_arg(ptr, int))
                kprint("True");
            else
                kprint("False");
            last = i + 1;
            break;
        }
    }
    kprint(&format[last]);
}

/**
 * Innermost print function for our kernel, directly accesses the video memory
 *
 * If 'col' and 'row' are negative, we will print at current cursor location
 * If 'attr' is zero it will use 'white on black' as default
 * Returns the offset of the next character
 * Sets the video cursor to the returned offset
 */
int print_char(char c, int col, int row, char attr) {
    if (!attr)
        attr = WHITE_ON_BLACK;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        video_memory[SCREEN_SIZE_BYTES - 2] = 'E';
        video_memory[SCREEN_SIZE_BYTES - 1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else
        offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row + 1);
    } else if (c == 0x08) { /* Backspace */
        video_memory[offset] = ' ';
        video_memory[offset + 1] = attr;
    } else {
        video_memory[offset] = c;
        video_memory[offset + 1] = attr;
        offset += 2;
    }

    /* Check if the offset is over screen size and scroll */
    if (offset >= SCREEN_SIZE_BYTES) {
        int i;
        for (i = 1; i < MAX_ROWS; i++)
            memory_copy(video_memory + get_offset(0, i), video_memory + get_offset(0, i - 1), MAX_COLS * 2);

        /* Blank last line */
        char *last_line = (char *)(video_memory + get_offset(0, MAX_ROWS - 1));
        for (i = 0; i < MAX_COLS * 2; i++)
            last_line[i] = 0;

        offset -= 2 * MAX_COLS;
    }

    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xff));
}

void clear_screen() {
    for (int i = 0; i < SCREEN_SIZE; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}
int get_offset_row(int offset) {
    return offset / (2 * MAX_COLS);
}
int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}

void copy_screen_to(uint8_t *address) {
    memory_copy(video_memory, address, SCREEN_SIZE_BYTES);
}

void copy_screen_from(uint8_t *address) {
    memory_copy(address, video_memory, SCREEN_SIZE_BYTES);
}

void save_screen_to(screenstate *state) {
    copy_screen_to(state->video_memory);
    state->offset = get_cursor_offset();
}

void load_screen_from(screenstate *state) {
    copy_screen_from(state->video_memory);
    set_cursor_offset(state->offset);
}
