#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

extern size_t _end;

void _start() {
    clear_screen();

    isr_install();
    irq_install();

    char str[10];
    hex_to_ascii(&_end, str);

    kprint("Address: ");
    kprint(str);
    kprint("\n");

    kprint("Type something, it will go through the kernel\n"
           "Type END to halt the CPU\n> ");
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}
