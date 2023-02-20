#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/meta.h"
#include "../libc/string.h"

void _start() {
    clear_screen();

    isr_install();
    irq_install();

    char etext[10];
    hex_to_ascii(ETEXT, etext);

    char edata[10];
    hex_to_ascii(EDATA, edata);

    char end[10];
    hex_to_ascii(END, end);

    kprint("etext: ");
    kprint(etext);
    kprint("\n");

    kprint("edata: ");
    kprint(edata);
    kprint("\n");

    kprint("end: ");
    kprint(end);
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
