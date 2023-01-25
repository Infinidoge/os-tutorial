#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "../cpu/types.h"
#include "../cpu/isr.h"

void init_keyboard();
static void keyboard_callback(registers_t regs);

#endif
