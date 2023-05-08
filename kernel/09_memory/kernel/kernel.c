#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/mem.h"
#include "scheduler.h"
#include "shell.h"

void _start() {
    clear_screen();

    isr_install();
    irq_install();

    init_memory();
    init_shell();

    run_scheduler();

    asm volatile("cli");
    asm volatile("hlt");
}
