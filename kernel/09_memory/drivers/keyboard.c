#include "keyboard.h"
#include "../cpu/isr.h"
#include "../cpu/ports.h"
#include "../kernel/shell.h"
#include "../libc/function.h"
#include "../libc/string.h"
#include "screen.h"

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

const int get_scancode(const char *name) {
    for (int i = 1; i < LEN(sc_name); i++) {
        if (strcmp(sc_name[i], name))
            return i;
    }

    kprintln("ERROR: get_scancode on invalid scancode name");
    asm volatile("hlt");
    return -1;
}

char get_letter(int scancode, bool upper) {
    return (upper ? sc_upper : sc_lower)[scancode];
}

static void empty_handler(uint8_t scancode){};

keyhandler key_handler = &empty_handler;

keyhandler swap_key_handler(keyhandler new_handler) {
    keyhandler current = key_handler;
    key_handler = new_handler;
    return current;
}

void return_key_handler(keyhandler new_handler) {
    key_handler = new_handler;
}

static void keyboard_callback(registers_t regs) {
    UNUSED(regs);

    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);

    if (scancode > SC_MAX)
        return;

    (*key_handler)(scancode);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
