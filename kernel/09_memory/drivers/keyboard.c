#include "keyboard.h"
#include "../cpu/isr.h"
#include "../cpu/ports.h"
#include "../kernel/shell.h"
#include "../libc/function.h"
#include "../libc/string.h"
#include "screen.h"

#define BACKSPACE 14
#define ENTER 28
#define LSHIFT 42
#define RSHIFT 54

static char key_buffer[256];

#define SC_MAX 57
const char *sc_name[] = {
    "ERROR",
    "Esc",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "=",
    "Backspace",
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "[",
    "]",
    "Enter",
    "Lctrl",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    ";",
    "'",
    "`",
    "LShift",
    "\\",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    ",",
    ".",
    "/",
    "RShift",
    "Keypad *",
    "LAlt",
    "Spacebar",
};

const char sc_lower[] = {
    '?',
    '?',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '?',
    '?',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '?',
    '?',
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    '?',
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    '?',
    '?',
    '?',
    ' ',
};

const char sc_upper[] = {
    '?',
    '?',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+',
    '?',
    '?',
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{',
    '}',
    '?',
    '?',
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':',
    '"',
    '~',
    '?',
    '?',
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<',
    '>',
    '|',
    '?',
    '?',
    '?',
    ' ',
};

bool next_upper = false;

static char get_letter(int scancode) {
    if (next_upper) {
        next_upper = false;
        return sc_upper[scancode];
    } else {
        return sc_lower[scancode];
    }
}

#define KEY_BUFFER_EMPTY (key_buffer[0] == '\0')

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);

    if (scancode > SC_MAX)
        return;
    if (scancode == BACKSPACE) {
        if (!KEY_BUFFER_EMPTY) {
            backspace(key_buffer);
            kprint_backspace();
        }
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else if (scancode == LSHIFT || scancode == RSHIFT) {
        next_upper = true;
    } else {
        char letter = get_letter(scancode);

        /* Remember that kprint only accepts char[] */
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        kprint(str);
    }
    UNUSED(regs);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
