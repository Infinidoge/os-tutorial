#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/mem.h"
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

    kprintf("etext: {}\n", etext);
    kprintf("edata: {}\n", edata);
    kprintf("end: {}\n", end);


    kprint("Type something, it will go through the kernel\n"
           "Type END to halt the CPU\n> ");
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "PAGE") == 0) {
        size_t phys_addr;
        size_t page = kmalloc(1000, true, &phys_addr);

        char page_str[16];
        hex_to_ascii(page, page_str);
        char phys_str[16];
        hex_to_ascii(phys_addr, phys_str);

        kprintf("Page: {}, Physical Address: {}\n", page_str, phys_str);
    }
    kprintf("You said: {}\n", input);
    kprint("> ");
}
