#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "../cpu/types.h"

#define BACKSPACE 14
#define ENTER 28
#define LSHIFT 42
#define RSHIFT 54

typedef void (*keyhandler)(uint8_t);

extern keyhandler key_handler;

keyhandler swap_key_handler(keyhandler new_handler);
void return_key_handler(keyhandler new_handler);

const int get_scancode(const char *name);
char get_letter(int scancode, bool upper);

void init_keyboard();

#endif
