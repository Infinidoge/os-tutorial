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

    kprint("Type something, it will go through the kernel\n"
           "Type help for a list of commands\n"
           "Type end to halt the CPU\n");

    print_prompt();
}
